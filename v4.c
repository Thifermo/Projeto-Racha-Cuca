#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define MAX 6
#define CIMA 0
#define BAIXO 1
#define ESQUERDA 2
#define DIREITA 3

typedef struct {
    int n;
    int pecas[MAX][MAX];
    int lv; // linha vazia
    int cv; // coluna vazia
} Tabuleiro;

typedef struct {
    unsigned char *dados; // Pixels da imagem original
    int larg_orig;        // Largura original lida do arquivo
    int alt_orig;         // Altura original lida do arquivo
    int canais;           // Canais de cor (forçado para 3 = RGB)
    int larg_bloco;       // Largura de cada recorte (larg_orig / n)
    int alt_bloco;        // Altura de cada recorte (alt_orig / n)
} Imagem;

int ler_n() {
    int n;
    while (1) {
        printf("Qual o tamanho do tabuleiro (2 a 6)? ");
        if (scanf("%d", &n) == 1 && n >= 2 && n <= 6) {
            return n;
        }
        printf("Entrada invalida! Digite um numero entre 2 e 6.\n");
        while (getchar() != '\n');
    }
}

void iniciar_tabuleiro(Tabuleiro *tab, int n) {
    tab->n = n;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            tab->pecas[i][j] = (i == n - 1 && j == n - 1) ? 0 : (i * n + j + 1);
        }
    }
    tab->lv = n - 1;
    tab->cv = n - 1;
}

bool mover(Tabuleiro *tab, int dir) {
    int nl = tab->lv;
    int nc = tab->cv;

    if (dir == CIMA && tab->lv > 0) nl--;
    else if (dir == BAIXO && tab->lv < tab->n - 1) nl++;
    else if (dir == ESQUERDA && tab->cv > 0) nc--;
    else if (dir == DIREITA && tab->cv < tab->n - 1) nc++;
    else return false;

    tab->pecas[tab->lv][tab->cv] = tab->pecas[nl][nc];
    tab->pecas[nl][nc] = 0;
    tab->lv = nl;
    tab->cv = nc;
    return true;
}

// Retorna a direcao oposta para evitar desfazer o movimento anterior
int direcao_oposta(int dir) {
    if (dir == CIMA) return BAIXO;
    if (dir == BAIXO) return CIMA;
    if (dir == ESQUERDA) return DIREITA;
    return ESQUERDA;
}

// Embaralha aplicando 'qtd_movimentos' jogadas validas a partir do estado resolvido
void embaralhar(Tabuleiro *tab, int qtd_movimentos) {
    int ultima_dir = -1;
    int feitos = 0;

    while (feitos < qtd_movimentos) {
        int dir = rand() % 4; // Sorteia 0 (CIMA), 1 (BAIXO), 2 (ESQUERDA) ou 3 (DIREITA)

        // Ignora se for o movimento exato que desfaz o anterior
        if (ultima_dir != -1 && dir == direcao_oposta(ultima_dir)) {
            continue;
        }

        // Tenta mover; se bater na borda, o loop apenas sorteia outra direcao
        if (mover(tab, dir)) {
            ultima_dir = dir;
            feitos++;
        }
    }
}

void imprimir(Tabuleiro *tab) {
    printf("\n");
    for (int i = 0; i < tab->n; i++) {
        for (int j = 0; j < tab->n; j++) {
            printf("%3d ", tab->pecas[i][j]);
        }
        printf("\n");
    }

    printf("\nMovimentos validos:\n");
    if (tab->lv > 0)          printf("- Cima\n");
    if (tab->lv < tab->n - 1) printf("- Baixo\n");
    if (tab->cv > 0)          printf("- Esquerda\n");
    if (tab->cv < tab->n - 1) printf("- Direita\n");
}

// Carrega a imagem e calcula as dimensoes dos recortes e trata sobras nao multiplas de N
bool carregar_imagem(Imagem *img, const char *caminho, int n) {
    // O ultimo parametro (3) padroniza a imagem em RGB (3 bytes por pixel)
    img->dados = stbi_load(caminho, &img->larg_orig, &img->alt_orig, &img->canais, 3);
    if (!img->dados) {
        return false;
    }

    img->canais = 3;
    img->larg_bloco = img->larg_orig / n;
    img->alt_bloco = img->alt_orig / n;

    if (img->larg_bloco <= 0 || img->alt_bloco <= 0) {
        stbi_image_free(img->dados);
        return false;
    }
    return true;
}

// Aqui ele monta a imagem atual baseada na matriz do tabuleiro e salva como passo_XXX.png
void salvar_passo(Tabuleiro *tab, Imagem *img, int passo) {
    int n = tab->n;
    int larg_saida = img->larg_bloco * n;
    int alt_saida = img->alt_bloco * n;
    int tamanho_buffer = larg_saida * alt_saida * 3;

    unsigned char *saida = (unsigned char *) malloc(tamanho_buffer);
    if (!saida) return;

    // Preenche tudo de branco (255) inicialmente; o espaco vazio (0) ja fica branco
    memset(saida, 255, tamanho_buffer);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int id = tab->pecas[i][j];
            if (id == 0) continue; // Espaco vazio permanece branco

            // Descobre a linha e coluna originais da peca (id vai de 1 a N²-1)
            int orig_i = (id - 1) / n;
            int orig_j = (id - 1) % n;

            // Copia pixel por pixel do bloco original para a posicao (i, j) na saida
            for (int y = 0; y < img->alt_bloco; y++) {
                for (int x = 0; x < img->larg_bloco; x++) {
                    int px_orig_x = orig_j * img->larg_bloco + x;
                    int px_orig_y = orig_i * img->alt_bloco + y;
                    int idx_orig = (px_orig_y * img->larg_orig + px_orig_x) * 3;

                    int px_dest_x = j * img->larg_bloco + x;
                    int px_dest_y = i * img->alt_bloco + y;
                    int idx_dest = (px_dest_y * larg_saida + px_dest_x) * 3;

                    saida[idx_dest]     = img->dados[idx_orig];     // R
                    saida[idx_dest + 1] = img->dados[idx_orig + 1]; // G
                    saida[idx_dest + 2] = img->dados[idx_orig + 2]; // B
                }
            }
        }
    }

    char nome_arquivo[64];
    sprintf(nome_arquivo, "passo_%03d.png", passo);
    stbi_write_png(nome_arquivo, larg_saida, alt_saida, 3, saida, larg_saida * 3);
    printf("Imagem salva: %s\n", nome_arquivo);

    free(saida);
}

int main() {
    srand(time(NULL)); // Inicializa a semente aleatoria

    Tabuleiro tab;
    Imagem img;
    char caminho[256];

    int n = ler_n();

    printf("Digite o caminho da imagem (foto.png): ");
    scanf("%255s", caminho);

    if (!carregar_imagem(&img, caminho, n)) {
        printf("Erro: Nao foi possivel carregar a imagem '%s' ou dimensoes muito pequenas.\n", caminho);
        return 1;
    }

    iniciar_tabuleiro(&tab, n);

    // Embaralha proporcionalmente ao tamanho do tabuleiro
    int movimentos_embaralhar = n * n * 10;
    embaralhar(&tab, movimentos_embaralhar);

    printf("\n--- Tabuleiro Embaralhado (%d movimentos) ---", movimentos_embaralhar);
    imprimir(&tab);
    salvar_passo(&tab, &img, 0); // Salva o ponto de partida embaralhado (passo_000.png)

    stbi_image_free(img.dados);
    return 0;
}

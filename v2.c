#include <stdio.h>
#include <stdbool.h>

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
    else return false; // movimento bateu na borda

    // Faz a troca com o espaco vazio (0)
    tab->pecas[tab->lv][tab->cv] = tab->pecas[nl][nc];
    tab->pecas[nl][nc] = 0;
    tab->lv = nl;
    tab->cv = nc;
    return true;
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

int main() {
    Tabuleiro tab;
    int n = ler_n();

    iniciar_tabuleiro(&tab, n);
    imprimir(&tab);

    // Teste rapido de movimento
    if (mover(&tab, CIMA)) {
        printf("\n--- Moveu para cima ---");
        imprimir(&tab);
    }

    return 0;
}

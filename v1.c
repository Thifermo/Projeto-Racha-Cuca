#include<stdio.h>
#include<stdlib.h>
int main(){
int n;
int linhavazia = 0, colunavazia = 0;
printf("Qual o tamanho do tabuleiro?\n");
scanf("%d", &n);
while(n < 2 || n > 6){
printf("Numero invalido insira um numero entre 2 e 6.\n");
scanf("%d", &n);
}
printf("Numero aceito\n");

int tabuleiro[n][n];
for (int i = 0; i < n; i++){
for(int j = 0; j < n; j++){
if (i == n - 1 && j == n - 1) {
    tabuleiro[i][j] = 0;
} else {
    tabuleiro[i][j] = i * n + j + 1;
}
}
}
for (int i = 0; i < n; i++){
for(int j = 0; j < n; j++)
if(tabuleiro[i][j] == 0){
linhavazia = i;
colunavazia = j;

if (linhavazia > 0) {
    printf("Pode movimentar a peca acima\n");
}
if (linhavazia < n - 1) {
    printf("Pode movimentar a peca abaixo\n");
}
if (colunavazia > 0) {
    printf("Pode movimentar a peca a esquerda\n");
}
if (colunavazia < n - 1) {
    printf("Pode movimentar a peca a direita\n");
}
}
}


printf("\n");
for (int i = 0; i < n; i++){
for(int j = 0; j < n; j++)
printf("%3d ", tabuleiro[i][j]);
printf("\n");
}

return 0;
}

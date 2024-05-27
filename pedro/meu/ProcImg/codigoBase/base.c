/*-------------------------------------------------------------------------
 *              UNIFAL - UNIVERSIDADE Federal de Alfenas.
 *                  BACHEARELADO EM CIENCIA DA COMPUTACAO.
 * Trabalho..:  Contagem de feijoes
 * Professor.:  Luiz Eduardo da Silva
 * Aluno.....:  Pedro Henrique Alves Barbosa - 2022.1.08.043
 * Data......:  20/05/2024
 -------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#if defined(_WIN32) || defined(__WIN64__) || defined(__CYGWIN__)
#include "..\\utils\\imagelib.h"
#elif defined(__linux__)
#include "../utils/imagelib.h"
#endif


// Função para calcular o negativo de uma imagem
image neg_pgm(image In)
{
    image Out = img_clone(In);
    for (int i = 0; i < In->nr * In->nc; i++)
        Out->px[i] = In->ml - In->px[i];
    return Out;
}

// Função para aplicar um limiar à imagem de entrada
image intensidade(image In) {
    float Limiar[In->ml + 1];
    image Out = img_clone(In);
    for (int i = 0; i< In->ml + 1; i++) {
        Limiar[i] = i< 160 ? 0 : 1;   
    }
    for (int i= 0; i< In->nr * In->nc; i++)
        Out->px[i] = Limiar[In->px[i]];
    return Out;
}

// Função similar à intensidade, mas com outro limiar
image intensidade2(image In) {
    float Limiar[In->ml + 1];
    image Out = img_clone(In);
    for (int i = 0; i< In->ml + 1; i++) {
        Limiar[i] = i< 6 ? 0 : 1;   
    }
    for (int i= 0; i< In->nr * In->nc; i++)
        Out->px[i] = Limiar[In->px[i]];
    return Out;
}

int find(int parent[], int i) {
    while (parent[i] != i)
        i = parent[i];
    return i;
}

void Union(int parent[], int i, int j) {
    int x = find(parent, i);
    int y = find(parent, j);
    parent[y] = x;
}

// Função para contar o número de rótulos únicos em uma imagem rotulada
int contar_rotulos(image img, int rotulos[]) {
    int cont = 0;
    int rotuloUsado[img->nc * img->nr]; 
    for (int i = 0; i < img->nc * img->nr; i++)
        rotuloUsado[i] = 0;
    for (int i = 0; i < img->nc * img->nr; i++) {
        int rotuloRaiz = find(rotulos, img->px[i]); 
        if (rotuloUsado[rotuloRaiz] == 0 && rotuloRaiz != 0) {
            rotuloUsado[rotuloRaiz] = 1; 
            cont++; 
        }
    }
    return cont;
}

// Função para rotular os componentes conectados em uma imagem binária
void label(image In) {
    int numeroLinhas = In->nr;
    int numeroColunas = In->nc;
    int *pixels = In->px;
    int numRotulo = 0;
    int parent[1000];

    // Inicializa o array parent com índices
    for (int i = 0; i < 1000; i++)
        parent[i] = i;

    // Percorre a imagem para rotular os componentes
    for (int i = 1; i < numeroLinhas; i++) {
        for (int j = 1; j < numeroColunas; j++) {
            int p = pixels[i * numeroColunas + j];
            int r = pixels[(i - 1) * numeroColunas + j];
            int t = pixels[i * numeroColunas + j - 1];
            if (p != 0) {
                if (r == 0 && t == 0)
                    pixels[i * numeroColunas + j] = ++numRotulo;
                if (r != 0 && t == 0)
                    pixels[i * numeroColunas
                     + j] = r;
                if (r == 0 && t != 0)
                    pixels[i * numeroColunas + j] = t;
                if (r != 0 && t != 0 && t == r)
                    pixels[i * numeroColunas + j] = r;
                if (r != 0 && t != 0 && t != r) {
                    pixels[i * numeroColunas + j] = t;
                    Union(parent, r, t);
                }
            }
        }
    }

    // Atualiza os rótulos dos pixels com os seus pais
    for (int i = 0; i < numeroLinhas * numeroColunas; i++)
        In->px[i] = find(parent, In->px[i]);

    // Define a quantidade total de componentes conectados
    In->ml = numRotulo;

    // Imprime o número de componentes conectados
    printf("#componentes= %d\n", contar_rotulos(In, parent));
}

// Função para encontrar o mínimo entre três números
int minimo3(int a, int b, int c) {
    if (a < b && a < c){
        return a;
    } 
    if (b < c){
        return b;
    } 
    return c;
}

// Função para calcular a distância de cada pixel ao fundo da imagem binária
int distancia(image In){
    int maxDist = -1;
    int nLinhas = In->nr;
    int nColunas = In->nc;
    int *pixels = In->px;

    // Calcula a distância da esquerda para a direita e de cima para baixo
    for(int i = 1; i < nLinhas - 1; i++){
        for(int j = 1; j < nColunas - 1; j++){
            int p = pixels[i * nColunas + j];
            int a = pixels[(i - 1) * nColunas + j];
            int b = pixels[i * nColunas + j - 1];
            
            if(p != 0){
                pixels[i * nColunas + j] = (a + 1) < (b + 1) ? (a + 1) : (b + 1);
            }
        }
    }

    // Calcula a distância da direita para a esquerda e de baixo para cima
    for(int i = nLinhas - 2; i > 0;i--){
        for(int j = nColunas - 2; j > 0; j--){
            int p = pixels[i * nColunas + j];
            int a = pixels[i * nColunas + j + 1];
            int b = pixels[(i + 1) * nColunas + j];

            if(p != 0){
                pixels[i * nColunas + j] = minimo3(a + 1,b + 1, p);
                if(maxDist < pixels[i * nColunas + j]){
                    maxDist = pixels[i * nColunas + j];
                }
            }
        }
    }
    return maxDist;
}

// Função para exibir uma mensagem de uso do programa
void exibir_mensagem(char *s) {
    printf("\nContafeijao");
    printf("\n-------------------------------");
    printf("\nUso:  %s  nome-imagem[.pgm] \n\n", s);
    printf("    nome-imagem[.pgm] é o nome do arquivo da imagem \n");
    exit(1);
}

int main(int argc, char *argv[]) {
    char nameIn[100], nameOut[100], comando[110];
    image In, Out;

    // Verifica se o número de argumentos é válido
    if (argc < 2)
        exibir_mensagem(argv[0]);

    // Obtém os nomes dos arquivos de In e Out
    img_name(argv[1], nameIn, nameOut, GRAY, GRAY);

    // Lê a imagem de In
    In = img_get(nameIn, GRAY);

    // Aplica as transformações na imagem
    Out = neg_pgm(In);
    Out = intensidade(Out);
    Out->ml = distancia(Out);
    Out = intensidade2(Out);
    label(Out);

    // Salva a imagem resultante
    img_put(Out, nameOut, GRAY);

    // Exibe a imagem resultante
    sprintf(comando, "%s %s &", VIEW, nameOut);
    system(comando);

    // Libera a memória alocada
    img_free(In);
    img_free(Out);

    return 0;
}
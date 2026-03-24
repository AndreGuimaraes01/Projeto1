#ifndef IMAGE_H
#define IMAGE_H

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

/* Estrutura para armazenar os dados do histograma e estatísticas (Item 4) */
typedef struct {
    int valores[256];      // Frequência de cada nível de cinza (0-255)
    int max_frequencia;    // Maior valor encontrado (usado para escalar o gráfico)
    float media;           // Média de intensidade (Brilho)
    float desvio_padrao;   // Desvio padrão (Contraste)
} Histograma;

/* --- Funções de Carregamento e Conversão (Item 1 e 2) --- */

/* Carrega imagem do disco e retorna uma surface SDL. Retorna NULL em caso de erro. */
SDL_Surface *image_carregar(const char *caminho);

/* Verifica se a surface é colorida (retorna 1) ou já está em cinza (retorna 0) */
int image_is_colorida(SDL_Surface *surface);

/* Converte surface colorida para escala de cinza usando a fórmula de luminância do enunciado. */
SDL_Surface *image_converter_cinza(SDL_Surface *surface);

/* Libera a surface da memória */
void image_destruir(SDL_Surface *surface);


/* --- Suas Funções: Histograma e Equalização (Item 4 e 5) --- */

/* Calcula o histograma, média e desvio padrão de uma imagem em escala de cinza */
Histograma image_calcular_histograma(SDL_Surface *surface);

/* Gera uma nova surface com o contraste melhorado através da equalização de histograma */
SDL_Surface *image_equalizar(SDL_Surface *surface, Histograma h);

#endif

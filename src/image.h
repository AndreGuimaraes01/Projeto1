#ifndef IMAGE_H
#define IMAGE_H

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

/* Carrega imagem do disco e retorna uma surface SDL.
   Retorna NULL em caso de erro. */
SDL_Surface *image_carregar(const char *caminho);

/* Verifica se a surface é colorida (retorna 1) ou já está em cinza (retorna 0) */
int image_is_colorida(SDL_Surface *surface);

/* Converte surface colorida para escala de cinza.
   Retorna nova surface em cinza. O chamador deve destruir com SDL_DestroySurface(). */
SDL_Surface *image_converter_cinza(SDL_Surface *surface);

/* Libera a surface da memória */
void image_destruir(SDL_Surface *surface);

typedef struct {
    int valores[256];      // Frequência de cada nível (0-255)
    int max_frequencia;    // Para escalar o gráfico na janela
    float media;           // Média de intensidade
    float desvio_padrao;   // Para calcular o contraste
} Histograma;

/* Calcula o histograma e estatísticas de uma surface em cinza */
Histograma image_calcular_histograma(SDL_Surface *surface);

#endif

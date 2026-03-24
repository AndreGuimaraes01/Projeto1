
#include "image.h"
#include <stdio.h>

SDL_Surface *image_carregar(const char *caminho) {
    SDL_Surface *surface = IMG_Load(caminho);
    if (!surface) {
        fprintf(stderr, "Erro ao carregar imagem '%s': %s\n", caminho, SDL_GetError());
        return NULL;
    }
    printf("Imagem carregada: %dx%d pixels\n", surface->w, surface->h);
    return surface;
}

int image_is_colorida(SDL_Surface *surface) {
    SDL_PixelFormat fmt = surface->format;
    /* Formatos de escala de cinza têm apenas 1 canal */
    if (fmt == SDL_PIXELFORMAT_INDEX8 ||
        fmt == SDL_PIXELFORMAT_RGB332) {
        return 0;
    }
    /* Verifica amostrando pixels: se R == G == B em todos, é cinza */
    SDL_Surface *conv = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGB24);
    if (!conv) return 1;

    Uint8 *pixels = (Uint8 *)conv->pixels;
    int total = conv->w * conv->h;
    int passo = total / 1000 + 1; /* amostra até 1000 pixels */

    for (int i = 0; i < total; i += passo) {
        Uint8 r = pixels[i * 3 + 0];
        Uint8 g = pixels[i * 3 + 1];
        Uint8 b = pixels[i * 3 + 2];
        if (r != g || g != b) {
            SDL_DestroySurface(conv);
            return 1; /* é colorida */
        }
    }
    SDL_DestroySurface(conv);
    return 0; /* já é cinza */
}

SDL_Surface *image_converter_cinza(SDL_Surface *surface) {
    /* Converte para RGB24 para facilitar acesso aos pixels */
    SDL_Surface *rgb = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGB24);
    if (!rgb) {
        fprintf(stderr, "Erro ao converter surface: %s\n", SDL_GetError());
        return NULL;
    }

    /* Cria surface de saída em RGB24 (vamos pintar R=G=B=Y) */
    SDL_Surface *cinza = SDL_CreateSurface(rgb->w, rgb->h, SDL_PIXELFORMAT_RGB24);
    if (!cinza) {
        SDL_DestroySurface(rgb);
        return NULL;
    }

    Uint8 *src = (Uint8 *)rgb->pixels;
    Uint8 *dst = (Uint8 *)cinza->pixels;

    for (int y = 0; y < rgb->h; y++) {
        for (int x = 0; x < rgb->w; x++) {
            int idx = y * rgb->pitch + x * 3;
            Uint8 r = src[idx + 0];
            Uint8 g = src[idx + 1];
            Uint8 b = src[idx + 2];

            /* Fórmula do enunciado */
            Uint8 Y = (Uint8)(0.2125 * r + 0.7154 * g + 0.0721 * b);

            int odx = y * cinza->pitch + x * 3;
            dst[odx + 0] = Y;
            dst[odx + 1] = Y;
            dst[odx + 2] = Y;
        }
    }

    SDL_DestroySurface(rgb);
    return cinza;
}

void image_destruir(SDL_Surface *surface) {
    if (surface) SDL_DestroySurface(surface);
}

Histograma image_calcular_histograma(SDL_Surface *surface) {
    Histograma h = {0};
    Uint8 *pixels = (Uint8 *)surface->pixels;
    int total_pixels = surface->w * surface->h;

    // 1. Contagem de frequências
    for (int i = 0; i < total_pixels * 3; i += 3) {
        Uint8 tom = pixels[i]; // Como é cinza, R=G=B
        h.valores[tom]++;
        if (h.valores[tom] > h.max_frequencia) h.max_frequencia = h.valores[tom];
    }

    // 2. Média (Intensidade)
    float soma = 0;
    for (int i = 0; i < 256; i++) soma += i * h.valores[i];
    h.media = soma / total_pixels;

    // 3. Desvio Padrão (Contraste)
    float soma_variancia = 0;
    for (int i = 0; i < 256; i++) {
        soma_variancia += h.valores[i] * (i - h.media) * (i - h.media);
    }
    h.desvio_padrao = sqrtf(soma_variancia / total_pixels);

    return h;
}

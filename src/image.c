#include "image.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* 1. Carregamento de imagem [cite: 16, 17, 18] */
SDL_Surface *image_carregar(const char *caminho) {
    SDL_Surface *surface = IMG_Load(caminho);
    if (!surface) {
        fprintf(stderr, "Erro ao carregar imagem '%s': %s\n", caminho, SDL_GetError());
        return NULL;
    }
    printf("Imagem carregada: %dx%d pixels\n", surface->w, surface->h);
    return surface;
}

/* 2. Verificação se a imagem é colorida [cite: 19, 20] */
int image_is_colorida(SDL_Surface *surface) {
    if (!surface) return 0;
    
    SDL_PixelFormat fmt = surface->format;
    
    /* Formatos de escala de cinza nativos têm apenas 1 canal */
    if (fmt == SDL_PIXELFORMAT_INDEX8 || fmt == SDL_PIXELFORMAT_RGB332) {
        return 0;
    }

    /* Converte temporariamente para RGB24 para analisar os pixels */
    SDL_Surface *conv = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGB24);
    if (!conv) return 1;

    Uint8 *pixels = (Uint8 *)conv->pixels;
    int total = conv->w * conv->h;
    int passo = total / 1000 + 1; /* Amostra para performance */

    for (int i = 0; i < total; i += passo) {
        Uint8 r = pixels[i * 3 + 0];
        Uint8 g = pixels[i * 3 + 1];
        Uint8 b = pixels[i * 3 + 2];
        if (r != g || g != b) {
            SDL_DestroySurface(conv);
            return 1; /* É colorida */
        }
    }
    SDL_DestroySurface(conv);
    return 0; /* É cinza */
}

/* 2. Conversão para escala de cinza usando a fórmula do enunciado [cite: 21, 22] */
SDL_Surface *image_converter_cinza(SDL_Surface *surface) {
    if (!surface) return NULL;

    SDL_Surface *rgb = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGB24);
    if (!rgb) return NULL;

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

            /* Fórmula: Y = 0.2125*R + 0.7154*G + 0.0721*B [cite: 21] */
            Uint8 Y = (Uint8)(0.2125 * r + 0.7154 * g + 0.0721 * b);

            int odx = y * cinza->pitch + x * 3;
            dst[odx + 0] = dst[odx + 1] = dst[odx + 2] = Y;
        }
    }

    SDL_DestroySurface(rgb);
    return cinza;
}

/* 4. Análise do histograma (Média e Desvio Padrão) [cite: 28, 29, 30, 31, 32] */
Histograma image_calcular_histograma(SDL_Surface *surface) {
    Histograma h = {0};
    if (!surface) return h;

    Uint8 *pixels = (Uint8 *)surface->pixels;
    
    /* 1. Frequência e Max (com proteção de pitch) [cite: 29] */
    for (int y = 0; y < surface->h; y++) {
        for (int x = 0; x < surface->w; x++) {
            int idx = y * surface->pitch + x * 3;
            Uint8 tom = pixels[idx]; 
            h.valores[tom]++;
            if (h.valores[tom] > h.max_frequencia) {
                h.max_frequencia = h.valores[tom];
            }
        }
    }

    int total_pixels = surface->w * surface->h;

    /* 2. Média de intensidade [cite: 31] */
    double soma = 0;
    for (int i = 0; i < 256; i++) {
        soma += (double)i * h.valores[i];
    }
    h.media = (float)(soma / total_pixels);

    /* 3. Desvio Padrão (Contraste) [cite: 32] */
    double soma_var = 0;
    for (int i = 0; i < 256; i++) {
        double diff = i - h.media;
        soma_var += h.valores[i] * (diff * diff);
    }
    h.desvio_padrao = (float)sqrt(soma_var / total_pixels);

    return h;
}

/* 5. Equalização do histograma [cite: 34, 36] */
SDL_Surface *image_equalizar(SDL_Surface *surface, Histograma h) {
    if (!surface) return NULL;

    SDL_Surface *out = SDL_CreateSurface(surface->w, surface->h, SDL_PIXELFORMAT_RGB24);
    if (!out) return NULL;

    int total_pixels = surface->w * surface->h;
    
    /* Calcular CDF (Função de Distribuição Acumulada) */
    float cdf[256];
    cdf[0] = (float)h.valores[0] / total_pixels;
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i - 1] + ((float)h.valores[i] / total_pixels);
    }

    Uint8 *src = (Uint8 *)surface->pixels;
    Uint8 *dst = (Uint8 *)out->pixels;

    for (int y = 0; y < surface->h; y++) {
        for (int x = 0; x < surface->w; x++) {
            int idx = y * surface->pitch + x * 3;
            int odx = y * out->pitch + x * 3;
            
            /* Mapeia o tom original para o novo tom usando a CDF */
            Uint8 novo_tom = (Uint8)(cdf[src[idx]] * 255);
            dst[odx] = dst[odx + 1] = dst[odx + 2] = novo_tom;
        }
    }
    return out;
}

/* Liberação de memória [cite: 48] */
void image_destruir(SDL_Surface *surface) {
    if (surface) SDL_DestroySurface(surface);
}

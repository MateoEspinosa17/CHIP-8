#ifndef CHIP8_GPU_H
#define CHIP8_GPU_H

#include <SDL2/SDL.h>
#include <stdbool.h>

enum {
    CHIP8_WINDOW_HEIGHT = 32,
    CHIP8_WINDOW_WIDTH  = 64,
    CHIP8_WINDOW_SCALE = 10,
    CHIP8_WINDOW_REFRESH_RATE_MS = 16
};

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    uint32_t pixels[CHIP8_WINDOW_HEIGHT*CHIP8_WINDOW_WIDTH];
} Chip8_Gfx;

// Chip8 GPU functions
bool Chip8_Gfx_Initialize(Chip8_Gfx *);
void Chip8_Gfx_TranslateBuffer(Chip8_Gfx *, uint8_t*);
void Chip8_Gfx_Display(Chip8_Gfx *, uint32_t *, uint8_t);
void Chip8_Gfx_Shutdown(Chip8_Gfx *, uint8_t);

#endif

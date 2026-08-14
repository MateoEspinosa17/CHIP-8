#include "chip8_gpu.h"

bool Chip8_Gfx_Initialize(Chip8_Gfx *gpu) {
    if (SDL_Init(SDL_INIT_VIDEO)){
        SDL_Log("Chip's 8 GFX (SDL) couldn't be initialized.");
        return true;
    }
    
    // If initialization was complete, lets set our variables.
    gpu->window = NULL;
    gpu->renderer = NULL;
    gpu->texture = NULL;
    for (uint16_t i = 0; i < (CHIP8_WINDOW_HEIGHT*CHIP8_WINDOW_WIDTH); i++){ gpu->pixels[i]  = 0x00; }

    // Let's create the window.
    gpu->window = SDL_CreateWindow(
        "CHIP-8 Emulator", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        CHIP8_WINDOW_WIDTH*CHIP8_WINDOW_SCALE, 
        CHIP8_WINDOW_HEIGHT*CHIP8_WINDOW_SCALE, 
        0
    ); 
    if (!gpu->window) {
        SDL_Log("Chip's 8 GFX (SDL) couldn't create window.");
        return true;        
    }

    // Now the renderer.
    gpu->renderer = SDL_CreateRenderer(gpu->window, -1, 0);
    if (!gpu->renderer) {
        SDL_Log("Chip's 8 GFX (SDL) couldn't create renderer");
        return true;        
    }

    // Create a 64x32 texture buffer
    gpu->texture = SDL_CreateTexture(
        gpu->renderer,
        SDL_PIXELFORMAT_RGBA8888, // 32-bit color format (Red, Green, Blue, Alpha)
        SDL_TEXTUREACCESS_STREAMING,
        CHIP8_WINDOW_WIDTH, 
        CHIP8_WINDOW_HEIGHT
    );
    if (!gpu->texture) {
        SDL_Log("Chip's 8 GFX (SDL) couldn't create texture");
        return true;  
    }

    // Set nearest-neighbor scaling for sharp retro edges
    SDL_SetTextureScaleMode(gpu->texture, SDL_ScaleModeNearest);

    return false;   
}   

void Chip8_Gfx_TranslateBuffer(Chip8_Gfx *gpu, uint8_t gfx_from_cpu[]) {
    for (int i = 0; i < CHIP8_WINDOW_WIDTH * CHIP8_WINDOW_HEIGHT; i++) {
        if (gfx_from_cpu[i] == 1) {
            gpu->pixels[i] = 0xFFFFFFFF; // White pixel (RGBA: 255, 255, 255, 255)
        } else {
            gpu->pixels[i] = 0x000000FF; // Black pixel (RGBA: 0, 0, 0, 255)
        }
    }
}


void Chip8_Gfx_Display(Chip8_Gfx *gpu, uint32_t buffer[], uint8_t display_flag) {
    if (display_flag) {
        SDL_RenderClear(gpu->renderer);
        // Upload the 32-bit buffer into the 64x32 texture
        SDL_UpdateTexture(gpu->texture, NULL, buffer, CHIP8_WINDOW_WIDTH * sizeof(uint32_t));
        // Copy texture to full window. NULL arguments stretch it automatically!
        SDL_RenderCopy(gpu->renderer, gpu->texture, NULL, NULL);
        SDL_RenderPresent(gpu->renderer);
        SDL_Delay(CHIP8_WINDOW_REFRESH_RATE_MS);
    }
}

void Chip8_Gfx_Shutdown(Chip8_Gfx *gpu, uint8_t exit_status){
    // Let's cleanup in reverse order from creation.
    SDL_DestroyTexture(gpu->texture);
    SDL_DestroyRenderer(gpu->renderer);
    SDL_DestroyWindow(gpu->window);
    SDL_Quit();
    exit(exit_status);
}
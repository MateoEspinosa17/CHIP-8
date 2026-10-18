#include <stdio.h>
#include "chip8_cpu.h"
#include "chip8_gpu.h"

int main() {
    // 1. INITIALIZE GFX
    Chip8_Gfx gpu;
    if(Chip8_Gfx_Initialize(&gpu)) {
        // If the initialization failed then lets clean up everything.
        Chip8_Gfx_Shutdown(&gpu, EXIT_FAILURE);
    }

    // 2. INITIALIZE CPU
    Chip8_Cpu cpu;
    Chip8_Cpu_Initialize(&cpu);

    // 3. LOAD ROM
    if(Chip8_Cpu_LoadROM(&cpu, "rom/IBM_Logo.ch8")) {
        // If the initialization failed then lets clean up everything.
        Chip8_Gfx_Shutdown(&gpu, EXIT_FAILURE);
    }

    // Main Loop
    for(;;) {
        cpu.main_start = SDL_GetTicks();    // We want to keep the REFRESH_RATE in main loop.
        SDL_Event event;
        
        // 4. GATHER INPUT STATE
        while(SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    Chip8_Gfx_Shutdown(&gpu, EXIT_SUCCESS);
                    break;
                case SDL_KEYUP: 
                    printf("UP %s = 0x%02X\n", SDL_GetKeyName(event.key.keysym.sym), event.key.keysym.scancode);
                    Chip8_Cpu_SetKey(&cpu, event.key.keysym.scancode, KEY_UP);
                    break;
                case SDL_KEYDOWN:
                    printf("DOWN %s = 0x%02X\n", SDL_GetKeyName(event.key.keysym.sym), event.key.keysym.scancode);
                    Chip8_Cpu_SetKey(&cpu, event.key.keysym.scancode, KEY_DOWN);
                    break;
                default:
                    break;
            }    
        }
        
        // 5. EMULATE CYCLE
        for (uint16_t i = 0; i < CHIP8_WINDOW_CYCLES_PER_FRAME; i++) {
            Chip8_Cpu_EmulateCycle(&cpu);
        }

        // 6. UPDATE TIMERS
        // Chip8_Cpu_UpdateTimers();

        // 7. DRAW
        Chip8_Gfx_TranslateBuffer(&gpu, cpu.gfx);
        Chip8_Gfx_Display(&gpu, gpu.pixels, cpu.draw);

        // 8. REFRESH RATE
        uint32_t main_elapsed = SDL_GetTicks() - cpu.main_start;
        if (main_elapsed < CHIP8_WINDOW_REFRESH_RATE_MS) { SDL_Delay(CHIP8_WINDOW_REFRESH_RATE_MS - main_elapsed); }
    }

    Chip8_Gfx_Shutdown(&gpu, EXIT_SUCCESS);

    return 0;
}

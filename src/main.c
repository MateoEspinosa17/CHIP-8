#include <stdio.h>
#include "chip8_cpu.h"
#include "chip8_gpu.h"

int main() {
    // 1. Initialize graphics.
    Chip8_Gfx gpu;
    if(Chip8_Gfx_Initialize(&gpu)) {
        // If the initialization failed then lets clean up everything.
        Chip8_Gfx_Shutdown(&gpu, EXIT_FAILURE);
    }

    // 2. INITIALIZE CPU
    Chip8_Cpu cpu;
    Chip8_Cpu_Initialize(&cpu);

    // 3. INITIALIZE INPUT
    // 4. LOAD ROM
    if(Chip8_Cpu_LoadROM(&cpu, "rom\\IBM_Logo.ch8")) {
        // If the initialization failed then lets clean up everything.
        Chip8_Gfx_Shutdown(&gpu, EXIT_FAILURE);
    }

    // Main Loop
    for(;;) {
        SDL_Event event_test;
        while(SDL_PollEvent(&event_test)) {
            switch (event_test.type) {
                case SDL_QUIT:
                    Chip8_Gfx_Shutdown(&gpu, EXIT_SUCCESS);
                    break;
                
                default:
                    break;
            }    
        }
        Chip8_Cpu_EmulateCycle(&cpu);
        Chip8_Gfx_TranslateBuffer(&gpu, cpu.gfx);
        Chip8_Gfx_Display(&gpu, gpu.pixels, cpu.draw);
        // 3. Check key states
    }

    Chip8_Gfx_Shutdown(&gpu, EXIT_SUCCESS);

    return 0;
}

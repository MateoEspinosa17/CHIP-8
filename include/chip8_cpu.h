#ifndef CHIP8_CPU_H
#define CHIP8_CPU_H

#include <stdint-gcc.h>

// Chip8 constants
enum {
    DATA_REGISTERS                  = 16,
    MEMORY_SIZE                     = 4096,
    STACK_SIZE                      = 12,
    USER_PROGRAM_INITIAL_ADDRESS    = 0x200, // First 512 bytes are for CHIP-8 interpreter itself. Porgrams start at 0x200 (512)
    GFX_BUFFER_SIZE                 = 64*32
};

// Chip8 architecture
typedef struct Chip8_Cpu{
    uint8_t  memory[MEMORY_SIZE];
    uint16_t stack[STACK_SIZE];
    uint8_t  V[DATA_REGISTERS]; // Data registers V0-VF
    uint16_t program_counter;
    uint16_t op_code;
    uint16_t I;                                         // Address register I - 12 bits wide.
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint8_t stack_pointer;
    uint8_t gfx[GFX_BUFFER_SIZE];
    uint8_t draw;
} Chip8_Cpu;


// Chip8 CPU functions
void Chip8_Cpu_Initialize(Chip8_Cpu *);
uint8_t Chip8_Cpu_LoadROM(Chip8_Cpu *, const char*);
void Chip8_Cpu_EmulateCycle(Chip8_Cpu *);



#endif


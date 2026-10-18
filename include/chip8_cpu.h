#ifndef CHIP8_CPU_H
#define CHIP8_CPU_H

#include <stdint-gcc.h>

// Chip8 constants
enum {
    DATA_REGISTERS                  = 16,
    MEMORY_SIZE                     = 4096,
    STACK_SIZE                      = 12,
    USER_PROGRAM_INITIAL_ADDRESS    = 0x200,    // First 512 bytes are for CHIP-8 interpreter itself. Porgrams start at 0x200 (512)
    GFX_BUFFER_SIZE                 = 64*32,
    KEYPAD_0                        = 0x27,
    KEYPAD_1                        = 0x1E,     // Physical scancode from my keypad, seems to be the same from my lpatotp as well.
    KEYPAD_2                        = 0x1F,
    KEYPAD_3                        = 0x20,
    KEYPAD_4                        = 0x21,
    KEYPAD_5                        = 0x22,
    KEYPAD_6                        = 0x23,
    KEYPAD_7                        = 0x24,
    KEYPAD_8                        = 0x25,
    KEYPAD_9                        = 0x26,
    KEYPAD_A                        = 0x04,
    KEYPAD_B                        = 0x05,
    KEYPAD_C                        = 0x06,
    KEYPAD_D                        = 0x07,
    KEYPAD_E                        = 0x08,
    KEYPAD_F                        = 0x09,
    KEYPAD_SIZE                     = 16,
    KEY_UP                          = 0,
    KEY_DOWN                        = 1
};

// Chip8 architecture
typedef struct Chip8_Cpu{
    uint8_t  memory[MEMORY_SIZE];
    uint16_t stack[STACK_SIZE];
    uint8_t  V[DATA_REGISTERS]; // Data registers V0-VF
    uint16_t program_counter;
    uint16_t op_code;
    uint16_t I;                 // Address register I - 12 bits wide.
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint8_t stack_pointer;
    uint8_t gfx[GFX_BUFFER_SIZE];
    uint8_t draw;
    uint8_t keypad[KEYPAD_SIZE];
    uint32_t main_start;
} Chip8_Cpu;


// Chip8 CPU functions
void Chip8_Cpu_Initialize(Chip8_Cpu *);
uint8_t Chip8_Cpu_LoadROM(Chip8_Cpu *, const char*);
void Chip8_Cpu_SetKey(Chip8_Cpu *, uint8_t, uint8_t);
void Chip8_Cpu_EmulateCycle(Chip8_Cpu *);



#endif


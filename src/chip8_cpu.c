#include "Chip8_Cpu.h"
#include <stdio.h>
#include <stdlib.h>
/*
    Function that sets the "BIOS" in the initial state.
*/
void Chip8_Cpu_Initialize(Chip8_Cpu *cpu) {
    // First clear everything.
    cpu->program_counter = USER_PROGRAM_INITIAL_ADDRESS;
    cpu->op_code         = 0x00;
    cpu->I               = 0x00;
    cpu->delay_timer     = 0x00;
    cpu->sound_timer     = 0x00;
    cpu->stack_pointer   = 0x00;

    for (uint8_t i = 0; i < DATA_REGISTERS; i++){ cpu->V[i]     = 0x00; }
    for (uint16_t i = 0; i < MEMORY_SIZE; i++){ cpu->memory[i]  = 0x00; }
    for (uint8_t i = 0; i < STACK_SIZE; i++){ cpu->stack[i]     = 0x00; }
    for (uint16_t i = 0; i < GFX_BUFFER_SIZE; i++){ cpu->gfx[i]  = 0x00; }
}

uint8_t Chip8_Cpu_LoadROM(Chip8_Cpu *cpu, const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("CHIP-8 can't load ROM %s\n", filename);
        return 0x01;
    }

    // Let's check size of incomming file.
    fseek(file, 0, SEEK_END);
    uint16_t rom_size = ftell(file);
    rewind(file);
    
    if (rom_size > (MEMORY_SIZE - USER_PROGRAM_INITIAL_ADDRESS)){
        printf("ROM size (%d bytes)is too big.\n", rom_size);
        fclose(file);
        return 0x01;
    }

    size_t bytes_read = fread(&cpu->memory[USER_PROGRAM_INITIAL_ADDRESS], sizeof(uint8_t), rom_size, file);
    if (bytes_read != rom_size) {
        printf("ROM couldn't be loaded completely. Loaded: %zu from %d total bytes.\n", bytes_read, rom_size);
        fclose(file);
        return 0x01;
    }
    fclose(file);
    
    printf("ROM successfully loaded! Size: %zu bytes starting at 0x%X\n", bytes_read, USER_PROGRAM_INITIAL_ADDRESS);
    return 0x00;    
}

void Chip8_Cpu_EmulateCycle(Chip8_Cpu *cpu) {
    uint16_t address = 0x0000;
    uint8_t register_index = 0x00;
    uint8_t data = 0x00;
    cpu->draw = 0x00;

    // Fetch Opcode
    cpu->op_code = (cpu->memory[cpu->program_counter] << 8) | cpu->memory[cpu->program_counter + 1];
    printf("[Info] Opcode: 0x%X\n", cpu->op_code);
    
    // Decode and Execute Opcode
    switch (cpu->op_code & 0xF000) {    // Usually the first nibble will have the instruction and the remaining 3 addresses or data
        case 0x0000:
            switch (cpu->op_code & 0x00FF) {
                case 0x00E0:    // Clear Display
                    for (uint16_t i = 0; i < GFX_BUFFER_SIZE; i++){ cpu->gfx[i]  = 0x00; }
                    cpu->program_counter += 2;
                    break;

                case 0x00EE:    // Return from subroutine.
                    cpu->program_counter = cpu->stack[cpu->stack_pointer];
                    --cpu->stack_pointer;
                    break;

                default:
                    printf("[Error] Unkown Opcode [0x0000]: 0x%X\n", cpu->op_code);
                    break;
            }
            break;

        case 0x1000:    // Jumps to address.
            // Update program counter with new address.
            address = 0x0FFF & cpu->op_code;
            cpu->program_counter = address;
            break;

        case 0x2000:    // Calls subroutines.
            // Save current program counter address in stack.
            cpu->stack[cpu->stack_pointer] = cpu->program_counter;
            ++cpu->stack_pointer;
            // Update program counter with new address.
            address = 0x0FFF & cpu->op_code;
            cpu->program_counter = address;
            break;

        case 0x6000:    // Set V[2nd nibble] register.
            register_index = (0x0F00 & cpu->op_code) >> 8;
            data = 0x00FF & cpu->op_code;
            // Set data on register.
            cpu->V[register_index] = data;
            // Update program counter.
            cpu->program_counter += 2;
            break;

        case 0x7000:    // Adds last 2 nibbles to register V[2nd nible].
            register_index = (0x0F00 & cpu->op_code) >> 8;
            data = 0x00FF & cpu->op_code;
            // Set data on register.
            cpu->V[register_index] += data;
            // Update program counter.
            cpu->program_counter += 2;
            break;

        case 0xA000:    // Sets I to the address.
            address = 0x0FFF & cpu->op_code;
            // Set I.
            cpu->I = address;
            // Update program counter.
            cpu->program_counter += 2;
            break;

        case 0xD000: {    // Display.
            uint8_t max_lines = (0x000F & cpu->op_code);
            uint8_t x = cpu->V[(0x0F00 & cpu->op_code) >> 8];
            uint8_t y = cpu->V[(0x00F0 & cpu->op_code) >> 4];
            
            for (uint8_t line = 0; line < max_lines; line++) {
                // Get 8bit line from memory address I + line number.
                uint8_t new_line = cpu->memory[cpu->I + line];

                // Get 8bit sprite from gfx buffer at location x, y.
                uint16_t gfx_buffer_index = x + (y * 32) + (line * 32);

                for (int16_t i = 7; i >= 0x00; i--) {
                    // XOR new vs old line to get new value going to buffer. Check collissions.
                    cpu->gfx[gfx_buffer_index] ^= (new_line >> i) & 0x01;
                    if ((cpu->gfx[gfx_buffer_index] == 0x01) && (((new_line >> i) & 0x01) == 0x00)) {
                        cpu->V[0x0F] = 0x01;
                    }
                    gfx_buffer_index ++;
                }
            }
            cpu->draw = 0x01;
            // Update program counter.
            cpu->program_counter += 2;
            break;
        }                             
    }

    // Update Timers
}
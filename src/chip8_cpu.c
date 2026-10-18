#include "Chip8_Cpu.h"
#include <stdio.h>
#include <stdlib.h>
/*
    Function that sets the "BIOS" in the initial state.
*/
void Chip8_Cpu_Initialize(Chip8_Cpu *cpu) {
    // First clear everything.
    cpu->program_counter    = USER_PROGRAM_INITIAL_ADDRESS;
    cpu->op_code            = 0x00;
    cpu->I                  = 0x00;
    cpu->delay_timer        = 0x00;
    cpu->sound_timer        = 0x00;
    cpu->stack_pointer      = 0x00;
    cpu->draw               = 0x00;
    cpu->main_start         = 0x00000000;

    for (uint8_t i = 0; i < DATA_REGISTERS; i++){ cpu->V[i]     = 0x00; }
    for (uint16_t i = 0; i < MEMORY_SIZE; i++){ cpu->memory[i]  = 0x00; }
    for (uint8_t i = 0; i < STACK_SIZE; i++){ cpu->stack[i]     = 0x00; }
    for (uint16_t i = 0; i < GFX_BUFFER_SIZE; i++){ cpu->gfx[i]  = 0x00; }
    for (uint16_t i = 0; i < KEYPAD_SIZE; i++){ cpu->keypad[i]  = 0x00; }
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
        printf("ROM size (%d bytes) is too big.\n", rom_size);
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

void Chip8_Cpu_SetKey(Chip8_Cpu *cpu, uint8_t scan_code, uint8_t value)
{
    uint8_t index = 0;
    switch (scan_code)
    {
        case KEYPAD_0: index =  0; break;
        case KEYPAD_1: index =  1; break;
        case KEYPAD_2: index =  2; break;
        case KEYPAD_3: index =  3; break;
        case KEYPAD_4: index =  4; break;
        case KEYPAD_5: index =  5; break;
        case KEYPAD_6: index =  6; break;
        case KEYPAD_7: index =  7; break;
        case KEYPAD_8: index =  8; break;
        case KEYPAD_9: index =  9; break;
        case KEYPAD_A: index = 10; break;
        case KEYPAD_B: index = 11; break;
        case KEYPAD_C: index = 12; break;   
        case KEYPAD_D: index = 13; break;
        case KEYPAD_E: index = 14; break;
        case KEYPAD_F: index = 15; break;       
        default:
            printf("[Info] Scan Code 0x%02X not supported.\n", scan_code);
            break;
    }

    cpu->keypad[index] = value;
}

void Chip8_Cpu_EmulateCycle(Chip8_Cpu *cpu) {
    uint16_t address = 0x0000;
    uint8_t register_index = 0x00;
    uint8_t data = 0x00;
    cpu->draw = 0x00;

    // Fetch Opcode
    cpu->op_code = (cpu->memory[cpu->program_counter] << 8) | cpu->memory[cpu->program_counter + 1];
    
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
                    printf("[Error] Unkown Opcode: 0x%X\n", cpu->op_code);
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

        case 0x3000: { // Skip if Vx == nn
            register_index = (0x0F00 & cpu->op_code) >> 8;
            uint8_t nn = 0x00FF & cpu->op_code;
            
            if (cpu->V[register_index] == nn) { cpu->program_counter += 4; } // Add 4 to skip next instruction.
            else { cpu->program_counter += 2; } // Cond. not met, go to next instruction.
            break;
        }
        
        case 0x4000: { // Skip if Vx != nn
            register_index = (0x0F00 & cpu->op_code) >> 8;
            uint8_t nn = 0x00FF & cpu->op_code;
            
            if (cpu->V[register_index] == nn) { cpu->program_counter += 4; } // Add 4 to skip next instruction.
            else { cpu->program_counter += 2; } // Cond. not met, go to next instruction.
            break;
        }

        case 0x5000: // Skip if Vx == Vy
            register_index = (0x0FF0 & cpu->op_code) >> 4; // register_index = 0xXY;
            
            if (cpu->V[register_index >> 4] == cpu->V[(register_index & 0x0F)]) { cpu->program_counter += 4; } // Add 4 to skip next instruction.
            else { cpu->program_counter += 2; } // Cond. not met, go to next instruction.
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

        case 0x8000: {
            uint8_t x = (cpu->op_code & 0x0F00) >> 8;
            uint8_t y = (cpu->op_code & 0x00F0) >> 4;
            switch (cpu->op_code & 0x000F){
                case 0x0000: //Set Vx to Vy
                    cpu->V[x] = cpu->V[y];
                    break;
                case 0x0001: // Set Vx |= Vy
                    cpu->V[x] |= cpu->V[y];
                    break;
                case 0x0002: // Set Vx &= Vy
                    cpu->V[x] &= cpu->V[y];
                    break;
                case 0x0003: // Set Vx ^= Vy
                    cpu->V[x] ^= cpu->V[y];
                    break;
                case 0x0004: // Set Vx += Vy
                    cpu->V[x] += cpu->V[y];
                    break;
                case 0x0005: // Set Vx -= Vy
                    cpu->V[x] -= cpu->V[y];
                    break;
                case 0x0006: // Shift Vx >>= 1, store least significant bit of Vx in Vf
                    cpu->V[0xF] = cpu->V[x] & 0x01;
                    cpu->V[x] >>= 1;
                    break;
                case 0x0007: // Set Vx = Vy - Vx, Vf is set if there is no underflow, and 0 when there is (Vy > Vx)
                    cpu->V[x] = cpu->V[y] - cpu->V[x];
                    if (cpu->V[y] >= cpu->V[x]) { cpu->V[0xF] = 1; }
                    break;
                case 0x000E:// Shift Vx <<= 1, store most significant bit of Vx in Vf
                    cpu->V[0xF] = (cpu->V[x] >> 7) & 0x01;
                    cpu->V[x] <<= 1;
                    break;
                default:
                    printf("[Error] Unkown Opcode: 0x%X\n", cpu->op_code);
                    break;
            }
            cpu->program_counter += 2;
            break;
        }

        case 0x9000: {    // Skips if Vx != Vy
            uint8_t x = (cpu->op_code & 0x0F00) >> 8;
            uint8_t y = (cpu->op_code & 0x00F0) >> 4;
            
            if (cpu->V[x] != cpu->V[y]) { cpu->program_counter += 4; }
            else { cpu->program_counter += 2; }
            break;
        }

        case 0xA000:    // Sets I to the address.
            address = 0x0FFF & cpu->op_code;
            // Set I.
            cpu->I = address;
            // Update program counter.
            cpu->program_counter += 2;
            break;

        case 0xB000: // Jump to adress V0 + nnn
            cpu->program_counter = cpu->V[0x0] + (cpu->op_code & 0x0FFF);
            break;

        case 0xC000: // Set Vx = nn & rand()
            register_index = (cpu->op_code & 0x0F00) >> 8;
            uint8_t nn = cpu->op_code & 0x00FF;
            cpu->V[register_index] = (uint8_t)rand() & nn;
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
        
        case 0xE000:   
            switch (cpu->op_code & 0x00FF)
            {
                case 0x009E: //(key() == Vx) -> V[x] = 1
                    if (cpu->keypad[cpu->V[(cpu->op_code & 0x0F00) >> 8]]) { cpu->program_counter += 4; }
                    else { cpu->program_counter += 2; }
                    break;

                case 0x00A1: //(key() != Vx) -> V[x] = 0
                    if (!cpu->keypad[cpu->V[(cpu->op_code & 0x0F00) >> 8]]) { cpu->program_counter += 4; }
                    else { cpu->program_counter += 2; }
                    break;

                default:
                    printf("[Error] Unkown Opcode: 0x%X\n", cpu->op_code);
                    break;
            }
            break;

        case 0xF000: {
            uint8_t x = (cpu->op_code & 0x0F00) >> 8;
            switch (cpu->op_code & 0x00FF)
            {
                case 0x0007: 
                    cpu->V[x] = cpu->delay_timer;
                    break;
                case 0x000A: {
                    uint8_t is_key_pressed = 0;
                    uint8_t i = 0;
                    do {
                        if (cpu->keypad[i]) {
                            is_key_pressed = 1;
                            cpu->V[x] = i;
                            cpu->program_counter += 2;
                        }
                        i++;
                    } while(!is_key_pressed && (i < KEYPAD_SIZE));
                    break;
                }
                case 0x0015:
                    cpu->delay_timer = cpu->V[x];
                    break;
                case 0x0018:
                    cpu->sound_timer = cpu->V[x];
                    break;
                case 0x001E:
                    cpu->I += cpu->V[x];
                    break;
                case 0x0029:
                    break;
                case 0x0033:
                    break;
                case 0x0055:
                    break;
                case 0x0065:
                    break;
            }
            break;
        }
    }
}
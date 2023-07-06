#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "chip-8.h"
#include <time.h>
#define FONT_ARRAY_SIZE 80



void show_bytes(byte_pointer start, int len) {
    int i;
    for (i = 0; i < len; i++)
        printf(" %.2x", start[i]);
    printf("\n");
}


void show_int(int x) {
    show_bytes((byte_pointer) &x, sizeof(uint16_t));
}

void register_dump(CHIP_8 * chip8) {
  for(int i = 0; i < 16; i++) {
    printf("\nRegister: %d, Value: %d\n", i, chip8->registers[i]);
  }
}
void memory_dump(CHIP_8 * chip8, uint8_t x) {
  for(int i = x; i < 4096; i++) {
    printf("Memory location: %d, Value: %d\n", i, chip8->memory[i + chip8->index_register]);
  }
}

void keypad_dump(CHIP_8 * chip8) {
  for(int i = 0; i < 16; i++) {
    printf("Keypad[%d]: %d\n", i, chip8->keypad[i]);
  }
}
void execution(CHIP_8 * chip8) {
  printf("Currently executing: 0x%04x\n", chip8->opcode & 0xFFFF);
  
}
// Addresses where the font starts and ends in our emulated memory
const uint8_t FONT_START_ADDRESS = 0x00;
uint8_t font[FONT_ARRAY_SIZE] = {
0xF0, 0x90, 0x90, 0x90, 0xF0, 
0x20, 0x60, 0x20, 0x20, 0x70, 
0xF0, 0x10, 0xF0, 0x80, 0xF0,
0xF0, 0x10, 0xF0, 0x10, 0xF0,
0x90, 0x90, 0xF0, 0x10, 0x10,
0xF0, 0x80, 0xF0, 0x10, 0xF0,
0xF0, 0x80, 0xF0, 0x90, 0xF0,
0xF0, 0x10, 0x20, 0x40, 0x40,
0xF0, 0x90, 0xF0, 0x90, 0xF0, 
0xF0, 0x90, 0xF0, 0x10, 0xF0, 
0xF0, 0x90, 0xF0, 0x90, 0x90, 
0xE0, 0x90, 0xE0, 0x90, 0xE0, 
0xF0, 0x80, 0x80, 0x80, 0xF0, 
0xE0, 0x90, 0x90, 0x90, 0xE0, 
0xF0, 0x80, 0xF0, 0x80, 0xF0,  
0xF0, 0x80, 0xF0, 0x80, 0x80 };

// Function to load the font into memory 
void load_font(CHIP_8 *chip8) {
  for (int i = 0; i < FONT_ARRAY_SIZE; i++) {
    chip8->memory[i + FONT_START_ADDRESS] = font[i];
  }
}

// Zero out the registers
void init_registers(CHIP_8 *chip8) {
  for(uint8_t i = 0; i < 16; i++) {
    chip8->registers[i] = 0;
  }
}
// Zero out the memory 
void init_memory(CHIP_8 *chip8) {
  for(uint16_t i = 0; i < 4096 ; i++) {
    chip8->memory[i] = 0;
  }
}
void opcode_1NNN(CHIP_8 *chip8, uint16_t nnn) {
  chip8->pc = nnn;
}

void opcode_2NNN(CHIP_8 * chip8, uint16_t nnn) {
  // Push the current PC to the stack
  
  chip8->stack[chip8->stack_ptr] = chip8->pc;
  // Increase the size of the stack 
  chip8->stack_ptr += 1;
  // Then PC is set to nnn 
  chip8->pc = nnn; 
}

void opcode_3XKK(CHIP_8 * chip8, uint8_t x, uint8_t nn) {
  // If the value in v[x] is equal to nn, skip one instruction
  if(chip8->registers[x] == nn) 
    chip8->pc += 2;
}

void opcode_4XKK(CHIP_8 * chip8, uint8_t x, uint8_t nn) {
  // If the value in v[x] is not equal to nn, skip one instruction
  if(chip8->registers[x] != nn) 
    chip8->pc += 2;
}

void opcode_5XY0(CHIP_8 *chip8, uint8_t x, uint8_t y) {
  // Skip an instruction if v[x] == v[y]
  if(chip8->registers[x] == chip8->registers[y]) {
    chip8->pc += 2;
  }
}

void opcode_6XNN(CHIP_8 *chip8, uint8_t x, uint8_t nn) {
    chip8->registers[x] = nn;
}

void opcode_7XNN(CHIP_8 *chip8, uint8_t x, uint8_t nn) {
    // Add the value to the address
    chip8->registers[x] += nn;
}

void opcode_9XY0(CHIP_8 *chip8, uint8_t x, uint8_t y) {
  // Skip an instruction if v[x] != v[y]
  if(chip8->registers[x] != chip8->registers[y]) {
    chip8->pc += 2;
  }
}

void opcode_ANNN(CHIP_8 *chip8, uint16_t nnn) {
  chip8->index_register = nnn;
}

void opcode_BNNN(CHIP_8 *chip8, uint16_t nnn) {
  // Jump to address nnn + v[0]
  chip8->pc = nnn + chip8->registers[0];
}
void opcode_CXKK(CHIP_8 *chip8, uint8_t x, uint8_t nn) {
  
  // Generate random number between 0 and 255 
  uint8_t num = (rand() % 0xFF);
  // Binary AND the number with nn 
  // Put the result in v[x]
  chip8->registers[x] = num & nn;
}


  void opcode_DXYN(CHIP_8 *chip8, uint8_t x, uint8_t y, uint8_t n) {
    // TODO:
    // Currently clipping is inconsistent or wrong, as told to us by CHIP-8
    // Reset the flag register, which tells us whether pixels were changed
    chip8->registers[0xF] = 0; 
    // Get the x and y coordinates that the sprite will be drawn at 
    uint8_t x_coord = chip8->registers[x] % 64;
    uint8_t y_coord = chip8->registers[y] % 32;
    // Go through each of the rows of pixels 
    for(int y = 0; y < n; y++) {
      // Go through each of the pixels in that row 
      for(int x = 0; x < 8; x++) {
        
        uint8_t current_byte = chip8->memory[y + chip8->index_register];
        uint8_t current_pixel = current_byte & (0x80 >> x);
        // Get what is at display of 
        uint32_t* screen_pixel = &chip8->display[(y_coord + y) * SCREEN_WIDTH + (x_coord + x)];
        
        if(current_pixel) {
          if (*screen_pixel == 0xFFFFFFFF)
            chip8->registers[0xF] = 1;
          *screen_pixel ^= 0xFFFFFFFF;
        }
         
      }
    }
  chip8->drawFlag = true;
  }

// void opcode_DXYN(CHIP_8 *chip8, uint8_t x, uint8_t y, uint8_t n) {
//   // GOAL: Draw a n N pixels tall sprite at the memory location index_register holds given, 
//   // Get what is in register VX
//   chip8->registers[0xF] = 0;
//   uint8_t xPos = chip8->registers[x] % SCREEN_WIDTH;
//   uint8_t yPos = chip8->registers[y] % SCREEN_HEIGHT;
//   uint8_t original_x_pos = xPos;
//   for(int row = 0; row < n; row++) {
//     // Get the pixel at memory location Index register + y
//     uint8_t current_byte = chip8->memory[chip8->index_register + row];
//
//     for(int column = 0; column < 8; column++) {
//       uint8_t current_pixel = current_byte & (0x80u >> column);
//       uint32_t* screenPixel = &chip8->display[(yPos + row) * SCREEN_WIDTH + (xPos + column)];
//       if(current_pixel)
//       {
//         if(*screenPixel == 0xFFFFFFFF) {
//           chip8->registers[0xF] = 1;
//         }
//         *screenPixel ^= 0xFFFFFFFF;
//       }
//     }
//   }
// }

void opcode_00EE(CHIP_8 * chip8) {
  // Pop the last address from the stack 
  chip8->stack_ptr -= 1;
  chip8->pc = chip8->stack[chip8->stack_ptr];
  
  // Decrement the stack pointer 
}

// Clear the screen - DONE not tested
void opcode_00E0(CHIP_8 *chip8) {
  // Turn off all pixels in the display array
  for(int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
    chip8->display[i] = 0;
  }
}

// Jump instruction - DONE, not tested

/* Logical and arithemtic instructions */

// Set VX's value to be that of VY */
void opcode_8XY0(CHIP_8 *chip8, uint8_t x, uint8_t y) {
  chip8->registers[x] = chip8->registers[y];
}

// Set VX to the logical OR of VX and VY */
void opcode_8XY1(CHIP_8 *chip8, uint8_t x, uint8_t y) {
  chip8->registers[x] = chip8->registers[x] | chip8->registers[y];
}

void opcode_8XY2(CHIP_8 *chip8, uint8_t x, uint8_t y) {
  chip8->registers[x] = chip8->registers[x] & chip8->registers[y];
}

void opcode_8XY3(CHIP_8 *chip8, uint8_t x, uint8_t y) {
  chip8->registers[x] = chip8->registers[x] ^ chip8->registers[y];
}

void opcode_8XY4(CHIP_8 *chip8, uint8_t x, uint8_t y) {
  // Set VX to be the value of VX plus VY
  // Set the carry flag if the result is larger than 255
  uint16_t result = chip8->registers[x] + chip8->registers[y];
  chip8->registers[x] = result & 0xFFu; 
  if(result > 255)
    chip8->registers[0xF] = 1;
  else
    chip8->registers[0xF] = 0; 
  
}

void opcode_8XY5(CHIP_8 *chip8, uint8_t x, uint8_t y) {
  uint8_t carry = chip8->registers[y] <= chip8->registers[x];
  chip8->registers[x] -= chip8->registers[y];
  chip8->registers[0xF] = carry;
}

void opcode_8XY6(CHIP_8 *chip8, uint8_t x, uint8_t y) {
  // Get the least significant bit of v[x]
  // Store v[y] in v[x] 
  
  uint8_t lsb = chip8->registers[x] & 0x01;
  // Set the flag register
  
  if(lsb == 1)
    chip8->registers[0xF] = 1; 
  else
    chip8->registers[0xF] = 0;
  // Then divide by two 
  chip8->registers[x] = chip8->registers[x] >> 1;
  
}

void opcode_8XY7(CHIP_8 *chip8, int8_t x, int8_t y) {
  chip8->registers[x] = chip8->registers[y] - chip8->registers[x];
  if(chip8->registers[y] > chip8->registers[x])
    chip8->registers[0xF] = 1;
  else
    chip8->registers[0xF] = 0; 
  
}

void opcode_8XYE(CHIP_8 *chip8, uint8_t x, uint8_t y) {
  // TODO 
  // Get the bit that will be shifted out 
  uint8_t msb = (chip8->registers[x] & 0xF0u) >> 7u;
  // Set the flag register
  chip8->registers[x] <<= 1;
  chip8->registers[0xF] = msb; 
  
  // Then shift(multiply by two)
}

void opcode_EX9E(CHIP_8 *chip8, uint8_t x) {
  // If keypad[x] is currently down, skip the next instruction 
  if(chip8->keypad[chip8->registers[x]] == 1) {
    chip8->pc += 2;
  }
}

void opcode_EXA1(CHIP_8 *chip8, uint8_t x) {
  // If keypad[x] is currently up, skip the next instruction 
  if(chip8->keypad[chip8->registers[x]] == 0) {
    chip8->pc += 2;
  }
}

void opcode_FX07(CHIP_8 *chip8, uint8_t x) {
  chip8->registers[x] = chip8->delay_timer;
}

void opcode_FX15(CHIP_8 *chip8, uint8_t x) {
  chip8->delay_timer = chip8->registers[x];
}

void opcode_FX18(CHIP_8 *chip8, uint8_t x) {
  chip8->sound_timer = chip8->registers[x];
}

void opcode_FX1E(CHIP_8 *chip8, uint8_t x) {
  chip8->index_register += chip8->registers[x];
}

void opcode_FX0A(CHIP_8 *chip8, uint8_t x) {
 bool keyfound = false;
  for(int i = 0; i < 16; i++) {
    if(chip8->keypad[i] == 1) {
      keyfound = true;
      break;
    }
  }
  if(keyfound == false) {
   chip8->pc -= 2;
  }
}

void opcode_FX29(CHIP_8 *chip8, uint8_t x) {
  // Set the index register to the address of the corresponding sprite of V[x] 
  chip8->index_register = FONT_START_ADDRESS + (chip8->registers[x] * 5);
}

void opcode_FX33(CHIP_8 *chip8, uint8_t x) {
  uint8_t hundreds = (chip8->registers[x] / 100) % 10;
  uint8_t tens = (chip8->registers[x] / 10) % 10;
  uint8_t ones = (chip8->registers[x] / 1) % 10;
  chip8->memory[chip8->index_register] = hundreds;
  chip8->memory[chip8->index_register + 1] = tens;
  chip8->memory[chip8->index_register + 2] = ones;
}

void opcode_FX55(CHIP_8 *chip8, uint8_t x) {
  for(uint8_t i = 0; i <= x; i++) {
    chip8->memory[chip8->index_register + i] = chip8->registers[i];
  }
}

void opcode_FX65(CHIP_8 *chip8, uint8_t x) {
  for(uint8_t i = 0; i <= x; i++) {
    chip8->registers[i] = chip8->memory[chip8->index_register + i];
  }
}

// Function to initialize the CHIP-8
int initialize(CHIP_8 *chip8, char* filename) {
  // Load the font into memory
  // Initialize the registers to be zeroed out
  init_memory(chip8);
  load_font(chip8);
  init_registers(chip8);
  

  for(int i = 0; i < 16; i++) {
    chip8->keypad[i] = 0;
  }
  // TODO: Read in the ROM file in bytes
  FILE* file = fopen(filename, "rb");
  if(file == NULL) {
    printf("File not found!");
    return -1;
  }
  fseek(file, 0, SEEK_END);
  long filelen = ftell(file);
  fseek(file, 0, SEEK_SET);
  uint16_t start_address = 0x200;
  fread(chip8->memory + start_address, sizeof(uint16_t), filelen, file);
  fclose(file);
  return 0;
}

void emulate_cycle(CHIP_8 *chip8) {
  chip8->opcode = chip8->memory[chip8->pc + 0];
  chip8->opcode <<= 8;
  chip8->opcode |= chip8->memory[chip8->pc + 1];
  chip8->pc += 2;
  // Second nibble, used to  x, nnook up a register
  uint8_t x = (chip8->opcode & 0x0F00u) >> 8u;
  // Third nibble, used to up a register
  uint8_t y = (chip8->opcode & 0x00F0u) >> 4u;
  // Last nibble, a 4 bit number
  uint8_t n = (chip8->opcode & 0x000Fu);
  // Second byte, 8-bit immediate number
  uint8_t nn = (chip8->opcode & 0x00FFu);
  // Second, third, and fourth nibbles, 12-bit immediate memory address
  uint16_t nnn = (chip8->opcode & 0xFFFu);

  switch(chip8->opcode & 0xF000) 
  {
    
    // Here is the case where the opcode is entirely unique
    case 0x1000:
      opcode_1NNN(chip8, nnn);
      break; 
    case 0x2000:
      opcode_2NNN(chip8, nnn);
      break; 
    case 0x3000:
      opcode_3XKK(chip8, x, nn);
      break;
    case 0x4000:
      opcode_4XKK(chip8, x, nn);
      break;
    case 0x5000:
      opcode_5XY0(chip8, x, y);
      break;
    case 0x6000:
      opcode_6XNN(chip8, x, nn);
      break;
    case 0x7000:
      opcode_7XNN(chip8, x, nn);
      break;
    case 0x9000:
      opcode_9XY0(chip8, x, y);
      break;
    case 0xA000:
      opcode_ANNN(chip8, nnn);
      break;
    case 0xB000:
      opcode_BNNN(chip8, nnn);
      break;
    case 0xC000:
      opcode_CXKK(chip8, x, nn);
      break;
    case 0xD000:
      opcode_DXYN(chip8, x, y, n); 
      break;

    // Here is the case where the opcode has the first three digits 00E, and the last either 0 or E
    case 0x0000:
      switch(chip8->opcode & 0x00FF) {
        case 0x00EE:
          opcode_00EE(chip8);
          break;
        case 0x00E0:
          opcode_00E0(chip8);
          break;
        default:
          break; 
      } break;
    
    // Here is the case where the first digit is 8 but the last digit is unique
    case 0x8000:
      switch(chip8->opcode & 0xF00F) {
        case 0x8000:
          opcode_8XY0(chip8, x, y);
          break;
        case 0x8001:
          opcode_8XY1(chip8, x, y);
          break;
        case 0x8002:
          opcode_8XY2(chip8, x, y);
          break;
        case 0x8003:
          opcode_8XY3(chip8, x, y);
          break;
        case 0x8004:
          opcode_8XY4(chip8, x, y);
          break;
        case 0x8005:
          opcode_8XY5(chip8, x, y);
          break;
        case 0x8006:
          opcode_8XY6(chip8, x, y);
          break;
        case 0x8007:
          opcode_8XY7(chip8, x, y);
          break;
        case 0x800E:
          opcode_8XYE(chip8, x, y);
          break;
    } break; 

    
    // Here is the case where the first digit repeats but the last two are unique 
    case 0xE000:
      switch(chip8->opcode & 0xF0FF) {
        case 0xE09E: 
          opcode_EX9E(chip8, x);
          break;
        case 0xE0A1:
          opcode_EXA1(chip8, x);
          break;
    } break;

    case 0xF000:
      switch(chip8->opcode & 0xF0FF) {
        case 0xF00A:
          opcode_FX0A(chip8, x);
          break;
        case 0xF007:
          opcode_FX07(chip8, x);
          break;
        case 0xF015:
          opcode_FX15(chip8, x);
          break;
        case 0xF018:
          opcode_FX18(chip8, x);
          break;
        case 0xF01E:
          opcode_FX1E(chip8, x); 
          break;
        case 0xF029:
          opcode_FX29(chip8, x);
          break;
        case 0xF033:
          opcode_FX33(chip8, x); 
          break;
        case 0xF055:
          opcode_FX55(chip8, x);
          break;
        case 0xF065:
          opcode_FX65(chip8, x);
          break;
      } break;
    default:
      printf("Unknown opcode: ");
      show_int(chip8->opcode);
      break;
  }
}

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "chip-8.h"
#include <time.h>
#define FONT_ARRAY_SIZE 80
// The available memory to programs

// Addresses where the font starts and ends in our emulated memory
const uint8_t FONT_START_ADDRESS = 0x50;
const uint8_t FONT_END_ADDRESS = 0x9F;
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
  uint8_t start = FONT_START_ADDRESS;
  for (uint8_t i = 0; i < FONT_ARRAY_SIZE; i++) {
    chip8->memory[start] = font[i];
    start += 1;
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

// Clear the screen - DONE not tested
void opcode_00E0(CHIP_8 *chip8) {
  // Turn off all pixels in the display array
  for(int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
    chip8->display[i] = chip8->display[i] & 0x00;
  }
  chip8->drawFlag = true;
};

// Jump instruction - DONE, not tested
void opcode_1NNN(CHIP_8 *chip8) {
  chip8->pc = chip8->opcode & 0x0FFF;
};

// Set register VX - DONE, not tested
void opcode_6XNN(CHIP_8 *chip8) {
    uint16_t reg_buffer = chip8->opcode & 0x0F00;
    reg_buffer = reg_buffer >> 8; 
    chip8->registers[reg_buffer] = chip8->opcode & 0x00FF;
};

// Add value to register VX - DONE, not tested
void opcode_7XNN(CHIP_8 *chip8) {
  // Store the number containing the third byte in a buffer;
    uint16_t reg_buffer = chip8->opcode & 0x0F00;
  // Shift right by 8 bits to make it a valid address;
    reg_buffer = reg_buffer >> 8; 
    // Get the value that should be at that address, as a 16 bit integer
    // Cast it to be an 8-bit integer
    uint8_t value = (uint8_t)(chip8->opcode & 0x00FF);
    // Add the value to the address
    chip8->registers[reg_buffer] += value;
};

// Set index register I - DONE, not tested;
void opcode_ANNN(CHIP_8 *chip8) {
  chip8->index_register = chip8->opcode & 0x0FFF;
};



// Display/draw 
// THIS IS NOT MY OWN CODE
void opcode_DXYN(CHIP_8 *chip8, uint8_t x, uint8_t y, uint8_t n) {
  // GOAL: Draw a n N pixels tall sprite at the memory location index_register holds given, 
  // Get what is in register VX
  chip8->registers[0xF] = 0;
  uint8_t xPos = chip8->registers[x] % SCREEN_WIDTH;
  uint8_t yPos = chip8->registers[y] % SCREEN_HEIGHT;
  for(int row = 0; row < n; row++) {
    // Get the pixel at memory location Index register + y
    uint8_t current_byte = chip8->memory[chip8->index_register + row];

    for(int column = 0; column < 8; column++) {
      uint8_t current_pixel = current_byte & (0x80u >> column);
      uint32_t* screenPixel = &chip8->display[(yPos + row) * SCREEN_WIDTH + (xPos + column)];
      if(current_pixel)
      {
        if(*screenPixel == 0xFF) {
          chip8->registers[0xF] = 1;
        }
        *screenPixel ^= 0xFF;
      }
    }
  }
  chip8->drawFlag = true;
};

void opcode_2NNN(CHIP_8 * chip8, uint16_t nnn) {
  // CALL the subroutine at memory location NNN
  // increment the stack pointer
  chip8->stack_ptr += 1;
  // Then put the current program counter at the top of the stack 
  chip8->stack[chip8->stack_ptr] = chip8->pc;
  // Then PC is set to nnn 
  chip8->pc = nnn; 
}

void opcode_00EE(CHIP_8 * chip8) {
  // Return from the subroutine 
  // Remove the last address from the stack
  
  // Get the address of the last element in the stack
  uint16_t last_address = chip8->stack[chip8->stack_ptr];
  // Eliminate the value 
  chip8->stack[chip8->stack_ptr] = 0;
  // Decrement the stack pointer 
  chip8->stack_ptr -= 1;
}
// Conditional branch instructions 
// TODO: Edit function headers
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

void opcode_9XY0(CHIP_8 *chip8, uint8_t x, uint8_t y) {
  // Skip an instruction if v[x] != v[y]
  if(chip8->registers[x] != chip8->registers[y]) {
    chip8->pc += 2;
  }
}

/* Logical and arithemtic instructions */

// Set VX's value to be that of VY */
void opcode_8XY0(CHIP_8 *chip8, uint8_t x, uint8_t y) {
  chip8->registers[y] = chip8->registers[x];
}

// Set VX to the logical OR of VX and VY */
void opcode_8XY1(CHIP_8 *chip8, uint8_t x, uint8_t y) {
  chip8->registers[x] = x | y;
}

void opcode_8XY2(CHIP_8 *chip8, uint8_t x, uint8_t y) {
  chip8->registers[x] = x & y;
}

void opcode_8XY3(CHIP_8 *chip8, uint8_t x, uint8_t y) {
  chip8->registers[x] = x ^ y;
}

void opcode_8XY4(CHIP_8 *chip8, uint8_t x, uint8_t y) {
  // Set VX to be the value of VX plus VY
  // Set the carry flag if the result is larger than 255
  uint16_t result = chip8->registers[x] + chip8->registers[y];
  if(result > 255)
    chip8->registers[0xF] = 1;
  chip8->registers[x] = chip8->registers[x] + chip8->registers[y];
}

void opcode_8XY5(CHIP_8 *chip8, uint8_t x, uint8_t y) {
  if(chip8->registers[x] > chip8->registers[y])
    chip8->registers[0xF] = 1;
  else  
    chip8->registers[0xF] = 0; 
  chip8->registers[x] = chip8->registers[x] - chip8->registers[y];
}

void opcode_8XY6(CHIP_8 *chip8, uint8_t x, uint8_t y) {
  // Get the least significant bit of v[x]
  uint8_t lsb = chip8->registers[x] & 0x01;
  // Set the flag register
  if(lsb == 1)
    chip8->registers[0xF] = 1; 
  else
    chip8->registers[0xF] = 0;
  // Then divide by two 
  chip8->registers[x] = chip8->registers[x] >> 1;
}

void opcode_8XY7(CHIP_8 *chip8, uint8_t x, uint8_t y) {
  if(chip8->registers[y] > chip8->registers[x])
    chip8->registers[0xF] = 1;
  else
    chip8->registers[0xF] = 0; 
  chip8->registers[x] = chip8->registers[y] - chip8->registers[x];
}

void opcode_8XYE(CHIP_8 *chip8, uint8_t x, uint8_t y) {
  // Get the bit that will be shifted out 
  uint8_t lsb = chip8->registers[x] & 0x01;
  // Set the flag register
  if(lsb == 1)
    chip8->registers[0xF] = 1; 
  else
    chip8->registers[0xF] = 0;
  // Then shift(multiply by two)
  chip8->registers[x] = chip8->registers[x] << 1;
}

void opcode_BNNN(CHIP_8 *chip8, uint16_t nnn) {
  // Jump to address nnn + v[0]
  chip8->pc = nnn + chip8->registers[0];
}

void opcode_CXKK(CHIP_8 *chip8, uint8_t x, uint8_t nn) {
  srand(time(NULL));
  // Generate random number between 0 and 255 
  uint8_t num = (rand() % (255 - 0 + 1)) + 0;
  // Binary AND the number with nn 
  num = num & nn;
  // Put the result in v[x]
  chip8->registers[x] = num;
}

void opcode_EX9E(CHIP_8 *chip8, uint8_t x) {
  // If keypad[x] is currently down, skip the next instruction 
  if(chip8->keypad[x] == 1) {
    chip8->pc += 2;
  }
}

void opcode_EXA1(CHIP_8 *chip8, uint8_t x) {
  // If keypad[x] is currently up, skip the next instruction 
  if(chip8->keypad[x] == 0) {
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
  chip8->index_register = chip8->index_register + chip8->registers[x];
}

void opcode_FX0A(CHIP_8 *chip8, uint8_t x) {
  // Stop the flow of execution 
  bool keyfound = false;
  for(int i = 0; i < 17; i++) {
    if(chip8->keypad[i] == 1) {
      keyfound = true;
      break;
    }
  }
  if(!keyfound)
   chip8->pc -= 2;
}

void opcode_FX29(CHIP_8 *chip8, uint8_t x) {
  // Set the index register to the address of the corresponding sprite of V[x] 
  chip8->index_register = chip8->memory[FONT_START_ADDRESS + (x * 5)];
}

void opcode_FX33(CHIP_8 *chip8, uint8_t x) {
  // TODO: 
  uint8_t hundreds = (x / 100) % 10;
  uint8_t tens = (x / 10) % 10;
  uint8_t ones = (x / 1) % 10;

  chip8->memory[chip8->index_register] = ones;
  chip8->memory[chip8->index_register + 1] = tens;
  chip8->memory[chip8->index_register + 2] = hundreds;
}

void opcode_FX55(CHIP_8 *chip8, uint8_t x) {
  if(x == 0)
    chip8->memory[chip8->index_register] = chip8->registers[0];
  return;
  int current_address = chip8->index_register;
  for(int i = 0; i <= x; i++) {
    chip8->memory[current_address] = chip8->registers[i];
    current_address += 1;
  }
}

void opcode_FX65(CHIP_8 *chip8, uint8_t x) {
  if(x == 0)
    chip8->registers[0] = chip8->memory[chip8->index_register];
  return;
  int current_address = chip8->index_register;
  for(int i = 0; i <= x; i++) {
    chip8->registers[i] = chip8->memory[current_address];
    current_address += 1;
  }
}

// Function to initialize the CHIP-8
int initialize(CHIP_8 *chip8, char* filename) {
  // Load the font into memory
  load_font(chip8);
  // Initialize the registers to be zeroed out
  init_registers(chip8);
  init_memory(chip8);
  // TODO: Read in the ROM file in bytes
  FILE* file = fopen(filename, "rb");
  if(file == NULL) {
    printf("File not found!");
    return -1;
  }
  fseek(file, 0, SEEK_END);
  long filelen = ftell(file);
  printf("%li", filelen);
  fseek(file, 0, SEEK_SET);
  uint16_t start_address = 0x200;
  fread(chip8->memory + start_address, sizeof(uint16_t), filelen, file);
  fclose(file);
  printf("Successfully initialized!");
  return 0;
}

void emulate_cycle(CHIP_8 *chip8) {
  chip8->opcode = chip8->memory[chip8->pc + 0];
  chip8->opcode <<= 8;
  chip8->opcode |= chip8->memory[chip8->pc + 1];
  chip8->pc += 2;
  // Second nibble, used to look up a register
  uint8_t x = (chip8->opcode & 0x0F00) >> 8;
  // Third nibble, used to up a register
  uint8_t y = (chip8->opcode & 0x00F0) >> 4;
  // Last nibble, a 4 bit number
  uint8_t n = (chip8->opcode & 0x000F);
  // Second byte, 8-bit immediate number
  uint8_t nn = (chip8->opcode & 0x00FF);
  // Second, third, and fourth nibbles, 12-bit immediate memory address
  uint16_t nnn = (chip8->opcode & 0xFFF);

  switch(chip8->opcode & 0xF000) 
  {
    // If an instruction to set the index register is called
    case 0xA000:
      opcode_ANNN(chip8);
      break;
    // If a jump instruction is called
    case 0x1000:
      // Set the program counter to be the address of NNN 
      opcode_1NNN(chip8);
      break;
    case 0x6000:
      opcode_6XNN(chip8);
      break;
    case 0x7000:
      opcode_7XNN(chip8);
      break;
    case 0xD000:
      opcode_DXYN(chip8, x, y, n);
      break;
    case 0x00E0:
      opcode_00E0(chip8);
      break;
    case 0x3000:
      opcode_3XKK(chip8, x, nn);
    case 0x4000:
      opcode_4XKK(chip8, x, nn);
    case 0x5000:
      opcode_5XY0(chip8, x, y);
    case 0x8000:
      opcode_8XY0(chip8, x, y);
    case 0x9000:
      opcode_9XY0(chip8, x, y);
    case 0x8001:
      opcode_8XY1(chip8, x, y);
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
    case 0xB000:
      opcode_BNNN(chip8, nnn);
      break;
    case 0xC000:
      opcode_CXKK(chip8, x, nn);
      break;
    case 0xE00E:
      opcode_EX9E(chip8, x);
      break;
    case 0xE0A1:
      opcode_EXA1(chip8, x);
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
    case 0xF00A:
      opcode_FX0A(chip8, x);
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
    default:
      break;
  }
}



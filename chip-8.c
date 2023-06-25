#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "chip-8.h"

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
  // TODO: 
  // All of these should be passed ONLY what they require to function 
  // And if they require more than what's given, provide a reference to the chip8 object
  // Get the first nibble to determine the type of instruction
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
  }
}



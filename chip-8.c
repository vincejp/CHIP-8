#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#define FONT_ARRAY_SIZE 80 
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

// The available memory to programs

uint8_t memory[4096];
// The display
uint8_t display[SCREEN_WIDTH * SCREEN_HEIGHT];
// Program counter, to store the current executing instruction
uint16_t pc; 
// Stack, to store the order of execution
uint16_t stack[16];
// Stack pointer to keep track of where we are in the stack
uint8_t stack_ptr;
// Delay timer for timing 
uint8_t delay_timer;
// Sound timer for sound 
uint8_t sound_timer;
// 16 eight bit general purpose registers
uint8_t registers[16];
// The opcode that we want to execute
uint16_t opcode;
// The keypad mapping
uint16_t keypad[16];
// Index register
uint16_t index_register = 0;
bool drawFlag = false;

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
void load_font() {
  uint8_t start = FONT_START_ADDRESS;
  for (uint8_t i = 0; i < FONT_ARRAY_SIZE; i++) {
    memory[start] = font[i];
    start += 1;
  }
}

// Zero out the registers
void init_registers() {
  for(uint8_t i = 0; i < 16; i++) {
    registers[i] = 0;
  }
}
// Zero out the memory 
void init_memory() {
  for(uint16_t i = 0; i < 4096 ; i++) {
    memory[i] = 0;
  }
}

// Clear the screen - DONE not tested
void opcode_00E0() {
  // Turn off all pixels in the display array
  for(int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
    display[i] = display[i] & 0x00;
  }
  drawFlag = true;
  pc += 2;
};

// Jump instruction - DONE, not tested
void opcode_1NNN() {
  pc = opcode & 0x0FFF;
};

// Set register VX - DONE, not tested
void opcode_6XNN() {
    uint16_t reg_buffer = opcode & 0x0F00;
    reg_buffer = reg_buffer >> 8; 
    registers[reg_buffer] = opcode & 0x00FF;
};

// Add value to register VX - DONE, not tested
void opcode_7XNN() {
  // Store the number containing the third byte in a buffer;
    uint16_t reg_buffer = opcode & 0x0F00;
  // Shift right by 8 bits to make it a valid address;
    reg_buffer = reg_buffer >> 8; 
    // Get the value that should be at that address, as a 16 bit integer
    // Cast it to be an 8-bit integer
    uint8_t value = (uint8_t)(opcode & 0x00FF);
    // Add the value to the address
    registers[reg_buffer] += value;
};

// Set index register I - DONE, not tested;
void opcode_ANNN() {
  index_register = opcode & 0x0FFF;
  pc += 2;
};

// Display/draw 
// THIS IS NOT MY OWN CODE
void opcode_DXYN(uint8_t x, uint8_t y, uint8_t n) {
  // GOAL: Draw a n N pixels tall sprite at the memory location index_register holds given, 
  // Get what is in register VX
  registers[0xF] = 0;

  for(int yline = 0; yline < n; yline++) {
    // Get the pixel at memory location Index register + y
    uint8_t current_byte = memory[index_register + y];

    for(int xline = 0; xline < 8; xline++) {
      if((current_byte & (0x80 >> x)) != 0)
      {
        if(display[(x + xline + ((y + yline) * 64))] == 1)
          registers[0xF] = 1;
        display[x + xline + ((y + yline) * 64)] ^= 1;
      }
    }
  }
  drawFlag = true;
  pc += 2;
};

// Function to initialize the CHIP-8
int initialize(char* filename) {
  // Load the font into memory
  load_font();
  // Initialize the registers to be zeroed out
  init_registers();
  // TODO: Read in the ROM file in bytes
  FILE* file = fopen(filename, "rb");
  if(file == NULL) {
    printf("File not found!");
    return -1;
  }
  fseek(file, 0, SEEK_END);
  long filelen = ftell(file);
  printf("%li", filelen);
  rewind(file);
  char *file_buffer = (char*)malloc(filelen * sizeof(char));
  fread(file_buffer, filelen, 1, file);
  uint16_t start_address = 0x200;
  for(int i = start_address; i < filelen; i++)
  {
    memory[i] = file_buffer[i];
  }
  fclose(file);
  free(file_buffer);
  printf("Successfully initialized!");
  return 0;
}

void emulate_cycle() {
  opcode = memory[pc] << 8 | memory[pc + 1];
  uint8_t n;
  uint8_t x;
  uint8_t y;
  switch(opcode & 0xF000) 
  {
    // If an instruction to set the index register is called
    case 0xA000:
      opcode_ANNN();
      break;
    // If a jump instruction is called
    case 0x1000:
      // Set the program counter to be the address of NNN 
      opcode_1NNN();
      break;
    case 0x6000:
      opcode_6XNN();
      break;
    case 0x7000:
      opcode_7XNN();
      break;
    case 0xD000:
      n = 0xD000 & 0x000F;
      x = (0xD000 & 0x0F00) >> 8;
      y = (0xD000 & 0x00F0) >> 4;
      opcode_DXYN(x, y, n);
      break;
    case 0x00E0:
      opcode_00E0();
      break;
  }
}

int main(void) 
{
  initialize("IBM_Logo.ch8");
  return 0;
}

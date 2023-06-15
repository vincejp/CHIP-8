#include <stdio.h>
#include <stdint.h>

// The available memory to programs

uint8_t memory[4096];
// The display
uint32_t display[64 * 32];
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

// Addresses where the font starts and ends in our emulated memory
const uint8_t FONT_START_ADDRESS = 0x50;
const uint8_t FONT_END_ADDRESS = 0x9F;
const uint8_t FONT_ARRAY_SIZE = 80;
uint8_t font[80] = {
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

uint8_t start = FONT_START_ADDRESS;

// Function to load the font into memory 
void load_font() {
  for (uint8_t i = 0; i < FONT_ARRAY_SIZE; i++) {
    memory[start] = font[i];
    start += 1;
  }
}

// Function to initialize the CHIP-8
void initialize() {
  load_font();
}

int main(void) {
  // Call the initialization function
  initialize();
  printf("%d\n", memory[80]);
  return 0;
}

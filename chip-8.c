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

int main(void) {
  return 0;
}

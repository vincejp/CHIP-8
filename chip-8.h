#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#define FONT_ARRAY_SIZE 80 
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
typedef struct CHIP_8 {
  uint8_t memory[4096];
  // The display
  uint32_t display[SCREEN_WIDTH * SCREEN_HEIGHT];
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
  uint16_t index_register;
  bool drawFlag;
} CHIP_8;
// Function to load the font into memory 
void load_font();
void init_registers();
void init_memory();
/* All available opcodes */
void opcode_DXYN(CHIP_8 *chip8, uint8_t x, uint8_t y, uint8_t n);
void opcode_ANNN(CHIP_8 *chip8);
void opcode_7XNN(CHIP_8 *chip8);
void opcode_6XNN(CHIP_8 *chip8);
void opcode_1NNN(CHIP_8 *chip8);
void opcode_00E0(CHIP_8 *chip8);
void opcode_00EE();
void opcode_0NNN();
void opcode_3XKK();
void opcode_4XKK();
void opcode_5XY0();
// Stores value of register Vy in register Vx 
void opcode_8XY0();
// Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx
void opcode_8XY1();
// Performs a bitwise AND on the values of Vx and Vy, then stores the result
void opcode_8XY2();
// Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx
void opcode_8XY3();
// The values of Vx and Vy are added together. If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0. 
// Only the lowest 8 bits of the result are kept, and stored in Vx.
void opcode_8XY4();
// If Vx > Vy, then VF is set to 1
// Then Vy is subtracted from Vx, and the results stored in Vx.
void opcode_8XY5();
// If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. 
// Then Vx is divided by 2
void opcode_8XY6();
// If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.
void opcode_8XY7();
// If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. 
// Then Vx is multiplied by 2.
void opcode_8XYE();
void opcode_9XY0();
void opcode_BNNN();
void opcode_CXKK();
void opcode_EX9E();
void opcode_EXA1();
void opcode_FX07();
void opcode_FX0A();
void opcode_FX15();
void opcode_FX18();
void opcode_FX1E();
void opcode_FX29();
void opcode_FX33();
void opcode_FX55();
void opcode_FX65();
int initialize(CHIP_8 * chip8, char * filename);
void emulate_cycle(CHIP_8 * chip8);
void load_font(CHIP_8 * chip8);
void init_registers(CHIP_8 * chip8);
void init_memory(CHIP_8 * chip8);


#include <stdint.h>
#include <stdbool.h>
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
  uint16_t keypad[17];
  // Index register
  uint16_t index_register;
  bool drawFlag;
} CHIP_8;
/* All available opcodes */

// Function to load the font into memory 
void load_font(CHIP_8 *chip8);
// Zero out the registers
void init_registers(CHIP_8 *chip8);
// Zero out the memory 
void init_memory(CHIP_8 *chip8);
// Clear the screen - DONE not tested
void opcode_00E0(CHIP_8 *chip8);
// Jump instruction - DONE, not tested
void opcode_1NNN(CHIP_8 *chip8); 
// Set register VX - DONE, not tested
void opcode_6XNN(CHIP_8 *chip8); 

// Add value to register VX - DONE, not tested
void opcode_7XNN(CHIP_8 *chip8); 

// Set index register I - DONE, not tested;
void opcode_ANNN(CHIP_8 *chip8);

// Display/draw 
void opcode_DXYN(CHIP_8 *chip8, uint8_t x, uint8_t y, uint8_t n); 

void opcode_2NNN(CHIP_8 * chip8, uint16_t nnn); 


void opcode_00EE(CHIP_8 * chip8); 


void opcode_3XKK(CHIP_8 * chip8, uint8_t x, uint8_t nn); 

void opcode_4XKK(CHIP_8 * chip8, uint8_t x, uint8_t nn); 


void opcode_5XY0(CHIP_8 *chip8, uint8_t x, uint8_t y); 


void opcode_9XY0(CHIP_8 *chip8, uint8_t x, uint8_t y); 


/* Logical and arithemtic instructions */

// Set VX's value to be that of VY */
void opcode_8XY0(CHIP_8 *chip8, uint8_t x, uint8_t y); 


// Set VX to the logical OR of VX and VY */
void opcode_8XY1(CHIP_8 *chip8, uint8_t x, uint8_t y); 


void opcode_8XY2(CHIP_8 *chip8, uint8_t x, uint8_t y); 


void opcode_8XY3(CHIP_8 *chip8, uint8_t x, uint8_t y); 


void opcode_8XY4(CHIP_8 *chip8, uint8_t x, uint8_t y); 


void opcode_8XY5(CHIP_8 *chip8, uint8_t x, uint8_t y); 


void opcode_8XY6(CHIP_8 *chip8, uint8_t x, uint8_t y); 


void opcode_8XY7(CHIP_8 *chip8, uint8_t x, uint8_t y); 


void opcode_8XYE(CHIP_8 *chip8, uint8_t x, uint8_t y); 


void opcode_BNNN(CHIP_8 *chip8, uint16_t nnn); 


void opcode_CXKK(CHIP_8 *chip8, uint8_t x, uint8_t nn); 


void opcode_EX9E(CHIP_8 *chip8, uint8_t x); 


void opcode_EXA1(CHIP_8 *chip8, uint8_t x); 


void opcode_FX07(CHIP_8 *chip8, uint8_t x); 


void opcode_FX15(CHIP_8 *chip8, uint8_t x); 


void opcode_FX18(CHIP_8 *chip8, uint8_t x); 


void opcode_FX1E(CHIP_8 *chip8, uint8_t x); 


void opcode_FX0A(CHIP_8 *chip8, uint8_t x); 


void opcode_FX29(CHIP_8 *chip8, uint8_t x); 


void opcode_FX33(CHIP_8 *chip8, uint8_t x); 


void opcode_FX55(CHIP_8 *chip8, uint8_t x); 


void opcode_FX65(CHIP_8 *chip8, uint8_t x); 


// Function to initialize the CHIP-8
int initialize(CHIP_8 *chip8, char* filename); 


void emulate_cycle(CHIP_8 *chip8); 



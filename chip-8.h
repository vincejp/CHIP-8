#include <stdint.h>
#include <stdbool.h>
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

typedef struct CHIP_8 {
  // Available system memory 
  uint8_t memory[4096];
  // Display which is 64 pixels wide by 32 pixels tall
  uint32_t display[SCREEN_WIDTH * SCREEN_HEIGHT];
  // Program counter 
  uint16_t pc; 
  // 16 level stack 
  uint16_t stack[16];
  // Stack pointer to determine location on the stack 
  uint8_t stack_ptr;
  uint8_t delay_timer;
  uint8_t sound_timer;
  // 16 eight bit registers, registers[0xF] is the flag register
  uint8_t registers[16];
  // Current opcode being executed 
  uint16_t opcode;
  // Keypad 
  uint16_t keypad[16];
  // Index register that points at memory locations 
  uint16_t index_register;
  // Flag to indicate whether something has been drawn to the screen 
  bool drawFlag;
} CHIP_8;

void load_font(CHIP_8 *chip8);

void init_registers(CHIP_8 *chip8);

void init_memory(CHIP_8 *chip8);

void opcode_00E0(CHIP_8 *chip8);

void opcode_1NNN(CHIP_8 *chip8, uint16_t nnn);

void opcode_6XNN(CHIP_8 *chip8, uint8_t x, uint8_t nn);

void keypad_dump(CHIP_8 * chip8);

void opcode_7XNN(CHIP_8 *chip8, uint8_t x, uint8_t nn);

void opcode_ANNN(CHIP_8 *chip8, uint16_t nnn);

void opcode_DXYN(CHIP_8 *chip8, uint8_t x, uint8_t y, uint8_t n); 

void opcode_2NNN(CHIP_8 * chip8, uint16_t nnn); 

void opcode_00EE(CHIP_8 * chip8); 

void opcode_3XKK(CHIP_8 * chip8, uint8_t x, uint8_t nn); 

void opcode_4XKK(CHIP_8 * chip8, uint8_t x, uint8_t nn); 

void opcode_5XY0(CHIP_8 *chip8, uint8_t x, uint8_t y); 

void opcode_9XY0(CHIP_8 *chip8, uint8_t x, uint8_t y); 

void opcode_8XY0(CHIP_8 *chip8, uint8_t x, uint8_t y); 

void opcode_8XY1(CHIP_8 *chip8, uint8_t x, uint8_t y); 

void opcode_8XY2(CHIP_8 *chip8, uint8_t x, uint8_t y); 

void opcode_8XY3(CHIP_8 *chip8, uint8_t x, uint8_t y); 

void opcode_8XY4(CHIP_8 *chip8, uint8_t x, uint8_t y); 

void opcode_8XY5(CHIP_8 *chip8, uint8_t x, uint8_t y); 

void opcode_8XY6(CHIP_8 *chip8, uint8_t x, uint8_t y); 

void opcode_8XY7(CHIP_8 *chip8, int8_t x, int8_t y); 

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

int initialize(CHIP_8 *chip8, char* filename); 

void emulate_cycle(CHIP_8 *chip8); 

void memory_dump(CHIP_8 * chip8, uint8_t start, uint8_t end); 

void execution(CHIP_8 * chip8);

void init_keypad(CHIP_8 * chip8);

void init_display(CHIP_8 *chip8);

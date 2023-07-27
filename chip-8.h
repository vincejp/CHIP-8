#include <stdint.h>
#include <stdbool.h>
#define FONT_ARRAY_SIZE 80
#define PROGRAM_START_ADDRESS 0x200 
#define FONT_START_ADDRESS 0x00 
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

// CHIP-8 struct to emulate CHIP-8 specifications
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
  // CPU timer
  uint8_t delay_timer;
  // Sound timer
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

// Function to output the current contents of the registers
void register_dump(CHIP_8 * chip8);

// Function to display the desired contents of memory
void memory_dump(CHIP_8 * chip8, uint8_t start, uint8_t end); 

// Function to display the contents of the keypad 
void keypad_dump(CHIP_8 * chip8);

// Function to output the current instruction being executed
void execution(CHIP_8 * chip8);

// Function to initalize the keypad
void init_keypad(CHIP_8 * chip8);

// A function that loads the font into memory 
void load_font(CHIP_8 *chip8);

// Function to initialize registers
void init_registers(CHIP_8 *chip8);

// Function to initialize memory  
void init_memory(CHIP_8 *chip8);

// Function to initialize the CHIP-8
int initialize(CHIP_8 *chip8, char* filename); 

// Function to jump to address nnn 
void opcode_1NNN(CHIP_8 *chip8, uint16_t nnn);

// Call the subroutine at nnn 
void opcode_2NNN(CHIP_8 * chip8, uint16_t nnn); 

// Skip the next instruction if registers[x] == nn  
void opcode_3XNN(CHIP_8 * chip8, uint8_t x, uint8_t nn); 

// Skip the next instruction if registers[x] != nn 
void opcode_4XNN(CHIP_8 * chip8, uint8_t x, uint8_t nn); 

// Skip the next instruction if registers[x] == registers[y]
void opcode_5XY0(CHIP_8 *chip8, uint8_t x, uint8_t y); 

// Function to put the value of nn into registers[x]
void opcode_6XNN(CHIP_8 *chip8, uint8_t x, uint8_t nn);

// Add the value of nn to registers[x], store result in registers[x] 
void opcode_7XNN(CHIP_8 *chip8, uint8_t x, uint8_t nn);

// Skip the next instruction if registers[x] != registers[y]
void opcode_9XY0(CHIP_8 *chip8, uint8_t x, uint8_t y); 

// Set the value of the index register to nnn 
void opcode_ANNN(CHIP_8 *chip8, uint16_t nnn);

// Set index register to nnn 
void opcode_BNNN(CHIP_8 *chip8, uint16_t nnn); 

// Generate a random number between 0 and 255, AND-ed with nn 
void opcode_CXNN(CHIP_8 *chip8, uint8_t x, uint8_t nn); 

// Function to draw an N pixels tall sprite from memory to the screen, at registers[x], registers[y] 
void opcode_DXYN(CHIP_8 *chip8, uint8_t x, uint8_t y, uint8_t n); 

// Return from a subroutine 
void opcode_00EE(CHIP_8 * chip8); 

// Function to clear the screen 
void opcode_00E0(CHIP_8 *chip8);

// Set registers[x] = registers[y] 
void opcode_8XY0(CHIP_8 *chip8, uint8_t x, uint8_t y); 

// Set registers[x] = registers[x] OR registers[y]
void opcode_8XY1(CHIP_8 *chip8, uint8_t x, uint8_t y);

// Set registers[x] = registers[x] AND registers[y]
void opcode_8XY2(CHIP_8 *chip8, uint8_t x, uint8_t y); 

// Set registers[x] = registers[x] XOR registers[y]
void opcode_8XY3(CHIP_8 *chip8, uint8_t x, uint8_t y); 

// Set registers[x] = registers[x] + registers[y], registers[0xF] = carry 
void opcode_8XY4(CHIP_8 *chip8, uint8_t x, uint8_t y); 

// Set registers[x] = registers[x] - registers[y] registers[0xF] = NOT borrow
void opcode_8XY5(CHIP_8 *chip8, uint8_t x, uint8_t y); 

// Set registers[x] = registers[x] >> 1
void opcode_8XY6(CHIP_8 *chip8, uint8_t x, uint8_t y); 

// Set registers[x] = registers[y] - registers[x], registers[0xF] = NOT borrow 
void opcode_8XY7(CHIP_8 *chip8, int8_t x, int8_t y); 

// Set registers[x] = registers[x] << 1
void opcode_8XYE(CHIP_8 *chip8, uint8_t x, uint8_t y); 

// Skip next instruction if keypad at registers[x] is pressed
void opcode_EX9E(CHIP_8 *chip8, uint8_t x); 

// Skip next instruction if keypad at registers[x] is NOT pressed
void opcode_EXA1(CHIP_8 *chip8, uint8_t x); 

// Set registers[x] to be the value of the delay timer 
void opcode_FX07(CHIP_8 *chip8, uint8_t x); 

// Set the delay timer to be equal to registers[x]
void opcode_FX15(CHIP_8 *chip8, uint8_t x); 

// Set the sound timer to be equal to registers[x]
void opcode_FX18(CHIP_8 *chip8, uint8_t x); 

// Add registers[x] to the index register
void opcode_FX1E(CHIP_8 *chip8, uint8_t x); 

// Wait for a keypress and store value of the key in registers[x]
void opcode_FX0A(CHIP_8 *chip8, uint8_t x); 

// Set the index register to be the location of the sprite for digit registers[x]
void opcode_FX29(CHIP_8 *chip8, uint8_t x); 

// Store a number in binary coded decimal representation
void opcode_FX33(CHIP_8 *chip8, uint8_t x); 

// Store registers[0] to registers[x] into memory starting at location of index register
void opcode_FX55(CHIP_8 *chip8, uint8_t x); 

// Read registers[0] to registers[x] from memory starting at location of index register 
void opcode_FX65(CHIP_8 *chip8, uint8_t x); 

// Fetch, decode, execute cycle of CHIP-8
void emulate_cycle(CHIP_8 *chip8); 
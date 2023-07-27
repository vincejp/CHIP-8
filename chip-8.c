#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "chip-8.h"

void register_dump(CHIP_8 * chip8) {
  // Prints the contents of the 16 registers
  for(int i = 0; i < 16; i++) {
    printf("Register: %d, Value: %d\n", i, chip8->registers[i]);
  }
}

void memory_dump(CHIP_8 * chip8, uint8_t start, uint8_t end) {
  // Prints the contents of memory starting at start and ending at end
  for(int i = start; i < end; i++) {
    printf("Memory location: %d, Value: %d\n", i, chip8->memory[i + chip8->index_register]);
  }
}

void keypad_dump(CHIP_8 * chip8) {
  // Prints the contents of the keypad
  for(int i = 0; i < 16; i++) {
    printf("Keypad[%d]: %d\n", i, chip8->keypad[i]);
  }
}

void execution(CHIP_8 * chip8) {
  // Prints the current opcode being executed
  printf("Currently executing: 0x%04x\n", chip8->opcode);
}

void load_font(CHIP_8 *chip8) {
  // Function to load the fontset into memory
  // Array to store the font, each sprite is 5 bytes 
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

  for (uint8_t i = 0; i < FONT_ARRAY_SIZE; i++) {
    chip8->memory[i + FONT_START_ADDRESS] = font[i];
  }
}

void init_registers(CHIP_8 *chip8) {
  // Zero out registers
  for(uint8_t i = 0; i < 16; i++) {
    chip8->registers[i] = 0;
  }
}

void init_memory(CHIP_8 *chip8) {
  // Zero out memory 
  for(uint16_t i = 0; i < 4096 ; i++) {
    chip8->memory[i] = 0;
  }
}

void init_keypad(CHIP_8 * chip8) {
  // Zero out keypad  
  for(int i = 0; i < 16; i++) {
    chip8->keypad[i] = 0;
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

void opcode_3XNN(CHIP_8 * chip8, uint8_t x, uint8_t nn) {
  if(chip8->registers[x] == nn) 
    chip8->pc += 2;
}

void opcode_4XNN(CHIP_8 * chip8, uint8_t x, uint8_t nn) {
  if(chip8->registers[x] != nn) 
    chip8->pc += 2;
}

void opcode_5XY0(CHIP_8 *chip8, uint8_t x, uint8_t y) {
  if(chip8->registers[x] == chip8->registers[y]) {
    chip8->pc += 2;
  }
}

void opcode_6XNN(CHIP_8 *chip8, uint8_t x, uint8_t nn) {
  chip8->registers[x] = nn;
}

void opcode_7XNN(CHIP_8 *chip8, uint8_t x, uint8_t nn) {
  chip8->registers[x] += nn;
}

void opcode_9XY0(CHIP_8 *chip8, uint8_t x, uint8_t y) {
  if(chip8->registers[x] != chip8->registers[y]) {
    chip8->pc += 2;
  }
}

void opcode_ANNN(CHIP_8 *chip8, uint16_t nnn) {
  chip8->index_register = nnn;
}

void opcode_BNNN(CHIP_8 *chip8, uint16_t nnn) {
  chip8->pc = nnn + chip8->registers[0];
}

void opcode_CXNN(CHIP_8 *chip8, uint8_t x, uint8_t nn) {
  // Get random number between 0 and 255 
  uint8_t num = (rand() % 0xFF);
  // Binary AND the number with nn 
  // Put the result in v[x]
  chip8->registers[x] = num & nn;
}

void opcode_DXYN(CHIP_8 *chip8, uint8_t x, uint8_t y, uint8_t n) {
  
  // Reset the flag register 
  chip8->registers[0xF] = 0; 
  // Get the x and y coordinates that the sprite will be drawn at 
  uint8_t x_coord = chip8->registers[x] % 64;
  uint8_t y_coord = chip8->registers[y] % 32;
  
  // For N rows

  for (int y = 0; y < n; y++) {
    // For each bit in this sprite row 
    for (int x = 0; x < 8; x++) {
      // Get the byte of sprite data from memory, offset by y 
      uint8_t current_byte = chip8->memory[y + chip8->index_register];
      // Get the current pixel 
      uint8_t current_pixel = current_byte & (0x80 >> x);
      // Get the pixel on the screen at memory[(y_coord + y) * SCREEN_WIDTH + (x_coord + x)]
      // Keep a pointer to that memory address so that we are able to change the pixel 
      uint32_t *screen_pixel = &chip8->display[(y_coord + y) * SCREEN_WIDTH + (x_coord + x)];
      // If we are drawing too high, clip the drawing 
      if (x_coord + x > SCREEN_WIDTH || y_coord + y > SCREEN_HEIGHT)
        break;
      // If the current pixel is on, 
      if (current_pixel) {
        // If the screen pixel is on
        if (*screen_pixel == 0xFFFFFFFF)
          // Set the flag register to indicate pixels have been turned off 
          chip8->registers[0xF] = 1;
        // XOR the new pixels onto the screen 
        *screen_pixel ^= 0xFFFFFFFF;
      }
    }
  }
  // Indicate that something was drawn to the screen
 chip8->drawFlag = true;
}

void opcode_00EE(CHIP_8 * chip8) {
  // Decrement the stack pointer 
  chip8->stack_ptr -= 1;
  // Set the PC to whatever is at the current stack pointer 
  chip8->pc = chip8->stack[chip8->stack_ptr];
}

void opcode_00E0(CHIP_8 *chip8) {
  // Zero out the display
  for(int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
    chip8->display[i] = 0;
  }
}

void opcode_8XY0(CHIP_8 *chip8, uint8_t x, uint8_t y) {
  chip8->registers[x] = chip8->registers[y];
}

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
  // Add the registers together 
  uint16_t result = chip8->registers[x] + chip8->registers[y];
  // Store the last 8 bits into registers[x] 
  chip8->registers[x] = result & 0xFF;
  // If the result was greater than 255, set carry flag 
  if(result > 255)
    chip8->registers[0xF] = 1;
  else
    chip8->registers[0xF] = 0; 
}

void opcode_8XY5(CHIP_8 *chip8, uint8_t x, uint8_t y) {
  // If registers[x] >= registers[y], there is no borrow,
  // Registers[0xF] is set to 1
  uint8_t carry = chip8->registers[y] <= chip8->registers[x];
  chip8->registers[x] -= chip8->registers[y];
  chip8->registers[0xF] = carry;
}

void opcode_8XY6(CHIP_8 *chip8, uint8_t x, uint8_t y) {
  // Get the least significant bit of v[x], this is what will be shifted out
  uint8_t lsb = chip8->registers[x] & 0x01;
  // Set the flag register if lsb is 1, 0 otherwise
  if(lsb == 1)
    chip8->registers[0xF] = 1; 
  else
    chip8->registers[0xF] = 0;
  // Divide by two by shifting one bit to the right
  chip8->registers[x] = chip8->registers[x] >> 1;
}

void opcode_8XY7(CHIP_8 *chip8, int8_t x, int8_t y) {
  // Do subtraction of registers
  chip8->registers[x] = chip8->registers[y] - chip8->registers[x];
  // Check for borrow, set flag accordingly 
  if(chip8->registers[y] > chip8->registers[x])
    chip8->registers[0xF] = 1;
  else
    chip8->registers[0xF] = 0; 
}

void opcode_8XYE(CHIP_8 *chip8, uint8_t x, uint8_t y) {
  // Get the bit that will be shifted out 
  uint8_t msb = (chip8->registers[x] & 0xF0) >> 7;
  // Multiply by two(shift left one bit) 
  chip8->registers[x] <<= 1;
  // Set the flag register to be the bit shifted out 
  chip8->registers[0xF] = msb; 
}

void opcode_EX9E(CHIP_8 *chip8, uint8_t x) {
  // If keypad at registers[x] is currently down, increment PC by 2
  if(chip8->keypad[chip8->registers[x]] == 1) {
    chip8->pc += 2;
  }
}

void opcode_EXA1(CHIP_8 *chip8, uint8_t x) {
  // If keypad[x] is currently up, increment PC by 2
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
  // Flag to set if key is found 
  bool keyfound = false;
  // Check all keys on the keypad
  for(int i = 0; i < 16; i++) {
    if(chip8->keypad[i] != 0) {
      keyfound = true;
    }
  }
  if(keyfound == false) {
   // Wait for a keypress, stop execution 
   chip8->pc -= 2;
  }
}

void opcode_FX29(CHIP_8 *chip8, uint8_t x) {
  // Set the index register to the address of the corresponding sprite of V[x] 
  // Multiplied by 5 because of how the font is stored in memory 
  chip8->index_register = FONT_START_ADDRESS + (chip8->registers[x] * 5);
}

void opcode_FX33(CHIP_8 *chip8, uint8_t x) {
  /* Store the binary coded decimal representation of registers[x] in memory at
   * index_register
   * index_register + 1
   * index_register + 2
   */ 
  uint8_t hundreds = (chip8->registers[x] / 100) % 10;
  uint8_t tens = (chip8->registers[x] / 10) % 10;
  uint8_t ones = (chip8->registers[x] / 1) % 10;
  chip8->memory[chip8->index_register] = hundreds;
  chip8->memory[chip8->index_register + 1] = tens;
  chip8->memory[chip8->index_register + 2] = ones;
}

void opcode_FX55(CHIP_8 *chip8, uint8_t x) {
  // Start at location of index register
  for(uint8_t i = 0; i <= x; i++) {
    chip8->memory[chip8->index_register + i] = chip8->registers[i];
  }
}

void opcode_FX65(CHIP_8 *chip8, uint8_t x) {
  // Read starting at index register
  for(uint8_t i = 0; i <= x; i++) {
    chip8->registers[i] = chip8->memory[chip8->index_register + i];
  }
}

int initialize(CHIP_8 *chip8, char* filename) {
  // Attempt to open the desired file 
  FILE* file = fopen(filename, "rb");
  if(file == NULL) {
    // Return error code -1 if file is not found 
    printf("File not found!\n");
    // Program execution ends 
    return -1;
  }
  // Zero out the display, memory, registers, and keypad
  opcode_00E0(chip8);
  init_memory(chip8);
  init_registers(chip8);
  init_keypad(chip8);
  // Load the font into memory 
  load_font(chip8);
  // Get the length of the file in bytes 
  fseek(file, 0, SEEK_END);
  long filelen = ftell(file);
  fseek(file, 0, SEEK_SET);
  // Read the bytes of the program into memory 
  fread(chip8->memory + PROGRAM_START_ADDRESS, sizeof(uint16_t), filelen, file);
  // Close the file 
  fclose(file);
  return 0;
}

void emulate_cycle(CHIP_8 *chip8) {

  // Fetch the first byte of the instruction from memory 
  chip8->opcode = chip8->memory[chip8->pc + 0];
  // Shift it 8 bits to the left
  chip8->opcode <<= 8;
  // OR it with the instruction at the next address
  chip8->opcode |= chip8->memory[chip8->pc + 1];
  // Increment PC 
  chip8->pc += 2;
  
  // Get each piece of data from the opcode

  // Second nibble of the opcode, used to look up  a register
  uint8_t x = (chip8->opcode & 0x0F00) >> 8;
  // Third nibble, used to up a register
  uint8_t y = (chip8->opcode & 0x00F0) >> 4;
  // Last nibble, a 4 bit number
  uint8_t n = (chip8->opcode & 0x000F);
  // Second byte, 8-bit immediate number
  uint8_t nn = (chip8->opcode & 0x00FF);
  // Second, third, and fourth nibbles, 12-bit immediate memory address
  uint16_t nnn = (chip8->opcode & 0xFFF);

  // Switch statement decodes the opcodes, determines what it should do
  switch(chip8->opcode & 0xF000) 
  {
    // Functions are executed based on the opcode 
    // Cases where the opcode is entirely unique
    case 0x1000:
      opcode_1NNN(chip8, nnn);
      break; 
    case 0x2000:
      opcode_2NNN(chip8, nnn);
      break; 
    case 0x3000:
      opcode_3XNN(chip8, x, nn);
      break;
    case 0x4000:
      opcode_4XNN(chip8, x, nn);
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
      opcode_CXNN(chip8, x, nn);
      break;
    case 0xD000:
      opcode_DXYN(chip8, x, y, n); 
      break;

    // Case where the opcode has the first three digits 00E, and the last either 0 or E
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
    
    // Case where the first digit is 8 but the last digit is unique
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

    
    // The cases where the first digit repeats but the last two are unique 
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
      printf("Unknown opcode: 0x%04x\n", chip8->opcode); 
      break;
  }
}

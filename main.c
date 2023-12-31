#include "chip-8.h"
#include "SDL.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char* argv[])
{
  srand(time(NULL));
  // Create a new instance of the struct 
  CHIP_8 chip8;
  // If ROM is properly initialized, continue execution
  // Otherwise, quit 
  if(initialize(&chip8, argv[1]) == -1)
  {
    printf("Error initializing ROM\n");
    return -1;
  };
  // Initialize SDL_video 

  if(SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("Error initializing SDL video");
    return -1;
  };
  // Set the delay timer 
  chip8.delay_timer = 60; 
  // Set instructions per second 
  int instructions_per_second = 200;
  // Create the SDL Window 
  SDL_Window * window = SDL_CreateWindow("CHIP-8",
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 64 * 10, 32 * 10, 0);
  // Create the SDL Renderer 
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
  // Create the texture 
  SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
  bool quit = false;
  
  while (!quit)
  {
      SDL_Event event; 
      while (SDL_PollEvent(&event))
      {
        switch(event.type) {
          case SDL_QUIT:
            quit = true;
            break;
          case SDL_KEYDOWN:
            switch(event.key.keysym.sym) {
              case SDLK_x:
                chip8.keypad[0] = 1;
                break;
              case SDLK_1:
                chip8.keypad[1] = 1;
                break;
              case SDLK_2:
                chip8.keypad[2] = 1;
                break;
              case SDLK_3:
                chip8.keypad[3] = 1;
                break;
              case SDLK_q:
                chip8.keypad[4] = 1;
                break;
              case SDLK_w:
                chip8.keypad[5] = 1;
                break;
              case SDLK_e:
                chip8.keypad[6] = 1; 
                break;
              case SDLK_a:
                chip8.keypad[7] = 1;
                break;
              case SDLK_s: 
                chip8.keypad[8] = 1;
                break;
              case SDLK_d: 
                chip8.keypad[9] = 1;
                break;
              case SDLK_z:
                chip8.keypad[0xA] = 1;
                break;
              case SDLK_c:
                chip8.keypad[0xB] = 1;
                break; 
              case SDLK_4:
                chip8.keypad[0xC] = 1; 
                break;
              case SDLK_r:
                chip8.keypad[0xD] = 1;
                break;
              case SDLK_f:
                chip8.keypad[0xE] = 1;
                break;
              case SDLK_v:
                chip8.keypad[0xF] = 1;
                break;
          } break;
          case SDL_KEYUP:
            switch(event.key.keysym.sym) {
              case SDLK_x:
                chip8.keypad[0] = 0;
                break;
              case SDLK_1:
                chip8.keypad[1] = 0;
                break;
              case SDLK_2:
                chip8.keypad[2] = 0;
                break;
              case SDLK_3:
                chip8.keypad[3] = 0;
                break;
              case SDLK_q:
                chip8.keypad[4] = 0;
                break;
              case SDLK_w:
                chip8.keypad[5] = 0;
                break;
              case SDLK_e:
                chip8.keypad[6] = 0; 
                break;
              case SDLK_a:
                chip8.keypad[7] = 0;
                break;
              case SDLK_s:
                chip8.keypad[8] = 0;
                break;
              case SDLK_d:
                chip8.keypad[9] = 0;
                break;
              case SDLK_z:
                chip8.keypad[0xA] = 0;
                break;
              case SDLK_c:
                chip8.keypad[0xB] = 0;
              case SDLK_4:
                chip8.keypad[0xC] = 0; 
                break;
              case SDLK_r:
                chip8.keypad[0xD] = 0;
                break;
              case SDLK_f:
                chip8.keypad[0xE] = 0;
                break;
              case SDLK_v:
                chip8.keypad[0xF] = 0;
                break;
          } break;
        }
      }
      for(int i = 0; i < instructions_per_second / 60; i++) {
        // Clock cycles 
        emulate_cycle(&chip8);
      }
      // Update the texture, update renderer, and present to the screen 
      SDL_UpdateTexture(texture, NULL, chip8.display, 64 * sizeof(uint32_t));    
      SDL_RenderClear(renderer);
      SDL_RenderCopy(renderer, texture, NULL, NULL);
      SDL_RenderPresent(renderer);
      if(chip8.delay_timer > 0) {
        chip8.delay_timer -= 1;
      } 
  }

  // Clean up and quit SDL 
  
  SDL_DestroyWindow(window);
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();
  return 0;
}

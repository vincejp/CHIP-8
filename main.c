#include "chip-8.h"
#include "SDL.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int main(int argc, char* argv[])
{
  srand(time(NULL));
  bool quit = false;
  CHIP_8 chip8;
  SDL_Init(SDL_INIT_VIDEO);
  if(initialize(&chip8, argv[1]) == -1)
  {
    printf("Error initializing ROM");
    return -1;
  };

  chip8.delay_timer = 60; 
  int instructions_per_second = 500;
  SDL_Window * window = SDL_CreateWindow("CHIP-8",
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 64 * 10, 32 * 10, 0);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
  SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, 64, 32);
  memset(chip8.display, 0, 64 * 32 * sizeof(uint32_t));
  SDL_Event event;

  while (!quit)
  {
      while(SDL_PollEvent(&event))
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
      // Get the time at the start 
      const uint64_t start_frame_time = SDL_GetPerformanceCounter();
      for(int i = 0; i < instructions_per_second / 60; i++) {
         emulate_cycle(&chip8);
      }
      const uint64_t end_frame_time = SDL_GetPerformanceCounter();
      const double time_elapsed = (double)((end_frame_time - start_frame_time) * 1000) / SDL_GetPerformanceFrequency();
      SDL_Delay(16.67f > time_elapsed ? 16.67f - time_elapsed : 0);
      SDL_UpdateTexture(texture, NULL, chip8.display, 64 * sizeof(uint32_t));    
      SDL_RenderClear(renderer);
      SDL_RenderCopy(renderer, texture, NULL, NULL);
      SDL_RenderPresent(renderer);
      if(chip8.delay_timer > 0) {
        chip8.delay_timer -= 1;
      } 
  }

  SDL_DestroyWindow(window);
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();
  return 0;
}

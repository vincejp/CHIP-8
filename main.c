#include "chip-8.h"
#include "SDL.h"
#include <stdbool.h>
#include <stdio.h>

int main(void)
{
  bool quit = false;
  CHIP_8 chip8;
  SDL_Init(SDL_INIT_VIDEO);
  if(initialize(&chip8, "test_opcode.ch8") == -1)
  {
    printf("Error initializing ROM");
    return -1;
  };
  SDL_Window * window = SDL_CreateWindow("CHIP-8",
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 64 * 10, 32 * 10, 0);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
  SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 64, 32);
  memset(chip8.display, 0, 64 * 32 * sizeof(uint32_t));
  SDL_Event event;

  while (!quit)
  {
      while(SDL_PollEvent(&event))
      {
        if(event.type == SDL_QUIT)
        {
          quit = true;
        }
      }
      emulate_cycle(&chip8);
      SDL_UpdateTexture(texture, NULL, chip8.display, 64 * sizeof(uint32_t));    
      SDL_RenderClear(renderer);
      SDL_RenderCopy(renderer, texture, NULL, NULL);
      SDL_RenderPresent(renderer);
  }

  SDL_DestroyWindow(window);
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();
  return 0;
}

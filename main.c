#include <SDL.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <time.h>
#include "timer.c"
#include "time_input.c"

TTF_Font* font;
SDL_Renderer* renderer;
int window_width = 400;
int window_height = 600;

int main() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("SDL_Init Error: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Window* window = SDL_CreateWindow("Chess Timer", 0, 0, window_width, window_height, SDL_WINDOW_RESIZABLE);
  if (window == NULL) {
    printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
    return 1;
  }

  TTF_Init();
  font = TTF_OpenFont("./assets/Monocraft.ttf", 80);
  if (font == NULL) {
    printf("TTF_OpenFont Error: %s\n", TTF_GetError());
    return 1;
  }

  while (true) {
    time_input(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;

    int ret = timer_ui(window, 30 * 60);
    if (!ret) {
      TTF_CloseFont(font);
      TTF_Quit();
      SDL_DestroyWindow(window);
      SDL_Quit();
      return 0;
    }
  }
}

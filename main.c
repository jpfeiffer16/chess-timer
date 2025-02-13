#include <SDL.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <stdbool.h>

int main() {
  bool _quit;
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("SDL_Init Error: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Window* window = SDL_CreateWindow("Chess Timer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN);
  if (window == NULL) {
    printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
    return 1;
  }

  SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
  SDL_RenderClear(renderer);

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderDrawRect(renderer, &(SDL_Rect) {
    .h = 100,
    .w = 100,
    .x = 10,
    .y = 10
  });

  SDL_RenderPresent(renderer);

  SDL_Event event;
  while (true) {
    SDL_Delay(100); // TODO: Verify there's no better way to do thsi
    while (SDL_PollEvent(&event)) {
      printf("%d\n", event.type);
      if (event.type == SDL_QUIT) {
        _quit = true;
        SDL_DestroyWindow(window);
        SDL_Quit();
        break;
      }
    }

    if (_quit) return 0;
  }
}

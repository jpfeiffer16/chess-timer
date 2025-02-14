#include <SDL.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <stdbool.h>

int main() {
  bool _quit;
  int window_width = 0;
  int window_height = 0;

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("SDL_Init Error: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Window* window = SDL_CreateWindow("Chess Timer", 0, 0, 200, 500, SDL_WINDOW_RESIZABLE);
  if (window == NULL) {
    printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
    return 1;
  }

  void draw() {

    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderFillRect(renderer, &(SDL_Rect) {
      .h = window_height - 20,
      .w = window_width - 40,
      .x = 20,
      .y = 10
    });

    SDL_RenderPresent(renderer);
  }
  draw();

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
      if (event.type == SDL_WINDOWEVENT) {
	window_width = event.window.data1;
	window_height = event.window.data2;
	draw();
      }
    }

    if (_quit) return 0;
  }
}

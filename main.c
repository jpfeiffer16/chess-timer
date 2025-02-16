#include <SDL.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <stdbool.h>


void draw(SDL_Renderer *renderer, int win_width, int win_height) {
  SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
  SDL_RenderClear(renderer);

  SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
  SDL_RenderFillRect(renderer, &(SDL_Rect) {
    .w = win_width - 80,
    .h = (win_height / 2) - 20,
    .x = 10,
    .y = 10
  });

  SDL_RenderFillRect(renderer, &(SDL_Rect) {
    .w = win_width - 80,
    .h = (win_height / 2) - 20,
    .x = 10,
    .y = (win_height / 2) + 10
  });

  SDL_RenderPresent(renderer);
}

int main() {
  bool _quit;
  int window_width = 400;
  int window_height = 600;

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("SDL_Init Error: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Window* window = SDL_CreateWindow("Chess Timer", 0, 0, window_width, window_height, SDL_WINDOW_RESIZABLE);
  if (window == NULL) {
    printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
    return 1;
  }

  draw(renderer, window_width, window_height);

  SDL_Event event;
  while (true) {
    SDL_Delay(100); // TODO: Verify there's no better way to do thsi
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        _quit = true;
        SDL_DestroyWindow(window);
        SDL_Quit();
        break;
      }
      if (event.type == SDL_WINDOWEVENT) {
        printf("%d\n", event.type);
        printf("%d\n", event.window.event);
        if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED
	 || event.window.type == SDL_WINDOWEVENT_RESIZED) {
          window_width = event.window.data1;
          window_height = event.window.data2;
          draw(renderer, window_width, window_height);
        }
      }
    }

    if (_quit) return 0;
  }
}

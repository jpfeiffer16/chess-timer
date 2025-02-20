#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_timer.h>
#include <SDL_video.h>
#include <stdbool.h>
#include <time.h>
#include "layout.h"
#include "gfx.c"

SDL_Renderer* renderer;
SDL_Rect sight = { 0 };
SDL_Rect minutes_wheel = { 0 };
SDL_Rect seconds_wheel = { 0 };
extern int window_width;
extern int window_height;

int time_input_draw() {
  set_render_color(renderer, SecondaryBlack);
  SDL_RenderClear(renderer);

  if (SDL_RenderDrawRect(renderer, &sight) < 0) {
    printf("SDL_RenderDrawRect Error: %s\n", SDL_GetError());
    return -1;
  }

  SDL_RenderPresent(renderer);

  return 0;
}

void time_input_flow() {
  sight.x = padding;
  sight.y = (window_height / 2) - 40;
  sight.w = window_width - (padding * 2);
  sight.h = 80;

  minutes_wheel.x = padding;
  minutes_wheel.y = padding;
  minutes_wheel.w = (window_width / 2) - (padding + (padding / 2));
  minutes_wheel.h = window_height - (padding * 2);

  minutes_wheel.x = (window_width / 2) + (padding * 2);
  minutes_wheel.y = padding;
  minutes_wheel.w = (window_width / 2) - (padding / 2);
  minutes_wheel.h = window_height - (padding * 2);
}

time_t time_input(SDL_Window* window) {
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Event event;
  while(true) {
    SDL_Delay(50);
    while(SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        SDL_DestroyRenderer(renderer);
        return 0;
      }
      if (event.type == SDL_WINDOWEVENT) {
        if (event.type == SDL_WINDOWEVENT_EXPOSED) {
          time_input_draw();
        }
        if (event.type == SDL_WINDOWEVENT_RESIZED || event.type == SDL_WINDOWEVENT_SIZE_CHANGED) {
          time_input_flow();
          time_input_draw();
        }
      }
    }

    time_input_draw();
  }

  return 30 * 60;
}

#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_timer.h>
#include <SDL_video.h>
#include <stdbool.h>
#include <time.h>
#include "layout.h"
#include "gfx.c"

extern TTF_Font* font;
SDL_Renderer* renderer;
SDL_Rect sight = { 0 };
SDL_Rect minutes_wheel = { 0 };
SDL_Rect seconds_wheel = { 0 };
SDL_Rect submit_button = { 0 };
extern int window_width;
extern int window_height;

int time_input_draw() {
  set_render_color(renderer, SecondaryBlack);
  SDL_RenderClear(renderer);

  set_render_color(renderer, PrimaryBlack);
  if (SDL_RenderFillRect(renderer, &minutes_wheel) < 0) {
    printf("SDL_RenderFillRect Error: %s\n", SDL_GetError());
    return -1;
  }

  if (SDL_RenderFillRect(renderer, &seconds_wheel) < 0) {
    printf("SDL_RenderFillRect Error: %s\n", SDL_GetError());
    return -1;
  }

  if (SDL_RenderFillRect(renderer, &submit_button) < 0) {
    printf("SDL_RenderFillRect Error: %s\n", SDL_GetError());
    return -1;
  }

  render_text(renderer, font, "✔", PrimaryWhite,
              submit_button.x + (submit_button.w / 2),
              submit_button.y + (submit_button.h / 2), 0);

  set_render_color(renderer, PrimaryWhite);
  SDL_Rect sliding_sight = sight;
  for (uint i = 0; i < 10; i++) {
    sliding_sight.x++;
    sliding_sight.y++;
    sliding_sight.w -= 2;
    sliding_sight.h -= 2;
    if (SDL_RenderDrawRect(renderer, &sliding_sight) < 0) {
      printf("SDL_RenderDrawRect Error: %s\n", SDL_GetError());
      return -1;
    }
  }


  SDL_RenderPresent(renderer);

  return 0;
}

void time_input_flow() {
  uint button_section = window_height - (80 + padding + (padding / 2));

  sight.x = padding;
  sight.y = (button_section / 2) - 40;
  sight.w = window_width - (padding * 2);
  sight.h = 80;

  minutes_wheel.x = padding;
  minutes_wheel.y = padding;
  minutes_wheel.w = (window_width / 2) - (padding + (padding / 2));
  minutes_wheel.h = button_section - (padding + (padding / 2));

  seconds_wheel.x = (window_width / 2) + (padding / 2);
  seconds_wheel.y = padding;
  seconds_wheel.w = (window_width / 2) - (padding + (padding / 2));
  seconds_wheel.h = button_section - (padding + (padding / 2));

  submit_button.x = padding;
  submit_button.y = button_section + (padding / 2);
  submit_button.w = window_width - (padding * 2);
  submit_button.h = 80;
}

time_t time_input(SDL_Window* window) {
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
    return 1;
  }

  time_input_flow();
  time_input_draw();

  SDL_Event event;
  while(true) {
    SDL_Delay(50);
    while(SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        SDL_DestroyRenderer(renderer);
        return 0;
      }
      if (event.type == SDL_WINDOWEVENT) {
        if (event.window.event == SDL_WINDOWEVENT_EXPOSED) {
          time_input_draw();
        }
        if ( event.window.event == SDL_WINDOWEVENT_RESIZED
          || event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED
        ) {
          window_width = event.window.data1;
          window_height = event.window.data2;
          time_input_flow();
          time_input_draw();
        }
      }
    }

    time_input_draw();
  }

  return 30 * 60;
}

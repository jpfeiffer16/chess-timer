#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_timer.h>
#include <SDL_video.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include "layout.h"
#include "gfx.c"

#define MIN(a, b) (a < b ? a : b)

typedef enum {
  DRAGGING_SECONDS,
  DRAGGING_MINUTES,
  NORMAL
} t_drag_state;

typedef struct {
  SDL_Texture* texture;
  uint width;
  uint height;
} t_time_part;

extern TTF_Font* font;
extern int window_width;
extern int window_height;

SDL_Renderer* renderer;
SDL_Rect sight = { 0 };
SDL_Rect minutes_wheel = { 0 };
SDL_Rect seconds_wheel = { 0 };
SDL_Rect submit_button = { 0 };
t_time_part time_parts[60] = { 0 };
int minutes_offset = 0;
int seconds_offset = 0;
int selected_minutes = 0;
int selected_seconds = 0;
t_drag_state drag_state = NORMAL;
int max_glyph_width = 0;
int max_glyph_height = 0;
int sight_thickness = 10;

void cleanup() {
  for (int i = 0; i < 60; i++) {
    SDL_DestroyTexture(time_parts[i].texture);
  }
  SDL_DestroyRenderer(renderer);
}

t_time_part init_text(char* str) {
  t_time_part part = { 0 };

  int width, height;

  int size_result = TTF_SizeText(font, str, &width, &height);
  assert(size_result != -1);

  part.width = width;
  part.height = height;

  SDL_Surface* sfc = TTF_RenderUTF8_Solid(font, str, PrimaryWhite);
  assert(sfc != NULL);

  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, sfc);
  assert(texture != NULL);
  part.texture = texture;

  return part;
}

void set_time_part(int* offset, int time_part) {
  *offset = sight.y + sight_thickness - (time_part * (max_glyph_height + sight_thickness));
}

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

  for (int i = 0; i < 60; i++) {
    t_time_part part = time_parts[i];

    int y = i * (max_glyph_height + sight_thickness) + minutes_offset;

    if (SDL_RenderCopy(renderer, part.texture, NULL, &(SDL_Rect) {
      .x = minutes_wheel.x + sight_thickness + padding,
      .y = y,
      .w = part.width,
      .h = part.height
    }) < 0) {
      printf("SDL_RenderCopy Error: %s\n", SDL_GetError());
      return -1;
    }
  }

  for (int i = 0; i < 60; i++) {
    t_time_part part = time_parts[i];

    int y = i * (max_glyph_height + sight_thickness) + seconds_offset;

    if (SDL_RenderCopy(renderer, part.texture, NULL, &(SDL_Rect) {
      .x = (window_width / 2) + (padding / 2) + (padding),
      .y = y,
      .w = part.width,
      .h = part.height
    }) < 0) {
      printf("SDL_RenderCopy Error: %s\n", SDL_GetError());
    }
  }

  set_render_color(renderer, PrimaryWarning);
  SDL_Rect sliding_sight = sight;
  for (uint i = 0; i < sight_thickness; i++) {
    sliding_sight.x++;
    sliding_sight.y++;
    sliding_sight.w -= 2;
    sliding_sight.h -= 2;
    if (SDL_RenderDrawRect(renderer, &sliding_sight) < 0) {
      printf("SDL_RenderDrawRect Error: %s\n", SDL_GetError());
      return -1;
    }
  }

  render_text(renderer, font, ":", PrimaryWhite,
              window_width / 2 + 3, seconds_wheel.y + (seconds_wheel.h / 2), 0);

  set_render_color(renderer, SecondaryBlack);
  if (SDL_RenderFillRect(renderer, &(SDL_Rect){
    .x = 0,
    .y = 0,
    .w = window_width,
    .h = padding
  }) < 0) {
    printf("SDL_RenderFillRect error: %s\n", SDL_GetError());
    return -1;
  }
  int bottom_blank_y = seconds_wheel.y + seconds_wheel.h;
  if (SDL_RenderFillRect(renderer, &(SDL_Rect){
    .x = 0,
    .y = bottom_blank_y,
    .w = window_width,
    .h = window_height - bottom_blank_y
  }) < 0) {
    printf("SDL_RenderFillRect error: %s\n", SDL_GetError());
    return -1;
  }

  set_render_color(renderer, PrimaryBlack);
  if (SDL_RenderFillRect(renderer, &submit_button) < 0) {
    printf("SDL_RenderFillRect Error: %s\n", SDL_GetError());
    return -1;
  }

  render_text(renderer, font, "✔", PrimaryWhite,
              submit_button.x + (submit_button.w / 2),
              submit_button.y + (submit_button.h / 2), 0);


  SDL_RenderPresent(renderer);

  return 0;
}

void time_input_flow() {
  uint button_section = window_height - (80 + padding + (padding / 2));

  int sight_width = (padding * 5) + (max_glyph_width * 2) + (sight_thickness * 2);
  sight.x = (window_width / 2) - (sight_width / 2);
  sight.y = (button_section / 2) - (max_glyph_height / 2);
  sight.w = sight_width;
  sight.h = max_glyph_height + (sight_thickness * 2);

  int wheel_width = max_glyph_width + (padding * 2);
  minutes_wheel.x = sight.x + sight_thickness;
  minutes_wheel.y = padding;
  minutes_wheel.w = wheel_width;
  minutes_wheel.h = button_section - (padding + (padding / 2));

  seconds_wheel.x = (sight.x + sight_width) - wheel_width - sight_thickness;
  seconds_wheel.y = padding;
  seconds_wheel.w = wheel_width;
  seconds_wheel.h = button_section - (padding + (padding / 2));

  submit_button.x = padding;
  submit_button.y = button_section + (padding / 2);
  submit_button.w = window_width - (padding * 2);
  submit_button.h = 80;
}

time_t time_input(SDL_Window* window, time_t default_time) {
  if (default_time) {
    selected_minutes = default_time / 60;
    selected_seconds = default_time % 60;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
    return -1;
  }

  for (uint i = 0; i < 60; i++) {
    char str[3];
    snprintf(str, 3, "%.2d", i);
    t_time_part part = init_text(str);
    if (part.width > max_glyph_width) max_glyph_width = part.width;
    if (part.height > max_glyph_height) max_glyph_height = part.height;
    time_parts[i] = part;
  }

  time_input_flow();
  time_input_draw();

  SDL_Event event;
  while(true) {
    SDL_Delay(50);
    while(SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        cleanup();
        return -1;
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
        set_time_part(&minutes_offset, selected_minutes);
        set_time_part(&seconds_offset, selected_seconds);
      }

      if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == 1) {
          if(EVT_BOUNDS_CHECK(event.button, minutes_wheel)) {
            drag_state = DRAGGING_MINUTES;
          }
          if(EVT_BOUNDS_CHECK(event.button, seconds_wheel)) {
            drag_state = DRAGGING_SECONDS;
          }
        }
      }

      if (event.type == SDL_MOUSEBUTTONUP) {
        if (event.button.button == 1) {

          int *selected;
          int *offset;
          if (drag_state == DRAGGING_SECONDS || drag_state == DRAGGING_MINUTES) {

            if (drag_state == DRAGGING_SECONDS) {
              selected = &selected_seconds;
              offset = &seconds_offset;
            } else if (drag_state == DRAGGING_MINUTES) {
              selected = &selected_minutes;
              offset = &minutes_offset;
            }

            int x = sight.x + sight_thickness;
            int max_overlap = 0;
            for (int j = 0; j < 60; j++) {
              int y = j * (max_glyph_height + sight_thickness) + *offset;
              if ((PNT_BOUNDS_CHECK(x, y, sight))
               || (PNT_BOUNDS_CHECK(x, (y + max_glyph_height), sight))) {
                int overlap_top = (y + max_glyph_height) - sight.y;
                int overlap_bottom = (sight.y + sight.h) - y;
                int overlap = MIN(overlap_top, overlap_bottom);
                if (overlap > max_overlap) {
                  max_overlap = overlap;
                  *selected = j;
                }
              }
            }
            set_time_part(offset, *selected);

            drag_state = NORMAL;
          } else if (drag_state == NORMAL) {
            if (EVT_BOUNDS_CHECK(event.button, submit_button)) {
              cleanup();
              return (selected_minutes * 60) + selected_seconds;
            }
          }
        }
      }

      if (event.type == SDL_MOUSEMOTION) {
        if (drag_state == DRAGGING_SECONDS) {
          seconds_offset += event.motion.yrel;
        }
        if (drag_state == DRAGGING_MINUTES) {
          minutes_offset += event.motion.yrel;
        }
      }
    }

    time_input_draw();
  }
}

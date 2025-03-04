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
int selected_seconds = 0;
t_drag_state drag_state = NORMAL;
int max_glyph_width = 0;
int max_glyph_height = 0;
int sight_thickness = 10;

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
  printf("setting time_part\n");
  *offset = sight.y + sight_thickness - (time_part * (max_glyph_height + sight_thickness));
  printf("%d\n", *offset);
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

  /* int total_height = max_glyph_height * 60; */

  for (int i = 0; i < 60; i++) {
    t_time_part part = time_parts[i];

    int y = i * (max_glyph_height + sight_thickness) + seconds_offset;

    SDL_RenderCopy(renderer, part.texture, NULL, &(SDL_Rect) {
      .x = (window_width / 2) + (padding / 2) + (padding),
      .y = y,
      .w = part.width,
      .h = part.height
    });

    /* set_render_color(renderer, PrimaryWhite); */
    /* SDL_RenderDrawRect(renderer, &(SDL_Rect){ */
    /*   .x = (window_width / 2) + (padding / 2), */
    /*   .y = y, */
    /*   .w = part.width, */
    /*   .h = part.height */
    /* });  */
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
  SDL_RenderFillRect(renderer, &(SDL_Rect){
    .x = 0,
    .y = 0,
    .w = window_width,
    .h = padding
  });
  int bottom_blank_y = seconds_wheel.y + seconds_wheel.h;
  SDL_RenderFillRect(renderer, &(SDL_Rect){
    .x = 0,
    .y = bottom_blank_y,
    .w = window_width,
    .h = window_height - bottom_blank_y
  });

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

  sight.x = (window_width / 2) - ((padding * 3) / 2) - max_glyph_width - padding - sight_thickness;
  sight.y = (button_section / 2) - 40;
  sight.w = (max_glyph_width * 2) + (padding * 5) + (sight_thickness * 2);
  sight.h = max_glyph_height + (sight_thickness * 2);

  minutes_wheel.x = sight.x + sight_thickness;
  minutes_wheel.y = padding;
  minutes_wheel.w = max_glyph_width + (padding * 2);
  minutes_wheel.h = button_section - (padding + (padding / 2));

  seconds_wheel.x = (window_width / 2) + (padding);
  seconds_wheel.y = padding;
  seconds_wheel.w = max_glyph_width + (padding * 2);
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
        SDL_DestroyRenderer(renderer);
        for (uint i = 0; i < 60; i++) {
          SDL_DestroyTexture(time_parts[i].texture);
        }
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
        set_time_part(&seconds_offset, selected_seconds);
      }

      if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == 1) {
          if(EVT_BOUNDS_CHECK(event.button, seconds_wheel)) {
            drag_state = DRAGGING_SECONDS;
          }
        }
      }

      if (event.type == SDL_MOUSEBUTTONUP) {
        if (event.button.button == 1) {
          if (drag_state == DRAGGING_SECONDS) {
            /* seconds_offset = round(((double)seconds_offset / max_glyph_height)) * max_glyph_height; */
            int x = sight.x + sight_thickness;
            int max_overlap = 0;
            for (int j = 0; j < 60; j++) {
              int y = j * (max_glyph_height + sight_thickness) + seconds_offset;
              if ((PNT_BOUNDS_CHECK(x, y, sight))
               || (PNT_BOUNDS_CHECK(x, y + max_glyph_height, sight))) {
                
                int overlap_top = (y + max_glyph_height) - sight.y;
                int overlap_bottom = (sight.y + sight.h) - y;
                printf("i: %d, t: %d, b: %d, total: %d\n", j, overlap_top, overlap_bottom, MIN(overlap_top, overlap_bottom));
                int overlap = MIN(overlap_top, overlap_bottom);
                if (overlap > max_overlap) {
                  max_overlap = overlap;
                  selected_seconds = j;
                }
              }
            }
            printf("max: %d, selected_idx: %d\n", max_overlap, selected_seconds);
            set_time_part(&seconds_offset, selected_seconds);
          }
          drag_state = NORMAL;
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

  return 30 * 60;
}

#include <SDL.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <time.h>
#include "gfx.c"
#include "layout.h"

#define MIRROR 1

typedef enum {
  WHITE_RUNNING,
  BLACK_RUNNING,
  PAUSED
} t_mode;
typedef enum {
  WHITE_BOTTOM,
  BLACK_BOTTOM,
} t_orientation;

extern TTF_Font* font;
SDL_Renderer* renderer;
extern int window_width;
extern int window_height;
time_t prev_time;
time_t white_timer;
time_t black_timer;
t_mode mode = PAUSED;
t_mode prev_mode = PAUSED;
t_orientation orientation = WHITE_BOTTOM;
Uint32 wav_length;
Uint32 wav_pos;
Uint8 *wav_buffer;
SDL_Rect top_button = { 0 };
SDL_Rect pause_button = { 0 };
SDL_Rect flip_button = { 0 };
SDL_Rect bottom_button = { 0 };

void timer_flow() {
  top_button.w = window_width - (padding * 2);
  top_button.h = (window_height / 2) - 40 - (padding * 2);
  top_button.x = padding;
  top_button.y = padding;

  pause_button.w = (window_width / 2) - (padding + (padding / 2));
  pause_button.h = 80;
  pause_button.x = padding;
  pause_button.y = (window_height / 2) - 40;

  flip_button.w = (window_width / 2) - (padding + (padding / 2));
  flip_button.h = 80;
  flip_button.x = (window_width / 2) + (padding / 2);
  flip_button.y = (window_height / 2) - 40;

  bottom_button.w = window_width - (padding * 2);
  bottom_button.h = (window_height / 2) - 40 - (padding * 2);
  bottom_button.x = padding;
  bottom_button.y = (window_height / 2) + 40 + padding;
}

int timer_draw() {
  set_render_color(renderer, SecondaryBlack);
  SDL_RenderClear(renderer);

  SDL_Color top_fg;
  SDL_Color top_bg;
  SDL_Color bottom_fg;
  SDL_Color bottom_bg;

  if (orientation == WHITE_BOTTOM) {
    top_fg = PrimaryWhite;
    top_bg = PrimaryBlack;
    bottom_fg = PrimaryBlack;
    bottom_bg = PrimaryWhite;
    if (black_timer < 60) {
      top_bg = PrimaryWarning;
    }
    if (black_timer < 1) {
      top_bg = PrimaryFlag;
    }
    if (white_timer < 60) {
      bottom_bg = PrimaryWarning;
    }
    if (white_timer < 1) {
      bottom_bg = PrimaryFlag;
    }
  } else if (orientation == BLACK_BOTTOM) {
    top_fg = PrimaryBlack;
    top_bg = PrimaryWhite;
    bottom_fg = PrimaryWhite;
    bottom_bg = PrimaryBlack;
    if (white_timer < 60) {
      top_bg = PrimaryWarning;
    }
    if (white_timer < 1) {
      top_bg = PrimaryFlag;
    }
    if (black_timer < 60) {
      bottom_bg = PrimaryWarning;
    }
    if (black_timer < 1) {
      bottom_bg = PrimaryFlag;
    }
  }

  set_render_color(renderer, top_bg);
  SDL_RenderFillRect(renderer, &top_button);

  set_render_color(renderer, PrimaryBlack);
  SDL_RenderFillRect(renderer, &pause_button);
  SDL_RenderFillRect(renderer, &flip_button);

  set_render_color(renderer, bottom_bg);
  SDL_RenderFillRect(renderer, &bottom_button);

  char* pause_icon = (mode == PAUSED) ? "▶" : "⏸";
  render_text(renderer, font,
              pause_icon, PrimaryWhite,
              pause_button.x + (pause_button.w / 2),
              pause_button.y + (pause_button.h / 2),
              0);

  render_text(renderer, font,
              "⇵", PrimaryWhite,
              flip_button.x + (flip_button.w / 2),
              flip_button.y + (flip_button.h / 2),
              0);


  char white_time_str[6];
  snprintf( white_time_str, 6, "%.2d:%.2d",
    ((uint)white_timer) / 60 % 60,
    ((uint)white_timer) % 60);

  char black_time_str[6];
  snprintf(black_time_str, 6, "%.2d:%.2d",
    ((uint)black_timer) / 60 % 60,
    ((uint)black_timer) % 60);

  char *top_time = orientation == WHITE_BOTTOM ? black_time_str : white_time_str;
  char *bottom_time = orientation == WHITE_BOTTOM ? white_time_str : black_time_str;
  render_text(renderer, font,
              top_time, top_fg,
              top_button.x + (top_button.w / 2),
              top_button.y + (top_button.h / 2),
#ifdef MIRROR
              180
#else
              0
#endif
  );
  render_text(renderer, font,
              bottom_time, bottom_fg,
              bottom_button.x + (bottom_button.w / 2),
              bottom_button.y + (bottom_button.h / 2),
              0);

  SDL_RenderPresent(renderer);

  return 1;
}

void audio_cb(void *userdata, Uint8 *stream, int len) {
  if (!(wav_pos < wav_length)) {
    SDL_memset(stream, 0, len);
    return;
  }

  uint delta = wav_length - wav_pos;

  len = (len > delta ? delta: len);
  SDL_memcpy(stream, wav_buffer + wav_pos, len);

  wav_pos += len;
}

int timer_ui(SDL_Window* window, time_t clock_time) {
  prev_time = time(NULL);
  if (prev_time == -1) {
    printf("time() error: failed to obtain current time.");
    return -1;
  }
  white_timer = clock_time;
  black_timer = clock_time;

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
    return -1;
  }

  SDL_AudioSpec wav_spec;

  if (SDL_LoadWAV("./assets/timer_click.wav", &wav_spec, &wav_buffer, &wav_length) == NULL) {
    printf("SDL_LoadWAV Error: %s\n", SDL_GetError());
    return -1;
  }

  wav_pos = wav_length;
  wav_spec.callback = audio_cb;
  wav_spec.userdata = NULL;

  if (SDL_OpenAudio(&wav_spec, NULL) < 0) {
    printf("SDL_OpenAudio Error: %s", SDL_GetError());
    return -1;
  }

  SDL_PauseAudio(0);

  timer_flow();
  if (!timer_draw()) {
    printf("Unrecoverable error in draw() function. Exiting.");
    return -1;
  }

  SDL_Event event;
  while (true) {
    SDL_Delay(50); // TODO: Verify there's no better way to do this
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        SDL_DestroyRenderer(renderer);
        SDL_CloseAudio();
        SDL_FreeWAV(wav_buffer);
        return 0;
      }
      if (event.type == SDL_WINDOWEVENT) {
        if ( event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED
         || event.window.type == SDL_WINDOWEVENT_RESIZED) {
          window_width = event.window.data1;
          window_height = event.window.data2;
          timer_flow();
        }
        if (event.window.event == SDL_WINDOWEVENT_EXPOSED) {
          // For now, force a draw. May be overkill. Remove if not needed.
          if (!timer_draw()) {
            printf("Unrecoverable error in draw() function. Exiting.");
            return -1;
          }
        }
      }
      if (event.type == SDL_MOUSEBUTTONUP) {
        if (event.button.button == 1) { // Left click
          if (EVT_BOUNDS_CHECK(event.button, pause_button)) {
            t_mode cur_mode = mode;
            if (mode != PAUSED) {
              mode = PAUSED;
            } else {
              mode = prev_mode;
            }
            prev_mode = cur_mode;
          }

          if (EVT_BOUNDS_CHECK(event.button, flip_button)) {
            if (orientation == WHITE_BOTTOM)  {
              orientation = BLACK_BOTTOM;
            } else {
              orientation = WHITE_BOTTOM;
            }
          }

          if (EVT_BOUNDS_CHECK(event.button, bottom_button)) {
            prev_mode = mode;
            if (orientation == WHITE_BOTTOM) {
              if (mode != BLACK_RUNNING) {
                wav_pos = 0;
              }
              mode = BLACK_RUNNING;
            } else {
              if (mode != WHITE_RUNNING) {
                wav_pos = 0;
              }
              mode = WHITE_RUNNING;
            }
          }

          if (EVT_BOUNDS_CHECK(event.button, top_button)) {
            prev_mode = mode;
            if (orientation ==  WHITE_BOTTOM) {
              if (mode != WHITE_RUNNING) {
                wav_pos = 0;
              }
              mode = WHITE_RUNNING;
            } else {
              if (mode != BLACK_RUNNING) {
                wav_pos = 0;
              }
              mode = BLACK_RUNNING;
            }
          }
        }
      }
    }

    time_t cur_time = time(NULL);
    int delta = cur_time - prev_time;
    if (mode == WHITE_RUNNING) {
      if (white_timer > 0) {
        white_timer -= delta;
      }
    } else if (mode == BLACK_RUNNING) {
      if (black_timer > 0) {
        black_timer -= delta;
      }
    }
    if (delta) {
      prev_time = cur_time;
    }
    if (!timer_draw()) {
      printf("Unrecoverable error in draw() loop. Exiting.");
      return -1;
    }
  }
}

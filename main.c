#include <SDL.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <time.h>

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

TTF_Font* font;
SDL_Renderer* renderer;
int window_width = 400;
int window_height = 600;
time_t prev_time;
time_t white_timer;
time_t black_timer;
t_mode mode = PAUSED;
t_mode prev_mode = PAUSED;
t_orientation orientation = WHITE_BOTTOM;
Uint32 wav_length;
Uint32 wav_orig_length;
Uint8 *wav_pos;
Uint8 *wav_buffer;
SDL_Color PrimaryWhite = {
  .r = 200,
  .g = 200,
  .b = 200 ,
  .a = 255
};
SDL_Color PrimaryBlack = {
  .r = 50,
  .g = 50,
  .b = 50 ,
  .a = 255
};
SDL_Color SecondaryBlack = {
  .r = 30,
  .g = 30,
  .b = 30 ,
  .a = 255
};
SDL_Color PrimaryWarning = {
  .r = 180,
  .g = 133,
  .b = 50,
  .a = 255
};
SDL_Color PrimaryFlag = {
  .r = 100,
  .g = 30,
  .b = 30 ,
  .a = 255
};
SDL_Rect top_button = { 0 };
SDL_Rect pause_button = { 0 };
SDL_Rect flip_button = { 0 };
SDL_Rect bottom_button = { 0 };

static inline void set_render_color(SDL_Color color) {
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

int render_text(char str[], SDL_Color* color, int x, int y, int angle) {
  SDL_Surface* s_timer = TTF_RenderUTF8_Solid(
    font,
    str,
    *color);
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, s_timer);

  SDL_FreeSurface(s_timer);
  if (!s_timer) {
    return 0;
  }

  int width, height;

  if (TTF_SizeText(font, str, &width, &height) == -1) {
    printf("TFF_SizeFont Error: %s\n", TTF_GetError());
    return 0;
  }

  if (SDL_RenderCopyEx(renderer, texture, NULL, &(SDL_Rect){
    .w = width,
    .h = height,
    .x = x - (width / 2),
    .y = y - (height / 2),
  }, angle, NULL, SDL_FLIP_NONE) == -1) {
    printf("SDL_RenderCopyEx error: unable to render text: %s", SDL_GetError());
    return 0;
  }

  SDL_DestroyTexture(texture);

  return 1;
}

void flow() {
  const int padding = 10;

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

int draw() {
  set_render_color(SecondaryBlack);
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

  set_render_color(top_bg);
  SDL_RenderFillRect(renderer, &top_button);

  set_render_color(PrimaryBlack);
  SDL_RenderFillRect(renderer, &pause_button);
  SDL_RenderFillRect(renderer, &flip_button);

  set_render_color(bottom_bg);
  SDL_RenderFillRect(renderer, &bottom_button);

  char* pause_icon = (mode == PAUSED) ? "▶" : "⏸";
  render_text(pause_icon, &PrimaryWhite,
              pause_button.x + (pause_button.w / 2),
              pause_button.y + (pause_button.h / 2),
              0);

  render_text("⇵", &PrimaryWhite,
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
  render_text(top_time, &top_fg,
              top_button.x + (top_button.w / 2),
              top_button.y + (top_button.h / 2),
#ifdef MIRROR
              180
#else
              0
#endif
              );
  render_text(bottom_time, &bottom_fg,
              bottom_button.x + (bottom_button.w / 2),
              bottom_button.y + (bottom_button.h / 2),
              0);

  SDL_RenderPresent(renderer);

  return 1;
}

void audio_cb(void *userdata, Uint8 *stream, int len) {
  if (wav_length == 0) {
    SDL_memset(stream, 0, len);
    return;
  }

  len = (len > wav_length ? wav_length : len);
  SDL_memcpy (stream, wav_pos, len);

  wav_pos += len;
  wav_length -= len;
}

int main() {
  bool _quit;
  prev_time = time(NULL);
  if (prev_time == -1) {
    printf("time() error: failed to obtain current time.");
    return 0;
  }
  white_timer = 60 * 30;
  black_timer = 60 * 30;

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("SDL_Init Error: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Window* window = SDL_CreateWindow("Chess Timer", 0, 0, window_width, window_height, SDL_WINDOW_RESIZABLE);
  if (window == NULL) {
    printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
    return 1;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
    return 1;
  }

  TTF_Init();
  font = TTF_OpenFont("./assets/Monocraft.ttf", 80);
  if (font == NULL) {
    printf("TTF_OpenFont Error: %s\n", TTF_GetError());
    return 1;
  }

  SDL_AudioSpec wav_spec;

  if (SDL_LoadWAV("./assets/timer_click.wav", &wav_spec, &wav_buffer, &wav_length) == NULL) {
    printf("SDL_LoadWAV Error: %s\n", SDL_GetError());
    return 1;
  }

  wav_orig_length = wav_length;
  wav_length = 0;

  wav_pos = wav_buffer;
  wav_spec.callback = audio_cb;
  wav_spec.userdata = NULL;

  if (SDL_OpenAudio(&wav_spec, NULL) < 0) {
    printf("SDL_OpenAudio Error: %s", SDL_GetError());
    return 1;
  }

  SDL_PauseAudio(0);

  flow();
  if (!draw()) {
    printf("Unrecoverable error in draw() function. Exiting.");
    return 1;
  }

  SDL_Event event;
  while (true) {
    SDL_Delay(50); // TODO: Verify there's no better way to do this
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        _quit = true;
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_CloseAudio();
        SDL_FreeWAV(wav_buffer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        break;
      }
      if (event.type == SDL_WINDOWEVENT) {
        if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED
         || event.window.type == SDL_WINDOWEVENT_RESIZED) {
          window_width = event.window.data1;
          window_height = event.window.data2;
	  flow();
        }
        if (event.window.event == SDL_WINDOWEVENT_EXPOSED) {
          // For now, force a draw. May be overkill. Remove if not needed.
          if (!draw()) {
            printf("Unrecoverable error in draw() function. Exiting.");
            return 1;
          }
        }
      }
      if (event.type == SDL_MOUSEBUTTONUP) {
        if (event.button.button == 1) { // Left click
          if (event.button.x > pause_button.x
           && event.button.x < pause_button.x + pause_button.w
           && event.button.y > pause_button.y
           && event.button.y < pause_button.y + pause_button.h) {
            t_mode cur_mode = mode;
            if (mode != PAUSED) {
              mode = PAUSED;
            } else {
              mode = prev_mode;
            }
            prev_mode = cur_mode;
          }

          if (event.button.x > flip_button.x
           && event.button.x < flip_button.x + flip_button.w
           && event.button.y > flip_button.y
           && event.button.y < flip_button.y + flip_button.h) {
            if (orientation == WHITE_BOTTOM)  {
              orientation = BLACK_BOTTOM;
            } else {
              orientation = WHITE_BOTTOM;
            }
          }

          if (event.button.x > bottom_button.x
           && event.button.x < bottom_button.x + bottom_button.w
           && event.button.y > bottom_button.y
           && event.button.y < bottom_button.y + bottom_button.h) {
            prev_mode = mode;
            if (orientation == WHITE_BOTTOM) {
              if (mode != BLACK_RUNNING) {
                wav_length = wav_orig_length;
                wav_pos = wav_buffer;
              }
              mode = BLACK_RUNNING;
            } else {
              if (mode != WHITE_RUNNING) {
                wav_length = wav_orig_length;
                wav_pos = wav_buffer;
              }
              mode = WHITE_RUNNING;
            }
          }

          if (event.button.x > top_button.x
           && event.button.x < top_button.x + top_button.w
           && event.button.y > top_button.y
           && event.button.y < top_button.y + top_button.h) {
            prev_mode = mode;
            if (orientation ==  WHITE_BOTTOM) {
              if (mode != WHITE_RUNNING) {
                wav_length = wav_orig_length;
                wav_pos = wav_buffer;
              }
              mode = WHITE_RUNNING;
            } else {
              if (mode != BLACK_RUNNING) {
                wav_length = wav_orig_length;
                wav_pos = wav_buffer;
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
    if (!draw()) {
      printf("Unrecoverable error in draw() loop. Exiting.");
      return 1;
    }

    if (_quit) return 0;
  }
}

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

TTF_Font* timer_font;
TTF_Font* button_font;
SDL_Renderer* renderer;
int window_width = 400;
int window_height = 600;
time_t prev_time;
time_t white_timer;
time_t black_timer;
t_mode mode = PAUSED;
t_mode prev_mode = PAUSED;
t_orientation orientation = WHITE_BOTTOM;
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
SDL_Rect top_box = { 0 };
SDL_Rect pause_box = { 0 };
SDL_Rect flip_box = { 0 };
SDL_Rect bottom_box = { 0 };

static inline void set_render_color(SDL_Color color) {
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

int render_text(char str[], SDL_Color* color, int x, int y, int angle) {
  SDL_Surface* s_timer = TTF_RenderUTF8_Solid(
    timer_font,
    str,
    *color);
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, s_timer);

  SDL_FreeSurface(s_timer);
  if (!s_timer) {
    return 0;
  }

  int width, height;

  if (TTF_SizeText(timer_font, str, &width, &height) == -1) {
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

int draw() {
  const int padding = 10;
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
  top_box.w = window_width - (padding * 2);
  top_box.h = (window_height / 2) - 40 - (padding * 2);
  top_box.x = padding;
  top_box.y = padding;
  SDL_RenderFillRect(renderer, &top_box);

  set_render_color(PrimaryBlack);
  pause_box.w = (window_width / 2) - (padding + (padding / 2));
  pause_box.h = 80;
  pause_box.x = padding;
  pause_box.y = (window_height / 2) - 40;
  SDL_RenderFillRect(renderer, &pause_box);


  flip_box.w = (window_width / 2) - (padding + (padding / 2));
  flip_box.h = 80;
  flip_box.x = (window_width / 2) + (padding / 2);
  flip_box.y = (window_height / 2) - 40;
  SDL_RenderFillRect(renderer, &flip_box);

  set_render_color(bottom_bg);
  bottom_box.w = window_width - (padding * 2);
  bottom_box.h = (window_height / 2) - 40 - (padding * 2);
  bottom_box.x = padding;
  bottom_box.y = (window_height / 2) + 40 + padding;
  SDL_RenderFillRect(renderer, &bottom_box);

  char* pause_icon = "⏸";
  if (mode == PAUSED) {
    pause_icon = "▶";
  }
  render_text(pause_icon, &PrimaryWhite,
              pause_box.x + (pause_box.w / 2),
              pause_box.y + (pause_box.h / 2),
              0);

  render_text("⇅", &PrimaryWhite,
              flip_box.x + (flip_box.w / 2),
              flip_box.y + (flip_box.h / 2),
              0);


  char white_time_str[6];
  uint white_minutes = ((uint)white_timer) / 60;
  uint white_seconds = ((uint)white_timer) % 60;
  snprintf(white_time_str, 6, "%.2d:%.2d", white_minutes % 60, white_seconds % 60);

  char black_time_str[6];
  uint black_minutes = ((uint)black_timer) / 60;
  uint black_seconds = ((uint)black_timer) % 60;
  snprintf(black_time_str, 6, "%.2d:%.2d", black_minutes % 60, black_seconds % 60);

  if (orientation == WHITE_BOTTOM) {
    render_text(black_time_str, &top_fg,
                top_box.w / 2 + padding,
                (top_box.h / 2) + padding,
#ifdef MIRROR
                180
#else
                0
#endif
                );
    render_text(white_time_str, &bottom_fg,
                bottom_box.x + (bottom_box.w / 2),
                bottom_box.y + (bottom_box.h / 2),
                0);

  } else if (orientation == BLACK_BOTTOM) {
    render_text(white_time_str, &top_fg,
                top_box.w / 2 + padding,
                (top_box.h / 2) + padding,
#ifdef MIRROR
                180
#else
                0
#endif
                );
    render_text(black_time_str, &bottom_fg,
                bottom_box.x + (bottom_box.w / 2),
                bottom_box.y + (bottom_box.h / 2),
                0);
  }

  SDL_RenderPresent(renderer);

  return 1;
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
  timer_font = TTF_OpenFont("./assets/Monocraft.ttf", 80);
  if (timer_font == NULL) {
    printf("TTF_OpenFont Error: %s\n", TTF_GetError());
    return 1;
  }

  button_font = TTF_OpenFont("./assets/unicode.impact.ttf", 80);
  if (timer_font == NULL) {
    printf("TTF_OpenFont Error: %s\n", TTF_GetError());
    return 1;
  }

  if (!draw()) {
    printf("Unrecoverable error in draw() loop. Exiting.");
    return 1;
  }

  SDL_Event event;
  while (true) {
    SDL_Delay(50); // TODO: Verify there's no better way to do this
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        _quit = true;
        TTF_CloseFont(timer_font);
        TTF_CloseFont(button_font);
        TTF_Quit();
        SDL_DestroyWindow(window);
        SDL_Quit();
        break;
      }
      if (event.type == SDL_WINDOWEVENT) {
        if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED
         || event.window.type == SDL_WINDOWEVENT_RESIZED) {
          window_width = event.window.data1;
          window_height = event.window.data2;
        }
        if (event.window.event == SDL_WINDOWEVENT_EXPOSED) {
          // For now, force a draw. May be overkill. Remove if not needed.
          if (!draw()) {
            printf("Unrecoverable error in draw() loop. Exiting.");
            return 1;
          }
        }
      }
      if (event.type == SDL_MOUSEBUTTONUP) {
        if (event.button.button == 1) { // Left click
          if (event.button.x > pause_box.x
           && event.button.x < pause_box.x + pause_box.w
           && event.button.y > pause_box.y
           && event.button.y < pause_box.y + pause_box.h) {
            t_mode cur_mode = mode;
            if (mode != PAUSED) {
              mode = PAUSED;
            } else {
              mode = prev_mode;
            }
            prev_mode = cur_mode;
          }

          if (event.button.x > flip_box.x
           && event.button.x < flip_box.x + flip_box.w
           && event.button.y > flip_box.y
           && event.button.y < flip_box.y + flip_box.h) {
            if (orientation == WHITE_BOTTOM)  {
              orientation = BLACK_BOTTOM;
            } else {
              orientation = WHITE_BOTTOM;
            }
          }

          if (event.button.x > bottom_box.x
           && event.button.x < bottom_box.x + bottom_box.w
           && event.button.y > bottom_box.y
           && event.button.y < bottom_box.y + bottom_box.h) {
            prev_mode = mode;
            if (orientation == WHITE_BOTTOM) {
              mode = BLACK_RUNNING;
            } else {
              mode = WHITE_RUNNING;
            }
          }

          if (event.button.x > top_box.x
           && event.button.x < top_box.x + top_box.w
           && event.button.y > top_box.y
           && event.button.y < top_box.y + top_box.h) {
            prev_mode = mode;
            if (orientation ==  WHITE_BOTTOM) {
              mode = WHITE_RUNNING;
            } else {
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

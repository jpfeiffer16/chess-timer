#include <SDL.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <time.h>

TTF_Font* timer_font;
TTF_Font* button_font;
SDL_Renderer* renderer;
int window_width = 400;
int window_height = 600;
time_t prev_time;
time_t timer_time;

typedef enum {
  WHITE_RUNNING,
  BLACK_RUNNING,
  PAUSED
} t_mode;

int render_text(char str[], int x, int y) {
  SDL_Surface* s_timer1 = TTF_RenderUTF8_Solid(
    timer_font,
    str,
    (SDL_Color) {
      .r = 200,
      .g = 200,
      .b = 200 
    });
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, s_timer1);

  SDL_FreeSurface(s_timer1);
  if (!s_timer1) {
    return 0;
  }

  int width, height;

  if (TTF_SizeText(timer_font, str, &width, &height) == -1) {
    printf("TFF_SizeFont Error: %s\n", TTF_GetError());
    return 0;
  }

  SDL_RenderCopy(renderer, texture, NULL, &(SDL_Rect){
    .w = width,
    .h = height,
    .x = x - (width / 2),
    .y = y - (height / 2),
  });

  SDL_DestroyTexture(texture);

  return 1;
}

int draw() {
  const int padding = 10;
  SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
  SDL_RenderClear(renderer);

  SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
  SDL_Rect top_box = {
    .w = window_width - (padding * 2),
    .h = (window_height / 2) - 40 - (padding * 2),
    .x = padding,
    .y = padding
  };
  SDL_RenderFillRect(renderer, &top_box);

  SDL_Rect pause_box = {
    .w = (window_width / 2) - (padding + (padding / 2)),
    .h = 80,
    .x = padding,
    .y = (window_height / 2) - 40
  };
  SDL_RenderFillRect(renderer, &pause_box);


  SDL_Rect flip_box = {
    .w = (window_width / 2) - (padding + (padding / 2)),
    .h = 80,
    .x = (window_width / 2) + (padding / 2),
    .y = (window_height / 2) - 40
  };
  SDL_RenderFillRect(renderer, &flip_box);

  SDL_Rect bottom_box = {
    .w = window_width - (padding * 2),
    .h = (window_height / 2) - 40 - (padding * 2),
    .x = padding,
    .y = (window_height / 2) + 40 + padding
  };
  SDL_RenderFillRect(renderer, &bottom_box);

  render_text("▶", pause_box.x + (pause_box.w / 2), pause_box.y + (pause_box.h / 2));

  render_text("⇅", flip_box.x + (flip_box.w / 2), flip_box.y + (flip_box.h / 2));


  char time_str[6];
  uint minutes = ((uint)timer_time) / 60;
  uint seconds = ((uint)timer_time) % 60;
  snprintf(time_str, 6, "%.2d:%.2d", minutes % 60, seconds % 60);

  render_text(time_str, top_box.w / 2, top_box.h / 2);

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
  timer_time = 30 * 60;

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
    }

    time_t cur_time = time(NULL);
    int delta = cur_time - prev_time;
    timer_time -= delta;
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

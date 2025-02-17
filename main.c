#include <SDL.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <stdbool.h>

TTF_Font* font;
SDL_Renderer* renderer;

SDL_Texture* get_texture(char str[]) {
  SDL_Surface* s_timer1 = TTF_RenderText_Solid(
    font,
    str,
    (SDL_Color) {
      .r = 200,
      .g = 200,
      .b = 200 
    });
  SDL_Texture* text_timer1 = SDL_CreateTextureFromSurface(renderer, s_timer1);

  SDL_FreeSurface(s_timer1);

  return text_timer1;
}

int draw(int win_width, int win_height) {
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

  SDL_Texture* texture = get_texture("this is a test");
  if (!texture) {
    return 0;
  }

  int width, height;

  if (TTF_SizeText(font, "this is a test", &width, &height) == -1) {
    printf("TFF_SizeFont Error: %s\n", TTF_GetError());
    return 0;
  }

  printf("%d, %d", width, height);

  SDL_RenderCopy(renderer, texture, NULL, &(SDL_Rect){
      .w = width,
      .h = height,
      .x = 10,
      .y = 10,
  });

  SDL_DestroyTexture(texture);

  SDL_RenderPresent(renderer);

  return 1;
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

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
    return 1;
  }

  TTF_Init();
  font = TTF_OpenFont("./assets/unicode.impact.ttf", 100);
  if (font == NULL) {
    printf("TTF_OpenFont Error: %s\n", TTF_GetError());
    return 1;
  }

  if (!draw(window_width, window_height)) {
    printf("Unrecoverable error in draw() loop. Exiting.");
    return 1;
  }

  SDL_Event event;
  while (true) {
    SDL_Delay(50); // TODO: Verify there's no better way to do thsi
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        _quit = true;
        TTF_CloseFont(font);
        TTF_Quit();
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
          if (!draw(window_width, window_height)) {
            printf("Unrecoverable error in draw() loop. Exiting.");
            return 1;
          }
        }
      }
    }

    if (_quit) return 0;
  }
}

#include <SDL.h>
#include <SDL_render.h>
#include <SDL_ttf.h>

#ifndef gfx_loaded
#define EVT_BOUNDS_CHECK(evt, btn) (evt.x > btn.x \
                      && evt.x < btn.x + btn.w \
                      && evt.y > btn.y \
                      && evt.y < btn.y + btn.h)

#define PNT_BOUNDS_CHECK(x_pnt, y_pnt, bounds) (x_pnt > bounds.x \
                      && x_pnt < bounds.x + bounds.w \
                      && y_pnt > bounds.y \
                      && y_pnt < bounds.y + bounds.h)

#define SET_RENDER_COLOR(renderer_ptr, color_struct) do { \
  SDL_SetRenderDrawColor(renderer_ptr, color_struct.r, \
                                       color_struct.g, \
                                       color_struct.b, \
                                       color_struct.a); \
} while(false);


int render_text(
  SDL_Renderer* renderer,
  TTF_Font* font,
  char str[],
  SDL_Color color,
  int x,
  int y,
  int angle
) {
  SDL_Surface* s_timer = TTF_RenderUTF8_Solid(
    font,
    str,
    color);
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
    printf("SDL_RenderCopyEx Error: unable to render text: %s\n", SDL_GetError());
    return 0;
  }

  SDL_DestroyTexture(texture);

  return 1;
}
#endif
#define gfx_loaded

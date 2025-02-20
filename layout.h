#include "SDL_pixels.h"

#ifndef layout_loaded
const int padding = 10;

const SDL_Color PrimaryWhite = {
  .r = 200,
  .g = 200,
  .b = 200 ,
  .a = 255
};
const SDL_Color PrimaryBlack = {
  .r = 50,
  .g = 50,
  .b = 50 ,
  .a = 255
};
const SDL_Color SecondaryBlack = {
  .r = 30,
  .g = 30,
  .b = 30 ,
  .a = 255
};
const SDL_Color PrimaryWarning = {
  .r = 180,
  .g = 133,
  .b = 50,
  .a = 255
};
const SDL_Color PrimaryFlag = {
  .r = 100,
  .g = 30,
  .b = 30 ,
  .a = 255
};
#endif

#define layout_loaded

#include <SDL.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <time.h>
#include <sys/stat.h>
#include <errno.h>
#include "gfx.c"
#include "layout.h"

#ifndef state_loaded

#define PATH_MAX_LEN 300

typedef struct {
  time_t time;
} t_app_state;

int ensure_config_dir() {
  char *config_dir = getenv("HOME");
  if (config_dir == NULL) {
    return -1;
  }
  char path[PATH_MAX_LEN] = { 0 };
  snprintf(path, PATH_MAX_LEN - 1, "%s/.local/state/chess-timer", config_dir);
  struct stat stats;
  if (stat(path, &stats) != 0) {
    if (mkdir(path, 0740) < 0) {
      printf("mkdir Error. ERRORNO: %d\n", errno);
      return -1;
    }
  }
  if (!S_ISDIR(stats.st_mode)) {
    printf(
      "Error, config path is not a directory: ERRORNO: %d\n", errno);
    return -1;
  }

  return 0;
}

t_app_state get_app_state() {
  char path[PATH_MAX_LEN] = { 0 };

  snprintf(
    path, PATH_MAX_LEN - 1,
    "%s/.local/state/chess-timer/state.bin",
    getenv("HOME"));

  t_app_state state = { 0 };

  FILE *config_file;
  if ((config_file = fopen(path, "r")) == NULL) {
    printf(
      "Did not find state file. Using defaults. ERRNO: %d\n", errno);
    return state;
  }

  // Not checking return values here, since we'll just fall back to the default values if we fail to read for any reason.
  fread(&state, sizeof(t_app_state), 1, config_file);
  fclose(config_file);

  return state;
}

int set_app_state(t_app_state state) {
  if (ensure_config_dir() < 0) {
    printf("ensure_config_dir error: %s\n", SDL_GetError());
    return -1;
  }
  char path[PATH_MAX_LEN] = { 0 };

  snprintf(
    path, PATH_MAX_LEN - 1,
    "%s/.local/state/chess-timer/state.bin",
    getenv("HOME"));

  FILE *config_file;
  if ((config_file = fopen(path, "w")) == NULL) {
    printf(
      "fopen Error. Error writing to config file. ERRNO: %d\n", errno);
    return -1;
  }

  fwrite(&state, sizeof(t_app_state), 1, config_file);
  fclose(config_file);

  return 0;
}

#endif
#define state_loaded

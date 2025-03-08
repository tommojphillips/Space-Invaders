/* window_sdl2.h
* GitHub: https:\\github.com\tommojphillips
*/

#ifndef WINDOW_SDL2_H
#define WINDOW_SDL2_H

#include <stdint.h>

#include "SDL.h"

/* SDL state */
typedef struct {
	SDL_Window* game_window;
	SDL_Renderer* game_renderer;
	SDL_Event e;
} SDL_STATE;

/* Window state */
typedef struct {
	int window_open;
	int win_x;
	int win_y;
	int win_h;
	int win_w;
	int last_win_x;
	int last_win_y;
	int last_win_h;
	int last_win_w;
	int last_window_state;
	SDL_Rect px;
} WINDOW_STATE;

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} PIXEL_COLOR;

/* Window cfg */
typedef struct {
	PIXEL_COLOR px_on;
	PIXEL_COLOR px_off;
} WINDOW_CFG;

#ifdef __cplusplus
extern "C" {
#endif

extern SDL_STATE sdl;
extern WINDOW_CFG* window_cfg;
extern WINDOW_STATE* window_state;

/* SDL2 Init */
void sdl_init();

/* SDL2 Create window */
void sdl_create_window();

/* SDL2 Destroy */
void sdl_destroy();

/* SDL2 Update */
void sdl_update();

/* SDL2 Render */
void sdl_render();

#ifdef __cplusplus
};
#endif

#endif

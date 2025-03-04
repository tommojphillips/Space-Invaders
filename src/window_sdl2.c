/* window_sdl2.c
* GitHub: https:\\github.com\tommojphillips
*/

#include <stdint.h>
#include <stdio.h>

#include "SDL.h"
#include "SDL_syswm.h"

#include "window_sdl2.h"

SDL_STATE sdl = { 0 };
WINDOW_CFG* window_cfg = NULL;
WINDOW_STATE* window_state = NULL;

static void set_default_settings();

void imgui_toggle_menu();
void imgui_process_event(); 

void display_draw_buffer();
void display_process_event();

void input_process_event();
static void sdl_process_event();

void sdl_init() {

	window_cfg = (WINDOW_CFG*)malloc(sizeof(WINDOW_CFG));
	if (window_cfg == NULL) {
		printf("Failed to allocate window cfg\n");
		exit(1);
	}
	memset(window_cfg, 0, sizeof(WINDOW_CFG));

	window_state = (WINDOW_STATE*)malloc(sizeof(WINDOW_STATE));
	if (window_state == NULL) {
		printf("Failed to allocate window state\n");
		exit(1);
	}
	memset(window_state, 0, sizeof(WINDOW_STATE));

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Failed to initialize SDL\n");
		exit(1);
	}

	set_default_settings();
}
void sdl_create_window() {

	sdl.game_window = SDL_CreateWindow("Space Invaders - i8080",
		window_state->win_x, window_state->win_y, 
		window_state->win_w, window_state->win_h,
		SDL_WINDOW_RESIZABLE | window_state->last_window_state);

	if (sdl.game_window == NULL) {
		printf("Failed to create game window\n");
		exit(1);
	}

	sdl.game_renderer = SDL_CreateRenderer(sdl.game_window, -1, 0);
	if (sdl.game_renderer == NULL) {
		printf("Failed to create game renderer\n");
		exit(1);
	}

	window_state->window_open = 1;
}
void sdl_destroy() {

	if (window_cfg != NULL) {
		free(window_cfg);
		window_cfg = NULL;
	}

	if (window_state != NULL) {
		free(window_state);
		window_state = NULL;
	}

	if (sdl.icon_surface != NULL) {
		SDL_FreeSurface(sdl.icon_surface);
		sdl.icon_surface = NULL;
	}

	if (sdl.game_renderer != NULL) {
		SDL_DestroyRenderer(sdl.game_renderer);
		sdl.game_renderer = NULL;
	}

	if (sdl.game_window != NULL) {
		SDL_DestroyWindow(sdl.game_window);
		sdl.game_window = NULL;
	}

	SDL_Quit();
}
void sdl_update() {
	while (SDL_PollEvent(&sdl.e)) {
		sdl_process_event();	
		imgui_process_event();
		display_process_event();
		input_process_event();
	}
}
void sdl_render() {
	SDL_RenderPresent(sdl.game_renderer);
	SDL_SetRenderDrawColor(sdl.game_renderer, window_cfg->px_off.r, window_cfg->px_off.g, window_cfg->px_off.b, 0xFF);
	SDL_RenderClear(sdl.game_renderer);
	display_draw_buffer();
}

void sdl_process_event() {
	switch (sdl.e.type) {
		case SDL_KEYDOWN: {
			switch (sdl.e.key.keysym.sym) {
				case SDLK_ESCAPE:
					imgui_toggle_menu();
					break;

				case SDLK_F11: {
					if (window_state->last_window_state != SDL_WINDOW_FULLSCREEN_DESKTOP) {
						window_state->last_window_state = SDL_WINDOW_FULLSCREEN_DESKTOP;
						SDL_SetWindowFullscreen(sdl.game_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
					}
					else {
						window_state->last_window_state = 0;
						SDL_SetWindowFullscreen(sdl.game_window, 0);
					}
				} break;
			}
		} break;
	}
}

static void set_default_settings() {
	window_state->win_x = SDL_WINDOWPOS_CENTERED;
	window_state->win_y = SDL_WINDOWPOS_CENTERED;
	window_state->last_window_state = 0;
	window_state->win_w = 448 + 20;
	window_state->win_h = 512 + 20;
	window_cfg->px_on.g = 206;
}

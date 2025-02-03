
#include "SDL.h"

#include "window_sdl2.h"
#include "taito8080.h"

#define WINDOW_W (window_state->win_w)
#define WINDOW_H (window_state->win_h)
#define DISPLAY_W (224)
#define DISPLAY_H (256)
#define DISPLAY_PX_SIZE (WINDOW_H / DISPLAY_H)

#define X_CENTER ((WINDOW_W - (DISPLAY_W * DISPLAY_PX_SIZE)) >> 1)
#define Y_CENTER ((WINDOW_H - (DISPLAY_H * DISPLAY_PX_SIZE)) >> 1)

void display_process_event() {
	switch (sdl.e.type) {

	case SDL_QUIT:
		window_state->window_open = 0;
		break;

	case SDL_WINDOWEVENT:
		switch (sdl.e.window.event) {

		case SDL_WINDOWEVENT_RESIZED:
			window_state->last_win_w = window_state->win_w;
			window_state->last_win_h = window_state->win_h;
			SDL_GetWindowSize(sdl.game_window, &window_state->win_w, &window_state->win_h);
			break;

		case SDL_WINDOWEVENT_MOVED:
			window_state->last_win_x = window_state->win_x;
			window_state->last_win_y = window_state->win_y;
			SDL_GetWindowPosition(sdl.game_window, &window_state->win_x, &window_state->win_y);
			break;

		case SDL_WINDOWEVENT_MAXIMIZED:
			window_state->last_window_state = SDL_WINDOW_MAXIMIZED;
			break;

		case SDL_WINDOWEVENT_RESTORED:
			window_state->last_window_state = 0;
			break;
		}
	}
}
void display_draw_buffer() {
	if (taito8080.mm.video == NULL) {
		return;
	}

	int i = 0;
	for (int x = 0; x < DISPLAY_W; ++x) {
		for (int y = 0; y < DISPLAY_H; y += 8) {
			uint8_t byte = taito8080.mm.video[i++];
			for (int n = 0; n < 8; ++n) {

				window_state->px.x = X_CENTER + (x * DISPLAY_PX_SIZE);
				window_state->px.y = Y_CENTER + ((DISPLAY_H - (y + n)) * DISPLAY_PX_SIZE);
				window_state->px.w = DISPLAY_PX_SIZE;
				window_state->px.h = DISPLAY_PX_SIZE;

				if ((byte & 0x1) == 0x1) {
					SDL_SetRenderDrawColor(sdl.game_renderer, window_cfg->px_on.r, window_cfg->px_on.g, window_cfg->px_on.b, 0xFF);
				}
				else {
					SDL_SetRenderDrawColor(sdl.game_renderer, window_cfg->px_off.r, window_cfg->px_off.g, window_cfg->px_off.b, 0xFF);
				}

				SDL_RenderFillRect(sdl.game_renderer, &window_state->px);
				byte >>= 1;
			}
		}
	}
}

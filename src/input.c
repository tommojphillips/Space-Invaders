
#include "SDL.h"

#include "window_sdl2.h"
#include "invaders.h"

static void game_input(uint8_t v);

void input_process_event() {
	switch (sdl.e.type) {
	case SDL_KEYDOWN:
		game_input(1);
		break;
	case SDL_KEYUP:
		game_input(0);
		break;
	}
}

static void game_input(uint8_t v) {
	switch (sdl.e.key.keysym.sym) {

		case SDLK_LEFT:
			invaders.io_input.input1.player_left = v;
			invaders.io_input.input2.player_left = v;
			break;

		case SDLK_RIGHT:
			invaders.io_input.input1.player_right = v;
			invaders.io_input.input2.player_right = v;
			break;

		case SDLK_SPACE:
			invaders.io_input.input1.player_fire = v;
			invaders.io_input.input2.player_fire = v;
			break;

		case SDLK_1:
			invaders.io_input.input1.one_player = v;
			break;

		case SDLK_2:
			invaders.io_input.input1.two_player = v;
			break;

		case SDLK_3:
		case SDLK_c:
			invaders.io_input.input1.coin = v;
			break;

		case SDLK_r: // RESET
			if (sdl.e.key.keysym.mod & KMOD_LCTRL) invaders_reset();
			break;
	}
	
	if (sdl.e.key.keysym.mod & KMOD_LCTRL) {
		switch (sdl.e.key.keysym.sym) {
		
		}
	}
}

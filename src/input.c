
#include <stdio.h>

#include "SDL.h"

#include "window_sdl2.h"
#include "taito8080.h"
#include "emulator.h"

static void game_input(uint8_t v);
static void sys_input();

void input_process_event() {
	switch (sdl.e.type) {
	case SDL_KEYDOWN:
		game_input(1);
		sys_input();
		break;
	case SDL_KEYUP:
		game_input(0);
		break;
	}
}


static void game_input(uint8_t v) {
	switch (sdl.e.key.keysym.sym) {

		case SDLK_LEFT:
			emu.controls.player1.left = v;
			emu.controls.player2.left = v;
			break;

		case SDLK_RIGHT:
			emu.controls.player1.right = v;
			emu.controls.player2.right = v;
			break;

		case SDLK_UP:
			emu.controls.player1.up = v;
			emu.controls.player2.up = v;
			break;

		case SDLK_DOWN:
			emu.controls.player1.down = v;
			emu.controls.player2.down = v;
			break;

		case SDLK_SPACE:
			emu.controls.player1.fire = v;
			emu.controls.player2.fire = v;
			break;

		case SDLK_1:
			emu.controls.player1.start = v;
			break;

		case SDLK_2:
			emu.controls.player2.start = v;
			break;

		case SDLK_3:
			emu.controls.insert_coin = v;
			break;

		case SDLK_t:
			emu.controls.tilt_switch = v;
			break;

		case SDLK_F2:
			emu.controls.name_reset = v;
			break;
		case SDLK_F3:
			emu.controls.preset_mode = v;
			break;

	}
}

static void sys_input() {
	switch (sdl.e.key.keysym.sym) {

		case SDLK_F1:
			emu.controls.coin_info ^= 1;
			break;
		case SDLK_F4:
			taito8080_reset();
			break;

		case SDLK_F5:
			taito8080_save_state();
			break;
		case SDLK_F9:
			taito8080_load_state();
			break;

		case SDLK_p:
			emu.single_step = emu.single_step ? SINGLE_STEP_NONE : SINGLE_STEP_AWAIT;
			break;

	}
}

#include "SDL.h"

#include "window_sdl2.h"
#include "taito8080.h"
#include "emulator.h"

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

#include "stdio.h"

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
		case SDLK_c:
			emu.controls.insert_coin = v;
			break;

		case SDLK_t:
			emu.controls.tilt_switch = v;
			break;

		case SDLK_F1:
			if (v) emu.controls.coin_info ^= 1;
			break;
		case SDLK_F2:
			emu.controls.name_reset = v;
			break;
		case SDLK_F3:
			emu.controls.preset_mode = v;
			break;

		case SDLK_r:
			if (v && sdl.e.key.keysym.mod & KMOD_LCTRL) emu.machine->reset();
			break;

		case SDLK_F5:
			if (v && emu.machine->save_state) emu.machine->save_state();
			break;
		case SDLK_F9:
			if (v && emu.machine->load_state) emu.machine->load_state();
			break;

		case SDLK_p:
			if (v) emu.single_step = emu.single_step ? SINGLE_STEP_NONE : SINGLE_STEP_AWAIT;
			break;

		case SDLK_i: /* spawn space ship */
			*(uint16_t*)(taito8080.mm.memory + 0x2091) = 0;
			break;
		case SDLK_u: /* kill space ship */
			taito8080.mm.memory[0x2085] = 1;
			break;
	}
}


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
			taito8080.io_input.input1.player_left = v;
			//taito8080.io_input.input2.player_left = v;
			break;

		case SDLK_RIGHT:
			taito8080.io_input.input1.player_right = v;
			//taito8080.io_input.input2.player_right = v;
			break;

		case SDLK_SPACE:
			taito8080.io_input.input1.player_fire = v;
			//taito8080.io_input.input2.player_fire = v;
			break;

		case SDLK_1:
			taito8080.io_input.input1.one_player = v;
			break;

		case SDLK_2:
			taito8080.io_input.input1.two_player = v;
			break;

		case SDLK_3:
		case SDLK_c:
			taito8080.io_input.input1.coin = v;
			break;

		case SDLK_t:
			taito8080.io_input.input2.tilt = v;
			break;

		case SDLK_r:
			if (v && sdl.e.key.keysym.mod & KMOD_LCTRL) {
				emu.machine->reset();
			}
			break;

		case SDLK_F5:
			if (v) {
				if (emu.machine->save_state) emu.machine->save_state();
			}
			break;
		case SDLK_F9:
			if (v) {
				if (emu.machine->load_state) emu.machine->load_state();
			}
			break;

		case SDLK_p:
			if (v) {
				emu.single_step = emu.single_step ? SINGLE_STEP_NONE : SINGLE_STEP_AWAIT;
			}
			break;

		case SDLK_i: /* spawn space ship */
			*(uint16_t*)(taito8080.mm.ram + 0x91) = 0;
			break;
		case SDLK_u: /* kill space ship */
			taito8080.mm.ram[0x85] = 1;
			break;
	}	
}

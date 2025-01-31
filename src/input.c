
#include "SDL.h"

#include "window_sdl2.h"
#include "invaders.h"
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
static void save_state() {
	FILE* file;
	fopen_s(&file, "state.bin", "wb");
	if (file == NULL) {
		return;
	}
	fwrite(invaders.mm.ram, 1, 0x2000, file);
	fwrite(&invaders.shift_amount, 1, 1, file);
	fwrite(&invaders.shift_reg, 1, 2, file);
	fwrite(&invaders.io_output, 1, 2, file);
	fwrite(&cpu, 1, sizeof(I8080), file);
	fclose(file);
}

static void load_state() {
	FILE* file;
	fopen_s(&file, "state.bin", "rb");
	if (file == NULL) {
		return;
	}

	fread(invaders.mm.ram, 1, 0x2000, file);
	fread(&invaders.shift_amount, 1, 1, file);
	fread(&invaders.shift_reg, 1, 2, file);
	fread(&invaders.io_output, 1, 2, file);

	I8080 c = { 0 };
	fread(&c, 1, sizeof(I8080), file);
	cpu.pc = c.pc;
	cpu.sp = c.sp;
	cpu.cycles = c.cycles;
	for (int i = 0; i < 8; ++i) {
		cpu.registers[i] = c.registers[i];
	}
	fclose(file);
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

		case SDLK_r:
			if (v && sdl.e.key.keysym.mod & KMOD_LCTRL) {
				emu.reset();
			}
			break;

		case SDLK_F5:
			if (v) {
				save_state();
			}
			break;
		case SDLK_F9:
			if (v) {
				load_state();
			}
			break;

		case SDLK_p:
			if (v) {
				emu.single_step = emu.single_step ? SINGLE_STEP_NONE : SINGLE_STEP_AWAIT;
			}
			break;

		case SDLK_i: /* spawn space ship */
			*(uint16_t*)(invaders.mm.ram + 0x91) = 0;
			break;
		case SDLK_u: /* kill space ship */
			invaders.mm.ram[0x85] = 1;
			break;
	}	
}

/* main.c
 * Github: https:\\github.com\tommojphillips
 */

#include "stdio.h"

#include "window_sdl2.h"
#include "ui.h"
#include "i8080.h"
#include "taito8080.h"
#include "cpm.h"

#include "emulator.h"

#define CPM			0
#define TAITO8080	1

const MACHINE machines[] = {
	{ CPM, "CPM i8080",	cpm_init, cpm_destroy,
	  cpm_reset, cpm_update, cpm_vblank,
	  NULL, NULL,
	  cpm_load_test, cpm_tests, 5 },
	
	{ TAITO8080, "taito i8080",	taito8080_init, taito8080_destroy,
	  taito8080_reset, taito8080_update, taito8080_vblank,
	  taito8080_save_state, taito8080_load_state, 
	  taito8080_load_romset, taito8080_romsets, 7 },
};

static float render_elapsed_time;
static void start_frame() {
	static uint64_t start_frame_time;
	static float delta_time;
	delta_time = (SDL_GetPerformanceCounter() - start_frame_time) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
	render_elapsed_time += delta_time;
	start_frame_time = SDL_GetPerformanceCounter();
}

int main(int argc, char** argv) {

	sdl_init();
	sdl_create_window();
	imgui_init();
	imgui_create_renderer();

	emu.machine = &machines[TAITO8080];
	//emu.machine = &machines[CPM];
	emu.machine->init();

	while (window_state->window_open) {
		start_frame();
		sdl_update();
		imgui_update();

		emu.machine->update();

		if (16.666f < render_elapsed_time) {
			render_elapsed_time -= 16.666f;
			sdl_render();

			if (emu.machine->vblank) {
				emu.machine->vblank();
			}
		}
	}

	emu.machine->destroy();
	imgui_destroy();
	sdl_destroy();

	return 0;
}

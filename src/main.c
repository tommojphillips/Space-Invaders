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
	{ 
	  .id           = CPM,
	  .name         = "CP/M i8080",
	  .init         = cpm_init,
	  .destroy      = cpm_destroy,
	  .reset        = cpm_reset,
	  .update       = cpm_update,
	  .vblank       = cpm_vblank,
	  .save_state   = NULL,
	  .load_state   = NULL,
	  .load_romset  = cpm_load_test,
	  .romsets      = cpm_tests,
	  .romset_count = 5
	},
	
	{ 
	  .id           = TAITO8080, 
	  .name         = "taito i8080",
	  .init         = taito8080_init,
	  .destroy      = taito8080_destroy,
	  .reset        = taito8080_reset,
	  .update       = taito8080_update,
	  .vblank       = taito8080_vblank,
	  .save_state   = taito8080_save_state,
	  .load_state   = taito8080_load_state,
	  .load_romset  = taito8080_load_romset,
	  .romsets      = taito8080_romsets,
	  .romset_count = 7
	},
};

static float render_elapsed_time;
static void start_frame() {
	static uint64_t start_frame_time;
	static float delta_time;
	delta_time = (SDL_GetPerformanceCounter() - start_frame_time) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
	start_frame_time = SDL_GetPerformanceCounter();
	render_elapsed_time += delta_time;
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
			render_elapsed_time = 0; //-= 16.666f;
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

/* main.c
 * Github: https:\\github.com\tommojphillips
 */

#include "stdio.h"

#include "i8080.h"
#include "i8080_mnem.h"
#include "invaders.h"
#include "cpm.h"

#include "window_sdl2.h"
#include "ui.h"
#include "emulator.h"

I8080 cpu = { 0 };

//#define CPM

static uint64_t start_frame_time;
static float delta_time; 
static float render_elapsed_time;
void start_frame() {
	delta_time = (SDL_GetPerformanceCounter() - start_frame_time) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
	start_frame_time = SDL_GetPerformanceCounter();
}
int main(int argc, char** argv) {

	sdl_init();
	sdl_create_window();
	imgui_init();
	imgui_create_renderer();

	emu.single_step = SINGLE_STEP_AWAIT;
	emu.single_step_count = 0;
	emu.single_step_increment = 1;

#ifdef CPM
	emu.reset = cpm_reset;
	emu.update = cpm_update;
	emu.init = cpm_init;
	emu.destroy = cpm_destroy;
	emu.vblank = NULL;
#else
	emu.reset = invaders_reset;
	emu.update = invaders_update;
	emu.init = invaders_init;
	emu.destroy = invaders_destroy;
	emu.vblank = invaders_vblank;
#endif

	if (emu.init() != 0) {
		return 1; /* LEAK */
	}

	while (window_state->window_open) {
		start_frame();
		sdl_update();
		imgui_update();

		emu.update();

		render_elapsed_time += delta_time;
		if (16.666f < render_elapsed_time) {
			render_elapsed_time -= 16.666f;
			sdl_render();

			if (emu.vblank) {
				emu.vblank();
			}
		}
	}

	emu.destroy();
	imgui_destroy();
	sdl_destroy();

	return 0;
}

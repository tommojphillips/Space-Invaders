/* main.c
 * Github: https:\\github.com\tommojphillips
 */

#include "stdio.h"

#include "i8080.h"
#include "i8080_mnem.h"
#include "invaders.h"
#include "lrescue.h"
#include "ballbomb.h"
#include "cpm.h"

#include "window_sdl2.h"
#include "ui.h"
#include "emulator.h"

//#define CPM
#define LRESCUE
//#define BALLBOMBER

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

	//emu.single_step = SINGLE_STEP_AWAIT;
	emu.single_step = SINGLE_STEP_NONE;
	emu.single_step_increment = 1;

#ifdef CPM
	emu.reset = cpm_reset;
	emu.update = cpm_update;
	emu.init = cpm_init;
	emu.destroy = cpm_destroy;
	emu.vblank = NULL;
	emu.save_state = NULL;
	emu.load_state = NULL;
#else
#ifdef LRESCUE
	emu.reset = lrescue_reset;
	emu.update = lrescue_update;
	emu.init = lrescue_init;
	emu.destroy = lrescue_destroy;
	emu.vblank = lrescue_vblank;
	emu.save_state = lrescue_save_state;
	emu.load_state = lrescue_load_state;
#else
#ifdef BALLBOMBER
	emu.reset = ballbomb_reset;
	emu.update = ballbomb_update;
	emu.init = ballbomb_init;
	emu.destroy = ballbomb_destroy;
	emu.vblank = ballbomb_vblank;
	emu.save_state = ballbomb_save_state;
	emu.load_state = ballbomb_load_state;
#else
	emu.reset = invaders_reset;
	emu.update = invaders_update;
	emu.init = invaders_init;
	emu.destroy = invaders_destroy;
	emu.vblank = invaders_vblank;
	emu.save_state = invaders_save_state;
	emu.load_state = invaders_load_state;
#endif
#endif
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

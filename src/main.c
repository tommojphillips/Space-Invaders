/* main.c
 * Github: https:\\github.com\tommojphillips
 */

#include "stdio.h"

#include "i8080.h"
#include "invaders.h"

#include "window_sdl2.h"
#include "ui.h"

void test();

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

	if (invaders_init() != 0) {
		return 1;
	}

	window_cfg->px_on.g = 206;

	while (window_state->window_open) {
		start_frame();
		sdl_update();
		imgui_update();

		invaders_update();

		const float render_duration = (1000.0f / 60);
		render_elapsed_time += delta_time;
		if (render_duration < render_elapsed_time) {
			render_elapsed_time -= render_duration;			
			sdl_render();
			invaders_vblank();
		}
	}

	invaders_destroy();
	imgui_destroy();
	sdl_destroy();

	return 0;
}

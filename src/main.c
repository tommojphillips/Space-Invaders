/* main.c
 * Github: https:\\github.com\tommojphillips
 */

#include "window_sdl2.h"
#include "ui.h"
#include "i8080.h"
#include "taito8080.h"

static float render_elapsed_time;
static void start_frame() {
	static uint64_t start_frame_time;
	static float delta_time;
	delta_time = (SDL_GetPerformanceCounter() - start_frame_time) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
	start_frame_time = SDL_GetPerformanceCounter();
	render_elapsed_time += delta_time;
}

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;

	sdl_init();
	sdl_create_window();
	imgui_init();
	imgui_create_renderer();
	taito8080_init();

	while (window_state->window_open) {
		start_frame();
		sdl_update();
		imgui_update();

		taito8080_update();

		if (16.666f < render_elapsed_time) {
			render_elapsed_time = 0;// -= 16.666f;
			sdl_render();
			taito8080_vblank();
		}
	}

	taito8080_destroy();
	imgui_destroy();
	sdl_destroy();

	return 0;
}

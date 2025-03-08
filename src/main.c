/* main.c
 * Github: https:\\github.com\tommojphillips
 */

#include <stdio.h>
#include <string.h>

#include "window_sdl2.h"
#include "ui.h"
#include "i8080.h"
#include "taito8080.h"

int args(int argc, char** argv) {
	for (int i = 1; i < argc; ++i) {
		size_t len = (size_t)strlen(argv[i]);
		for (size_t j = 0; j < len;) {
			const char* arg = argv[i] + j;

			if (strncmp("-l", arg, 2) == 0) {				
				for (int k = 0; k < emu.romset_count; ++k) {
					fprintf(stdout, "%s\n", taito8080_romsets[k].filename);
				}
				return 1;
			}

			if (strncmp("-f", arg, 2) == 0) {
				arg += 2;
			}
			int found = 0;
			for (int k = 0; k < emu.romset_count; ++k) {
				if (strcmp(taito8080_romsets[k].filename, arg) == 0) {
					found = 1;
					emu.romset_index = k;
				}
			}
			if (!found) {
				printf("Unknown romset: \"%s\"\n", arg);
				return 1;
			}
			break;
		}
	}
	return 0;
}

static float render_elapsed_time;
static void start_frame() {
	static uint64_t start_frame_time;
	static float delta_time;
	delta_time = (SDL_GetPerformanceCounter() - start_frame_time) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
	start_frame_time = SDL_GetPerformanceCounter();
	render_elapsed_time += delta_time;
}

int main(int argc, char** argv) {

	taito8080_init();

	if (args(argc, argv)) {
		exit(0);
	}

	sdl_init();
	sdl_create_window();
#ifndef NO_UI
	imgui_init();
	imgui_create_renderer();
#endif

	if (taito8080_load_romset(emu.romset_index) != 0) {
		return 1;
	}

	while (window_state->window_open) {
		start_frame();
		sdl_update();
#ifndef NO_UI
		imgui_update();
#endif
		taito8080_update();

		if (16.666f < render_elapsed_time) {
			render_elapsed_time = 0;// -= 16.666f;
			sdl_render();
			taito8080_vblank();
		}
	}

	taito8080_destroy();
#ifndef NO_UI
	imgui_destroy();
#endif
	sdl_destroy();

	return 0;
}

/* window_ui.h
* GitHub: https:\\github.com\tommojphillips
*/

#ifndef IMGUI_WINDOW_UI_H
#define IMGUI_WINDOW_UI_H

typedef struct {
	int show_menu_window;
	int show_debug_window;
	int show_decode_window;
	int show_hl_window;
	int show_de_window;
	int show_stack_window;
	int show_dip_switch_window;

	int show_ram_window;
	int cols_ram_window;
	int ascii_ram_window;

	int show_rom_window;
	int cols_rom_window;
	int ascii_rom_window;

	int show_video_window;
	int cols_video_window;
	int ascii_video_window;

	float window_scale;
} UI_STATE;

#ifdef __cplusplus
extern "C" {
#endif

// init imgui
void imgui_init();

void imgui_create_renderer();

// destroy imgui
void imgui_destroy();

// update imgui
void imgui_update();

// imgui process event
void imgui_process_event();

// imgui toggle menu ui
void imgui_toggle_menu();

void imgui_refresh_ui_state();

#ifdef __cplusplus
};
#endif

#endif

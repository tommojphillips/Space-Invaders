/* window_ui.h
* GitHub: https:\\github.com\tommojphillips
*/

#ifndef NO_UI
#ifndef IMGUI_WINDOW_UI_H
#define IMGUI_WINDOW_UI_H

typedef struct {
	int show_menu_window;
	int show_debug_window;
	int show_decode_window;
	int show_hl_window;
	int show_bc_window;
	int show_de_window;
	int show_stack_window;
	int show_dip_switch_window;
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

#ifdef __cplusplus
};
#endif

#endif
#endif

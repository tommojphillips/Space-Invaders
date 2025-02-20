/* ui.cpp
* GitHub: https:\\github.com\tommojphillips
*/

#include <stdio.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
using namespace ImGui;

#include "ui.h"
#include "window_sdl2.h"

#include "i8080.h"
#include "i8080_mnem.h"
#include "taito8080.h"
#include "cpm.h"
#include "emulator.h"

#define renderer_new_frame \
	ImGui_ImplSDLRenderer2_NewFrame(); \
	ImGui_ImplSDL2_NewFrame

#define renderer_draw_data(renderer) \
	ImGui_ImplSDLRenderer2_RenderDrawData(GetDrawData(), renderer)

#define imgui_new_frame \
	renderer_new_frame(); \
	NewFrame

#define imgui_render_frame(renderer) \
	Render(); \
	renderer_draw_data(renderer)

#define init_renderer(window, renderer) \
	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer); \
	ImGui_ImplSDLRenderer2_Init(renderer)

#define destroy_renderer ImGui_ImplSDLRenderer2_Shutdown(); \
	ImGui_ImplSDL2_Shutdown

#define process_event ImGui_ImplSDL2_ProcessEvent(&sdl.e)

/* Imgui state */
typedef struct {
	ImGuiContext* context;
	ImGuiIO* io;
	char tmp_s[32];
} IMGUI_STATE;

static IMGUI_STATE imgui = { 0 };

extern "C" UI_STATE ui_state = { 0 };
extern "C" I8080_MNEM mnem = { 0 };

int step = 1;
int follow_next_instruction = 1;

static void menu_window();
static void debug_window();
static void decode_window();
static void stack_window();
static void hl_window();
static void bc_window();
static void de_window();
static void dip_switch_window();
static void dip_switch8(uint8_t* v, uint8_t id, const char* dip_name);
static void dip_switch16(uint16_t* v, uint8_t id, const char* dip_name);
static void set_default_settings();

void imgui_init() {
	set_default_settings();
	mnem.cpu = &taito8080.cpu;
}
void imgui_create_renderer() {

	IMGUI_CHECKVERSION();
	imgui.context = CreateContext();
	if (imgui.context == NULL) {
		printf("Failed to create IMGUI Context");
		return;
	}

	StyleColorsDark();

	imgui.io = &GetIO();
	imgui.io->FontGlobalScale = ui_state.window_scale;

	ImGui_ImplSDL2_InitForSDLRenderer(sdl.game_window, sdl.game_renderer);
	ImGui_ImplSDLRenderer2_Init(sdl.game_renderer);
}

void imgui_destroy() {
	ImGui_ImplSDLRenderer2_Shutdown(); 
	ImGui_ImplSDL2_Shutdown();
	DestroyContext(imgui.context);
}
void imgui_update() {
	ImGui_ImplSDLRenderer2_NewFrame(); 
	ImGui_ImplSDL2_NewFrame();
	NewFrame();

	if (ui_state.show_menu_window) {
		menu_window();
	}	
	if (ui_state.show_debug_window) {
		debug_window();
	}
	if (ui_state.show_decode_window) {
		decode_window();
	}
	if (ui_state.show_stack_window) {
		stack_window();
	}
	if (ui_state.show_hl_window) {
		hl_window();
	}
	if (ui_state.show_bc_window) {
		bc_window();
	}
	if (ui_state.show_de_window) {
		de_window();
	}

	Render();	
	ImGui_ImplSDLRenderer2_RenderDrawData(GetDrawData(), sdl.game_renderer);
}
void imgui_process_event() {
	ImGui_ImplSDL2_ProcessEvent(&sdl.e);
}
void imgui_toggle_menu() {
	ui_state.show_menu_window ^= 1;
}

static void set_default_settings() {
	ui_state.window_scale = 1.0f;
	ui_state.show_menu_window = 0;
	ui_state.show_debug_window = 0;
	ui_state.show_dip_switch_window = 0;
}

static void decode_window() {
	Begin("Decode", (bool*)&ui_state.show_decode_window);
	Checkbox("Follow flow", (bool*)&follow_next_instruction);
	Separator();
	BeginChild("Decode"); 

	uint16_t pc = taito8080.cpu.pc;
	cpu_mnem(&mnem, pc);
	Text("%04X: %s", pc, mnem.str);
	Separator();
	for (int i = 0; i < 10; ++i) {
		if (follow_next_instruction)
			pc = mnem.pc;
		else
			pc += mnem.count;
		cpu_mnem(&mnem, pc);
		Text("%04X: %s", pc, mnem.str);
	}
	EndChild();
	End();
}
static void stack_window() {
	Begin("Stack", (bool*)&ui_state.show_stack_window);
	uint16_t k = 0;
	for (int i = 0; i < 10; ++i) {
		Text("%04X: ", taito8080.cpu.sp + k);
		SameLine();
		for (int j = 0; j < 4; ++j) {
			Text("%02X ", taito8080.cpu.read_byte(taito8080.cpu.sp + k));
			if (j < 3) SameLine();
			k++;
		}
	}
	End();
}
static void hl_window() {
	Begin("HL", (bool*)&ui_state.show_hl_window);
	uint16_t ptr = ((taito8080.cpu.registers[REG_H] << 8) | taito8080.cpu.registers[REG_L]);
	uint16_t k = 0;
	for (int i = 0; i < 10; ++i) {
		Text("%04X: ", ptr + k);
		SameLine();
		for (int j = 0; j < 4; ++j) {
			Text("%02X ", taito8080.cpu.read_byte(ptr + k));
			if (j < 3) SameLine();
			k++;
		}
	}
	End();
}
static void bc_window() {
	Begin("BC", (bool*)&ui_state.show_bc_window);
	uint16_t ptr = ((taito8080.cpu.registers[REG_B] << 8) | taito8080.cpu.registers[REG_C]);
	uint16_t k = 0;
	for (int i = 0; i < 10; ++i) {
		Text("%04X: ", ptr + k);
		SameLine();
		for (int j = 0; j < 4; ++j) {
			Text("%02X ", taito8080.cpu.read_byte(ptr + k));
			if (j < 3) SameLine();
			k++;
		}
	}
	End();
}
static void de_window() {
	Begin("DE", (bool*)&ui_state.show_de_window);
	uint16_t ptr = ((taito8080.cpu.registers[REG_D] << 8) | taito8080.cpu.registers[REG_E]);
	uint16_t k = 0;
	for (int i = 0; i < 10; ++i) {
		Text("%04X: ", ptr + k);
		SameLine();
		for (int j = 0; j < 4; ++j) {
			Text("%02X ", taito8080.cpu.read_byte(ptr + k));
			if (j < 3) SameLine();
			k++;
		}
	}
	End();
}
static void dip_switch_window() {
		
	int tmp = emu.controls.lives_min + emu.controls.lives;
	if (SliderInt("Lives", &tmp, emu.controls.lives_min, emu.controls.lives_max)) {
		emu.controls.lives = ((uint8_t)tmp - emu.controls.lives_min);
	}

}
static void debug_window() {
	Begin("Debug", (bool*)&ui_state.show_debug_window);
	
	if (emu.single_step != SINGLE_STEP_NONE) {
		if (ArrowButton("Continue", ImGuiDir_Right)) {
			emu.single_step = SINGLE_STEP_NONE;
		}
		SameLine();
		if (Button(">>>")) {
			emu.single_step = SINGLE_STEPPING;
		}
		SameLine();
		PushItemWidth(GetFontSize() * 6);
		Text("Step: %06d", emu.single_step_increment);
		SameLine();
		if (SliderInt("###Cycles", (int*)&step, 1, 6)) {
			switch (step) {
				case 1:
					emu.single_step_increment = 1;
					break;
				case 2:
					emu.single_step_increment = 10;
					break;
				case 3:
					emu.single_step_increment = 100;
					break;
				case 4:
					emu.single_step_increment = 1000;
					break;
				case 5:
					emu.single_step_increment = 10000;
					break;
				case 6:
					emu.single_step_increment = 100000;
					break;
			}
		}
		PopItemWidth();
	}
	else {
		if (Button("||")) {
			emu.single_step = SINGLE_STEP_AWAIT;
		}
	}

	Separator();
	Text("PC: %04X", taito8080.cpu.pc);

	Separator();
	Text("SP: %04X", taito8080.cpu.sp);

	Separator();
	Text("BC: %02X%02X", taito8080.cpu.registers[REG_B], taito8080.cpu.registers[REG_C]);

	Separator();
	Text("DE: %02X%02X", taito8080.cpu.registers[REG_D], taito8080.cpu.registers[REG_E]);

	Separator();
	Text("HL: %02X%02X", taito8080.cpu.registers[REG_H], taito8080.cpu.registers[REG_L]);

	Separator();
	Text("A: %02X", taito8080.cpu.registers[REG_A]);

	Separator();
	Text("PSW: %02X%02X", taito8080.cpu.registers[REG_A], taito8080.cpu.registers[REG_FLAGS]);
	
#if 0
	Separator();
	Text("SF: %01X ", taito8080.cpu.status_flags->s);
	SameLine();
	Text("ZF: %01X ", taito8080.cpu.status_flags->z);
	Text("PF: %01X ", taito8080.cpu.status_flags->p);
	SameLine();
	Text("CF: %01X ", taito8080.cpu.status_flags->c);
	Text("AF: %01X ", taito8080.cpu.status_flags->h);
#else
	bool tmp;

	Separator();
	tmp = taito8080.cpu.status_flags->s;
	if (Checkbox("SF", &tmp)) taito8080.cpu.status_flags->s ^= 1;
	SameLine();
	tmp = taito8080.cpu.status_flags->z;
	if (Checkbox("ZF", &tmp)) taito8080.cpu.status_flags->z ^= 1;
	tmp = taito8080.cpu.status_flags->p;
	if (Checkbox("PF", &tmp)) taito8080.cpu.status_flags->p ^= 1;
	SameLine();
	tmp = taito8080.cpu.status_flags->c;
	if (Checkbox("CF", &tmp)) taito8080.cpu.status_flags->c ^= 1;
	tmp = taito8080.cpu.status_flags->h;
	if (Checkbox("AF", &tmp)) taito8080.cpu.status_flags->h ^= 1;
#endif
	Separator();	
	tmp = taito8080.cpu.flags.interrupt;
	if (Checkbox("INT", &tmp)) taito8080.cpu.flags.interrupt = tmp;
	SameLine();
	tmp = taito8080.cpu.flags.halt;
	if (Checkbox("HALT", &tmp)) taito8080.cpu.flags.halt = tmp;

	Separator();	
	Text("Shift16: %04X", taito8080.shift_register.data);
	
	Separator();
	Text("Cycles: %d", taito8080.cpu.cycles);

	End();
}
static void menu_window() {
	
	Begin("Menu", (bool*)&ui_state.show_menu_window);	
	Separator();
	if (Button("Debug")) {
		ui_state.show_debug_window ^= 1;
	}	
	SameLine();
	if (Button("Decode")) {
		ui_state.show_decode_window ^= 1;
	}
	SameLine();
	if (Button("HL")) {
		ui_state.show_hl_window ^= 1;
	}
	SameLine();
	if (Button("BC")) {
		ui_state.show_bc_window ^= 1;
	}
	SameLine();
	if (Button("DE")) {
		ui_state.show_de_window ^= 1;
	}
	SameLine();
	if (Button("SP")) {
		ui_state.show_stack_window ^= 1;
	}
	Separator();
	if (Button("Dip Switches")) {
		ui_state.show_dip_switch_window ^= 1;
	}
	Separator();

	if (ui_state.show_dip_switch_window) {
		dip_switch_window();
	}
	else {
		if (BeginCombo("###rom_set", emu.machine->romsets[emu.romset_index].name)) {
			for (int i = 0; i < emu.machine->romset_count; ++i) {
				if (Selectable(emu.machine->romsets[i].name)) {
					if (emu.machine->load_romset(i) == 0) {
						emu.machine->reset();
					}
					else {
						emu.machine->load_romset(emu.romset_index); /*reload roms/reconfig for current system.*/
					}
				}
			}
			EndCombo();
		}

		Separator();

		if (Button("Reset Machine")) {
			emu.machine->reset();
		}
	}

	End();
}

static void dip_switch8(uint8_t* v, uint8_t id_offset, const char* dip_name) {
	const ImU32 on = IM_COL32(255, 255, 255, 255);
	const ImU32 off = IM_COL32(0, 0, 0, 255);
	const ImVec2 scale = ImVec2(15, 25);

	Text("%s (%X)", dip_name, *v);

	/* DIP SWITCH ON ROW */
	for (int i = 0; i < 8; ++i) {
		PushID(i + id_offset);
		PushStyleColor(ImGuiCol_Button, get_bit(*v, i) == 1 ? on : off);
		if (Button("", scale)) {
			set_bit(*v, i);
		}
		PopStyleColor();
		PopID();
		if (i < 7) SameLine();
	}

	/* DIP SWITCH OFF ROW */
	for (int i = 0; i < 8; ++i) {
		PushID(i + 8 + id_offset);
		PushStyleColor(ImGuiCol_Button, get_bit(*v, i) == 0 ? on : off);
		if (Button("", scale)) {
			clear_bit(*v, i);
		}
		PopStyleColor();
		PopID();
		if (i < 7) SameLine();
	}
}
static void dip_switch16(uint16_t* v, uint8_t id_offset, const char* dip_name) {
	const ImU32 on = IM_COL32(255, 255, 255, 255);
	const ImU32 off = IM_COL32(0, 0, 0, 255);
	const ImVec2 scale = ImVec2(15, 25);

	Text("%s (%X)", dip_name, *v);

	/* DIP SWITCH ON ROW */
	for (int i = 0; i < 16; ++i) {
		PushID(i + id_offset);
		PushStyleColor(ImGuiCol_Button, get_bit(*v, i) == 1 ? on : off);
		if (Button("", scale)) {
			set_bit(*v, i);
		}
		PopStyleColor();
		PopID();
		if (i < 15) SameLine();
	}

	/* DIP SWITCH OFF ROW */
	for (int i = 0; i < 16; ++i) {
		PushID(i + 16 + id_offset);
		PushStyleColor(ImGuiCol_Button, get_bit(*v, i) == 0 ? on : off);
		if (Button("", scale)) {
			clear_bit(*v, i);
		}
		PopStyleColor();
		PopID();
		if (i < 15) SameLine();
	}
}

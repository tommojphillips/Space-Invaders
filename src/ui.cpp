/* ui.cpp
* GitHub: https:\\github.com\tommojphillips
*/

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "imgui_memory_editor/imgui_memory_editor.h"
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

#define set_bit(v, n) v |= (1 << n)
#define clear_bit(v, n) v &= (~(1 << n))
#define get_bit(v, n) ((v & (1 << n)) >> n)

/* Imgui state */
typedef struct {
	ImGuiContext* context;
	MemoryEditor* ram_editor;
	MemoryEditor* rom_editor; 
	MemoryEditor* video_editor; 
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
	//StyleColorsLight();

	imgui.io = &GetIO();
	imgui.io->FontGlobalScale = ui_state.window_scale;

	static MemoryEditor ram_editor;
	ram_editor.Open = ui_state.show_ram_window;
	ram_editor.Cols = ui_state.cols_ram_window;
	ram_editor.OptShowAscii = ui_state.ascii_ram_window;
	ram_editor.GotoAddr = 0;
	imgui.ram_editor = &ram_editor;

	static MemoryEditor rom_editor;
	rom_editor.Open = ui_state.show_rom_window;
	rom_editor.Cols = ui_state.cols_rom_window;
	rom_editor.OptShowAscii = ui_state.ascii_rom_window;
	imgui.rom_editor = &rom_editor;

	static MemoryEditor video_editor;
	video_editor.Open = ui_state.show_video_window;
	video_editor.Cols = ui_state.cols_video_window;
	video_editor.OptShowAscii = ui_state.ascii_video_window;
	imgui.video_editor = &video_editor;

	ImGui_ImplSDL2_InitForSDLRenderer(sdl.game_window, sdl.game_renderer);
	ImGui_ImplSDLRenderer2_Init(sdl.game_renderer);
}

void imgui_destroy() {

	
	/* Cleanup */
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
	else {

		if (imgui.ram_editor->Open) {
			imgui.ram_editor->DrawWindow("RAM (1K)", taito8080.mm.ram, 1024, 0);
		}

		if (imgui.rom_editor->Open) {
			imgui.rom_editor->DrawWindow("ROM (8K)", taito8080.mm.rom, 8 * 1024, 0);
		}

		if (imgui.video_editor->Open) {
			imgui.video_editor->DrawWindow("VIDEO (7K)", taito8080.mm.video, 7 * 1024, 0);
		}
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

	ui_state.show_ram_window = 0;
	ui_state.cols_ram_window = 16;
	ui_state.ascii_ram_window = 0;

	ui_state.show_rom_window = 0;
	ui_state.cols_rom_window = 16;
	ui_state.ascii_rom_window = 0;

	ui_state.show_video_window = 0;
	ui_state.cols_video_window = 16;
	ui_state.ascii_video_window = 0;
}

static void decode_window() {
	Begin("Decode", (bool*)&ui_state.show_decode_window);
	Checkbox("Follow flow", (bool*)&follow_next_instruction);
	Separator();
	BeginChild("Decode");
	uint16_t pc = taito8080.cpu.pc;
	for (int i = 0; i < 10; ++i) {
		cpu_mnem(&mnem, pc);
		Text("%04X: %s", pc, mnem.str);
		if (follow_next_instruction)
			pc = mnem.pc;
		else
			pc += 1;
	}
	EndChild();
	End();
}
static void stack_window() {
	Begin("Stack", (bool*)&ui_state.show_stack_window);
	int k = 0;
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
	int k = 0;
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
	int k = 0;
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

	/*bool tmp = !taito8080.io_input.input2.extra_ship;
	if (Checkbox("Extra ship at 1500", &tmp)) {
		taito8080.io_input.input2.extra_ship = !tmp;
	}
	SameLine();
	tmp = taito8080.io_input.input2.extra_ship;
	if (Checkbox("Extra ship at 1000", &tmp)) {
		taito8080.io_input.input2.extra_ship = tmp;
	}

	static int lives = 3;
	Text("Lives: ");
	SameLine();
	if (SliderInt("###Lives", &lives, 3, 6)) {
		switch (lives) {
		case 3:
			taito8080.io_input.input2.ship1 = 0;
			taito8080.io_input.input2.ship2 = 0;
			break;
		case 4:
			taito8080.io_input.input2.ship1 = 1;
			taito8080.io_input.input2.ship2 = 0;
			break;
		case 5:
			taito8080.io_input.input2.ship1 = 0;
			taito8080.io_input.input2.ship2 = 1;
			break;
		case 6:
			taito8080.io_input.input2.ship1 = 1;
			taito8080.io_input.input2.ship2 = 1;
			break;
		}
	}*/

	dip_switch8(&taito8080.io_output.sound1, 0, "Sound1");
	
	Separator();
	dip_switch8(&taito8080.io_output.sound2, 16, "Sound2");
	
	Separator();
	dip_switch8(&taito8080.io_output.sound3, 32, "Sound3");
	
	Separator();
	dip_switch8(&taito8080.io_input.input0, 48, "Input0");

	Separator();
	dip_switch8((uint8_t*)&taito8080.io_input.input1, 64, "Input1");

	Separator();
	dip_switch8((uint8_t*)&taito8080.io_input.input2, 96, "Input2");
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
	
	Separator();
	Text("SF: %01X ", taito8080.cpu.status_flags->s);
	SameLine();
	Text("ZF: %01X ", taito8080.cpu.status_flags->z);
	Text("PF: %01X ", taito8080.cpu.status_flags->p);
	SameLine();
	Text("CF: %01X ", taito8080.cpu.status_flags->c);
	Text("AF: %01X ", taito8080.cpu.status_flags->h);

	Separator();	
	Text("INT: %01X ", taito8080.cpu.flags.interrupt);
	SameLine();
	Text("HALT: %01X ", taito8080.cpu.flags.halt);

	Separator();	
	Text("Shift16: %04X", taito8080.shift_reg);
	
	Separator();
	Text("Cycles: %d", taito8080.cpu.cycles);

	End();
}
static void menu_window() {const ImU32 on = IM_COL32(255, 255, 255, 255);
	
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
	if (Button("RAM")) {
		imgui.ram_editor->Open ^= 1;
	}
	SameLine();
	if (Button("ROM")) {
		imgui.rom_editor->Open ^= 1;
	}
	SameLine();
	if (Button("Video")) {
		imgui.video_editor->Open ^= 1;
	}
	SameLine();
	if (Button("HL")) {
		ui_state.show_hl_window ^= 1;
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
	const ImU32 background = IM_COL32(0, 255, 0, 0);
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
	const ImU32 background = IM_COL32(0, 255, 0, 0);
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
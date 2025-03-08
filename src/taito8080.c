/* taito_8080_machine.c
 * Github: https:\\github.com\tommojphillips
 */

#include <stdint.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "i8080.h"
#include "taito8080.h"
#include "file.h"

/* ROMSETS */
#include "invaders.h"
#include "lrescue.h"
#include "ballbomb.h"
#include "invaderspt2.h"
#include "spclaser.h"
#include "solfight.h"
#include "galxwars.h"
#include "schaser.h"
#include "galactic.h"
#include "indianbt.h"

#define HALF_VBLANK 16666 // 2 Mhz @ 60 hz 
#define FULL_VBLANK 33333 // 2 Mhz @ 60 hz 

#define ROM_SIZE   0x4000
#define RAM_SIZE   0x0400
#define VIDEO_SIZE 0x1C00

const ROMSET romsets[] = {
	{ invaders_init,    "invaders", "Space Invaders" },
	{ invaderspt2_init, "invadpt2", "Space Invaders Part II" },
	{ lrescue_init,     "lrescue",  "Lunar Rescue" },
	{ ballbomb_init,    "ballbomb", "Balloon Bomber" },
	{ spclaser_init,    "spclaser", "Space Laser" },
	{ galxwars_init,    "galxwars", "Galaxy Wars (set 1)" },
	{ solfight_init,    "ozmawars", "Ozma Wars (set 1)" },
	{ schaser_init,     "schaser",  "Space Chaser (set 1)" },
	{ galactic_init,    "galactic", "Galactic - Batalha Espacial" },
	{ indianbt_init,    "indianbt", "Indian Battle" }
};

TAITO8080 emu = { 0 };

int taito8080_load_romset(int i) {
	memset(emu.mm.memory, 0, 0x10000);
	printf("Loading romset: %s\n", romsets[i].name);
	if (romsets[i].init() != 0) {
		return 1;
	}
	emu.romset_index = i;
	return 0;
}
int taito8080_read_rom(const char* filename, uint32_t offset, uint32_t expected_size) {
	return read_file_into_buffer(filename, emu.mm.memory, 0x10000, offset, expected_size);
}

#define B_COUNT    emu.mm.region_count
#define B_START    emu.mm.regions[i].start
#define B_SIZE     emu.mm.regions[i].size
#define B_END      (B_START + B_SIZE)
#define B_MASK     (B_END - 1)
#define B_WRITABLE ((emu.mm.regions[i].flags & MREGION_FLAG_WRITE_PROTECTED) == 0)
#define B_PTR      emu.mm.memory

uint8_t taito8080_read_byte(uint16_t address) {
	for (int i = 0; i < B_COUNT; ++i) {
		if (address < B_END && address >= B_START) {
			return *(uint8_t*)(B_PTR + (address & B_MASK));
		}
	}
	//printf("reading from %x\n", address);
	return 0;
}
void taito8080_write_byte(uint16_t address, uint8_t value) {
	for (int i = 0; i < B_COUNT; ++i) {
		if (address < B_END && address >= B_START) {
			if (B_WRITABLE) {
				*(uint8_t*)(B_PTR + (address & B_MASK)) = value;
			}
			return;
		}
	}
	//printf("writing %x to %x\n", value, address);
}

void push_word(I8080* cpu, uint16_t value);

static void taito8080_interrupt(uint8_t rst_num) {
	/* process cpu interrupt */
	if (emu.cpu.flags.interrupt) {
		emu.cpu.flags.interrupt = 0;
		push_word(&emu.cpu, emu.cpu.pc);
		emu.cpu.pc = (rst_num & 0b111) << 3;
	}
}
void taito8080_reset() {
	i8080_reset(&emu.cpu);
	mb14241_reset(&emu.shift_register);
}

void taito8080_update() {	
	if (emu.single_step == SINGLE_STEP_NONE) {
		while (emu.cpu.cycles < HALF_VBLANK) {
			i8080_execute(&emu.cpu);
		}
		taito8080_interrupt(1);

		while (emu.cpu.cycles < FULL_VBLANK) {
			i8080_execute(&emu.cpu);
		}
		taito8080_interrupt(2);
	}
}
void taito8080_vblank() {
	/* Reset cpu cycles for the next frame; 
	this is so the interrupts get served at the right time. */
	emu.cpu.cycles = 0;
}

int taito8080_init() {
	emu.mm.memory = (uint8_t*)malloc(0x10000);
	if (emu.mm.memory == NULL) {
		printf("Failed to allocate Memory\n");
		return 1;
	}
	memset(emu.mm.memory, 0, 0x10000);
	emu.mm.memory_size = 0x10000;

	emu.mm.video = (emu.mm.memory + 0x2400);

	i8080_init(&emu.cpu);
	emu.cpu.read_byte = taito8080_read_byte;
	emu.cpu.write_byte = taito8080_write_byte;

	emu.single_step = SINGLE_STEP_NONE;
	emu.romset_index = 0;
	emu.romset_count = 10;

	taito8080_reset();

	return 0;
}
void taito8080_destroy() {
	if (emu.mm.memory != NULL) {
		free(emu.mm.memory);
		emu.mm.memory = NULL;
		emu.mm.video = NULL;
	}
}

void taito8080_save_state() {
	FILE* file = NULL;
	char tmp[32] = { 0 };
	sprintf_s(tmp, 32, "%s.bin", romsets[emu.romset_index].name);
	fopen_s(&file, tmp, "wb");
	if (file == NULL) {
		return;
	}
	fwrite(emu.mm.memory + 0x2000, 1, RAM_SIZE+VIDEO_SIZE, file);
	fwrite(&emu.shift_register, 1, sizeof(MB14241), file);
	fwrite(&emu.cpu, 1, sizeof(I8080), file);
	fclose(file);
}
void taito8080_load_state() {
	FILE* file = NULL;
	char tmp[32] = { 0 };
	sprintf_s(tmp, 32, "%s.bin", romsets[emu.romset_index].name);
	fopen_s(&file, tmp, "rb");
	if (file == NULL) {
		return;
	}

	fread(emu.mm.memory + 0x2000, 1, RAM_SIZE+VIDEO_SIZE, file);
	fread(&emu.shift_register, 1, sizeof(MB14241), file);

	I8080 c = { 0 };
	fread(&c, 1, sizeof(I8080), file);
	emu.cpu.pc = c.pc;
	emu.cpu.sp = c.sp;
	emu.cpu.cycles = c.cycles;
	for (int i = 0; i < 8; ++i) {
		emu.cpu.registers[i] = c.registers[i];
	}
	fclose(file);
}

uint8_t taito8080_default_inp1() {
	uint8_t v = 0;
	set_port_bit(v, 0, emu.controls.insert_coin);
	set_port_bit(v, 1, emu.controls.player2.start);
	set_port_bit(v, 2, emu.controls.player1.start);
	set_port_bit(v, 3, LOW);
	set_port_bit(v, 4, emu.controls.player1.fire);
	set_port_bit(v, 5, emu.controls.player1.left);
	set_port_bit(v, 6, emu.controls.player1.right);
	set_port_bit(v, 7, HIGH);
	return v;
}
uint8_t taito8080_default_inp2() {
	uint8_t v = 0;
	set_port_bit(v, 0, emu.controls.lives & 0x1);
	set_port_bit(v, 1, emu.controls.lives & 0x2);
	set_port_bit(v, 2, emu.controls.tilt_switch);
	set_port_bit(v, 3, emu.controls.bonus_life); // extra life at 1000, 1500
	set_port_bit(v, 4, emu.controls.player2.fire);
	set_port_bit(v, 5, emu.controls.player2.left);
	set_port_bit(v, 6, emu.controls.player2.right);
	set_port_bit(v, 7, emu.controls.coin_info);
	return v;
}

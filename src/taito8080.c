/* taito_8080_machine.c
 * Github: https:\\github.com\tommojphillips
 */

#include <stdint.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "i8080.h"
#include "emulator.h"
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

#define HALF_VBLANK 16666 // (2000000 / 60 / 2)
#define FULL_VBLANK 33333 // (2000000 / 60) // 2 Mhz @ 60 hz 

#define ROM_SIZE   0x4000
#define RAM_SIZE   0x0400
#define VIDEO_SIZE 0x1C00

#define ROMSET_INVADERS     0
#define ROMSET_INVADERS_PT2 1
#define ROMSET_LRESCUE      2
#define ROMSET_BALLBOMBER   3
#define ROMSET_SPACE_LASER  4
#define ROMSET_GALX_WARS    5
#define ROMSET_OZMA_WARS    6
#define ROMSET_SCHASER      7
#define ROMSET_GALACTIC     8
#define ROMSET_INDIANBT     9

const ROMSET taito8080_romsets[] = {
	{ ROMSET_INVADERS,      "space invaders",       invaders_init		},
	{ ROMSET_INVADERS_PT2,  "space invaders pt2",   invaderspt2_init	},
	{ ROMSET_LRESCUE,       "lunar rescue",         lrescue_init		},
	{ ROMSET_BALLBOMBER,    "ballon bomber",        ballbomb_init		},
	{ ROMSET_SPACE_LASER,   "space laser",          spclaser_init		},
	{ ROMSET_GALX_WARS,     "galaxy wars",          galxwars_init		},
	{ ROMSET_OZMA_WARS,     "ozma wars",            solfight_init		},
	{ ROMSET_SCHASER,       "space chaser",         schaser_init        },
	{ ROMSET_GALACTIC,      "galactic",             galactic_init       },
	{ ROMSET_INDIANBT,      "indianbt",             indianbt_init       },
};

TAITO8080 taito8080 = { 0 };

int taito8080_load_romset(int i) {
	memset(taito8080.mm.memory, 0, 0x10000);
	printf("Loading romset: %s\n", taito8080_romsets[i].name);
	if (taito8080_romsets[i].init_romset() != 0) {
		printf("Failed to load romset\n");
		return 1;
	}
	emu.romset_index = i;
	return 0;
}
int taito8080_read_rom(const char* filename, uint32_t offset, uint32_t expected_size) {
	return read_file_into_buffer(filename, taito8080.mm.memory, 0x10000, offset, expected_size);
}

uint8_t taito8080_read_byte(uint16_t address) {
	return emu_read_byte(taito8080.mm.regions, taito8080.mm.region_count, taito8080.mm.memory, address);
}
void taito8080_write_byte(uint16_t address, uint8_t value) {
	emu_write_byte(taito8080.mm.regions, taito8080.mm.region_count, taito8080.mm.memory, address, value);
}

void push_word(I8080* cpu, uint16_t value);

static void taito8080_interrupt(uint8_t rst_num) {
	/* process cpu interrupt */
	if (taito8080.cpu.flags.interrupt) {
		taito8080.cpu.flags.interrupt = 0;
		push_word(&taito8080.cpu, taito8080.cpu.pc);
		taito8080.cpu.pc = (rst_num & 0b111) << 3;
	}
}
void taito8080_step(int steps) {
	int c = 0;
	while (c < steps) {
		++c;
		i8080_execute(&taito8080.cpu);
	}
}
void taito8080_tick(uint32_t cycles) {
	while (taito8080.cpu.cycles < cycles) {
		i8080_execute(&taito8080.cpu);
	}
}
void taito8080_reset() {
	i8080_reset(&taito8080.cpu);
	mb14241_reset(&taito8080.shift_register);
}

void taito8080_update() {
	
	if (emu.single_step == SINGLE_STEP_NONE) {
		taito8080_tick(HALF_VBLANK);
		taito8080_interrupt(1);
		taito8080_tick(FULL_VBLANK);
		taito8080_interrupt(2);
	}
	else {
		if (emu.single_step == SINGLE_STEPPING) {
			emu.single_step = SINGLE_STEP_AWAIT;
			taito8080_step(emu.single_step_increment);
		}
	}
}
void taito8080_vblank() {
	/* Reset cpu cycles for the next frame; 
	this is so the interrupts get served at the right time. */
	taito8080.cpu.cycles = 0;
}

int taito8080_init() {
	taito8080.mm.memory = (uint8_t*)malloc(0x10000);
	if (taito8080.mm.memory == NULL) {
		printf("Failed to allocate Memory\n");
		return 1;
	}
	memset(taito8080.mm.memory, 0, 0x10000);
	taito8080.mm.memory_size = 0x10000;

	taito8080.mm.video = (taito8080.mm.memory + 0x2400);

	i8080_init(&taito8080.cpu);
	taito8080.cpu.read_byte = taito8080_read_byte;
	taito8080.cpu.write_byte = taito8080_write_byte;

	emu.single_step = SINGLE_STEP_NONE;
	emu.single_step_increment = 1;

	taito8080_reset();

	if (taito8080_load_romset(ROMSET_INVADERS) != 0) {
		return 1;
	}

	return 0;
}
void taito8080_destroy() {
	if (taito8080.mm.memory != NULL) {
		free(taito8080.mm.memory);
		taito8080.mm.memory = NULL;
		taito8080.mm.video = NULL;
	}
}

void taito8080_save_state() {
	FILE* file = NULL;
	char tmp[32] = { 0 };
	sprintf_s(tmp, 32, "%s.bin", taito8080_romsets[emu.romset_index].name);
	fopen_s(&file, tmp, "wb");
	if (file == NULL) {
		return;
	}
	fwrite(taito8080.mm.memory + 0x2000, 1, RAM_SIZE+VIDEO_SIZE, file);
	fwrite(&taito8080.shift_register, 1, sizeof(MB14241), file);
	fwrite(&taito8080.cpu, 1, sizeof(I8080), file);
	fclose(file);
}
void taito8080_load_state() {
	FILE* file = NULL;
	char tmp[32] = { 0 };
	sprintf_s(tmp, 32, "%s.bin", taito8080_romsets[emu.romset_index].name);
	fopen_s(&file, tmp, "rb");
	if (file == NULL) {
		return;
	}

	fread(taito8080.mm.memory + 0x2000, 1, RAM_SIZE+VIDEO_SIZE, file);
	fread(&taito8080.shift_register, 1, sizeof(MB14241), file);

	I8080 c = { 0 };
	fread(&c, 1, sizeof(I8080), file);
	taito8080.cpu.pc = c.pc;
	taito8080.cpu.sp = c.sp;
	taito8080.cpu.cycles = c.cycles;
	for (int i = 0; i < 8; ++i) {
		taito8080.cpu.registers[i] = c.registers[i];
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

void taito8080_set_life_def(uint8_t min, uint8_t max) {
	emu.controls.lives = 0;
	emu.controls.lives_min = min;
	emu.controls.lives_max = max;
}
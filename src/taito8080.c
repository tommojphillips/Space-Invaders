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

const ROMSET taito8080_romsets[] = {
	{ ROMSET_INVADERS,      "space invaders",       invaders_init		},
	{ ROMSET_INVADERS_PT2,  "space invaders pt2",   invaderspt2_init	},
	{ ROMSET_LRESCUE,       "lunar rescue",         lrescue_init		},
	{ ROMSET_BALLBOMBER,    "ballon bomber",        ballbomb_init		},
	{ ROMSET_SPACE_LASER,   "space laser",          spclaser_init		},
	{ ROMSET_GALX_WARS,     "galaxy wars",          galxwars_init		},
	{ ROMSET_OZMA_WARS,     "ozma wars",            solfight_init		},
	{ ROMSET_SCHASER,       "space chaser",         schaser_init        },
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
	return read_file_into_buffer(filename, taito8080.mm.rom, 0x10000, offset, expected_size);
}

#define B_START    taito8080.mm.regions[i].start
#define B_SIZE     taito8080.mm.regions[i].size
#define B_END      (B_START + B_SIZE)
#define B_MASK     (B_SIZE - 1)
#define B_WRITABLE ((taito8080.mm.regions[i].flags & MREGION_FLAG_WRITE_PROTECTED) == 0)
#define B_MIRRORED ((taito8080.mm.regions[i].flags & MREGION_FLAG_MIRRORED) != 0)
#define B_PTR      taito8080.mm.memory
#define B_COUNT    taito8080.mm.region_count

uint8_t taito8080_read_byte(uint16_t address) {
	for (int i = 0; i < B_COUNT; ++i) {
		if ((address < B_END || B_MIRRORED) && address >= B_START) {
			return *(uint8_t*)(B_PTR + B_START + (address & B_MASK));
		}
	}
	//printf("reading from %x\n", address);
	return 0;
}
void taito8080_write_byte(uint16_t address, uint8_t value) {
	for (int i = 0; i < B_COUNT; ++i) {
		if (B_WRITABLE && (address < B_END || B_MIRRORED) && address >= B_START) {
			*(uint8_t*)(B_PTR + B_START + (address & B_MASK)) = value;
			return;
		}
	}
	//printf("writing %x to %x\n", value, address);
}

void taito8080_set_writeable_regions(uint8_t value) {
	for (int i = 0; i < B_COUNT; ++i) {
		if (B_WRITABLE) {
			for (int j = 0; j < B_SIZE; ++j) {
				*(uint8_t*)(B_PTR + B_START + (j & B_MASK)) = value;
			}
		}
	}
}

void push_word(I8080* cpu, uint16_t value);

static void taito8080_interrupt(uint8_t rst_num) {
	/* process cpu interrupt */
	if (taito8080.cpu.flags.interrupt) {
		taito8080.cpu.flags.interrupt = 0;
		push_word(&taito8080.cpu, taito8080.cpu.pc);
		uint16_t rst_address = (rst_num & 0b111) << 3;
		taito8080.cpu.pc = rst_address;
	}
}
void taito8080_step(int steps) {
	int c = 0;
	while (!taito8080.cpu.flags.halt && c < steps) {
		++c;
		if (i8080_execute(&taito8080.cpu) != 0) {
			break;
		}
	}
}
void taito8080_tick(uint32_t cycles) {
	while (!taito8080.cpu.flags.halt && taito8080.cpu.cycles < cycles) {
		if (i8080_execute(&taito8080.cpu) != 0) {
			break;
		}
	}
}
void taito8080_reset() {
	i8080_reset(&taito8080.cpu);
	mb14241_reset(&taito8080.shift_register);
	taito8080_set_writeable_regions(0);
}

void taito8080_update() {

	if (taito8080.cpu.flags.halt) return;

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

	taito8080.mm.rom = (taito8080.mm.memory);
	taito8080.mm.ram = (taito8080.mm.rom + 0x2000);
	taito8080.mm.video = (taito8080.mm.ram + 0x0400);

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
		taito8080.mm.rom = NULL;
		taito8080.mm.ram = NULL;
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
	fwrite(taito8080.mm.ram, 1, RAM_SIZE+VIDEO_SIZE, file);
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

	fread(taito8080.mm.ram, 1, RAM_SIZE+VIDEO_SIZE, file);
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
	PORT1 port1 = {
		.coin          = emu.controls.insert_coin,
		.player1_start = emu.controls.player1.start,
		.player2_start = emu.controls.player2.start,
		.player1_fire  = emu.controls.player1.fire,
		.player1_left  = emu.controls.player1.left,
		.player1_right = emu.controls.player1.right,
	};
	return *(uint8_t*)&port1;
}
uint8_t taito8080_default_inp2() {
	PORT2 port2 = {
		.player2_fire  = emu.controls.player2.fire,
		.player2_left  = emu.controls.player2.left,
		.player2_right = emu.controls.player2.right,
		.tilt          = emu.controls.tilt_switch,
		.lives         = emu.controls.lives & 0x3
	};
	return *(uint8_t*)&port2;
}

void taito8080_set_life_def(uint8_t min, uint8_t max) {
	emu.controls.lives = 0;
	emu.controls.lives_min = min;
	emu.controls.lives_max = max;
}
/* altair8800.c
 * Github: https:\\github.com\tommojphillips
 */

#include <stdint.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "file.h"
#include "taito8080.h"
#include "altair8800.h"
#include "i8080.h"

#include "emulator.h"

#define REFRESH_RATE 60
#define CPU_CLOCK 2000000 /* 2 Mhz */
#define VBLANK_RATE (CPU_CLOCK / REFRESH_RATE)

int bas4k_init();
int bas8k_init();

#define BASIC_4K 0
#define BASIC_8K 1

const ROMSET altair8800_roms[] = {
	{ BASIC_4K, "Altair Basic 4K", bas4k_init },
	{ BASIC_8K, "Altair Basic 8K", bas8k_init },
};

int bas4k_init() {
	if (read_file_into_buffer("Basic4k/4kBas_e0.bin", taito8080.mm.memory, 0x10000, 0xE000, 0x800) != 0) return 1;
	if (read_file_into_buffer("Basic4k/4kBas_e8.bin", taito8080.mm.memory, 0x10000, 0xE800, 0x800) != 0) return 1;
	return 0;
}
int bas8k_init() {
	if (read_file_into_buffer("Basic8k/8kBas_e0.bin", taito8080.mm.memory, 0x10000, 0xE000, 0x800) != 0) return 1;
	if (read_file_into_buffer("Basic8k/8kBas_e8.bin", taito8080.mm.memory, 0x10000, 0xE800, 0x800) != 0) return 1;
	if (read_file_into_buffer("Basic8k/8kBas_f0.bin", taito8080.mm.memory, 0x10000, 0xF000, 0x800) != 0) return 1;
	if (read_file_into_buffer("Basic8k/8kBas_f8.bin", taito8080.mm.memory, 0x10000, 0xF800, 0x800) != 0) return 1;
	return 0;
}

int altair8800_load_rom(int i) {
	printf("Loading rom: %s\n", altair8800_roms[i].name);
	if (altair8800_roms[i].init_romset() != 0) {
		return 1;
	}
	emu.romset_index = i;
	return 0;
}

static void altair8800_execute() {
	i8080_execute(&taito8080.cpu);
}

static void cpu_step(int steps) {
	if (emu.single_step == SINGLE_STEPPING) {
		emu.single_step = SINGLE_STEP_AWAIT;
		int c = 0;
		while (c++ < steps) {
			altair8800_execute();
		}
	}
}
static void cpu_tick(uint32_t cycles) {
	while (taito8080.cpu.cycles < cycles) {
		altair8800_execute();
	}
}

uint8_t altair8800_read_byte(uint16_t address) {
	return *(uint8_t*)(taito8080.mm.memory + (address & 0xFFFF));
}
void altair8800_write_byte(uint16_t address, uint8_t value) {
	*(uint8_t*)(taito8080.mm.memory + (address & 0xFFFF)) = value;
}

#define PORT_2SIO_STATUS  0x10
#define PORT_2SIO_READ    0x11

#define PORT_2SIO_CONTROL 0x10
#define PORT_2SIO_WRITE   0x11

#define PORT_FRONT_PANEL_SWITCHES 0xFF

uint8_t term_in() {
	return (uint8_t)getchar();
}
void term_out(uint8_t value) {
	printf("%c", value);
}

#include <conio.h>
#include <ctype.h>
char ch = 0;
uint8_t has_keyboard_input() {
	uint8_t t = _kbhit() ? 1 : 0;
	if (t) {
		ch = (char)_getch();
		return 0x03;
	}
	return 0x02;
}
uint8_t read_keyboard_input() {
	return (uint8_t)toupper(ch);
}

uint8_t altair8800_read_io(uint8_t port) {
	switch (port) {

		case PORT_2SIO_STATUS:
			return has_keyboard_input();

		case PORT_2SIO_READ:
			return read_keyboard_input();

		case PORT_FRONT_PANEL_SWITCHES:
			return 0x0;

		default:
			printf("Reading from undefined port: %02X\n", port);
			return 0;
	}

}
void altair8800_write_io(uint8_t port, uint8_t value) {
	switch (port) {
	
		case PORT_2SIO_CONTROL:
			break;

		case PORT_2SIO_WRITE:
			term_out(value);
			break;

		case PORT_FRONT_PANEL_SWITCHES:
			break;

		default:
			printf("Writing to undefined port: %02X = %02X\n", port, value);
			break;
	}
}

void altair8800_reset() {
	i8080_reset(&taito8080.cpu);
	taito8080.cpu.pc = 0xe000;
	taito8080.cpu.sp = 0xFF00;
	fprintf(stderr, "RESET\n");
}
void altair8800_update() {

	if (taito8080.cpu.flags.halt) return;

	if (emu.single_step == SINGLE_STEP_NONE) {
		cpu_tick(VBLANK_RATE);
	}
	else {
		cpu_step(emu.single_step_increment);
	}
}

int altair8800_init() {
	taito8080.mm.memory = (uint8_t*)malloc(0x10000);
	if (taito8080.mm.memory == NULL) {
		printf("Failed to allocate ROM\n");
		return 1;
	}
	memset(taito8080.mm.memory, 0, 0x10000);
	taito8080.mm.memory_size = 0x10000;
	taito8080.mm.video = NULL;

	i8080_init(&taito8080.cpu);
	taito8080.cpu.read_byte = altair8800_read_byte;
	taito8080.cpu.write_byte = altair8800_write_byte;
	taito8080.cpu.read_io = altair8800_read_io;
	taito8080.cpu.write_io = altair8800_write_io;

	emu.single_step = SINGLE_STEP_NONE;
	emu.single_step_increment = 1;

	altair8800_reset();

	if (altair8800_load_rom(BASIC_8K) != 0) {
		return 1;
	}

	return 0;
}
void altair8800_destroy() {
	if (taito8080.mm.memory != NULL) {
		free(taito8080.mm.memory);
		taito8080.mm.memory = NULL;
		taito8080.mm.video = NULL;
	}
}

void altair8800_vblank() {
	taito8080.cpu.cycles = 0;
}

/* invaders.c
 * Github: https:\\github.com\tommojphillips
 */

#include <stdint.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "file.h"
#include "invaders.h"
#include "emulator.h"
#include "i8080.h"
#include "i8080_mnem.h"

#define ROM_SIZE   0x2000
#define RAM_SIZE   0x0400
#define VIDEO_SIZE 0x1C00

#define ROM_START   0x0000
#define RAM_START   0x2000
#define VIDEO_START 0x2400

#define ROM_END   0x2000
#define RAM_END   0x2400
#define VIDEO_END 0x4000

#define HALF_VBLANK 16666 // (2000000 / 60 / 2)
#define FULL_VBLANK 33333 // (2000000 / 60) // 2 Mhz @ 60 hz 

INVADERS invaders = { 0 };
extern I8080 cpu;

void push_word(I8080* cpu, uint16_t value);

static int load_rom() {
	if (read_file_into_buffer("invaders.h", invaders.mm.rom + 0x000, 0x800) != 0) {
		return 1;
	}
	if (read_file_into_buffer("invaders.g", invaders.mm.rom + 0x800, 0x800) != 0) {
		return 1;
	}
	if (read_file_into_buffer("invaders.f", invaders.mm.rom + 0x1000, 0x800) != 0) {
		return 1;
	}
	if (read_file_into_buffer("invaders.e", invaders.mm.rom + 0x1800, 0x800) != 0) {
		return 1;
	}

	return 0;
}
static void interrupt(uint8_t rst_num) {
	/* process cpu interrupt */
	if (cpu.flags.interrupt) {
		push_word(&cpu, cpu.pc);
		uint16_t rst_address = (rst_num & 0b111) << 3;
		cpu.pc = rst_address;
	}
}

uint8_t invaders_read_byte(uint16_t address) {
	if (address < 0x2000) {
		return *(uint8_t*)(invaders.mm.rom + address);
	}
	else {
		return *(uint8_t*)(invaders.mm.ram + (address & 0x1FFF));
	}
}
void invaders_write_byte(uint16_t address, uint8_t value) {
	if (address < 0x2000) {
		//*(uint8_t*)(invaders.mm.rom + address) = value;
	}
	else {
		*(uint8_t*)(invaders.mm.ram + (address & 0x1FFF)) = value;
	}
}

uint8_t invaders_read_io(uint8_t port) {
	switch (port) {
		case PORT_INPUT1:
			return (*(uint8_t*)&invaders.io_input.input1);
		case PORT_INPUT2:
			if (cpu.flags.interrupt && invaders.io_input.input2.tilt)
				cpu.flags.interrupt = 0; /* prevents redrawing aliens after the screen is cleared. (Hardware probably did this) */
			return (*(uint8_t*)&invaders.io_input.input2);
		
		case PORT_SHIFT_REG:
			return (invaders.shift_reg >> (8 - invaders.shift_amount)) & 0xFF;

		default:
			printf("Reading from undefined port: %02X\n", port);
			break;
	}
	return 0;
}
void invaders_write_io(uint8_t port, uint8_t value) {
	switch (port) {
		case PORT_SHIFT_AMNT:
			invaders.shift_amount = (value & 0x7);
			break;		
		case PORT_SHIFT_DATA:			
			invaders.shift_reg = (value << 8) | (invaders.shift_reg >> 8);
			break;

		case PORT_SOUND1: /* Bank1 Sound */
			invaders.io_output.sound1 = value;
			break;
		case PORT_SOUND2: /* Bank2 Sound */
			invaders.io_output.sound2 = value;
			break;

		case PORT_WATCHDOG:
			/*WATCHDOG*/
			break;

		default:
			printf("Writing to undefined port: %02X = %02X\n", port, value);
			break;
	}
}

int invaders_init() {
	invaders.mm.rom = (uint8_t*)malloc(ROM_SIZE);
	if (invaders.mm.rom == NULL) {
		printf("Failed to allocate ROM\n");
		return 1;
	}
	memset(invaders.mm.rom, 0, ROM_SIZE);

	invaders.mm.ram = (uint8_t*)malloc(RAM_SIZE + VIDEO_SIZE);
	if (invaders.mm.ram == NULL) {
		printf("Failed to allocate RAM\n");
		return 1;
	}
	memset(invaders.mm.ram, 0, RAM_SIZE + VIDEO_SIZE);

	invaders.mm.video = (invaders.mm.ram + RAM_SIZE);
	
	i8080_init(&cpu);
	cpu.read_byte = invaders_read_byte;
	cpu.write_byte = invaders_write_byte;
	cpu.read_io = invaders_read_io;
	cpu.write_io = invaders_write_io;
	
	if (load_rom() != 0) {
		return 1;
	}

	return 0;
}
void invaders_destroy() {
	if (invaders.mm.rom != NULL) {
		free(invaders.mm.rom);
		invaders.mm.rom = NULL;
	}
	if (invaders.mm.ram != NULL) {
		free(invaders.mm.ram);
		invaders.mm.ram = NULL;
		invaders.mm.video = NULL;
	}
}
void invaders_reset() {
	i8080_reset(&cpu);
	emu.single_step_count = 0;
	invaders.shift_amount = 0;
	invaders.shift_reg = 0;
}

static void cpu_step(int steps) {
	int c = 0;
	while (!cpu.flags.halt && c < steps) {
		++c;
		++emu.single_step_count;
		if (i8080_execute(&cpu) != 0) {
			break;
		}
	}
}
static void cpu_tick(uint32_t cycles) {
	while (!cpu.flags.halt && cpu.cycles < cycles) {
		if (i8080_execute(&cpu) != 0) {
			break;
		}
	}
}

void invaders_update() {

	if (cpu.flags.halt)
		return;

	if (emu.single_step == SINGLE_STEP_NONE) {
		cpu_tick(HALF_VBLANK);
		interrupt(1);
		cpu_tick(FULL_VBLANK);
		interrupt(2);
	}
	else {
		if (emu.single_step == SINGLE_STEPPING) {
			emu.single_step = SINGLE_STEP_AWAIT;
			cpu_step(emu.single_step_increment);
		}
	}
}

void invaders_vblank() {
	/* Reset cpu cycles for the next frame; this is so the interrupts get served at the right time. */
	cpu.cycles = 0;
}

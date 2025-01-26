/* invaders.c
 * Github: https:\\github.com\tommojphillips
 */

#include <stdint.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "invaders.h"
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

#define INVADERS_DUMP_SIZE	0x800
#define INVADERS_H_FILE		0x000
#define INVADERS_G_FILE		0x800
#define INVADERS_F_FILE		0x1000
#define INVADERS_E_FILE		0x1800

#define REFRESH_RATE 60
#define CPU_CLOCK 2000000
#define VBLANK_RATE (CPU_CLOCK / REFRESH_RATE)

INVADERS invaders = { 0 };
I8080 cpu = { 0 };
CPU_MNEM mnem = { 0 };
int single_step = 0;
uint32_t single_step_instruction_increment = 1; 
int single_step_instruction_count = 0;

void push_word(I8080* cpu, uint16_t value);

static int read_file_into_buffer(const char* filename, void* buff, const uint32_t expectedSize) {
	FILE* file = NULL;
	uint32_t size = 0;
	if (filename == NULL)
		return 1;

	fopen_s(&file, filename, "rb");
	if (file == NULL) {
		printf("Error: could not open file: %s\n", filename);
		return 1;
	}

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);

	if (expectedSize != 0 && size != expectedSize) {
		printf("Error: invalid file size. Expected %u bytes. Got %u bytes\n", expectedSize, size);
		fclose(file);
		return 1;
	}

	fread(buff, 1, size, file);
	fclose(file);
	printf("Loaded %s\n", filename);
	return 0;
}

uint8_t i8080_read_byte(I8080* cpu, uint16_t address) {
	if (address < 0x2000) {
		return *(uint8_t*)(invaders.mm.rom + address);
	}
	else {
		return *(uint8_t*)(invaders.mm.ram + (address & 0x1FFF));
	}
}
void i8080_write_byte(I8080* cpu, uint16_t address, uint8_t value) {
	if (address < 0x2000) {
		//*(uint8_t*)(invaders.mm.rom + address) = value;
	}
	else {
		*(uint8_t*)(invaders.mm.ram + (address & 0x1FFF)) = value;
	}
}

uint8_t i8080_read_io(I8080* cpu, uint8_t port) {
	switch (port) {
		case PORT_INPUT1:
			return (*(uint8_t*)&invaders.io_input.input1);
		case PORT_INPUT2:
			return (*(uint8_t*)&invaders.io_input.input2);
		case PORT_SHIFT_REG:
			return (invaders.shift_reg >> (8 - invaders.shift_amount)) & 0xFF;
		default:
			printf("Reading from undefined port: %02X\n", port);
			break;
	}
	return 0;
}
void i8080_write_io(I8080* cpu, uint8_t port, uint8_t value) {
	switch (port) {
		case PORT_SHIFT_AMNT:
			invaders.shift_amount = (value & 0x7);
			break;		
		case PORT_SHIFT_DATA:
			//invaders.shift_lsb = invaders.shift_msb;
			//invaders.shift_msb = value;
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

static void ISR(uint8_t rst_num) {
	if (cpu.flags.interrupt) {
		push_word(&cpu, cpu.pc);
		cpu.pc = (rst_num & 0b111) << 3;
	}
}

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
	
	i8080_reset(&cpu);
	mnem.cpu = &cpu;

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
	single_step_instruction_count = 0;
	invaders.shift_amount = 0;
	invaders.shift_reg = 0;
}

void cpu_step(int steps) {
	int c = 0;
	while (!cpu.flags.halt && c < steps) {
		c++;
		single_step_instruction_count++;
		if (i8080_execute(&cpu) != 0) {
			break;
		}
		if (cpu.flags.halt) {
			break;
		}
	}	
}

void cpu_tick(uint32_t cycles) {
	while (!cpu.flags.halt && cpu.cycles < cycles) {
		if (i8080_execute(&cpu) != 0) {
			break;
		}
		if (cpu.flags.halt) {
			break;
		}
	}
}

void invaders_update() {

	if (cpu.flags.halt)
		return;

	if (!single_step) {
		cpu_tick(VBLANK_RATE / 2);
		ISR(1);
		cpu_tick(VBLANK_RATE);
		ISR(2);
	}
	else {
		if (single_step == 2) {
			single_step = 1;
			cpu_step(single_step_instruction_increment);
		}
	}
}

void invaders_vblank() {
	cpu.cycles = 0;
}

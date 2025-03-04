/* cpm.c
 * Github: https:\\github.com\tommojphillips
 */

#include <stdint.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "file.h"
#include "taito8080.h"
#include "cpm.h"
#include "i8080.h"
#include "i8080_mnem.h"

#include "emulator.h"

#define REFRESH_RATE 60
//#define CPU_CLOCK 2000000 /* 2 Mhz */
#define CPU_CLOCK 1000000000 /* 100 Mhz */
#define VBLANK_RATE (CPU_CLOCK / REFRESH_RATE)

extern I8080_MNEM mnem;

int test_init();
int cputest_init();
int tst8080_init();
int pre8080_init();
int exer8080_init();
int ex18080_init();
int exm8080_init();

const ROMSET cpm_tests[] = {
	{ 0, "TEST.COM",    test_init },
	{ 1, "CPUTEST.COM", cputest_init },
	{ 2, "TST8080.COM", tst8080_init },
	{ 3, "8080PRE.COM", pre8080_init },
	{ 4, "8080EXER.COM", exer8080_init },
	{ 5, "8080EXM.COM", exm8080_init },
	{ 6, "8080EX1.COM", ex18080_init },
};

void inject_bdos_signals() {

	// inject "out 0,a" at 0x0000 (signal to stop the test)
	taito8080.mm.memory[0x0000] = 0xD3; /* OUT opcode */
	taito8080.mm.memory[0x0001] = 0x00; /* PORT 0 */
	taito8080.mm.memory[0x0002] = 0x76; /* HLT */

	// inject "out 1,a" at 0x0005 (signal to output some characters)
	taito8080.mm.memory[0x0005] = 0xD3; /* OUT opcode */
	taito8080.mm.memory[0x0006] = 0x01; /* PORT 1 */
	taito8080.mm.memory[0x0007] = 0xC9; /* RET */
}

int test_init() {
	if (read_file_into_buffer("TEST.COM", taito8080.mm.memory, 0x10000, 0x100, 0) != 0) return 1;
	return 0;
}
int cputest_init() {
	if (read_file_into_buffer("CPUTEST.COM", taito8080.mm.memory, 0x10000, 0x100, 0) != 0) return 1;
	return 0;
}
int tst8080_init() {
	if (read_file_into_buffer("TST8080.COM", taito8080.mm.memory, 0x10000, 0x100, 0) != 0) return 1;
	return 0;
}
int pre8080_init() {
	if (read_file_into_buffer("8080PRE.COM", taito8080.mm.memory, 0x10000, 0x100, 0) != 0) return 1;
	return 0;
}
int exer8080_init() {
	if (read_file_into_buffer("8080EXER.COM", taito8080.mm.memory, 0x10000, 0x100, 0) != 0) return 1;
	return 0;
}
int ex18080_init() {
	if (read_file_into_buffer("8080EX1.COM", taito8080.mm.memory, 0x10000, 0x100, 0) != 0) return 1;
	return 0;
}
int exm8080_init() {
	if (read_file_into_buffer("8080EXM.COM", taito8080.mm.memory, 0x10000, 0x100, 0) != 0) return 1;
	return 0;
}
int cpm_load_test(int i) {
	printf("Loading program: %s\n", cpm_tests[i].name);
	if (cpm_tests[i].init_romset() != 0) {
		return 1;
	}
	emu.romset_index = i;
	return 0;
}

static void p_term_cpm() {
	fprintf(stderr, "\nCPM program terminated\n");
}
static void c_write_char() {
	fprintf(stderr, "%c", taito8080.cpu.registers[REG_E]);
}
static void c_write_str() {
	uint16_t i = (taito8080.cpu.registers[REG_D] << 8) | taito8080.cpu.registers[REG_E];
	uint8_t b = taito8080.cpu.read_byte(i++);
	while (b != '$') {
		fprintf(stderr, "%c", b);
		b = taito8080.cpu.read_byte(i++);
	}
}

static void bdos_function(uint8_t func) {
	switch (func) {
		case 0x00:
			p_term_cpm();
			break;

		case 0x02:
			c_write_char();
			break;

		case 0x09:
			c_write_str();
			break;

		default:
			printf("BDOS function %x not implemented\n", func);
			break;
	}
}

static void cpu_step(int steps) {
	int c = 0;
	while (c < steps) {
		++c;
		i8080_execute(&taito8080.cpu);
	}
}
static void cpu_tick(uint32_t cycles) {
	while (taito8080.cpu.cycles < cycles) {
		i8080_execute(&taito8080.cpu);
	}
}

uint8_t cpm_read_byte(uint16_t address) {
	return *(uint8_t*)(taito8080.mm.memory + (address & 0xFFFF));
}
void cpm_write_byte(uint16_t address, uint8_t value) {
	*(uint8_t*)(taito8080.mm.memory + (address & 0xFFFF)) = value;
}

uint8_t cpm_read_io(uint8_t port) {
	(void)port;
	return 0;
}
void cpm_write_io(uint8_t port, uint8_t value) {
	switch (port) {	
		case 0x00:
			p_term_cpm();
			break;

		case 0x01:
			bdos_function(taito8080.cpu.registers[REG_C]);
			break;

		default:
			printf("Writing to undefined port: %02X = %02X\n", port, value);
			break;
	}
}

void cpm_reset() {
	i8080_reset(&taito8080.cpu);
	taito8080.cpu.pc = 0x100;
	taito8080.cpu.sp = 0xFF00;
	fprintf(stderr, "RESET\n");
}
void cpm_update() {

	if (emu.single_step == SINGLE_STEP_NONE) {		
		cpu_tick(VBLANK_RATE);
	}
	else {
		if (emu.single_step == SINGLE_STEPPING) {
			emu.single_step = SINGLE_STEP_AWAIT;
			cpu_step(emu.single_step_increment);
		}
	}
}

int cpm_init() {
	taito8080.mm.memory = (uint8_t*)malloc(0x10000);
	if (taito8080.mm.memory == NULL) {
		printf("Failed to allocate ROM\n");
		return 1;
	}
	memset(taito8080.mm.memory, 0, 0x10000);
	taito8080.mm.memory_size = 0x10000;
	taito8080.mm.video = NULL;

	i8080_init(&taito8080.cpu);
	taito8080.cpu.read_byte = cpm_read_byte;
	taito8080.cpu.write_byte = cpm_write_byte;
	taito8080.cpu.read_io = cpm_read_io;
	taito8080.cpu.write_io = cpm_write_io;

	emu.single_step = SINGLE_STEP_NONE;
	emu.single_step_increment = 1;

	cpm_reset();
	inject_bdos_signals();

	if (cpm_load_test(0) != 0) {
		return 1;
	}

	return 0;
}
void cpm_destroy() {
	if (taito8080.mm.memory != NULL) {
		free(taito8080.mm.memory);
		taito8080.mm.memory = NULL;
		taito8080.mm.video = NULL;
	}
}

void cpm_vblank() {
	taito8080.cpu.cycles = 0;
}

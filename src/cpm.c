/* cpm.c
 * Github: https:\\github.com\tommojphillips
 */

#include <stdint.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "file.h"
#include "emulator.h"
#include "i8080.h"
#include "i8080_mnem.h"

#define REFRESH_RATE 60
#define CPU_CLOCK 2000000
#define VBLANK_RATE (CPU_CLOCK / REFRESH_RATE)

typedef struct {
	uint8_t memory[0x10000]; /* 64 K */
} CPM;

static CPM* cpm;
extern I8080 cpu;
extern I8080_MNEM mnem;
static int failed = 0;

static void cpu_log() {
	if (failed) return;
	cpu_mnem(&mnem, cpu.pc);
	printf("PC: %04X, AF: %02X%02X, BC: %02X%02X, DE: %02X%02X, HL: %02X%02X, SP: %04X, CYC: %d    (%02X %02X %02X %02X) %s\n", 
		cpu.pc,
		cpu.registers[REG_A], cpu.registers[REG_FLAGS],
		cpu.registers[REG_B], cpu.registers[REG_C],
		cpu.registers[REG_D], cpu.registers[REG_E],
		cpu.registers[REG_H], cpu.registers[REG_L], 
		cpu.sp, 
		cpu.cycles,
		cpu.read_byte(cpu.pc+0),
		cpu.read_byte(cpu.pc+1),
		cpu.read_byte(cpu.pc+2),
		cpu.read_byte(cpu.pc+3),
		mnem.str);
}

static void cpu_step(int steps) {
	if (emu.single_step == SINGLE_STEPPING) {
		emu.single_step = SINGLE_STEP_AWAIT;
		int c = 0;
		while (!cpu.flags.halt && c < steps) {
			++c;
			cpu_log();
			if (i8080_execute(&cpu) != 0) {
				break;
			}
		}
	}
}
static void cpu_tick() {
	cpu_log();
	i8080_execute(&cpu);
}

static int load_rom(const char* test) {
	if (read_file_into_buffer(test, cpm->memory + 0x100, 0) != 0) {
		return 1;
	}
	return 0;
}

uint8_t cpm_read_byte(uint16_t address) {
	return *(uint8_t*)(cpm->memory + (address & 0x1FFF));
}
void cpm_write_byte(uint16_t address, uint8_t value) {
	*(uint8_t*)(cpm->memory + (address & 0x1FFF)) = value;
}

uint8_t cpm_read_io(uint8_t port) {
	return 0;
}
void cpm_write_io(uint8_t port, uint8_t value) {
	switch (port) {	
		case 0x00:
			cpu.flags.halt = 0b1;
			break; /* TEST DONE */

		case 0x01:
			switch (cpu.registers[REG_C]) {

				case 0x09: { // output string from DE to char $
					uint16_t address = (cpu.registers[REG_D] << 8) | cpu.registers[REG_E];
					uint16_t i = address;
					uint8_t b = cpu.read_byte(i);
					while (b != '$') {
						printf("%c", b);
						b = cpu.read_byte(++i);
					}
					if (memcmp("\xd\xa CPU HAS FAILED!    ERROR EXIT=", cpm->memory + address, i - address) == 0) {
						failed = 1;					
					}
				} break;

				case 0x02: // output char from E
					printf("%c", cpu.registers[REG_E]);
					break;
			}
			break; /* OUTPUT_RESULT */

		default:
			printf("Writing to undefined port: %02X = %02X\n", port, value);
			break;
	}
}

void cpm_reset() {
	i8080_reset(&cpu);
	failed = 0;
	cpu.pc = 0x100;
}
void cpm_update() {

	if (cpu.flags.halt)
		return;

	if (emu.single_step == SINGLE_STEP_NONE) {		
		cpu_tick();
	}
	else {
		cpu_step(emu.single_step_increment);
	}
}

int cpm_init() {
	cpm = (CPM*)malloc(sizeof(CPM));
	if (cpm == NULL) {
		printf("Failed to allocate CPM\n");
		return 1;
	}	

	i8080_init(&cpu);
	cpu.read_byte = cpm_read_byte;
	cpu.write_byte = cpm_write_byte;
	cpu.read_io = cpm_read_io;
	cpu.write_io = cpm_write_io;

	for (int i = 0; i < sizeof(cpm->memory); i++) {
		cpm->memory[i] = 0;
	}

	// inject "out 0,a" at 0x0000 (signal to stop the test)
	cpm->memory[0x0000] = 0xD3; /* OUT opcode */
	cpm->memory[0x0001] = 0x00; /* PORT 0 */

	// inject "out 1,a" at 0x0005 (signal to output some characters)
	cpm->memory[0x0005] = 0xD3; /* OUT opcode */
	cpm->memory[0x0006] = 0x01; /* PORT 1 */
	cpm->memory[0x0007] = 0xC9; /* RET */

	cpm_reset();
	if (load_rom("CPUTEST.COM") != 0) {
		return 1;
	}

	return 0;
}
void cpm_destroy() {
	if (cpm != NULL) {
		free(cpm);
		cpm = NULL;
	}
}

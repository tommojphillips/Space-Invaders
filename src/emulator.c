/* emulator.c
 * Github: https:\\github.com\tommojphillips
 */

#include <stdint.h>
#include "emulator.h"
#include "i8080.h"

EMULATOR emu = { 0 };
I8080 cpu = { 0 };

/* step by instructions */
void emulator_step(int steps) {
	int c = 0;
	while (!cpu.flags.halt && c < steps) {
		++c;
		if (i8080_execute(&cpu) != 0) {
			break;
		}
	}
}

/* step by cycles */
void emulator_tick(uint32_t cycles) {
	while (!cpu.flags.halt && cpu.cycles < cycles) {
		if (i8080_execute(&cpu) != 0) {
			break;
		}
	}
}
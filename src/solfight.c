/* solfight.c
 * Github: https:\\github.com\tommojphillips
 */

#include <stdint.h>
#include <stdio.h>

#include "file.h"
#include "taito8080.h"

const MEMORY_REGION solfight_regions[] = {
	{ .start = 0x0000, .size = 0x2000, .flags = MREGION_FLAG_WRITE_PROTECTED },
	{ .start = 0x2000, .size = 0x2000, .flags = MREGION_FLAG_NONE            },
	{ .start = 0x4000, .size = 0x1000, .flags = MREGION_FLAG_WRITE_PROTECTED },
};

uint8_t solfight_read_io(uint8_t port) {
	switch (port) {
		
		case PORT_INP0:
			return taito8080.io_input.input0;

		case PORT_INP1:
			return taito8080_default_inp1();

		case PORT_INP2:
			return taito8080_default_inp2();

		default:
			printf("Reading from undefined port: %02X\n", port);
			break;
	}
	return 0;
}
void solfight_write_io(uint8_t port, uint8_t value) {
	switch (port) {

		case PORT_INP0:
			taito8080.io_input.input0 = value;
			break;

		case PORT_SOUND1: /* Bank1 Sound */
			taito8080.io_output.sound1 = value;
			break;
		
		//case PORT_SOUND2: /* Bank2 Sound */
		//	taito8080.io_output.sound2 = value;
		//	break;

		case PORT_WATCHDOG: /*WATCHDOG*/
			taito8080.io_output.watchdog = value;
			break;

		default:
			printf("Writing to undefined port: %02X = %02X\n", port, value);
			break;
	}
}

static int solfight_load_rom() {
	if (taito8080_read_rom("ozmawars/mw01", 0x0000, 0x800) != 0) return 1;
	if (taito8080_read_rom("ozmawars/mw02", 0x0800, 0x800) != 0) return 1;
	if (taito8080_read_rom("ozmawars/mw03", 0x1000, 0x800) != 0) return 1;
	if (taito8080_read_rom("ozmawars/mw04", 0x1800, 0x800) != 0) return 1;

	if (taito8080_read_rom("ozmawars/mw05", 0x4000, 0x800) != 0) return 1;
	if (taito8080_read_rom("ozmawars/mw06", 0x4800, 0x800) != 0) return 1;
	return 0;
}
int solfight_init() {
	taito8080.cpu.read_io = solfight_read_io;
	taito8080.cpu.write_io = solfight_write_io;
	taito8080.mm.regions = solfight_regions;
	taito8080.mm.region_count = 3;

	emu.controls.lives = 0;
	emu.controls.lives_min = 3;
	emu.controls.lives_max = 6;
	return solfight_load_rom();
}

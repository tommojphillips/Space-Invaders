/* invaders.c
 * Github: https:\\github.com\tommojphillips
 */

#include <stdint.h>
#include <stdio.h>

#include "file.h"
#include "taito8080.h"

const MEMORY_REGION invaders_regions[] = {
	{ .start = 0x0000, .size = 0x2000, .flags = MREGION_FLAG_WRITE_PROTECTED },
	{ .start = 0x2000, .size = 0x2000, .flags = MREGION_FLAG_NONE            },
};

uint8_t invaders_read_io(uint8_t port) {
	switch (port) {

		case PORT_INP1:
			return taito8080_default_inp1();

		case PORT_INP2:
			return taito8080_default_inp2();
		
		case PORT_SHIFT_REG:
			return mb14241_shift(&taito8080.shift_register);
			
		default:
			printf("Reading from undefined port: %02X\n", port);
			break;
	}
	return 0;
}
void invaders_write_io(uint8_t port, uint8_t value) {
	switch (port) {

		case PORT_SHIFT_AMNT:
			mb14241_amount(&taito8080.shift_register, value);
			break;

		case PORT_SHIFT_DATA:
			mb14241_data(&taito8080.shift_register, value);
			break;

		case PORT_SOUND1: /* Bank1 Sound */
			taito8080.io_output.sound1 = value;
			break;

		case PORT_SOUND2: /* Bank2 Sound */
			taito8080.io_output.sound2 = value;
			break;

		case PORT_WATCHDOG: /*WATCHDOG*/
			taito8080.io_output.watchdog = value;
			break;

		default:
			printf("Writing to undefined port: %02X = %02X\n", port, value);
			break;
	}
}

static int invaders_load_rom() {
	if (taito8080_read_rom("invaders/invaders.h", 0x0000, 0x800) != 0) return 1;
	if (taito8080_read_rom("invaders/invaders.g", 0x0800, 0x800) != 0) return 1;
	if (taito8080_read_rom("invaders/invaders.f", 0x1000, 0x800) != 0) return 1;
	if (taito8080_read_rom("invaders/invaders.e", 0x1800, 0x800) != 0) return 1;
	return 0;
}
int invaders_init() {
	taito8080.cpu.read_io = invaders_read_io;
	taito8080.cpu.write_io = invaders_write_io;
	taito8080.mm.regions = invaders_regions;
	taito8080.mm.region_count = 2;

	emu.controls.lives = 0;
	emu.controls.lives_min = 3;
	emu.controls.lives_max = 6;
	return invaders_load_rom();
}

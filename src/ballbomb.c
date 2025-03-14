/* ballbomb.c
 * Github: https:\\github.com\tommojphillips
 */

#include <stdint.h>

#include "file.h"
#include "taito8080.h"

const MEMORY_REGION ballbomb_regions[] = { 
	{ .start = 0x0000, .size = 0x2000, .flags = MREGION_FLAG_WRITE_PROTECTED },
	{ .start = 0x2000, .size = 0x2000, .flags = MREGION_FLAG_NONE            },
	{ .start = 0x4000, .size = 0x1000, .flags = MREGION_FLAG_WRITE_PROTECTED },
};

uint8_t ballbomb_read_io(uint8_t port) {
	switch (port) {		

		case PORT_INP1:
			return taito8080_default_inp1();

		case PORT_INP2:
			return taito8080_default_inp2();

		case PORT_SHIFT_REG:
			return mb14241_shift(&emu.shift_register);

		default:
			//printf("Reading from undefined port: %02X\n", port);
			break;
	}
	return 0;
}
void ballbomb_write_io(uint8_t port, uint8_t value) {
	switch (port) {
		
		case PORT_SHIFT_AMNT:
			mb14241_amount(&emu.shift_register, value);
			break;

		case PORT_SHIFT_DATA:
			mb14241_data(&emu.shift_register, value);
			break;

		case PORT_SOUND1: /* Bank1 Sound */
			emu.io_output.sound1 = value;
			break;

		case PORT_SOUND2: /* Bank2 Sound */
			emu.io_output.sound2 = value;
			break;

		case PORT_SOUND3: /* Bank3 Sound */
			emu.io_output.sound3 = value;
			break;

		case PORT_WATCHDOG: /*WATCHDOG*/
			emu.io_output.watchdog = value;
			break;

		default:
			//printf("Writing to undefined port: %02X = %02X\n", port, value);
			break;
	}
}

static int ballbomb_load_rom() {
	if (taito8080_read_rom("ballbomb/tn01",   0x0000, 0x800) != 0) return 1;
	if (taito8080_read_rom("ballbomb/tn02",   0x0800, 0x800) != 0) return 1;
	if (taito8080_read_rom("ballbomb/tn03",   0x1000, 0x800) != 0) return 1;
	if (taito8080_read_rom("ballbomb/tn04",   0x1800, 0x800) != 0) return 1;
	if (taito8080_read_rom("ballbomb/tn05-1", 0x4000, 0x800) != 0) return 1;
	if (taito8080_read_rom("ballbomb/tn06",   0x4800, 0x400) != 0) return 1;
	if (taito8080_read_rom("ballbomb/tn07",   0x4C00, 0x400) != 0) return 1;
	return 0;
}
int ballbomb_init() {
	emu.cpu.read_io = ballbomb_read_io;
	emu.cpu.write_io = ballbomb_write_io;
	emu.mm.regions = ballbomb_regions;
	emu.mm.region_count = 3;
	return ballbomb_load_rom();
}

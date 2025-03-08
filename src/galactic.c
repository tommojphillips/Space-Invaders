/* galactic.c
 * Github: https:\\github.com\tommojphillips
 */

#include <stdint.h>

#include "file.h"
#include "taito8080.h"

const MEMORY_REGION galactic_regions[] = {
	{ .start = 0x0000, .size = 0x2000, .flags = MREGION_FLAG_WRITE_PROTECTED },
	{ .start = 0x2000, .size = 0x2000, .flags = MREGION_FLAG_NONE            },
	{ .start = 0x4000, .size = 0x2000, .flags = MREGION_FLAG_WRITE_PROTECTED },
};

#define PORT3_SOUND 3
#define PORT5_SOUND 5
#define PORT7_SOUND 7

uint8_t galactic_read_io(uint8_t port) {
	switch (port) {		

		case PORT_INP0:
			return 0x40;

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
void galactic_write_io(uint8_t port, uint8_t value) {
	switch (port) {
		
		case PORT_SHIFT_AMNT:
			mb14241_amount(&emu.shift_register, value);
			break;

		case PORT_SHIFT_DATA:
			mb14241_data(&emu.shift_register, value);
			break;

		case PORT3_SOUND: /* Sound */
			break;
		case PORT5_SOUND: /* Sound */
			break;
		case PORT7_SOUND: /* Sound */
			break;

		case PORT_WATCHDOG: /*WATCHDOG*/
			emu.io_output.watchdog = value;
			break;

		default:
			//printf("Writing to undefined port: %02X = %02X\n", port, value);
			break;
	}
}

static int galactic_load_rom() {
	if (taito8080_read_rom("galactic/1", 0x0000, 0x800) != 0) return 1;
	if (taito8080_read_rom("galactic/2", 0x0800, 0x800) != 0) return 1;
	if (taito8080_read_rom("galactic/3", 0x1000, 0x800) != 0) return 1;
	if (taito8080_read_rom("galactic/4", 0x1800, 0x800) != 0) return 1;
	if (taito8080_read_rom("galactic/5", 0x4000, 0x800) != 0) return 1;
	if (taito8080_read_rom("galactic/6", 0x4800, 0x800) != 0) return 1;
	if (taito8080_read_rom("galactic/7", 0x5000, 0x800) != 0) return 1;
	return 0;
}
int galactic_init() {
	emu.cpu.read_io = galactic_read_io;
	emu.cpu.write_io = galactic_write_io;
	emu.mm.regions = galactic_regions;
	emu.mm.region_count = 3;
	return galactic_load_rom();
}

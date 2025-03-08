/* galxwars.c
 * Github: https:\\github.com\tommojphillips
 */

#include <stdint.h>
#include <stdio.h>

#include "file.h"
#include "taito8080.h"

const MEMORY_REGION galxwars_regions[] = {
	{ .start = 0x0000, .size = 0x1000, .flags = MREGION_FLAG_WRITE_PROTECTED },
	{ .start = 0x2000, .size = 0x2000, .flags = MREGION_FLAG_NONE            },
	{ .start = 0x4000, .size = 0x1000, .flags = MREGION_FLAG_WRITE_PROTECTED },
};

static uint8_t galxwars_inp0() {
	uint8_t v = 0;
	set_port_bit(v, 0, LOW);
	set_port_bit(v, 1, LOW);
	set_port_bit(v, 2, LOW);
	set_port_bit(v, 3, LOW);
	set_port_bit(v, 4, LOW);
	set_port_bit(v, 5, LOW);
	set_port_bit(v, 6, HIGH);
	set_port_bit(v, 7, LOW);
	return v;
}

uint8_t galxwars_read_io(uint8_t port) {
	switch (port) {

		case PORT_INP0:
			return galxwars_inp0();

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
void galxwars_write_io(uint8_t port, uint8_t value) {
	switch (port) {
		
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

static int galxwars_load_rom() {
	if (taito8080_read_rom("galxwars/univgw3.0", 0x0000, 0x400) != 0) return 1;
	if (taito8080_read_rom("galxwars/univgw4.1", 0x0400, 0x400) != 0) return 1;
	if (taito8080_read_rom("galxwars/univgw5.2", 0x0800, 0x400) != 0) return 1;
	if (taito8080_read_rom("galxwars/univgw6.3", 0x0C00, 0x400) != 0) return 1;
	if (taito8080_read_rom("galxwars/univgw1.4", 0x4000, 0x400) != 0) return 1;
	if (taito8080_read_rom("galxwars/univgw2.5", 0x4400, 0x400) != 0) return 1;
	return 0;
}
int galxwars_init() {
	taito8080.cpu.read_io = galxwars_read_io;
	taito8080.cpu.write_io = galxwars_write_io;
	taito8080.mm.regions = galxwars_regions;
	taito8080.mm.region_count = 3;

	taito8080_set_life_def(3, 6);
	return galxwars_load_rom();
}

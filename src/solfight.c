/* solfight.c
 * Github: https:\\github.com\tommojphillips
 */

#include <stdint.h>

#include "file.h"
#include "taito8080.h"

const MEMORY_REGION solfight_regions[] = {
	{ .start = 0x0000, .size = 0x2000, .flags = MREGION_FLAG_WRITE_PROTECTED },
	{ .start = 0x2000, .size = 0x2000, .flags = MREGION_FLAG_NONE            },
	{ .start = 0x4000, .size = 0x1000, .flags = MREGION_FLAG_WRITE_PROTECTED },
};

static uint8_t solfight_inp0() {
	uint8_t v = 0;
	return v;
}

static uint8_t solfight_inp1() {
	uint8_t v = 0;
	set_port_bit(v, 0, emu.controls.insert_coin);
	set_port_bit(v, 1, LOW);
	set_port_bit(v, 2, emu.controls.player1.start);
	set_port_bit(v, 3, LOW);
	set_port_bit(v, 4, emu.controls.player1.fire);
	set_port_bit(v, 5, emu.controls.player1.left);
	set_port_bit(v, 6, emu.controls.player1.right);
	set_port_bit(v, 7, HIGH);
	return v;
}

uint8_t solfight_inp2() {
	uint8_t v = 0;
	set_port_bit(v, 0, emu.controls.lives & 0x1);
	set_port_bit(v, 1, emu.controls.lives & 0x2); // energy 0 = 15000; 1 = 20000; 2 = 25000; 3 = 35000
	set_port_bit(v, 2, LOW);
	set_port_bit(v, 3, emu.controls.bonus_life); // extra energy 10000, 15000
	set_port_bit(v, 4, LOW);
	set_port_bit(v, 5, LOW);
	set_port_bit(v, 6, LOW);
	set_port_bit(v, 7, emu.controls.coin_info);
	return v;
}

uint8_t solfight_read_io(uint8_t port) {
	switch (port) {
		
		case PORT_INP0:
			return solfight_inp0();

		case PORT_INP1:
			return solfight_inp1();

		case PORT_INP2:
			return solfight_inp2();

		default:
			//printf("Reading from undefined port: %02X\n", port);
			break;
	}
	return 0;
}
void solfight_write_io(uint8_t port, uint8_t value) {
	switch (port) {

		case PORT_INP0:
			break;
		
		case PORT_WATCHDOG: /*WATCHDOG*/
			emu.io_output.watchdog = value;
			break;

		default:
			//printf("Writing to undefined port: %02X = %02X\n", port, value);
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
	emu.cpu.read_io = solfight_read_io;
	emu.cpu.write_io = solfight_write_io;
	emu.mm.regions = solfight_regions;
	emu.mm.region_count = 3;
	return solfight_load_rom();
}

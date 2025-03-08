/* schaser.c
 * Github: https:\\github.com\tommojphillips
 */

#include <stdint.h>

#include "file.h"
#include "taito8080.h"

const MEMORY_REGION schaser_regions[] = { 
	{ .start = 0x0000, .size = 0x2000, .flags = MREGION_FLAG_WRITE_PROTECTED },
	{ .start = 0x2000, .size = 0x2000, .flags = MREGION_FLAG_NONE },
	{ .start = 0x4000, .size = 0x1000, .flags = MREGION_FLAG_WRITE_PROTECTED },
};

static uint8_t schaser_inp0() {
	uint8_t v = 0;
	set_port_bit(v, 0, emu.controls.player2.up);
	set_port_bit(v, 1, emu.controls.player2.left);
	set_port_bit(v, 2, emu.controls.player2.down);
	set_port_bit(v, 3, emu.controls.player2.right);
	set_port_bit(v, 4, emu.controls.player2.fire);
	set_port_bit(v, 5, LOW); // starting_level_bit1; 00b = 3; 01b = 4; 10b = 5; 11b = 6
	set_port_bit(v, 6, LOW); // starting_level_bit2
	set_port_bit(v, 7, LOW);
	return v;
}
static uint8_t schaser_inp1() {
	uint8_t v = 0;
	set_port_bit(v, 0, emu.controls.player1.up);
	set_port_bit(v, 1, emu.controls.player1.left);
	set_port_bit(v, 2, emu.controls.player1.down);
	set_port_bit(v, 3, emu.controls.player1.right);
	set_port_bit(v, 4, emu.controls.player1.fire);
	set_port_bit(v, 5, emu.controls.player2.start);
	set_port_bit(v, 6, emu.controls.player1.start);
	set_port_bit(v, 7, emu.controls.insert_coin);
	return v; 
}
static uint8_t schaser_inp2() {
	uint8_t v = 0;
	set_port_bit(v, 0, emu.controls.lives & 0x1);
	set_port_bit(v, 1, emu.controls.lives & 0x2);
	set_port_bit(v, 2, LOW);
	set_port_bit(v, 3, LOW); // difficulty; 0b = easy; b1 = hard
	set_port_bit(v, 4, emu.controls.name_reset);
	set_port_bit(v, 5, emu.controls.tilt_switch);
	set_port_bit(v, 6, LOW); // upright = 0; cocktail = 1
	set_port_bit(v, 7, LOW);
	return v;
}

uint8_t schaser_read_io(uint8_t port) {
	switch (port) {

		case PORT_INP0:
			return schaser_inp0();

		case PORT_INP1:
			return schaser_inp1();

		case PORT_INP2:
			return schaser_inp2();

		case PORT_SHIFT_REG:
			return mb14241_shift(&emu.shift_register);

		default:
			//printf("Reading from undefined port: %02X\n", port);
			break;
	}
	return 0;
}
void schaser_write_io(uint8_t port, uint8_t value) {
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

		case PORT_WATCHDOG: /*WATCHDOG*/
			emu.io_output.watchdog = value;
			break;

		default:
			//printf("Writing to undefined port: %02X = %02X\n", port, value);
			break;
	}
}

static int schaser_load_rom() {
	if (taito8080_read_rom("schaser/rt13.bin", 0x0000, 0x400) != 0) return 1;
	if (taito8080_read_rom("schaser/rt14.bin", 0x0400, 0x400) != 0) return 1;
	if (taito8080_read_rom("schaser/rt15.bin", 0x0800, 0x400) != 0) return 1;
	if (taito8080_read_rom("schaser/rt16.bin", 0x0C00, 0x400) != 0) return 1;
	if (taito8080_read_rom("schaser/rt17.bin", 0x1000, 0x400) != 0) return 1;
	if (taito8080_read_rom("schaser/rt18.bin", 0x1400, 0x400) != 0) return 1;
	if (taito8080_read_rom("schaser/rt19.bin", 0x1800, 0x400) != 0) return 1;
	if (taito8080_read_rom("schaser/rt20.bin", 0x1C00, 0x400) != 0) return 1;
	if (taito8080_read_rom("schaser/rt21.bin", 0x4000, 0x400) != 0) return 1;
	if (taito8080_read_rom("schaser/rt22.bin", 0x4400, 0x400) != 0) return 1;
	return 0;
}
int schaser_init() {
	emu.cpu.read_io = schaser_read_io;
	emu.cpu.write_io = schaser_write_io;
	emu.mm.regions = schaser_regions;
	emu.mm.region_count = 3;
	return schaser_load_rom();
}

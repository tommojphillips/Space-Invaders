/* schaser.c
 * Github: https:\\github.com\tommojphillips
 */

#include <stdint.h>
#include <stdio.h>

#include "file.h"
#include "taito8080.h"

const MEMORY_REGION schaser_regions[] = { 
	{ .start = 0x0000, .size = 0x2000, .flags = MREGION_FLAG_WRITE_PROTECTED },
	{ .start = 0x2000, .size = 0x2000, .flags = MREGION_FLAG_NONE },
	{ .start = 0x4000, .size = 0x1000, .flags = MREGION_FLAG_WRITE_PROTECTED },
};

typedef struct {
	uint8_t p_two_up       : 1;
	uint8_t p_two_left     : 1;
	uint8_t p_two_down     : 1;
	uint8_t p_two_right    : 1;
	uint8_t p_two_fire     : 1;
	uint8_t starting_level : 2; // 00b = 3; 01b = 4; 10b = 5; 11b = 6
	uint8_t undefined_1    : 1;
} SCHASER_PORT0;
typedef struct {
	uint8_t p_one_up     : 1;
	uint8_t p_one_left   : 1;
	uint8_t p_one_down   : 1;
	uint8_t p_one_right  : 1;
	uint8_t p_one_fire   : 1;
	uint8_t p_two_start  : 1;
	uint8_t p_one_start  : 1;
	uint8_t coin         : 1;
} SCHASER_PORT1;
typedef struct {
	uint8_t lives        : 2;  // 00b = 3 lives  01b = 4 lives; 10b = 5 lives;  11b = 6 lives
	uint8_t undefined_1  : 1;
	uint8_t difficulty   : 1;
	uint8_t name_reset   : 1;
	uint8_t tilt         : 1;
	uint8_t cabinet_type : 1; // upright = 0; cocktail = 1
	uint8_t undefined_2  : 1;
} SCHASER_PORT2;

static uint8_t schaser_inp0() {
	SCHASER_PORT0 port0 = {
		.p_two_fire  = emu.controls.player2.fire,
		.p_two_left  = emu.controls.player2.left,
		.p_two_right = emu.controls.player2.right,
		.p_two_up    = emu.controls.player2.up,
		.p_two_down  = emu.controls.player2.down,
	};
	return *(uint8_t*)&port0;
}
static uint8_t schaser_inp1() {
	SCHASER_PORT1 port1 = {
		.coin         = emu.controls.insert_coin,
		.p_one_start  = emu.controls.player1.start,
		.p_two_start  = emu.controls.player2.start,
		.p_one_fire   = emu.controls.player1.fire,
		.p_one_left   = emu.controls.player1.left,
		.p_one_right  = emu.controls.player1.right,
		.p_one_up     = emu.controls.player1.up,
		.p_one_down   = emu.controls.player1.down,
	};
	return *(uint8_t*)&port1;
}
static uint8_t schaser_inp2() {
	SCHASER_PORT2 port2 = {
		.tilt  = emu.controls.tilt_switch,
		.lives = emu.controls.lives & 0x3,
		.name_reset = emu.controls.name_reset
	};
	return *(uint8_t*)&port2;
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
			return mb14241_shift(&taito8080.shift_register);

		default:
			printf("Reading from undefined port: %02X\n", port);
			break;
	}
	return 0;
}
void schaser_write_io(uint8_t port, uint8_t value) {
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
	taito8080.cpu.read_io = schaser_read_io;
	taito8080.cpu.write_io = schaser_write_io;
	taito8080.mm.regions = schaser_regions;
	taito8080.mm.region_count = 3;

	emu.controls.lives = 0;
	emu.controls.lives_min = 3;
	emu.controls.lives_max = 6;
	return schaser_load_rom();
}

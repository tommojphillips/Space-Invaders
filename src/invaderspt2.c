/* invaders.c
 * Github: https:\\github.com\tommojphillips
 */

#include <stdint.h>
#include <stdio.h>

#include "file.h"
#include "taito8080.h"

const MEMORY_REGION invaderspt2_regions[] = {
	{ .start = 0x0000, .size = 0x2000, .flags = MREGION_FLAG_WRITE_PROTECTED },
	{ .start = 0x2000, .size = 0x2000, .flags = MREGION_FLAG_NONE            },
	{ .start = 0x4000, .size = 0x1000, .flags = MREGION_FLAG_WRITE_PROTECTED },
};

typedef struct {
	uint8_t undefined_1 : 1;
	uint8_t undefined_2 : 1;
	uint8_t undefined_3 : 1;
	uint8_t undefined_4 : 1;
	uint8_t undefined_5 : 1;
	uint8_t undefined_6 : 1;
	uint8_t name_reset  : 1; // active low
	uint8_t undefined_7 : 1;
} INVADERSPT2_PORT0;

typedef struct {
	uint8_t lives       : 1;  // 0b = 3 lives; 1b = 4 lives
	uint8_t image_rot   : 1;
	uint8_t undefined_1 : 1; // tied high
	uint8_t preset_mode : 1; // 0b = game mode; 1b = name entry
	uint8_t undefined_2 : 1;
	uint8_t undefined_3 : 1;
	uint8_t undefined_4 : 1;
	uint8_t coin_info   : 1;
} INVADERSPT2_PORT2;

static uint8_t invaderspt2_inp0() {
	INVADERSPT2_PORT0 port0 = {
		.name_reset = !emu.controls.name_reset
	};
	return *(uint8_t*)&port0;
}
static uint8_t invaderspt2_inp2() {
	INVADERSPT2_PORT2 port2 = {
		.lives       = emu.controls.lives & 0x1,
		.coin_info   = emu.controls.coin_info,
		.preset_mode = emu.controls.preset_mode,
	};
	return *(uint8_t*)&port2;
}

uint8_t invaderspt2_read_io(uint8_t port) {
	switch (port) {

		case PORT_INP0:
			return invaderspt2_inp0();

		case PORT_INP1:
			return taito8080_default_inp1();

		case PORT_INP2:
			return invaderspt2_inp2();

		case PORT_SHIFT_REG:
			return mb14241_shift(&taito8080.shift_register);

		default:
			printf("Reading from undefined port: %02X\n", port);
			break;
	}
	return 0;
}
void invaderspt2_write_io(uint8_t port, uint8_t value) {
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

static int invaderspt2_load_rom() {
	if (taito8080_read_rom("invadpt2/pv01", 0x0000, 0x800) != 0) return 1;
	if (taito8080_read_rom("invadpt2/pv02", 0x0800, 0x800) != 0) return 1;
	if (taito8080_read_rom("invadpt2/pv03", 0x1000, 0x800) != 0) return 1;
	if (taito8080_read_rom("invadpt2/pv04", 0x1800, 0x800) != 0) return 1;
	if (taito8080_read_rom("invadpt2/pv05", 0x4000, 0x800) != 0) return 1;
	return 0;
}
int invaderspt2_init() {
	taito8080.cpu.read_io = invaderspt2_read_io;
	taito8080.cpu.write_io = invaderspt2_write_io;
	taito8080.mm.regions = invaderspt2_regions;
	taito8080.mm.region_count = 3;
	taito8080.io_input.input0 = 0x40;

	emu.controls.lives = 0;
	emu.controls.lives_min = 3;
	emu.controls.lives_max = 4;
	return invaderspt2_load_rom();
}

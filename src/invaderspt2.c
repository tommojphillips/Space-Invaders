/* invaders.c
 * Github: https:\\github.com\tommojphillips
 */

#include <stdint.h>

#include "file.h"
#include "taito8080.h"

const MEMORY_REGION invaderspt2_regions[] = {
	{ .start = 0x0000, .size = 0x2000, .flags = MREGION_FLAG_WRITE_PROTECTED },
	{ .start = 0x2000, .size = 0x2000, .flags = MREGION_FLAG_NONE            },
	{ .start = 0x4000, .size = 0x1000, .flags = MREGION_FLAG_WRITE_PROTECTED },
};

static uint8_t invaderspt2_inp0() {
	uint8_t v = 0;
	set_port_bit(v, 0, LOW);
	set_port_bit(v, 1, LOW);
	set_port_bit(v, 2, HIGH);
	set_port_bit(v, 3, LOW);
	set_port_bit(v, 4, HIGH);
	set_port_bit(v, 5, HIGH);
	set_port_bit(v, 6, !emu.controls.name_reset);
	set_port_bit(v, 7, HIGH);
	return v;
}
static uint8_t invaderspt2_inp1() {
	uint8_t v = 0;
	set_port_bit(v, 0, emu.controls.insert_coin);
	set_port_bit(v, 1, emu.controls.player2.start);
	set_port_bit(v, 2, emu.controls.player1.start);
	set_port_bit(v, 3, LOW);
	set_port_bit(v, 4, emu.controls.player1.fire);
	set_port_bit(v, 5, emu.controls.player1.left);
	set_port_bit(v, 6, emu.controls.player1.right);
	set_port_bit(v, 7, HIGH);
	return v;
}

static uint8_t invaderspt2_inp2() {
	uint8_t v = 0;
	set_port_bit(v, 0, emu.controls.lives & 0x1); // 0b = 3 lives; 1b = 4 lives
	set_port_bit(v, 1, LOW); // image rotation
	set_port_bit(v, 2, emu.controls.tilt_switch);
	set_port_bit(v, 3, emu.controls.preset_mode); // 0b = game mode; 1b = name entry
	set_port_bit(v, 4, emu.controls.player2.fire);
	set_port_bit(v, 5, emu.controls.player2.left);
	set_port_bit(v, 6, emu.controls.player2.right);
	set_port_bit(v, 7, emu.controls.coin_info);
	return v;
}

uint8_t invaderspt2_read_io(uint8_t port) {
	switch (port) {

		case PORT_INP0:
			return invaderspt2_inp0();

		case PORT_INP1:
			return invaderspt2_inp1();

		case PORT_INP2:
			return invaderspt2_inp2();

		case PORT_SHIFT_REG:
			return mb14241_shift(&emu.shift_register);

		default:
			//printf("Reading from undefined port: %02X\n", port);
			break;
	}
	return 0;
}
void invaderspt2_write_io(uint8_t port, uint8_t value) {
	switch (port) {
		
		case PORT_SHIFT_AMNT:
			mb14241_amount(&emu.shift_register, value);
			break;

		case PORT_SHIFT_DATA:
			mb14241_data(&emu.shift_register, value);
			break;

		case PORT_SOUND1: // Bank1 Sound
			emu.io_output.sound1 = value;
			break;

		case PORT_SOUND2: // Bank2 Sound
			emu.io_output.sound2 = value;
			break;

		case PORT_WATCHDOG: // WATCHDOG
			emu.io_output.watchdog = value;
			break;

		default:
			//printf("Writing to undefined port: %02X = %02X\n", port, value);
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
	emu.cpu.read_io = invaderspt2_read_io;
	emu.cpu.write_io = invaderspt2_write_io;
	emu.mm.regions = invaderspt2_regions;
	emu.mm.region_count = 3;
	return invaderspt2_load_rom();
}

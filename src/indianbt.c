/* indianbt.c
 * Github: https:\\github.com\tommojphillips
 */

#include <stdint.h>

#include "file.h"
#include "taito8080.h"

const MEMORY_REGION indianbt_regions[] = {
	{ .start = 0x0000, .size = 0x2000, .flags = MREGION_FLAG_WRITE_PROTECTED },
	{ .start = 0x2000, .size = 0x2000, .flags = MREGION_FLAG_NONE            },
	{ .start = 0x4000, .size = 0x2000, .flags = MREGION_FLAG_WRITE_PROTECTED },
};

#define PORT3_SOUND 3
#define PORT5_SOUND 5
#define PORT7_SOUND 7

uint8_t indianbt_inp1() {
	uint8_t v = 0;
	set_port_bit(v, 0, !emu.controls.insert_coin); // ACTIVE_LOW
	set_port_bit(v, 1, emu.controls.player2.start);
	set_port_bit(v, 2, emu.controls.player1.start);
	set_port_bit(v, 3, LOW);
	set_port_bit(v, 4, emu.controls.player1.fire);
	set_port_bit(v, 5, emu.controls.player1.left);
	set_port_bit(v, 6, emu.controls.player1.right);
	set_port_bit(v, 7, HIGH);
	return v;
}

uint8_t indianbt_read_io(uint8_t port) {
	switch (port) {		

		case PORT_INP0:
			switch (emu.cpu.pc) {
				case 0x5feb: return 0x10;
				case 0x5ffa: return 0x00;
			}
			return 0;

		case PORT_INP1:
			return indianbt_inp1();

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
void indianbt_write_io(uint8_t port, uint8_t value) {
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

static int indianbt_load_rom() {
	if (taito8080_read_rom("indianbt/1.36", 0x0000, 0x800) != 0) return 1;
	if (taito8080_read_rom("indianbt/2.35", 0x0800, 0x800) != 0) return 1;
	if (taito8080_read_rom("indianbt/3.34", 0x1000, 0x800) != 0) return 1;
	if (taito8080_read_rom("indianbt/4.33", 0x1800, 0x800) != 0) return 1;
	if (taito8080_read_rom("indianbt/5.32", 0x4000, 0x800) != 0) return 1;
	if (taito8080_read_rom("indianbt/6.31", 0x4800, 0x800) != 0) return 1;
	if (taito8080_read_rom("indianbt/7.42", 0x5000, 0x800) != 0) return 1;
	if (taito8080_read_rom("indianbt/8.41", 0x5800, 0x800) != 0) return 1;
	return 0;
}
int indianbt_init() {
	emu.cpu.read_io = indianbt_read_io;
	emu.cpu.write_io = indianbt_write_io;
	emu.mm.regions = indianbt_regions;
	emu.mm.region_count = 3;
	return indianbt_load_rom();
}

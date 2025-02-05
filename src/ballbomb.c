/* ballbomb.c
 * Github: https:\\github.com\tommojphillips
 */

#include <stdint.h>
#include <stdio.h>

#include "file.h"
#include "taito8080.h"

const MM ballbomb_rom_bank1 = {
	.start	= 0x0000,
	.size	= 0x2000,
	.flags	= MM_FLAG_WRITE_PROTECTED
};
const MM ballbomb_rom_bank2 = {
	.start	= 0x4000,
	.size	= 0x1000,
	.flags	= MM_FLAG_WRITE_PROTECTED
};
const MM ballbomb_ram = {
	.start	= 0x2000,
	.size	= 0x2000,
	.flags	= MM_FLAG_MIRROR
};
const MM* const ballbomb_banks[3] = { &ballbomb_rom_bank1, &ballbomb_rom_bank2, &ballbomb_ram };

uint8_t ballbomb_read_io(uint8_t port) {
	switch (port) {

		case PORT_INP1:
			return *(uint8_t*)&taito8080.io_input.input1;

		case PORT_INP2:		
			return *(uint8_t*)&taito8080.io_input.input2;

		case PORT_SHIFT_REG:
			return mb14241_shift(&taito8080.shift_register);

		default:
			printf("Reading from undefined port: %02X\n", port);
			break;
	}
	return 0;
}
void ballbomb_write_io(uint8_t port, uint8_t value) {
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

		case PORT_SOUND3: /* Bank3 Sound */
			taito8080.io_output.sound3 = value;
			break;

		case PORT_WATCHDOG: /*WATCHDOG*/
			taito8080.io_output.watchdog = value;
			break;

		default:
			printf("Writing to undefined port: %02X = %02X\n", port, value);
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
	return 0;
}
int ballbomb_init() {
	taito8080.cpu.read_io = ballbomb_read_io;
	taito8080.cpu.write_io = ballbomb_write_io;
	taito8080.mm.banks = ballbomb_banks;
	taito8080.mm.bank_count = 3;
	return ballbomb_load_rom();
}

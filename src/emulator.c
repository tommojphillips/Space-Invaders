/* emulator.c
 * Github: https:\\github.com\tommojphillips
 */

#include "emulator.h"

EMULATOR emu = { 0 };

#define B_START    regions[i].start
#define B_SIZE     regions[i].size
#define B_END      (B_START + B_SIZE)
#define B_MASK     (B_END - 1)
#define B_WRITABLE ((regions[i].flags & MREGION_FLAG_WRITE_PROTECTED) == 0)
#define B_MIRRORED ((regions[i].flags & MREGION_FLAG_MIRRORED) != 0)

uint8_t emu_read_byte(const MEMORY_REGION* regions, const int count, uint8_t* memory_ptr, uint16_t address) {
	for (int i = 0; i < count; ++i) {
		if ((address < B_END || B_MIRRORED) && address >= B_START) {
			return *(uint8_t*)(memory_ptr + (address & B_MASK));
		}
	}
	//printf("reading from %x\n", address);
	return 0;
}
void emu_write_byte(const MEMORY_REGION* regions, const int count, uint8_t* memory_ptr, uint16_t address, uint8_t value) {
	for (int i = 0; i < count; ++i) {
		if ((address < B_END || B_MIRRORED) && address >= B_START) {
			if (B_WRITABLE) {
				*(uint8_t*)(memory_ptr + (address & B_MASK)) = value;
			}
			return;
		}
	}
	//printf("writing %x to %x\n", value, address);
}

void emu_set_writeable_regions(const MEMORY_REGION* regions, const int count, uint8_t* memory_ptr, uint8_t value) {
	for (int i = 0; i < count; ++i) {
		if (B_WRITABLE) {
			for (int j = 0; j < B_SIZE; ++j) {
				*(uint8_t*)(memory_ptr + (j & B_MASK)) = value;
			}
		}
	}
}
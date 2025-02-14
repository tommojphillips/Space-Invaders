/* emulator.h
 * Github: https:\\github.com\tommojphillips
 */

#include <stdint.h>

#ifndef EMULATOR_H
#define EMULATOR_H

enum {
	SINGLE_STEP_NONE = 0,
	SINGLE_STEPPING = 1,
	SINGLE_STEP_AWAIT = 2,
};

typedef struct {
	int id;
	const char* name;
	int(*init_romset)();
} ROMSET;


#define MREGION_FLAG_NONE            0
#define MREGION_FLAG_WRITE_PROTECTED 2
#define MREGION_FLAG_MIRRORED        4

typedef struct {
	uint16_t start;
	uint16_t size;
	uint8_t flags;
} MEMORY_REGION;

typedef struct {
	uint8_t* memory;
	uint32_t memory_size;

	uint8_t* video;

	const MEMORY_REGION* regions;
	int region_count;
} MEMORY_MAP;

uint8_t emu_read_byte(const MEMORY_REGION* regions, const int count, uint8_t* memory_ptr, uint16_t address);
void emu_write_byte(const MEMORY_REGION* regions, const int count, uint8_t* memory_ptr, uint16_t address, uint8_t value);
void emu_set_writeable_regions(const MEMORY_REGION* regions, const int count, uint8_t* memory_ptr, uint8_t value);

#define get_mask(bit_pos) (1 << (bit_pos))

#define set_bit(v, bit_pos)   (v |= get_mask(bit_pos))
#define clear_bit(v, bit_pos) (v &= ~get_mask(bit_pos))
#define get_bit(v, bit_pos)   ((v & get_mask(bit_pos)) >> bit_pos)

//#define set_port_bit(v, bit_pos, x) ((x) ? set_bit(v,bit_pos) : clear_bit(v,bit_pos))
#define set_port_bit(v, bit_pos, x) (v |= (x << bit_pos))

#define LOW  0
#define HIGH 1

typedef struct {
	int id;
	const char name[32];
	int(*init)();
	void(*destroy)();
	void(*reset)();
	void(*update)();
	void(*vblank)();
	void(*save_state)();
	void(*load_state)();
	int(*load_romset)(int index);
	const ROMSET* romsets;
	int romset_count;
} MACHINE;

typedef struct {
	uint8_t left;
	uint8_t right;
	uint8_t up;
	uint8_t down;
	uint8_t fire;
	uint8_t start;
} PLAYER_INPUT;

typedef struct {

	uint8_t insert_coin;
	uint8_t tilt_switch;

	PLAYER_INPUT player1;
	PLAYER_INPUT player2;

	uint8_t lives;
	uint8_t lives_min;
	uint8_t lives_max;

	uint8_t bonus_life;
	uint8_t coin_info;
	uint8_t preset_mode;
	uint8_t name_reset;

} CONTROLS;

typedef struct {
	const MACHINE* machine;
	CONTROLS controls;
	int romset_index;
	int single_step;
	int single_step_increment;
} EMULATOR;

#ifdef __cplusplus
extern "C" {
#endif

extern EMULATOR emu;

#ifdef __cplusplus
};
#endif
#endif

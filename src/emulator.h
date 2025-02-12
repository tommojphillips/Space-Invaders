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
	char name[32];
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

	uint8_t* rom;
	uint8_t* ram;
	uint8_t* video;

	const MEMORY_REGION* regions;
	int region_count;
} MEMORY_MAP;

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
	uint8_t left  : 1;
	uint8_t right : 1;
	uint8_t up    : 1;
	uint8_t down  : 1;
	uint8_t fire  : 1;
	uint8_t start : 1;
} PLAYER_INPUT;

typedef struct {

	uint8_t insert_coin : 1;
	uint8_t tilt_switch : 1;

	PLAYER_INPUT player1;
	PLAYER_INPUT player2;

	uint8_t lives;
	uint8_t lives_min;
	uint8_t lives_max;

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

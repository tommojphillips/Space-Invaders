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


#define MM_FLAG_NONE			0
#define MM_FLAG_WRITE_PROTECTED 2
#define MM_FLAG_MIRROR          4

typedef struct {
	uint16_t start;
	uint16_t size;
	uint8_t flags;
} MM;

typedef struct {
	uint8_t* memory;
	uint32_t memory_size;

	uint8_t* rom;
	uint32_t rom_size;

	uint8_t* ram;
	uint32_t ram_size;

	uint8_t* video;
	uint32_t video_size;

	const MM* const* banks;
	int bank_count;
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
	const MACHINE* machine;
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

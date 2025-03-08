/* taito8080.h
 * Github: https:\\github.com\tommojphillips
 */

#include <stdint.h>

#include "i8080.h"
#include "mb14241.h"

#ifndef TAITO8080_H
#define TAITO8080_H

#define PORT_INP0 0
#define PORT_INP1 1
#define PORT_INP2 2

#define PORT_SHIFT_AMNT 2
#define PORT_SHIFT_DATA 4
#define PORT_SHIFT_REG  3

#define PORT_SOUND1 3
#define PORT_SOUND2 5
#define PORT_SOUND3 1 /* ball bomber */

#define PORT_WATCHDOG 6


enum {
	SINGLE_STEP_NONE = 0,
	SINGLE_STEPPING = 1,
	SINGLE_STEP_AWAIT = 2,
};

typedef struct {
	int(*init)();
	const char* id;
	const char* name;
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

#define set_port_bit(v, bit_pos, x) (v |= (x << bit_pos))

#define LOW  0
#define HIGH 1

typedef struct {
	uint8_t sound1;
	uint8_t sound2;
	uint8_t sound3;
	uint8_t watchdog;
} OUTPUT_PORT;

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
	I8080 cpu;
	MEMORY_MAP mm;
	MB14241 shift_register;
	OUTPUT_PORT io_output;
	CONTROLS controls;
	int romset_index;
	int romset_count;
	int single_step;
} TAITO8080;

#ifdef __cplusplus
extern "C" {
#endif

extern const ROMSET romsets[];
extern TAITO8080 emu;

int taito8080_init();
void taito8080_destroy();
void taito8080_reset();
void taito8080_update();
void taito8080_vblank();
void taito8080_save_state();
void taito8080_load_state();

int taito8080_load_romset(int i);
int taito8080_read_rom(const char* filename, uint32_t offset, uint32_t expected_size);

uint8_t taito8080_default_inp1();
uint8_t taito8080_default_inp2();

#ifdef __cplusplus
};
#endif
#endif

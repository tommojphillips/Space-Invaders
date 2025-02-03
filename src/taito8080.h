/* taito8080.h
 * Github: https:\\github.com\tommojphillips
 */

#include <stdint.h>

#include "I8080.h"
#include "emulator.h"

#ifndef TAITO8080_H
#define TAITO8080_H

#define PORT_INP0 0
#define PORT_INP1 1
#define PORT_INP2 2

#define PORT_SHIFT_AMNT 2
#define PORT_SHIFT_DATA 4
#define PORT_SHIFT_REG 3

#define PORT_SOUND1 3
#define PORT_SOUND2 5
#define PORT_SOUND3 1 /* ball bomber */

#define PORT_WATCHDOG 6

typedef struct {
	uint8_t coin : 1;
	uint8_t two_player : 1;
	uint8_t one_player : 1;
	uint8_t undefined_1 : 1;
	uint8_t player_fire : 1;
	uint8_t player_left : 1;
	uint8_t player_right : 1;
	uint8_t undefined_2 : 1;
} PORT1;

typedef struct {
	uint8_t ship1 : 1; // 00 = 3 ships  10 = 5 ships
	uint8_t ship2 : 1; // 01 = 4 ships  11 = 6 ships
	uint8_t tilt : 1;
	uint8_t extra_ship : 1; // 0 = extra ship at 1500, 1 = extra ship at 1000
	uint8_t player_fire : 1;
	uint8_t player_left : 1;
	uint8_t player_right : 1;
	uint8_t coin_info : 1; //  Coin info displayed in demo screen 0=ON
} PORT2;

typedef struct {
	uint8_t sound1;
	uint8_t sound2;
	uint8_t sound3;
	uint8_t watchdog;
} OUTPUT_PORT;

typedef struct {
	uint8_t input0;
	PORT1 input1;
	PORT2 input2;
} INPUT_PORT;

typedef struct {
	I8080 cpu;
	MEMORY_MAP mm;
	uint8_t shift_amount;
	uint16_t shift_reg;
	OUTPUT_PORT io_output;
	INPUT_PORT io_input;
} TAITO8080;

#ifdef __cplusplus
extern "C" {
#endif

extern const ROMSET taito8080_romsets[];
extern TAITO8080 taito8080;

int taito8080_init();
void taito8080_destroy();
void taito8080_reset();
void taito8080_update();
void taito8080_vblank();
void taito8080_save_state();
void taito8080_load_state();

int taito8080_load_romset(int i);
int taito8080_read_rom(const char* filename, uint32_t offset, uint32_t expected_size);

#ifdef __cplusplus
};
#endif
#endif

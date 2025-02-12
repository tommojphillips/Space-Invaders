/* taito8080.h
 * Github: https:\\github.com\tommojphillips
 */

#include <stdint.h>

#include "i8080.h"
#include "mb14241.h"
#include "emulator.h"

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

typedef struct {
	uint8_t coin           : 1;
	uint8_t player2_start  : 1;
	uint8_t player1_start  : 1;
	uint8_t undefined_1    : 1; // tied low
	uint8_t player1_fire   : 1;
	uint8_t player1_left   : 1;
	uint8_t player1_right  : 1;
	uint8_t undefined_2    : 1; // tied high
} PORT1;

typedef struct {
	uint8_t lives         : 2; // 00b = 3 lives  01b = 4 lives; 10b = 5 lives;  11b = 6 lives
	uint8_t tilt          : 1;
	uint8_t extra_life    : 1; // 0 = extra ship at 1500, 1 = extra ship at 1000
	uint8_t player2_fire  : 1;
	uint8_t player2_left  : 1;
	uint8_t player2_right : 1;
	uint8_t coin_info     : 1; //  Coin info displayed in demo screen 0=ON
} PORT2;

typedef struct {
	uint8_t sound1;
	uint8_t sound2;
	uint8_t sound3;
	uint8_t watchdog;
} OUTPUT_PORT;

typedef struct {
	uint8_t input0;
} INPUT_PORT;

typedef struct {
	I8080 cpu;
	MEMORY_MAP mm;
	MB14241 shift_register;
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

uint8_t taito8080_default_inp1();
uint8_t taito8080_default_inp2();

#ifdef __cplusplus
};
#endif
#endif

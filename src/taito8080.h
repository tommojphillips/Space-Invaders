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
	uint8_t sound1;
	uint8_t sound2;
	uint8_t sound3;
	uint8_t watchdog;
} OUTPUT_PORT;

typedef struct {
	I8080 cpu;
	MEMORY_MAP mm;
	MB14241 shift_register;
	OUTPUT_PORT io_output;
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

void taito8080_set_life_def(uint8_t min, uint8_t max);

#ifdef __cplusplus
};
#endif
#endif

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
typedef struct _EMULATOR {
	int(*init)();
	void(*destroy)();
	void(*reset)();
	void(*update)();
	void(*vblank)();

	int single_step;
	int single_step_increment;
	int single_step_count;
} EMULATOR;

#ifdef __cplusplus
extern "C" {
#endif

extern EMULATOR emu;

#ifdef __cplusplus
};
#endif
#endif

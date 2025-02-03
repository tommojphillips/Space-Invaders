/* cpm.h
 * Github: https:\\github.com\tommojphillips
 */

#include <stdint.h>

#include "emulator.h"

#ifndef CPM_H
#define CPM_H

#ifdef __cplusplus
extern "C" {
#endif

	extern const ROMSET cpm_tests[];

	int cpm_init();
	void cpm_destroy(); 
	void cpm_reset();
	void cpm_update();
	void cpm_vblank();
	int cpm_load_test(int i);
	
#ifdef __cplusplus
};
#endif
#endif
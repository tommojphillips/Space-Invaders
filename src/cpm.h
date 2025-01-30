/* cpm.h
 * Github: https:\\github.com\tommojphillips
 */

#include <stdint.h>
#include "i8080.h"

#ifndef CPM_H
#define CPM_H

typedef struct {
	uint8_t memory[0x10000]; /* 64 K */
} CPM;

#ifdef __cplusplus
extern "C" {
#endif

	extern CPM* cpm;

	int cpm_init();
	void cpm_destroy(); 
	void cpm_reset();
	void cpm_update();
	
#ifdef __cplusplus
};
#endif
#endif
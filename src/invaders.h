/* invaders.h
 * Github: https:\\github.com\tommojphillips
 */

#include <stdint.h>

#include "i8080.h"

typedef struct {
	uint8_t* rom;	// 8K: 0000 - 1FFF
	uint8_t* ram;	// 1K: 2000 - 23FF
	uint8_t* video;	// 7K: 2400 - 3FFF
} MEMORY_MAP;

#define PORT_INPUT1 1
#define PORT_INPUT2 2
#define PORT_SHIFT_AMNT 2
#define PORT_SHIFT_DATA 4
#define PORT_SHIFT_REG 3
#define PORT_SOUND1 3
#define PORT_SOUND2 5
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
} OUTPUT_PORT;

typedef struct {
	PORT1 input1;
	PORT2 input2;
} INPUT_PORT;

typedef struct {
	MEMORY_MAP mm;
	OUTPUT_PORT io_output;
	INPUT_PORT io_input;
	uint8_t shift_amount;
	uint16_t shift_reg;
} INVADERS;

#ifdef __cplusplus
extern "C" {
#endif

extern INVADERS invaders;
extern I8080 cpu;

int invaders_init();
void invaders_destroy();
void invaders_reset();
void invaders_update();
void invaders_vblank();

#ifdef __cplusplus
};
#endif

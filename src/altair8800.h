/* altair8800.h
 * Github: https:\\github.com\tommojphillips
 */

#ifndef ALTAIR_8800_H
#define ALTAIR_8800_H

extern const ROMSET altair8800_roms[];
int altair8800_load_rom(int i);

void altair8800_reset();
void altair8800_update();
int altair8800_init();
void altair8800_destroy();
void altair8800_vblank();

#endif

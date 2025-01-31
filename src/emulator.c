/* emulator.c
 * Github: https:\\github.com\tommojphillips
 */

#include <stdint.h>
#include "emulator.h"
#include "i8080.h"

EMULATOR emu = { 0 };
I8080 cpu = { 0 };
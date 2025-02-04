/* MB14241.c
 * Github: https:\\github.com\tommojphillips
 */

#include <stdint.h>

#include "mb14241.h"

void mb14241_reset(MB14241* mb14241) {
	mb14241->amount = 0;
	mb14241->data = 0;
}

void mb14241_amount(MB14241* mb14241, uint8_t value) {
	mb14241->amount = (~value & 0x7);
}
void mb14241_data(MB14241* mb14241, uint8_t value) {
	mb14241->data = (value << 7) | (mb14241->data >> 8);
}
uint8_t mb14241_shift(MB14241* mb14241) {
	return (mb14241->data >> mb14241->amount) & 0xFF;
}

/* i8080_mnem.h
 * Github: https:\\github.com\tommojphillips
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	I8080* cpu;
	uint8_t opcode;
	uint16_t pc;
	char str[32];
} CPU_MNEM;

void cpu_mnem_init(CPU_MNEM* mnem, I8080* cpu);
void cpu_mnem(CPU_MNEM* mnem, uint16_t pc);

#ifdef __cplusplus
};
#endif

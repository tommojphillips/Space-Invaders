/* i8080.h
 * Github: https:\\github.com\tommojphillips
 */

#include <stdint.h>

#ifndef i8080_H
#define i8080_H

enum {
	REG_B = 0b000,
	REG_C = 0b001,
	REG_D = 0b010,
	REG_E = 0b011,
	REG_H = 0b100,
	REG_L = 0b101,
	REG_FLAGS = 0b110,
	REG_A = 0b111,
	REG_COUNT
};

enum {
	RP_BC = 0b00,
	RP_DE = 0b01,
	RP_HL = 0b10,
	RP_PSW = 0b11,
	RP_COUNT
};

typedef struct _STATUS_FLAGS {
	uint8_t CF : 1;	
	uint8_t _one : 1;
	uint8_t PF : 1;
	uint8_t _zero2 : 1;
	uint8_t AF : 1;
	uint8_t _zero1 : 1;
	uint8_t ZF : 1;
	uint8_t SF : 1;
} STATUS_FLAGS;

typedef struct {
	uint8_t interrupt : 1;
	uint8_t halt : 1;
} CPU_FLAGS;

typedef struct {
	uint16_t pc;
	uint16_t sp;
	uint8_t registers[REG_COUNT];
	STATUS_FLAGS* status_flags;
	CPU_FLAGS flags;
	uint32_t cycles;
	uint8_t opcode;
} I8080;

#ifdef __cplusplus
extern "C" {
#endif

void i8080_reset(I8080* cpu);
int i8080_execute(I8080* cpu);

uint8_t i8080_read_byte(I8080* cpu, uint16_t address);
void i8080_write_byte(I8080* cpu, uint16_t address, uint8_t value);

uint8_t i8080_read_io(I8080* cpu, uint8_t port);
void i8080_write_io(I8080* cpu, uint8_t port, uint8_t value);

#ifdef __cplusplus
};
#endif
#endif

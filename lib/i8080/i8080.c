/* i8080.c
 * Github: https:\\github.com\tommojphillips
 */

#include "stdint.h"

#include "i8080.h"

#define CYCLES(x) (cpu->cycles += x)

#define PC cpu->pc
#define PCH (cpu->pc >> 8)
#define PCL (cpu->pc & 0xFF)

#define SP cpu->sp
#define SPH (cpu->sp >> 8)
#define SPL (cpu->sp & 0xFF)

#define A  cpu->registers[REG_A]
#define B  cpu->registers[REG_B]
#define C  cpu->registers[REG_C]
#define D  cpu->registers[REG_D]
#define E  cpu->registers[REG_E]
#define H  cpu->registers[REG_H]
#define L  cpu->registers[REG_L]

#define F  cpu->registers[REG_FLAGS]

#define BC	((B << 8) | C)
#define DE	((D << 8) | E)
#define HL	((H << 8) | L)

#define SF cpu->status_flags->SF
#define CF cpu->status_flags->CF
#define ZF cpu->status_flags->ZF
#define AF cpu->status_flags->AF
#define PF cpu->status_flags->PF

#define SSS (cpu->opcode & 0b111)
#define DDD ((cpu->opcode >> 3) & 0b111)

#define READ_BYTE(address) i8080_read_byte(cpu, address)
#define WRITE_BYTE(address, value) i8080_write_byte(cpu, address, value)

#define READ_IO(port) i8080_read_io(cpu, port)
#define WRITE_IO(port, value) i8080_write_io(cpu, port, value)

#define READ_WORD(address) ((READ_BYTE((address) + 1) << 8) | READ_BYTE(address))

#define READ_ADDRESS READ_WORD(PC+1);
#define READ_STACK_ADDRESS READ_WORD(SP)

//#define READ_ADDRESS ((READ_BYTE(PC + 2) << 8) | READ_BYTE(PC+1))
//#define READ_STACK_ADDRESS ((READ_BYTE(SP + 1) << 8) | READ_BYTE(SP))


static uint8_t cal_parity_8bit(uint8_t value) {
	value ^= value >> 4;
	value ^= value >> 2;
	value ^= value >> 1;	
	return (value & 1) ^ 1; // ~(value & 1) & 1;
}
static void alu_and(I8080* cpu, uint8_t* x1, uint8_t x2) {
	*x1 &= x2;
	SF = (*x1 & 0x80) != 0;
	ZF = (*x1 == 0);
	AF = 0;
	PF = cal_parity_8bit(*x1);
	CF = 0;
}
static void alu_xor(I8080* cpu, uint8_t* x1, uint8_t x2) {
	*x1 ^= x2;
	SF = (*x1 & 0x80) != 0;
	ZF = (*x1 == 0);
	AF = 0;
	PF = cal_parity_8bit(*x1);
	CF = 0;
}
static void alu_or(I8080* cpu, uint8_t* x1, uint8_t x2) {
	*x1 |= x2;
	SF = (*x1 & 0x80) != 0;
	ZF = (*x1 == 0);
	AF = 0;
	PF = cal_parity_8bit(*x1);
	CF = 0;
}
static void alu_add(I8080* cpu, uint8_t* x1, uint8_t x2) {
	uint16_t tmp = (*x1 + x2);
	SF = (tmp & 0x80) != 0;
	ZF = (tmp & 0xFF) == 0;
	AF = ((*x1 ^ x2 ^ tmp) & 0x10) != 0;
	PF = cal_parity_8bit(tmp & 0xFF);
	CF = (tmp > 0xFF);
	*x1 = (tmp & 0xFF);
}
static void alu_adc(I8080* cpu, uint8_t* x1, uint8_t x2) {
	uint16_t tmp = (*x1 + x2 + CF);
	SF = (tmp & 0x80) != 0;
	ZF = (tmp & 0xFF) == 0;
	AF = ((*x1 ^ x2 ^ tmp) & 0x10) != 0;
	PF = cal_parity_8bit(tmp & 0xFF);
	CF = (tmp > 0xFF);
	*x1 = (tmp & 0xFF);
}
static void alu_sub(I8080* cpu, uint8_t* x1, uint8_t x2) {
	uint16_t tmp = (*x1 - x2);
	SF = (tmp & 0x80) != 0;
	ZF = ((tmp & 0xFF) == 0);
	AF = ((*x1 ^ x2 ^ tmp) & 0x10) != 0;
	PF = cal_parity_8bit(tmp & 0xFF);
	CF = (*x1 < x2);
	*x1 = (tmp & 0xFF);
}
static void alu_sbb(I8080* cpu, uint8_t* x1, uint8_t x2) {
	uint16_t tmp = (*x1 - x2 - CF);
	SF = (tmp & 0x80) != 0;
	ZF = ((tmp & 0xFF) == 0);
	AF = ((*x1 ^ x2 ^ tmp) & 0x10) != 0;
	PF = cal_parity_8bit(tmp & 0xFF);
	CF = (*x1 < (x2 + CF));
	*x1 = (tmp & 0xFF);
}
static void alu_cmp(I8080* cpu, uint8_t x1, uint8_t x2) {
	alu_sub(cpu, &x1, x2);
}

void push_byte(I8080* cpu, uint8_t value) {
	SP -= 1;
	WRITE_BYTE(SP, value);
}
void pop_byte(I8080* cpu, uint8_t* value) {
	*value = READ_BYTE(SP);
	SP += 1;
}

void push_word(I8080* cpu, uint16_t value) {
	WRITE_BYTE(SP - 1, (value >> 8) & 0xFF);
	WRITE_BYTE(SP - 2, value & 0xFF);
	SP -= 2;
}
void pop_word(I8080* cpu, uint16_t* value) {
	*value = READ_STACK_ADDRESS;
	SP += 2;
}

static void call(I8080* cpu) {
	uint16_t call_address = READ_ADDRESS;
	uint16_t return_address = PC + 3;
	push_word(cpu, return_address);
	PC = call_address;
}
static void ret(I8080* cpu) {
	uint16_t return_address;
	pop_word(cpu, &return_address);
	PC = return_address;
}
static void jmp(I8080* cpu) {
	uint16_t address = READ_ADDRESS;
	PC = address;
}

/* OPCODES */

void CALL(I8080* cpu) {
	call(cpu);
	CYCLES(17);
}
void CC(I8080* cpu) {
	if (CF) {
		call(cpu);
		CYCLES(17);
	}
	else {
		PC += 3;
		CYCLES(11);
	}
}
void CNC(I8080* cpu) {
	if (!CF) {
		call(cpu);
		CYCLES(17);
	}
	else {
		PC += 3;
		CYCLES(11);
	}
}
void CZ(I8080* cpu) {
	if (ZF) {
		call(cpu);
		CYCLES(17);
	}
	else {
		PC += 3;
		CYCLES(11);
	}
}
void CNZ(I8080* cpu) {
	if (!CF) {
		call(cpu);
		CYCLES(17);
	}
	else {
		PC += 3;
		CYCLES(11);
	}
}
void CP(I8080* cpu) {
	if (!SF) {
		call(cpu);
		CYCLES(17);
	}
	else {
		PC += 3;
		CYCLES(11);
	}
}
void CM(I8080* cpu) {
	if (SF) {
		call(cpu);
		CYCLES(17);
	}
	else {
		PC += 3;
		CYCLES(11);
	}
}
void CPE(I8080* cpu) {
	if (PF) {
		call(cpu);
		CYCLES(17);
	}
	else {
		PC += 3;
		CYCLES(11);
	}
}
void CPO(I8080* cpu) {
	if (!PF) {
		call(cpu);
		CYCLES(17);
	}
	else {
		PC += 3;
		CYCLES(11);
	}
}

void RET(I8080* cpu) {
	ret(cpu);
	CYCLES(10);
}
void RC(I8080* cpu) {
	if (CF) {
		ret(cpu);
		CYCLES(11);
	}
	else {
		PC += 1;
		CYCLES(5);
	}
}
void RNC(I8080* cpu) {
	if (!CF) {
		ret(cpu);
		CYCLES(11);
	}
	else {
		PC += 1;
		CYCLES(5);
	}
}
void RZ(I8080* cpu) {
	if (ZF) {
		ret(cpu);
		CYCLES(11);
	}
	else {
		PC += 1;
		CYCLES(5);
	}
}
void RNZ(I8080* cpu) {
	if (!ZF) {
		ret(cpu);
		CYCLES(11);
	}
	else {
		PC += 1;
		CYCLES(5);
	}
}
void RP(I8080* cpu) {
	if (!SF) {
		ret(cpu);
		CYCLES(11);
	}
	else {
		PC += 1;
		CYCLES(5);
	}
}
void RM(I8080* cpu) {
	if (SF) {
		ret(cpu);
		CYCLES(11);
	}
	else {
		PC += 1;
		CYCLES(5);
	}
}
void RPE(I8080* cpu) {
	if (PF) {
		ret(cpu);
		CYCLES(11);
	}
	else {
		PC += 1;
		CYCLES(5);
	}
}
void RPO(I8080* cpu) {
	if (!PF) {
		ret(cpu);
		CYCLES(11);
	}
	else {
		PC += 1;
		CYCLES(5);
	}
}

void JMP(I8080* cpu) {
	/* Jump */
	jmp(cpu);
	CYCLES(10);
}
void JC(I8080* cpu) {
	/* Jump carry */
	if (CF) {
		jmp(cpu);
	}
	else {
		PC += 3;
	}
	CYCLES(10);
}
void JNC(I8080* cpu) {
	/* Jump not carry */
	if (!CF) {
		jmp(cpu);
	}
	else {
		PC += 3;
	}
	CYCLES(10);
}
void JZ(I8080* cpu) {
	/* Jump zero */
	if (ZF) {
		jmp(cpu);
	}
	else {
		PC += 3;
	}
	CYCLES(10);
}
void JNZ(I8080* cpu) {
	/* Jump not zero */
	if (!ZF) {
		jmp(cpu);
	}
	else {
		PC += 3;
	}
	CYCLES(10);
}
void JP(I8080* cpu) {
	/* Jump positive */
	if (!SF) {
		jmp(cpu);
	}
	else {
		PC += 3;
	}
	CYCLES(10);
}
void JM(I8080* cpu) {
	/* Jump minus */
	if (SF) {
		jmp(cpu);
	}
	else {
		PC += 3;
	}
	CYCLES(10);
}
void JPE(I8080* cpu) {
	/* Jump parity even */
	if (PF) {
		jmp(cpu);
	}
	else {
		PC += 3;
	}
	CYCLES(10);
}
void JPO(I8080* cpu) {
	/* Jump parity odd */
	if (!PF) {
		jmp(cpu);
	}
	else {
		PC += 3;
	}
	CYCLES(10);
}

void IN(I8080* cpu) {
	uint8_t port = READ_BYTE(PC + 1);
	A = READ_IO(port);
	PC += 2; 
	CYCLES(10);
}
void OUT(I8080* cpu) {
	uint8_t port = READ_BYTE(PC + 1);
	WRITE_IO(port, A);
	PC += 2;
	CYCLES(10);
}

void LXI_BC(I8080* cpu) {
	/* Load 16-bit IMM into BC */
	C = READ_BYTE(PC + 1);
	B = READ_BYTE(PC + 2);
	PC += 3;
	CYCLES(10);
}
void LXI_DE(I8080* cpu) {
	/* Load 16-bit IMM into DE */
	E = READ_BYTE(PC + 1);
	D = READ_BYTE(PC + 2);
	PC += 3;
	CYCLES(10);
}
void LXI_HL(I8080* cpu) {
	/* Load 16-bit IMM into HL */
	L = READ_BYTE(PC + 1);
	H = READ_BYTE(PC + 2);
	PC += 3;
	CYCLES(10);
}
void LXI_SP(I8080* cpu) {
	/* Load 16-bit IMM into SP */
	SP = READ_ADDRESS;
	PC += 3;
	CYCLES(10);
}

void PUSH_BC(I8080* cpu) {
	/* Push BC */
	push_byte(cpu, B);
	push_byte(cpu, C);
	PC += 1;
	CYCLES(11);
}
void PUSH_DE(I8080* cpu) {
	/* Push DE */
	push_byte(cpu, D);
	push_byte(cpu, E);
	PC += 1;
	CYCLES(11);
}
void PUSH_HL(I8080* cpu) {
	/* Push HL */
	push_byte(cpu, H);
	push_byte(cpu, L);
	PC += 1;
	CYCLES(11);
}
void PUSH_PSW(I8080* cpu) {
	/* Push PSW */
	push_byte(cpu, A);
	push_byte(cpu, F);
	PC += 1;
	CYCLES(11);
}

void POP_BC(I8080* cpu) {
	/* Pop BC */
	pop_byte(cpu, &C);
	pop_byte(cpu, &B);
	PC += 1;
	CYCLES(10);
}
void POP_DE(I8080* cpu) {
	/* Pop DE */
	pop_byte(cpu, &E);
	pop_byte(cpu, &D);
	PC += 1;
	CYCLES(10);
}
void POP_HL(I8080* cpu) {
	/* Pop HL */
	pop_byte(cpu, &L);
	pop_byte(cpu, &H);
	PC += 1;
	CYCLES(10);
}
void POP_PSW(I8080* cpu) {
	/* Pop PSW */
	pop_byte(cpu, &F);
	pop_byte(cpu, &A);
	PC += 1;
	CYCLES(10);
}

void STA(I8080* cpu) {
	/* Store A to [address] */
	uint16_t address = READ_ADDRESS;
	WRITE_BYTE(address, A);
	PC += 3;
	CYCLES(13);
}
void LDA(I8080* cpu) {
	/* Load A from [address] */
	uint16_t address = READ_ADDRESS;
	A = READ_BYTE(address);
	PC += 3;
	CYCLES(13);
}

void XCHG(I8080* cpu) {
	/* Exchange HL with DE */
	uint8_t tmp = L;
	L = E;
	E = tmp;
	tmp = H;
	H = D;
	D = tmp;
	PC += 1;
	CYCLES(4);
}
void XTHL(I8080* cpu) {
	/* Exchange HL with top of stack */
	uint8_t l = L;
	uint8_t h = H;
	L = READ_BYTE(SP);
	H = READ_BYTE(SP+1);
	WRITE_BYTE(SP, l);
	WRITE_BYTE(SP+1, h);
	PC += 1;
	CYCLES(18);
}

void SPHL(I8080* cpu) {
	/* Load SP with HL */
	SP = HL;
	PC += 1;
	CYCLES(5);
}
void PCHL(I8080* cpu) {
	/* Load PC with HL */
	PC = HL;
	CYCLES(5);
}

void DAD_BC(I8080* cpu) {
	/* Add BC to HL */
	uint32_t hl = HL;
	uint32_t bc = BC;
	uint32_t tmp = hl + bc;
	L = tmp & 0xFF;
	H = (tmp >> 8) & 0xFF;
	CF = (tmp > 0xFFFF);
	PC += 1;
	CYCLES(10);
}
void DAD_DE(I8080* cpu) {
	/* Add DE to HL */
	uint32_t tmp = HL + DE;
	L = tmp & 0xFF;
	H = (tmp >> 8) & 0xFF;
	CF = (tmp > 0xFFFF);
	PC += 1;
	CYCLES(10);
}
void DAD_HL(I8080* cpu) {
	/* Add HL to HL; Shift HL left 1 */
	uint32_t tmp = HL << 0x1;
	L = tmp & 0xFF;
	H = (tmp >> 8) & 0xFF;
	CF = (tmp > 0xFFFF);
	PC += 1;
	CYCLES(10);
}
void DAD_SP(I8080* cpu) {
	/* Add SP to HL */
	uint32_t tmp = HL + SP;
	L = tmp & 0xFF;
	H = (tmp >> 8) & 0xFF;
	CF = (tmp > 0xFFFF);
	PC += 1;
	CYCLES(10);
}

void STAX_BC(I8080* cpu) {
	/* Store A at [BC] */
	WRITE_BYTE(BC, A);
	PC += 1;
	CYCLES(7);
}
void STAX_DE(I8080* cpu) {
	/* Store A at [DE] */
	WRITE_BYTE(DE, A);
	PC += 1;
	CYCLES(7);
}

void LDAX_BC(I8080* cpu) {
	/* Load A from [BC] */
	A = READ_BYTE(BC);
	PC += 1;
	CYCLES(7);
}
void LDAX_DE(I8080* cpu) {
	/* Load A from [DE] */
	A = READ_BYTE(DE);
	PC += 1;
	CYCLES(7);
}

void MOV_R_R(I8080* cpu) {
	/* Move register to register */
	cpu->registers[DDD] = cpu->registers[SSS];
	PC += 1;
	CYCLES(5);
}
void MOV_M_R(I8080* cpu) {
	/* Move register to [HL] */
	uint8_t value = cpu->registers[SSS];
	WRITE_BYTE(HL, value);
	PC += 1;
	CYCLES(7);
}
void MOV_R_M(I8080* cpu) {
	/* mov [HL] to register */
	uint8_t value = READ_BYTE(HL);
	cpu->registers[DDD] = value;
	PC += 1;
	CYCLES(7);
}

void MOV_I_R(I8080* cpu) {
	/* Move immediate to register */
	cpu->registers[DDD] = READ_BYTE(PC + 1);
	PC += 2;
	CYCLES(7);
}
void MOV_I_M(I8080* cpu) {
	/* Move immediate to [HL] */
	uint8_t value = READ_BYTE(PC + 1);
	WRITE_BYTE(HL, value);
	PC += 2;
	CYCLES(10);
}

void INR_R(I8080* cpu) {
	/* Increment r */
	uint8_t carry = CF;
	alu_add(cpu, &cpu->registers[DDD], 1);
	CF = carry;
	PC += 1;
	CYCLES(5);
}
void DCR_R(I8080* cpu) {
	/* Decrement r */
	uint8_t carry = CF;
	alu_sub(cpu, &cpu->registers[((cpu->opcode >> 3) & 0b111)], 1);
	CF = carry;
	PC += 1;
	CYCLES(5);
}

void INR_M(I8080* cpu) {
	/* Increment [HL] */
	uint8_t tmp = READ_BYTE(HL);
	uint8_t carry = CF;
	alu_add(cpu, &tmp, 1);
	CF = carry;
	WRITE_BYTE(HL, tmp);
	PC += 1; 
	CYCLES(10);
}
void DCR_M(I8080* cpu) {
	/* Decrement [HL] */
	uint8_t tmp = READ_BYTE(HL);
	uint8_t carry = CF;
	alu_sub(cpu, &tmp, 1);
	CF = carry;
	WRITE_BYTE(HL, tmp);
	PC += 1;
	CYCLES(10);
}

void ADD_R(I8080* cpu) {
	/* ADD r to A */
	alu_add(cpu, &A, cpu->registers[SSS]);
	PC += 1;
	CYCLES(4);
}
void ADC_R(I8080* cpu) {
	/* ADC r to A */
	alu_adc(cpu, &A, cpu->registers[SSS]);
	PC += 1;
	CYCLES(4);
}
void SUB_R(I8080* cpu) {
	/* SUB r from A */
	alu_sub(cpu, &A, cpu->registers[SSS]);
	PC += 1;
	CYCLES(4);
}
void SBB_R(I8080* cpu) {
	/* SBB r from A */
	alu_sbb(cpu, &A, cpu->registers[SSS]);
	PC += 1;
	CYCLES(4);
}

void ANA_R(I8080* cpu) {
	/* AND A with r */
	alu_and(cpu, &A, cpu->registers[SSS]);
	PC += 1;
	CYCLES(4);
}
void XRA_R(I8080* cpu) {
	/* XOR A with r */
	alu_xor(cpu, &A, cpu->registers[SSS]);
	PC += 1;
	CYCLES(4);
}
void ORA_R(I8080* cpu) {
	/* OR A with r */
	alu_or(cpu, &A, cpu->registers[SSS]);
	PC += 1;
	CYCLES(4);
}
void CMP_R(I8080* cpu) {
	/* CMP A with r */
	alu_cmp(cpu, A, cpu->registers[SSS]);
	PC += 1;
	CYCLES(4);
}

void ADD_M(I8080* cpu) {
	/* ADD [HL] to A */
	uint8_t value = READ_BYTE(HL);
	alu_add(cpu, &A, value);
	PC += 1;
	CYCLES(7);
}
void ADC_M(I8080* cpu) {
	/* ADC [HL] to A */
	uint8_t value = READ_BYTE(HL);
	alu_adc(cpu, &A, value);
	PC += 1;
	CYCLES(7);
}
void SUB_M(I8080* cpu) {
	/* SUB [HL] from A */
	uint8_t value = READ_BYTE(HL);
	alu_sub(cpu, &A, value);
	PC += 1;
	CYCLES(7);
}
void SBB_M(I8080* cpu) {
	/* SBB [HL] from A */
	uint8_t value = READ_BYTE(HL);
	alu_sbb(cpu, &A, value);
	PC += 1;
	CYCLES(7);
}

void ANA_M(I8080* cpu) {
	/* AND A with [HL] */
	uint8_t value = READ_BYTE(HL);
	alu_and(cpu, &A, value);
	PC += 1;
	CYCLES(7);
}
void XRA_M(I8080* cpu) {
	/* XOR A with [HL] */
	uint8_t value = READ_BYTE(HL);
	alu_xor(cpu, &A, value);
	PC += 1;
	CYCLES(7);
}
void ORA_M(I8080* cpu) {
	/* OR A with [HL] */
	uint8_t value = READ_BYTE(HL);
	alu_or(cpu, &A, value);
	PC += 1;
	CYCLES(7);
}
void CMP_M(I8080* cpu) {
	/* CMP A with [HL] */
	uint8_t value = READ_BYTE(HL); 
	alu_cmp(cpu, A, value);
	PC += 1;
	CYCLES(7);
}

void ADD_I(I8080* cpu) {
	/* ADD imm to A */
	uint8_t imm = READ_BYTE(PC + 1);
	alu_add(cpu, &A, imm);
	PC += 2;
	CYCLES(7);
}
void ADC_I(I8080* cpu) {
	/* ADC imm to A */
	uint8_t imm = READ_BYTE(PC + 1);
	alu_adc(cpu, &A, imm);
	PC += 2;
	CYCLES(7);
}
void SUB_I(I8080* cpu) {
	/* SUB imm from A */
	uint8_t imm = READ_BYTE(PC + 1);
	alu_sub(cpu, &A, imm);
	PC += 2;
	CYCLES(7);
}
void SBB_I(I8080* cpu) {
	/* SBB imm from A */
	uint8_t imm = READ_BYTE(PC + 1);
	alu_sbb(cpu, &A, imm);
	PC += 2;
	CYCLES(7);
}

void ANA_I(I8080* cpu) {
	/* AND A with imm */
	uint8_t imm = READ_BYTE(PC + 1);
	alu_and(cpu, &A, imm);
	PC += 2;
	CYCLES(7);
}
void XRA_I(I8080* cpu) {
	/* XOR A with imm */
	uint8_t imm = READ_BYTE(PC + 1);
	alu_xor(cpu, &A, imm);
	PC += 2;
	CYCLES(7);
}
void ORA_I(I8080* cpu) {
	/* OR A with imm */
	uint8_t imm = READ_BYTE(PC + 1);
	alu_or(cpu, &A, imm);
	PC += 2;
	CYCLES(7);
}
void CMP_I(I8080* cpu) {
	/* CMP A with imm */
	uint8_t imm = READ_BYTE(PC + 1);
	alu_cmp(cpu, A, imm);
	PC += 2;
	CYCLES(7);
}

void RLC(I8080* cpu) {
	/* Rotate A left */
	uint8_t tmp = ((A & 0x80) >> 0x7);
	CF = tmp;
	A = (A << 0x1) | tmp;
	PC += 1;
	CYCLES(4);
}
void RRC(I8080* cpu) {
	/* Rotate A right */
	uint8_t tmp = (A & 0x1);
	CF = tmp;
	A = (A >> 0x1) | (tmp << 0x7);
	PC += 1;
	CYCLES(4);
}
void RAL(I8080* cpu) {
	/* Rotate A left through carry */
	uint8_t tmp = CF;
	CF = ((A & 0x80) >> 0x7);
	A = (A << 0x1) | tmp;
	PC += 1;
	CYCLES(4);
}
void RAR(I8080* cpu) {
	/* Rotate A right through carry */
	uint8_t tmp = CF;
	CF = (A & 0x1);
	A = (A >> 0x1) | (tmp << 0x7);
	PC += 1;
	CYCLES(4);
}

void INX_BC(I8080* cpu) {
	/* Increment BC */
	uint32_t tmp = (B << 8) | C;
	tmp += 1;
	C = tmp & 0xFF;
	B = (tmp >> 8) & 0xFF;
	PC += 1;
	CYCLES(5);
}
void INX_DE(I8080* cpu) {
	/* Increment DE */
	uint32_t tmp = (D << 8) | E;
	tmp += 1;
	E = tmp & 0xFF;
	D = (tmp >> 8) & 0xFF;
	PC += 1;
	CYCLES(5);
}
void INX_HL(I8080* cpu) {
	/* Increment HL */
	uint32_t tmp = (H << 8) | L;
	tmp += 1;
	L = tmp & 0xFF;
	H = (tmp >> 8) & 0xFF;
	PC += 1;
	CYCLES(5);
}
void INX_SP(I8080* cpu) {
	/* Increment SP */
	uint32_t tmp = SP;
	tmp += 1;
	SP = (tmp & 0xFFFF);
	PC += 1;
	CYCLES(5);
}

void DCX_BC(I8080* cpu) {
	/* Decrement BC */
	uint32_t tmp = BC;
	tmp -= 1;
	C = tmp & 0xFF;
	B = (tmp >> 8) & 0xFF;
	PC += 1;
	CYCLES(5);
}
void DCX_DE(I8080* cpu) {
	/* Decrement DE */
	uint32_t tmp = DE;
	tmp -= 1;
	E = tmp & 0xFF;
	D = (tmp >> 8) & 0xFF;
	PC += 1;
	CYCLES(5);
}
void DCX_HL(I8080* cpu) {
	/* Decrement HL */
	uint32_t tmp = HL;
	tmp -= 1;
	L = tmp & 0xFF;
	H = (tmp >> 8) & 0xFF;
	PC += 1;
	CYCLES(5);
}
void DCX_SP(I8080* cpu) {
	/* Decrement SP */
	uint32_t tmp = SP;
	tmp -= 1;
	SP = (tmp & 0xFFFF);
	PC += 1;
	CYCLES(5);
}

void CMA(I8080* cpu) {
	/* Complement A */
	A = ~A;
	PC += 1;
	CYCLES(4);
}
void STC(I8080* cpu) {
	/* Set carry */
	CF = 0b1;
	PC += 1;
	CYCLES(4);
}
void CMC(I8080* cpu) {
	/* Complement carry */
	CF ^= 0b1;
	PC += 1;
	CYCLES(4);
}
void DAA(I8080* cpu) {
	/* Decimal Adjust A */
	uint8_t correction = 0;
	uint8_t carry = CF;

	if ((A & 0x0F) > 9 || AF) {
		correction |= 0x06;
	}
	if ((A >> 4) > 9 || carry) {
		correction |= 0x60;
		carry = 1;
	}

	alu_add(cpu, &A, correction);
	CF = carry;

	PC += 1;
	CYCLES(4);
}

void SHLD(I8080* cpu) {
	/* Store HL to address */
	uint16_t address = READ_ADDRESS;
	WRITE_BYTE(address, L);
	WRITE_BYTE(address+1, H);
	PC += 3;
	CYCLES(16);
}
void LHLD(I8080* cpu) {
	/* Load HL from address */
	uint16_t address = READ_ADDRESS;
	L = READ_BYTE(address);
	H = READ_BYTE(address+1);
	PC += 3;
	CYCLES(16);
}

void RST(I8080* cpu) {
	uint8_t rst_address = (cpu->opcode & 0b00111000) << 3;
	uint16_t return_address = PC;
	push_word(cpu, return_address);
	PC = rst_address;
	CYCLES(11);
}
void HLT(I8080* cpu) {
	/* halt cpu */
	cpu->flags.halt = 0b1;
	//PC += 1;
	CYCLES(7);
}
void EI(I8080* cpu) {
	/* Enable interrupts */
	cpu->flags.interrupt = 0b1;
	PC += 1;
	CYCLES(4);
}
void DI(I8080* cpu) {
	/* Disable interrupts */
	cpu->flags.interrupt = 0b0;
	PC += 1;
	CYCLES(4);
}

void NOP(I8080* cpu) {
	PC += 1;
	CYCLES(4);
}

void i8080_reset(I8080* cpu) {
	cpu->flags.halt = 0;
	cpu->flags.interrupt = 0;
	cpu->status_flags = (STATUS_FLAGS*)&cpu->registers[REG_FLAGS];
	cpu->cycles = 0;
	cpu->pc = 0;
	cpu->sp = 0;

	for (int i = 0; i < REG_COUNT; ++i) {
		cpu->registers[i] = 0;
	}
}
int i8080_execute(I8080* cpu) {
	
	cpu->opcode = READ_BYTE(PC);

	switch (cpu->opcode) {
		
		case 0x00:
		case 0x08: /* ALT */
		case 0x10: /* ALT */
		case 0x18: /* ALT */
		case 0x20: /* ALT */
		case 0x28: /* ALT */
		case 0x30: /* ALT */
		case 0x38: /* ALT */
			NOP(cpu);
			break;

		case 0x01:
			LXI_BC(cpu);
			break;
		case 0x11:
			LXI_DE(cpu);
			break;
		case 0x21:
			LXI_HL(cpu);
			break;
		case 0x31:
			LXI_SP(cpu);
			break;

		case 0x02:
			STAX_DE(cpu);
			break;
		case 0x12:
			STAX_DE(cpu);
			break;

		case 0x22:
			SHLD(cpu);
			break;

		case 0x32:
			STA(cpu);
			break;

		case 0x03:
			INX_BC(cpu);
			break;
		case 0x13:
			INX_DE(cpu);
			break;
		case 0x23:
			INX_HL(cpu);
			break;
		case 0x33:
			INX_SP(cpu);
			break;

		case 0x04:
		case 0x14:
		case 0x24:
		case 0x0C:
		case 0x1C:
		case 0x2C:
		case 0x3C:
			INR_R(cpu);
			break;

		case 0x34:
			INR_M(cpu);
			break;

		case 0x05:
		case 0x15:
		case 0x25:
		case 0x0D:
		case 0x1D:
		case 0x2D:
		case 0x3D:
			DCR_R(cpu);
			break;

		case 0x35:
			DCR_M(cpu);
			break;

		case 0x06:
		case 0x16:
		case 0x26:
		case 0x0E:
		case 0x1E:
		case 0x2E:
		case 0x3E:
			MOV_I_R(cpu);
			break;

		case 0x36:
			MOV_I_M(cpu);
			break;

		case 0x07:
			RLC(cpu);
			break;
		case 0x17:
			RAL(cpu);
			break;

		case 0x27:
			DAA(cpu);
			break;

		case 0x37:
			STC(cpu);
			break;

		case 0x09:
			DAD_BC(cpu);
			break;
		case 0x19:
			DAD_DE(cpu);
			break;
		case 0x29:
			DAD_HL(cpu);
			break;
		case 0x39:
			DAD_SP(cpu);
			break;

		case 0x0A:
			LDAX_BC(cpu);
			break;
		case 0x1A:
			LDAX_DE(cpu);
			break;
		case 0x2A:
			LHLD(cpu);
			break;
		case 0x3A:
			LDA(cpu);
			break;

		case 0x0B:
			DCX_BC(cpu);
			break;
		case 0x1B:
			DCX_DE(cpu);
			break;
		case 0x2B:
			DCX_HL(cpu);
			break;
		case 0x3B:
			DCX_SP(cpu);
			break;

		case 0x0F:
			RRC(cpu);
			break;
		case 0x1F:
			RAR(cpu);
			break;
		case 0x2F:
			CMA(cpu);
			break;
		case 0x3F:
			CMC(cpu);
			break;

		case 0x40: // MOV r, r
		case 0x41:
		case 0x42:
		case 0x43:
		case 0x44:
		case 0x45:
		case 0x47:
		case 0x48:
		case 0x49:
		case 0x4A:
		case 0x4B:
		case 0x4C:
		case 0x4D:
		case 0x4F:
		case 0x50:
		case 0x51:
		case 0x52:
		case 0x53:
		case 0x54:
		case 0x55:
		case 0x57:
		case 0x58:
		case 0x59:
		case 0x5A:
		case 0x5B:
		case 0x5C:
		case 0x5D:
		case 0x5F:
		case 0x60:
		case 0x61:
		case 0x62:
		case 0x63:
		case 0x64:
		case 0x65:
		case 0x67:
		case 0x68:
		case 0x69:
		case 0x6A:
		case 0x6B:
		case 0x6C:
		case 0x6D:
		case 0x6F:
		case 0x78:
		case 0x79:
		case 0x7A:
		case 0x7B:
		case 0x7C:
		case 0x7D:
		case 0x7F:
			MOV_R_R(cpu);
			break;

		case 0x70: // MOV M, r
		case 0x71:
		case 0x72:
		case 0x73:
		case 0x74:
		case 0x75:
		case 0x77:
			MOV_M_R(cpu);
			break;

		case 0x46: // MOV r, M
		case 0x4E:
		case 0x56:
		case 0x5E:
		case 0x66:
		case 0x6E:
		case 0x7E:
			MOV_R_M(cpu);
			break;

		case 0x80:
		case 0x81:
		case 0x82:
		case 0x83:
		case 0x84:
		case 0x85:
		case 0x87:
			ADD_R(cpu);
			break;

		case 0x88:
		case 0x89:
		case 0x8A:
		case 0x8B:
		case 0x8C:
		case 0x8D:
		case 0x8F:
			ADC_R(cpu);
			break;

		case 0x90:
		case 0x91:
		case 0x92:
		case 0x93:
		case 0x94:
		case 0x95:
		case 0x97:
			SUB_R(cpu);
			break;

		case 0x98:
		case 0x99:
		case 0x9A:
		case 0x9B:
		case 0x9C:
		case 0x9D:
		case 0x9F:
			SBB_R(cpu);
			break;

		case 0xA0:
		case 0xA1:
		case 0xA2:
		case 0xA3:
		case 0xA4:
		case 0xA5:
		case 0xA7:
			ANA_R(cpu);
			break;

		case 0xA8:
		case 0xA9:
		case 0xAA:
		case 0xAB:
		case 0xAC:
		case 0xAD:
		case 0xAF:
			XRA_R(cpu);
			break;

		case 0xB0:
		case 0xB1:
		case 0xB2:
		case 0xB3:
		case 0xB4:
		case 0xB5:
		case 0xB7:
			ORA_R(cpu);
			break;

		case 0xB8:
		case 0xB9:
		case 0xBA:
		case 0xBB:
		case 0xBC:
		case 0xBD:
		case 0xBF:
			CMP_R(cpu);
			break;

		case 0x86:
			ADD_M(cpu);
			break;
		case 0x8E:
			ADC_M(cpu);
			break;
		case 0x96:
			SUB_M(cpu);
			break;
		case 0x9E:
			SBB_M(cpu);
			break;
		case 0xA6:
			ANA_M(cpu);
			break;
		case 0xAE:
			XRA_M(cpu);
			break;
		case 0xB6:
			ORA_M(cpu);
			break;
		case 0xBE:
			CMP_M(cpu);
			break;

		case 0xC6:
			ADD_I(cpu);
			break;
		case 0xCE:
			ADC_I(cpu);
			break;
		case 0xD6:
			SUB_I(cpu);
			break;
		case 0xDE:
			SBB_I(cpu);
			break;
		case 0xE6:
			ANA_I(cpu);
			break;
		case 0xEE:
			XRA_I(cpu);
			break;
		case 0xF6:
			ORA_I(cpu);
			break;
		case 0xFE:
			CMP_I(cpu);
			break;

		case 0xC0:
			RNZ(cpu);
			break;
		case 0xD0:
			RNC(cpu);
			break;
		case 0xE0:
			RPO(cpu);
			break;
		case 0xF0:
			RP(cpu);
			break;

		case 0xC1:
			POP_BC(cpu);
			break;
		case 0xD1:
			POP_DE(cpu);
			break;
		case 0xE1:
			POP_HL(cpu);
			break;
		case 0xF1:
			POP_PSW(cpu);
			break;

		case 0xC2:
			JNZ(cpu);
			break;
		case 0xD2:
			JNC(cpu);
			break;
		case 0xE2:
			JPO(cpu);
			break;
		case 0xF2:
			JP(cpu);
			break;

		case 0xC3:
			JMP(cpu);
			break;

		case 0xE3:
			XTHL(cpu);
			break;

		case 0xC4:
			CNZ(cpu);
			break;
		case 0xD4:
			CNC(cpu);
			break;
		case 0xE4:
			CPO(cpu);
			break;
		case 0xF4:
			CP(cpu);
			break;

		case 0xC5:
			PUSH_BC(cpu);
			break;
		case 0xD5:
			PUSH_DE(cpu);
			break;
		case 0xE5:
			PUSH_HL(cpu);
			break;
		case 0xF5:
			PUSH_PSW(cpu);
			break;

		case 0xC9:
		case 0xD9: /* ALT */
			RET(cpu);
			break;

		case 0xC8:
			RZ(cpu);
			break;
		case 0xD8:
			RC(cpu);
			break;
		case 0xE8:
			RPE(cpu);
			break;
		case 0xF8:
			RM(cpu);
			break;

		case 0xE9:
			PCHL(cpu);
			break;
		case 0xF9:
			SPHL(cpu);
			break;

		case 0xCA:
			JZ(cpu);
			break;
		case 0xDA:
			JC(cpu);
			break;
		case 0xEA:
			JPE(cpu);
			break;
		case 0xFA:
			JM(cpu);
			break;

		case 0xCB:
			JMP(cpu);
			break;

		case 0xD3:
			OUT(cpu);
			break;
		case 0xDB:
			IN(cpu);
			break;

		case 0xEB:
			XCHG(cpu);
			break;

		case 0xCC:
			CZ(cpu);
			break;
		case 0xDC:
			CC(cpu);
			break;
		case 0xEC:
			CPE(cpu);
			break;
		case 0xFC:
			CM(cpu);
			break;

		case 0xCD:
		case 0xDD: /* ALT */
		case 0xED: /* ALT */
		case 0xFD: /* ALT */
			CALL(cpu);
			break;

		case 0xC7:
		case 0xCF:
		case 0xD7:
		case 0xDF:
		case 0xE7:
		case 0xEF:
		case 0xF7:
		case 0xFF:
			RST(cpu);
			break;

		case 0xFB:
			EI(cpu);
			break;

		case 0xF3:
			DI(cpu);
			break;

		case 0x76:
			HLT(cpu);
			break;

		default:
			return 1;
	}

	return 0;
}

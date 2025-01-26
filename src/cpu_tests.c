
#include <stdio.h>
#include "i8080.h"

void CALL(I8080* cpu);
void CC(I8080* cpu);
void CNC(I8080* cpu);
void CZ(I8080* cpu);
void CNZ(I8080* cpu);
void CP(I8080* cpu);
void CM(I8080* cpu);
void CPE(I8080* cpu);
void CPO(I8080* cpu);

void RET(I8080* cpu);
void RC(I8080* cpu);
void RNC(I8080* cpu);
void RZ(I8080* cpu);
void RNZ(I8080* cpu);
void RP(I8080* cpu);
void RM(I8080* cpu);
void RPE(I8080* cpu);
void RPO(I8080* cpu);

void RST(I8080* cpu);

void IN(I8080* cpu);
void OUT(I8080* cpu);

void LXI_BC(I8080* cpu);
void LXI_DE(I8080* cpu);
void LXI_HL(I8080* cpu);
void LXI_SP(I8080* cpu);

void PUSH_BC(I8080* cpu);
void PUSH_DE(I8080* cpu);
void PUSH_HL(I8080* cpu);
void PUSH_PSW(I8080* cpu);

void POP_BC(I8080* cpu);
void POP_DE(I8080* cpu);
void POP_HL(I8080* cpu);
void POP_PSW(I8080* cpu);

void STA(I8080* cpu);
void LDA(I8080* cpu);

void XCHG(I8080* cpu);
void XTHL(I8080* cpu);

void SPHL(I8080* cpu);
void PCHL(I8080* cpu);

void DAD_BC(I8080* cpu);
void DAD_DE(I8080* cpu);
void DAD_HL(I8080* cpu);
void DAD_SP(I8080* cpu);

void STAX_BC(I8080* cpu);
void STAX_DE(I8080* cpu);

void LDAX_BC(I8080* cpu);
void LDAX_DE(I8080* cpu);

void MOV_R_R(I8080* cpu);
void MOV_M_R(I8080* cpu);
void MOV_R_M(I8080* cpu);

void HLT(I8080* cpu);

void MOV_I_R(I8080* cpu);
void MOV_I_M(I8080* cpu);

void INR_R(I8080* cpu);
void DCR_R(I8080* cpu);

void INR_M(I8080* cpu);
void DCR_M(I8080* cpu);

void ADD_R(I8080* cpu);
void ADC_R(I8080* cpu);
void SUB_R(I8080* cpu);
void SBB_R(I8080* cpu);

void ANA_R(I8080* cpu);
void XRA_R(I8080* cpu);
void ORA_R(I8080* cpu);
void CMP_R(I8080* cpu);

void ADD_M(I8080* cpu);
void ADC_M(I8080* cpu);
void SUB_M(I8080* cpu);
void SBB_M(I8080* cpu);

void ANA_M(I8080* cpu);
void XRA_M(I8080* cpu);
void ORA_M(I8080* cpu);
void CMP_M(I8080* cpu);

void ADD_I(I8080* cpu);
void ADC_I(I8080* cpu);
void SUB_I(I8080* cpu);
void SBB_I(I8080* cpu);

void ANA_I(I8080* cpu);
void XRA_I(I8080* cpu);
void ORA_I(I8080* cpu);
void CMP_I(I8080* cpu);

void RLC(I8080* cpu);
void RRC(I8080* cpu);
void RAL(I8080* cpu);
void RAR(I8080* cpu);

void JMP(I8080* cpu);
void JC(I8080* cpu);
void JNC(I8080* cpu);
void JZ(I8080* cpu);
void JNZ(I8080* cpu);
void JP(I8080* cpu);
void JM(I8080* cpu);
void JPE(I8080* cpu);
void JPO(I8080* cpu);

void INX_BC(I8080* cpu);
void INX_DE(I8080* cpu);
void INX_HL(I8080* cpu);
void INX_SP(I8080* cpu);
void DCX_BC(I8080* cpu);
void DCX_DE(I8080* cpu);
void DCX_HL(I8080* cpu);
void DCX_SP(I8080* cpu);
void CMA(I8080* cpu);
void STC(I8080* cpu);
void CMC(I8080* cpu);
void DAA(I8080* cpu);
void SHLD(I8080* cpu);
void LHLD(I8080* cpu);
void EI(I8080* cpu);
void DI(I8080* cpu);
void NOP(I8080* cpu);

void alu_and(I8080* cpu, uint8_t* x1, uint8_t x2);
void alu_xor(I8080* cpu, uint8_t* x1, uint8_t x2);
void alu_or(I8080* cpu, uint8_t* x1, uint8_t x2);
void alu_add(I8080* cpu, uint8_t* x1, uint8_t x2);
void alu_adc(I8080* cpu, uint8_t* x1, uint8_t x2);
void alu_sub(I8080* cpu, uint8_t* x1, uint8_t x2);
void alu_sbb(I8080* cpu, uint8_t* x1, uint8_t x2);
void alu_cmp(I8080* cpu, uint8_t x1, uint8_t x2);

#define PC cpu.pc
#define PCH (cpu.pc >> 8)
#define PCL (cpu.pc & 0xFF)

#define SP cpu.sp
#define SPH (cpu.sp >> 8)
#define SPL (cpu.sp & 0xFF)

#define A  cpu.registers[REG_A]
#define B  cpu.registers[REG_B]
#define C  cpu.registers[REG_C]
#define D  cpu.registers[REG_D]
#define E  cpu.registers[REG_E]
#define H  cpu.registers[REG_H]
#define L  cpu.registers[REG_L]

#define BC	cpu.register_pairs[RP_BC]
#define DE	cpu.register_pairs[RP_DE]
#define HL	cpu.register_pairs[RP_HL]
#define PSW	cpu.register_pairs[RP_PSW]

#define SF cpu.status_flags->SF
#define CF cpu.status_flags->CF
#define ZF cpu.status_flags->ZF
#define AF cpu.status_flags->AF
#define PF cpu.status_flags->PF

#define SSS (cpu.opcode & 0b111)
#define DDD ((cpu.opcode >> 3) & 0b111)

#define READ_BYTE(address) i8080_read_byte(&cpu, address)
#define WRITE_BYTE(address, value) i8080_write_byte(&cpu, address, value)

#define READ_IO(port) i8080_read_io(&cpu, port)
#define WRITE_IO(port, value) i8080_write_io(&cpu, port, value)

#define ASSERT_EQUAL(expected, actual, message) \
    if ((expected) != (actual)) { \
        fprintf(stderr, "Assertion failed: %s\n", message); \
        fprintf(stderr, "Expected: %d, Actual: %d\n", (expected), (actual)); \
        return 1; \
    }

#define ASSERT_EQUAL_HEX(expected, actual, message) \
    if ((expected) != (actual)) { \
        fprintf(stderr, "Assertion failed: %s\n", message); \
        fprintf(stderr, "Expected: 0x%X, Actual: 0x%X\n", (expected), (actual)); \
        return 1; \
    }

#define ASSERT_EQUAL_BOOL(expected, actual, message) \
    if ((expected) != (actual)) { \
        fprintf(stderr, "Assertion failed: %s\n", message); \
        fprintf(stderr, "Expected: %s, Actual: %s\n", (expected) ? "true" : "false", (actual) ? "true" : "false"); \
        return 1; \
    }


#define CPU_RESET i8080_reset(&cpu)
#define CPU_EXECUTE i8080_execute(&cpu)

#define PASS return 0
#define TEST_INIT \
    I8080 cpu; \
    CPU_RESET;

void run_test(const char* test_name, int (*test_func)()) {
    if (test_func() == 0) {
        printf("Test passed: %s\n", test_name);
    }
}

int test_dad() {
    TEST_INIT;

    // Test without carry

    H = 0x12;
    L = 0x34;
    B = 0x56;
    C = 0x78;
    CF = 0;

    PC = 0x4000;
    WRITE_BYTE(0x4000, 0x09); // DAD BC
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0x68, H, "H register after DAD_BC");
    ASSERT_EQUAL_HEX(0xAC, L, "L register after DAD_BC");
    ASSERT_EQUAL_BOOL(0, CF, "Carry flag after DAD_BC");

    // Test with carry

    H = 0xFF;
    L = 0xFF;   
    B = 0x00;
    C = 0x01;
    CF = 0;

    PC = 0x4000;
    WRITE_BYTE(0x4000, 0x09); // DAD BC
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0x00, H, "H register after DAD_BC with carry");
    ASSERT_EQUAL_HEX(0x00, L, "L register after DAD_BC with carry");
    ASSERT_EQUAL_BOOL(1, CF, "Carry flag after DAD_BC with carry");

    CPU_RESET;

    // Test without carry

    H = 0x12;
    L = 0x34;
    D = 0x56;
    E = 0x78;
    CF = 0;

    PC = 0x4000;
    WRITE_BYTE(0x4000, 0x19); // DAD DE
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0x68, H, "H register after DAD_DE");
    ASSERT_EQUAL_HEX(0xAC, L, "L register after DAD_DE");
    ASSERT_EQUAL_BOOL(0, CF, "Carry flag after DAD_DE");

    // Test with carry
    H = 0xFF;
    L = 0xFF;
    D = 0;
    E = 1;
    CF = 0;

    PC = 0x4000;
    WRITE_BYTE(0x4000, 0x19); // DAD DE
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0x00, H, "H register after DAD_DE with carry");
    ASSERT_EQUAL_HEX(0x00, L, "L register after DAD_DE with carry");
    ASSERT_EQUAL_BOOL(1, CF, "Carry flag after DAD_DE with carry");

    CPU_RESET;

    // Test without carry
    H = 0x00;
    L = 0x40;
    CF = 0;

    PC = 0x4000;
    WRITE_BYTE(0x4000, 0x29); // DAD HL
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0x00, H, "H register after DAD_HL");
    ASSERT_EQUAL_HEX(0x80, L, "L register after DAD_HL");
    ASSERT_EQUAL_BOOL(0, CF, "Carry flag after DAD_HL");

    // Test with carry
    H = 0xFF;
    L = 0xFF;
    CF = 0;

    PC = 0x4000;
    WRITE_BYTE(0x4000, 0x29); // DAD HL
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0xFF, H, "H register after DAD_HL with carry");
    ASSERT_EQUAL_HEX(0xFE, L, "L register after DAD_HL with carry");
    ASSERT_EQUAL_BOOL(1, CF, "Carry flag after DAD_HL with carry");

    CPU_RESET;

    // Test without carry

    H = 0x12;
    L = 0x34;
    SP = 0x5678;
    CF = 0;

    PC = 0x4000;
    WRITE_BYTE(0x4000, 0x39); // DAD SP
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0x68, H, "H register after DAD_SP");
    ASSERT_EQUAL_HEX(0xAC, L, "L register after DAD_SP");
    ASSERT_EQUAL_BOOL(0, CF, "Carry flag after DAD_SP");

    // Test with carry
    H = 0xFF;
    L = 0xFF;
    SP = 0x01;
    CF = 0;

    PC = 0x4000;
    WRITE_BYTE(0x4000, 0x39); // DAD SP
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0x00, H, "H register after DAD_SP with carry");
    ASSERT_EQUAL_HEX(0x00, L, "L register after DAD_SP with carry");
    ASSERT_EQUAL_BOOL(1, CF, "Carry flag after DAD_SP with carry");

    PASS;
}
int test_add() {
    TEST_INIT;

    A = 0x12;
    B = 0x34;

    PC = 0x4000;
    WRITE_BYTE(0x4000, 0x80); // ADD B
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0x46, A, "A register after ADD");
    ASSERT_EQUAL_BOOL(0, CF, "Carry flag after ADD");

    PASS;
}
int test_sub() {
    TEST_INIT;

    A = 0x34;
    B = 0x12;

    PC = 0x4000;
    WRITE_BYTE(0x4000, 0x90); // SUB B
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0x22, A, "A register after SUB");
    ASSERT_EQUAL_BOOL(0, CF, "Carry flag after SUB");

    PASS;
}
int test_and() {
    TEST_INIT;

    A = 0xF0;
    B = 0x0F;

    PC = 0x4000;
    WRITE_BYTE(0x4000, 0xA0); // ANA B
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0x00, A, "A register after AND");
    ASSERT_EQUAL_BOOL(0, CF, "Carry flag after AND");

    PASS;
}
int test_xor() {
    TEST_INIT;

    A = 0xF0;
    B = 0x0F;

    PC = 0x4000;
    WRITE_BYTE(0x4000, 0xA8); // XRA B
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0xFF, A, "A register after XOR");
    ASSERT_EQUAL_BOOL(0, CF, "Carry flag after XOR");

    PASS;
}
int test_or() {
    TEST_INIT;

    A = 0xF0;
    B = 0x0F;

    PC = 0x4000;
    WRITE_BYTE(0x4000, 0xB0); // ORA B
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0xFF, A, "A register after OR");
    ASSERT_EQUAL_BOOL(0, CF, "Carry flag after OR");

    PASS;
}
int test_dcr() {
    TEST_INIT;

    B = 0x00;
    CF = 0b0;

    PC = 0x4000;
    WRITE_BYTE(0x4000, 0x05); // DCR B
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0xFF, B, "B register after DCR_B");
    ASSERT_EQUAL_BOOL(0, CF, "Carry flag after DCR_B");

    CPU_RESET;

    C = 0x00;
    CF = 0b0;

    PC = 0x4000;
    WRITE_BYTE(0x4000, 0x0D); // DCR C
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0xFF, C, "C register after DCR_C");
    ASSERT_EQUAL_BOOL(0, CF, "Carry flag after DCR_C");

    CPU_RESET;

    D = 0x00;
    CF = 0b0;

    PC = 0x4000;
    WRITE_BYTE(0x4000, 0x15); // DCR D
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0xFF, D, "D register after DCR_D");
    ASSERT_EQUAL_BOOL(0, CF, "Carry flag after DCR_D");

    CPU_RESET;

    E = 0x00;
    CF = 0b0;

    PC = 0x4000;
    WRITE_BYTE(0x4000, 0x1D); // DCR E
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0xFF, E, "E register after DCR_E");
    ASSERT_EQUAL_BOOL(0, CF, "Carry flag after DCR_E");

    CPU_RESET;

    H = 0x00;
    CF = 0b0;

    PC = 0x4000;
    WRITE_BYTE(0x4000, 0x25); // DCR H
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0xFF, H, "H register after DCR_H");
    ASSERT_EQUAL_BOOL(0, CF, "Carry flag after DCR_H");

    CPU_RESET;

    L = 0x00;
    CF = 0b0;

    PC = 0x4000;
    WRITE_BYTE(0x4000, 0x2D); // DCR L
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0xFF, L, "L register after DCR_L");
    ASSERT_EQUAL_BOOL(0, CF, "Carry flag after DCR_L");

    CPU_RESET;

    A = 0x00;
    CF = 0b0;

    PC = 0x4000;
    WRITE_BYTE(0x4000, 0x03D); // DCR A
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0xFF, A, "A register after DCR_A");
    ASSERT_EQUAL_BOOL(0, CF, "Carry flag after DCR_A");

    PASS;
}
int test_inr() {
    TEST_INIT;

    B = 0xFF;
    CF = 0b0;

    PC = 0x4000;
    WRITE_BYTE(0x4000, 0x04); // INR B
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0x00, B, "B register after INR_B");
    ASSERT_EQUAL_BOOL(0, CF, "Carry flag after INR_B");

    CPU_RESET;

    C = 0xFF;
    CF = 0b0;

    PC = 0x4000;
    WRITE_BYTE(0x4000, 0x0C); // INR C
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0x00, C, "C register after INR_C");
    ASSERT_EQUAL_BOOL(0, CF, "Carry flag after INR_C");

    CPU_RESET;

    D = 0xFF;
    CF = 0b0;

    PC = 0x4000;
    WRITE_BYTE(0x4000, 0x14); // INR D
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0x00, D, "D register after INR_D");
    ASSERT_EQUAL_BOOL(0, CF, "Carry flag after INR_D");

    CPU_RESET;

    E = 0xFF;
    CF = 0b0;

    PC = 0x4000;
    WRITE_BYTE(0x4000, 0x1C); // INR E
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0x00, E, "E register after INR_E");
    ASSERT_EQUAL_BOOL(0, CF, "Carry flag after INR_E");

    CPU_RESET;

    H = 0xFF;
    CF = 0b0;

    PC = 0x4000;
    WRITE_BYTE(0x4000, 0x24); // INR H
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0x00, H, "H register after INR_H");
    ASSERT_EQUAL_BOOL(0, CF, "Carry flag after INR_H");

    CPU_RESET;

    L = 0xFF;
    CF = 0b0;

    PC = 0x4000;
    WRITE_BYTE(0x4000, 0x2C); // INR L
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0x00, L, "L register after INR_L");
    ASSERT_EQUAL_BOOL(0, CF, "Carry flag after INR_L");

    CPU_RESET;

    A = 0xFF;
    CF = 0b0;

    PC = 0x4000;
    WRITE_BYTE(0x4000, 0x3C); // INR A
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0x00, A, "A register after INR_A");
    ASSERT_EQUAL_BOOL(0, CF, "Carry flag after INR_A");

    PASS;
}

int test_sta() {
    TEST_INIT;

    A = 0x12;
    PC = 0x4000;
    WRITE_BYTE(0x4000, 0x32); // STA
    WRITE_BYTE(0x4001, 0x00);
    WRITE_BYTE(0x4002, 0x40);
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0x12, READ_BYTE(0x4000), "Memory at 0x2000 after STA");

    PASS;
}
int test_lda() {
    TEST_INIT;

    A = 0xFF;
    WRITE_BYTE(0x8200, 0x34);

    PC = 0x4000;
    WRITE_BYTE(0x4000, 0x3A); // LDA
    WRITE_BYTE(0x4001, 0x00);
    WRITE_BYTE(0x4002, 0x42);
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0x34, A, "A register after LDA");

    PASS;
}
int test_xchg() {
    TEST_INIT;

    D = 0x12;
    E = 0x34;
    H = 0x56;
    L = 0x78;
    XCHG(&cpu);

    ASSERT_EQUAL_HEX(0x12, H, "H register after XCHG");
    ASSERT_EQUAL_HEX(0x34, L, "L register after XCHG");
    ASSERT_EQUAL_HEX(0x56, D, "D register after XCHG");
    ASSERT_EQUAL_HEX(0x78, E, "E register after XCHG");

    PASS;
}
int test_xthl() {
    TEST_INIT;

    SP = 0x2000;
    WRITE_BYTE(0x2000, 0x12);
    WRITE_BYTE(0x2001, 0x34);
    H = 0x56;
    L = 0x78;
    XTHL(&cpu);

    ASSERT_EQUAL_HEX(0x34, H, "H register after XTHL");
    ASSERT_EQUAL_HEX(0x12, L, "L register after XTHL");
    ASSERT_EQUAL_HEX(0x78, READ_BYTE(0x2000), "Memory at 0x2000 after XTHL");
    ASSERT_EQUAL_HEX(0x56, READ_BYTE(0x2001), "Memory at 0x2001 after XTHL");

    PASS;
}
int test_sphl() {
    TEST_INIT;

    H = 0x12;
    L = 0x34;
    SPHL(&cpu);

    ASSERT_EQUAL_HEX(0x1234, SP, "SP register after SPHL");

    PASS;
}
int test_pchl() {
    TEST_INIT;

    H = 0x12;
    L = 0x34;
    PCHL(&cpu);

    ASSERT_EQUAL_HEX(0x1234, PC, "PC register after PCHL");

    PASS;
}

#include "invaders.h"
int test_reg16() {
    TEST_INIT;

    WRITE_IO(0x04, 0x00);
    WRITE_IO(0x04, 0x00);

    WRITE_IO(0x04, 0xAA);
    ASSERT_EQUAL_HEX(0xAA00, invaders.shift_reg, "REG 16");
    
    uint8_t s = READ_IO(0x03);
    ASSERT_EQUAL_HEX(0xAA, s, "REG 16");

    WRITE_IO(0x04, 0xFF);
    ASSERT_EQUAL_HEX(0xFFAA, invaders.shift_reg, "REG 16");

    s = READ_IO(0x03);
    ASSERT_EQUAL_HEX(0xFF, s, "REG 16");

    WRITE_IO(0x04, 0x12);
    ASSERT_EQUAL_HEX(0x12FF, invaders.shift_reg, "REG 16");

    s = READ_IO(0x03);
    ASSERT_EQUAL_HEX(0x12, s, "REG 16");

    WRITE_IO(0x02, 0x2);
    WRITE_IO(0x04, 0x39);
    ASSERT_EQUAL_HEX(0x3912, invaders.shift_reg, "REG 16");

    s = READ_IO(0x03);
    ASSERT_EQUAL_HEX(0xE4, s, "REG 16");

    WRITE_IO(0x02, 0x2);
    WRITE_IO(0x04, 0x78);
    ASSERT_EQUAL_HEX(0x7839, invaders.shift_reg, "REG 16");

    s = READ_IO(0x03);
    ASSERT_EQUAL_HEX(0xE0, s, "REG 16");

    WRITE_IO(0x02, 0x7);
    WRITE_IO(0x04, 0x43);
    ASSERT_EQUAL_HEX(0x4378, invaders.shift_reg, "REG 16");

    s = READ_IO(0x03);
    ASSERT_EQUAL_HEX(0xBC, s, "REG 16");

    PASS;
}
void test() {
    run_test("dad", test_dad);    
    run_test("sub", test_sub);
    run_test("and", test_and);
    run_test("xor", test_xor);
    run_test("or", test_or);
    run_test("dcr", test_dcr);
    run_test("inr", test_inr); 
    run_test("sta", test_sta);
    run_test("lda", test_lda);
    run_test("xchg", test_xchg);
    run_test("xthl", test_xthl);
    run_test("sphl", test_sphl);
    run_test("pchl", test_pchl);
    run_test("REGISTER 16", test_reg16);
}

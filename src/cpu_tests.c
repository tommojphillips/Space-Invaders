
#include <stdio.h>
#include "i8080.h"
#include "taito8080.h"
#include "mb14241.h"

#define PC  taito8080.cpu.pc
#define PCH (taito8080.cpu.pc >> 8)
#define PCL (taito8080.cpu.pc & 0xFF)

#define SP taito8080.cpu.sp
#define SPH (taito8080.cpu.sp >> 8)
#define SPL (taito8080.cpu.sp & 0xFF)

#define A  taito8080.cpu.registers[REG_A]
#define B  taito8080.cpu.registers[REG_B]
#define C  taito8080.cpu.registers[REG_C]
#define D  taito8080.cpu.registers[REG_D]
#define E  taito8080.cpu.registers[REG_E]
#define H  taito8080.cpu.registers[REG_H]
#define L  taito8080.cpu.registers[REG_L]

#define BC	taito8080.cpu.register_pairs[RP_BC]
#define DE	taito8080.cpu.register_pairs[RP_DE]
#define HL	taito8080.cpu.register_pairs[RP_HL]
#define PSW	taito8080.cpu.register_pairs[RP_PSW]

#define SF taito8080.cpu.status_flags->s
#define CF taito8080.cpu.status_flags->c
#define ZF taito8080.cpu.status_flags->z
#define AF taito8080.cpu.status_flags->h
#define PF taito8080.cpu.status_flags->p

#define SSS (taito8080.cpu.opcode & 0b111)
#define DDD ((taito8080.cpu.opcode >> 3) & 0b111)

#define READ_BYTE(address) taito8080.cpu.read_byte(address)
#define WRITE_BYTE(address, value) taito8080.cpu.write_byte(address, value)

#define READ_IO(port) taito8080.cpu.read_io(port)
#define WRITE_IO(port, value) taito8080.cpu.write_io(port, value)

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


#define CPU_RESET i8080_reset(&taito8080.cpu)
#define CPU_EXECUTE i8080_execute(&taito8080.cpu)

#define PASS return 0
#define TEST_INIT

void run_test(const char* test_name, int (*test_func)()) {
    if (test_func() == 0) {
        printf("Test passed: %s\n", test_name);
    }
}

/*int test_dad() {
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

int tst8080() {
    TEST_INIT;

    // TEST JUMP INSTRUCTIONS AND FLAGS

    //  TST8080 offset: 01BB
    PC = 0x4000;
    WRITE_BYTE(0x4000, 0xE6); // ANI 0h
    WRITE_BYTE(0x4001, 0x00);
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(1, ZF, "ZF after tst8080");
    ASSERT_EQUAL_HEX(0, CF, "CF after tst8080");
    ASSERT_EQUAL_HEX(1, PF, "PF after tst8080");

    //  TST8080 offset: 01E7
    PC = 0x4000;
    WRITE_BYTE(0x4000, 0xC6); // ADI 06h
    WRITE_BYTE(0x4001, 0x06);
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(6, A, "A after 01E7 tst8080");
    ASSERT_EQUAL_HEX(0, CF, "CF after 01E7 tst8080");
    ASSERT_EQUAL_HEX(1, PF, "PF after 01E7 tst8080");
    ASSERT_EQUAL_HEX(0, SF, "SF after 01E7 tst8080");
    ASSERT_EQUAL_HEX(0, ZF, "ZF after 01E7 tst8080");

    //  TST8080 offset: 01FB
    PC = 0x4000;
    WRITE_BYTE(0x4000, 0xC6); // ADI 70h
    WRITE_BYTE(0x4001, 0x70);
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0x76, A, "A after 01FB tst8080");
    ASSERT_EQUAL_HEX(0, CF, "CF after 01FB tst8080");
    ASSERT_EQUAL_HEX(0, PF, "PF after 01FB tst8080");
    ASSERT_EQUAL_HEX(0, SF, "SF after 01FB tst8080");
    ASSERT_EQUAL_HEX(0, ZF, "ZF after 01FB tst8080");

    //  TST8080 offset:  020F
    PC = 0x4000;
    WRITE_BYTE(0x4000, 0xC6); // ADI 81h
    WRITE_BYTE(0x4001, 0x81);
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0xf7, A, "A after 020F tst8080");
    ASSERT_EQUAL_HEX(0, CF, "CF after 020F tst8080");
    ASSERT_EQUAL_HEX(0, PF, "PF after 020F tst8080");
    ASSERT_EQUAL_HEX(1, SF, "SF after 020F tst8080");
    ASSERT_EQUAL_HEX(0, ZF, "ZF after 020F tst8080");

    //  TST8080 offset:  C6FE
    PC = 0x4000;
    WRITE_BYTE(0x4000, 0xC6); // ADI FEh
    WRITE_BYTE(0x4001, 0xfe);
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0xf5, A, "A after C6FE tst8080");
    ASSERT_EQUAL_HEX(1, CF, "CF after C6FE tst8080");
    ASSERT_EQUAL_HEX(1, PF, "PF after C6FE tst8080");
    ASSERT_EQUAL_HEX(1, SF, "SF after C6FE tst8080");
    ASSERT_EQUAL_HEX(0, ZF, "ZF after C6FE tst8080");

    // TEST ACCUMULATOR IMMEDIATE INSTRUCTIONS

    //  TST8080 offset:  0237
    PC = 0x4000;
    WRITE_BYTE(0x4000, 0xfe); // CPI 0h
    WRITE_BYTE(0x4001, 0x00);
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0xf5, A, "A after 0237 tst8080");
    ASSERT_EQUAL_HEX(0, CF, "CF after 0237 tst8080");
    ASSERT_EQUAL_HEX(0, ZF, "ZF after 0237 tst8080");

    //  TST8080 offset:  023F
    PC = 0x4000;
    WRITE_BYTE(0x4000, 0xfe); // CPI f5h
    WRITE_BYTE(0x4001, 0xf5);
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0xf5, A, "A after 023F tst8080");
    ASSERT_EQUAL_HEX(0, CF, "CF after 023F tst8080");
    ASSERT_EQUAL_HEX(1, ZF, "ZF after 023F tst8080");

    //  TST8080 offset:  0247

    PC = 0x4000;
    WRITE_BYTE(0x4000, 0xfe); // CPI ffh
    WRITE_BYTE(0x4001, 0xff);
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0xf5, A, "A after 0247 tst8080");
    ASSERT_EQUAL_HEX(1, CF, "CF after 0247 tst8080");
    ASSERT_EQUAL_HEX(0, ZF, "ZF after 0247 tst8080");


    //  TST8080 offset:  0252
    PC = 0x4000;
    WRITE_BYTE(0x4000, 0xce); // ACI 0Ah
    WRITE_BYTE(0x4001, 0x0A);
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0, A, "A after 0252 tst8080");
    ASSERT_EQUAL_HEX(1, CF, "CF after 0252 tst8080");

    //  TST8080 offset:  0254
    PC = 0x4000;
    WRITE_BYTE(0x4000, 0xce); // ACI 0Ah
    WRITE_BYTE(0x4001, 0x0A);
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0xB, A, "A after 0254 tst8080");
    ASSERT_EQUAL_HEX(0, CF, "CF after 0254 tst8080");


    //  TST8080 offset:  025E
    PC = 0x4000;
    WRITE_BYTE(0x4000, 0xD6); // SUI 0Ch (SUB I)
    WRITE_BYTE(0x4001, 0x0C);
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0xff, A, "A after 025E tst8080");
    ASSERT_EQUAL_HEX(0, CF, "CF after 025E tst8080");

    //  TST8080 offset:  0260
    PC = 0x4000;
    WRITE_BYTE(0x4000, 0xD6); // SUI 0Fh (SUB I)
    WRITE_BYTE(0x4001, 0x0F);
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0xf0, A, "A after 0260 tst8080");
    ASSERT_EQUAL_HEX(1, CF, "CF after 0260 tst8080");

    PASS;
}
*/
int test_reg16() {
    TEST_INIT;

    MB14241 shift_reg = { 0 };

    
    mb14241_data(&shift_reg, 0xAA);
    ASSERT_EQUAL_HEX(0xAA00, shift_reg.data, "REG 16");
    
    uint8_t s = mb14241_shift(&shift_reg);
    ASSERT_EQUAL_HEX(0xAA, s, "REG 16");

    mb14241_data(&shift_reg, 0xFF);
    ASSERT_EQUAL_HEX(0xFFAA, shift_reg.data, "REG 16");

    s = mb14241_shift(&shift_reg);
    ASSERT_EQUAL_HEX(0xFF, s, "REG 16");

    mb14241_data(&shift_reg, 0x12);
    ASSERT_EQUAL_HEX(0x12FF, shift_reg.data, "REG 16");

    s = mb14241_shift(&shift_reg);
    ASSERT_EQUAL_HEX(0x12, s, "REG 16");

    mb14241_amount(&shift_reg, 0x2);
    mb14241_data(&shift_reg, 0x39);
    ASSERT_EQUAL_HEX(0x3912, shift_reg.data, "REG 16");

    s = mb14241_shift(&shift_reg);
    ASSERT_EQUAL_HEX(0xE4, s, "REG 16");

    mb14241_amount(&shift_reg, 0x2);
    mb14241_data(&shift_reg, 0x78);
    ASSERT_EQUAL_HEX(0x7839, shift_reg.data, "REG 16");

    s = mb14241_shift(&shift_reg);
    ASSERT_EQUAL_HEX(0xE0, s, "REG 16");

    mb14241_amount(&shift_reg, 0x7);
    mb14241_data(&shift_reg, 0x43);
    ASSERT_EQUAL_HEX(0x4378, shift_reg.data, "REG 16");

    s = mb14241_shift(&shift_reg);
    ASSERT_EQUAL_HEX(0xBC, s, "REG 16");

    PASS;
}

int test_rst() {
    /*  case 0xC7:
		case 0xCF:
		case 0xD7:
		case 0xDF:
		case 0xE7:
		case 0xEF:
		case 0xF7:
		case 0xFF:*/

    PC = 0x2000;
    WRITE_BYTE(0x2000, 0xC7); // RST 0
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0x00, PC, "RST 0");

    PC = 0x2000;
    WRITE_BYTE(0x2000, 0xCF); // RST 1
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0x08, PC, "RST 1");

    PC = 0x2000;
    WRITE_BYTE(0x2000, 0xD7); // RST 2
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0x10, PC, "RST 2");

    PC = 0x2000;
    WRITE_BYTE(0x2000, 0xDF); // RST 3
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0x18, PC, "RST 3");

    PC = 0x2000;
    WRITE_BYTE(0x2000, 0xE7); // RST 4
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0x20, PC, "RST 4");

    PC = 0x2000;
    WRITE_BYTE(0x2000, 0xEF); // RST 5
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0x28, PC, "RST 5");

    PC = 0x2000;
    WRITE_BYTE(0x2000, 0xF7); // RST 6
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0x30, PC, "RST 6");

    PC = 0x2000;
    WRITE_BYTE(0x2000, 0xFF); // RST 7
    CPU_EXECUTE;

    ASSERT_EQUAL_HEX(0x38, PC, "RST 7");

    PASS;
}

void test() {
    /*run_test("dad", test_dad);
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
    run_test("pchl", test_pchl);*/
    //run_test("REGISTER 16", test_reg16);
    //run_test("tst8080", tst8080);
    run_test("rst", test_rst);
}

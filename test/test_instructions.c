#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <stdint.h>

#include "../src/instructions.h"
#include "../src/memory.h"
#include "../src/utils.h"

uint16_t reg[R_COUNT];
uint16_t memory[MEMORY_MAX];

// Test sign extension for a positive number with no extension needed
Test(sign_extend, positive_number_no_extension) {
  cr_assert(eq(uint16_t, sign_extend(0b01111, 5), 0b01111));
}

// Test sign extension for a negative number with extension
Test(sign_extend, negative_number_extension) {
  cr_assert(eq(uint16_t, sign_extend(0b10000, 5), 0xFFF0));
}

// Test ADD instruction in register mode (adds values in two registers)
Test(add_instr, register_mode_adds_values) {
  reg[R_R1] = 3;
  reg[R_R2] = 4;
  uint16_t instr = (OP_ADD << 12) | (R_R0 << 9) | (R_R1 << 6) | (0 << 5) | R_R2;
  add_instr(&instr);
  cr_assert(eq(uint16_t, reg[R_R0], 7));
}

// Test AND instruction in register mode (performs bitwise AND on two registers)
Test(and_instr, register_mode_and) {
  reg[R_R1] = 0xAAAA;
  reg[R_R2] = 0x0F0F;
  uint16_t instr = (OP_AND << 12) | (R_R0 << 9) | (R_R1 << 6) | (0 << 5) | R_R2;
  and_instr(&instr);
  cr_assert(eq(uint16_t, reg[R_R0], 0x0A0A));
}

// Test NOT instruction (inverts the bits of a register)
Test(not_instr, inverts_bits) {
  reg[R_R1] = 0xAAAA;
  uint16_t instr = (OP_NOT << 12) | (R_R0 << 9) | (R_R1 << 6) | 0x3F;
  not_instr(&instr);
  cr_assert(eq(uint16_t, reg[R_R0], (uint16_t)(~0xAAAA)));
}

// Test BR instruction (branches if the condition is true)
Test(branch_instr, condition_true_branches) {
  reg[R_PC] = 0x3000;
  reg[R_COND] = FL_POS;
  uint16_t offset = 0x1F;  // 5-bit offset
  uint16_t instr = (OP_BR << 12) | (0x1 << 9) | offset;
  branch_instr(&instr);
  cr_assert(eq(uint16_t, reg[R_PC], 0x3000 + sign_extend(offset, 9)));
}

// Test JMP instruction (sets the PC to the value in a register)
Test(jump_instr, sets_pc_to_register) {
  reg[R_R1] = 0xBEEF;
  uint16_t instr = (OP_JMP << 12) | (R_R1 << 6);
  jump_instr(&instr);
  cr_assert(eq(uint16_t, reg[R_PC], 0xBEEF));
}

// Test JSR instruction (sets the PC to a register value with an offset)
Test(jump_register_instr, jsr_offset_mode) {
  reg[R_PC] = 0x3000;
  uint16_t offset = 0x7FF;
  uint16_t instr = (OP_JSR << 12) | (1 << 11) | offset;  // JSR offset mode
  jump_register_instr(&instr);
  cr_assert(eq(uint16_t, reg[R_PC], 0x3000 + sign_extend(offset, 11)));
}

// Test LDI instruction (loads a value from memory at an address specified by
// another memory location)
Test(ldi_instr, loads_indirect_memory_value) {
  reg[R_PC] = 0x3000;
  memory[0x3002] = 0x1234;
  memory[0x1234] = 0xBEEF;
  uint16_t instr = (OP_LDI << 12) | (R_R0 << 9) | (0x2);
  ldi_instr(&instr);
  cr_assert(eq(uint16_t, reg[R_R0], 0xBEEF));
}

// Test LD instruction (loads a value from memory relative to the PC)
Test(load_instr, loads_pc_relative_value) {
  reg[R_PC] = 0x3000;
  memory[0x3005] = 0xABCD;
  uint16_t instr = (OP_LD << 12) | (R_R0 << 9) | (0x5);
  load_instr(&instr);
  cr_assert(eq(uint16_t, reg[R_R0], 0xABCD));
}

// Test ST instruction (stores a value from a register into memory relative to
// the PC)
Test(store_instr, stores_value_pc_relative) {
  reg[R_PC] = 0x3000;
  reg[R_R1] = 0xBEEF;
  uint16_t instr = (OP_ST << 12) | (R_R1 << 9) | (0x1);
  store_instr(&instr);
  cr_assert(eq(uint16_t, memory[0x3001], 0xBEEF));
}

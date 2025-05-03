#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <stdint.h>

#include "../src/instructions.h"
#include "../src/memory.h"
#include "../src/utils.h"

// NOLINTBEGIN

// --- sign_extend tests ---

Test(sign_extend, positive_number_no_extension) {
  cr_assert(eq(u16, sign_extend(0b01111, 5), 0b01111));
}

Test(sign_extend, negative_number_extension) {
  cr_assert(eq(u16, sign_extend(0b10000, 5), 0xFFF0));
}

// --- ADD instruction ---

Test(add_instr, register_mode_adds_values) {
  reg[R_R1] = 3;
  reg[R_R2] = 4;
  uint32_t instr = (OP_ADD << 12) | (R_R0 << 9) | (R_R1 << 6) | (0 << 5) | R_R2;
  add_instr(instr);
  cr_assert(eq(u16, reg[R_R0], 7));
}

Test(add_instr, immediate_mode_adds_value) {
  reg[R_R1] = 5;
  uint32_t instr =
      (OP_ADD << 12) | (R_R0 << 9) | (R_R1 << 6) | (1 << 5) | (0x1F);
  add_instr(instr);
  cr_assert(eq(u16, reg[R_R0], 5 + sign_extend(0x1F, 5)));
}

// --- AND instruction ---

Test(and_instr, register_mode_and) {
  reg[R_R1] = 0xAAAA;
  reg[R_R2] = 0x0F0F;
  uint32_t instr = (OP_AND << 12) | (R_R0 << 9) | (R_R1 << 6) | (0 << 5) | R_R2;
  and_instr(instr);
  cr_assert(eq(u16, reg[R_R0], 0x0A0A));
}

Test(and_instr, immediate_mode_and) {
  reg[R_R1] = 0x0F0F;
  uint32_t instr = (OP_AND << 12) | (R_R0 << 9) | (R_R1 << 6) | (1 << 5) | 0x0F;
  and_instr(instr);
  cr_assert(eq(u16, reg[R_R0], 0x0F0F & sign_extend(0x0F, 5)));
}

// --- NOT instruction ---

Test(not_instr, inverts_bits) {
  reg[R_R1] = 0xAAAA;
  uint32_t instr = (OP_NOT << 12) | (R_R0 << 9) | (R_R1 << 6) | 0x3F;
  not_instr(instr);
  cr_assert(eq(u16, reg[R_R0], (uint16_t)(~0xAAAA)));
}

// --- BRANCH instruction ---

Test(branch_instr, condition_true_branches) {
  reg[R_PC] = 0x3000;
  reg[R_COND] = FL_POS;
  uint16_t offset = 0x1F;  // 9-bit offset
  uint32_t instr = (OP_BR << 12) | (FL_POS << 9) | offset;
  branch_instr(instr);
  cr_assert(eq(u16, reg[R_PC], 0x3000 + sign_extend(offset, 9)));
}

Test(branch_instr, condition_false_no_branch) {
  reg[R_PC] = 0x3000;
  reg[R_COND] = FL_NEG;
  uint16_t offset = 0x1F;
  uint32_t instr = (OP_BR << 12) | (FL_POS << 9) | offset;
  branch_instr(instr);
  cr_assert(eq(u16, reg[R_PC], 0x3000));  // PC should not change
}

// --- JMP instruction ---

Test(jump_instr, sets_pc_to_register) {
  reg[R_R1] = 0xBEEF;
  uint32_t instr = (OP_JMP << 12) | (R_R1 << 6);
  jump_instr(instr);
  cr_assert(eq(u16, reg[R_PC], 0xBEEF));
}

// --- JSR/JSRR instruction ---

Test(jump_register_instr, jsr_offset_mode) {
  reg[R_PC] = 0x3000;
  uint16_t offset = 0x7FF;
  uint32_t instr = (OP_JSR << 12) | (1 << 11) | offset;  // JSR
  jump_register_instr(instr);
  cr_assert(eq(u16, reg[R_PC], 0x3000 + sign_extend(offset, 11)));
}

Test(jump_register_instr, jsrr_register_mode) {
  reg[R_PC] = 0x4000;
  reg[R_R2] = 0xBEEF;
  uint32_t instr = (OP_JSR << 12) | (R_R2 << 6);  // JSRR
  jump_register_instr(instr);
  cr_assert(eq(u16, reg[R_PC], 0xBEEF));
}

// --- LDI instruction ---

Test(ldi_instr, loads_indirect_memory_value) {
  reg[R_PC] = 0x3000;
  memory[0x3002] = 0x1234;
  memory[0x1234] = 0xBEEF;
  uint32_t instr = (OP_LDI << 12) | (R_R0 << 9) | 0x2;
  ldi_instr(instr);
  cr_assert(eq(u16, reg[R_R0], 0xBEEF));
}

// --- LD instruction ---

Test(load_instr, loads_pc_relative_value) {
  reg[R_PC] = 0x3000;
  memory[0x3005] = 0xABCD;
  uint32_t instr = (OP_LD << 12) | (R_R0 << 9) | 0x5;
  load_instr(instr);
  cr_assert(eq(u16, reg[R_R0], 0xABCD));
}

// --- LDR instruction ---

Test(load_reg_instr, loads_base_reg_plus_offset) {
  reg[R_R1] = 0x4000;
  memory[0x4005] = 0xCAFE;
  uint32_t instr = (OP_LDR << 12) | (R_R0 << 9) | (R_R1 << 6) | 0x5;
  load_reg_instr(instr);
  cr_assert(eq(u16, reg[R_R0], 0xCAFE));
}

// --- LEA instruction ---

Test(load_eff_addr_instr, loads_pc_plus_offset) {
  reg[R_PC] = 0x3000;
  uint32_t instr = (OP_LEA << 12) | (R_R0 << 9) | 0x10;
  load_eff_addr_instr(instr);
  cr_assert(eq(u16, reg[R_R0], 0x3000 + sign_extend(0x10, 9)));
}

// --- ST instruction ---

Test(store_instr, stores_value_pc_relative) {
  reg[R_PC] = 0x3000;
  reg[R_R1] = 0xBEEF;
  uint32_t instr = (OP_ST << 12) | (R_R1 << 9) | 0x1;
  store_instr(instr);
  cr_assert(eq(u16, memory[0x3001], 0xBEEF));
}

// --- STR instruction ---

Test(store_reg_instr, stores_value_base_reg_plus_offset) {
  reg[R_R1] = 0x4000;
  reg[R_R2] = 0xBEEF;
  uint32_t instr = (OP_STR << 12) | (R_R2 << 9) | (R_R1 << 6) | 0x5;
  store_reg_instr(instr);
  cr_assert(eq(u16, memory[0x4005], 0xBEEF));
}

// --- STI instruction ---

Test(store_indirect_instr, stores_indirect_value) {
  reg[R_PC] = 0x3000;
  reg[R_R1] = 0xDEAD;
  memory[0x3002] = 0x4321;
  uint32_t instr = (OP_STI << 12) | (R_R1 << 9) | 0x2;
  store_indirect_instr(instr);
  cr_assert(eq(u16, memory[0x4321], 0xDEAD));
}

// NOLINTEND

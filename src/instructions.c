#include "instructions.h"

#include <stdint.h>

#include "memory.h"
#include "utils.h"

// Sign-extend a value based on the number of meaningful bits
uint16_t sign_extend(uint16_t num, uint8_t bit_count) {
  // Check if the sign bit (highest meaningful bit) is 1
  if ((uint16_t)(num >> (bit_count - SIGN)) & SIGN) {
    // If so, fill the upper bits with 1s to preserve the negative value
    num |= (ONES << bit_count);
  }
  return num;
}

// ADD instruction: add two registers or a register and an immediate value
void add_instr(const uint32_t instr) {
  // Extract destination register from bits [11:9]
  uint16_t dest_reg = (instr >> DEST_REG_SHIFT) & REG;
  // Extract first source register from bits [8:6]
  uint16_t first_value_reg = (instr >> VALUE_REG_SHIFT) & REG;
  // Extract immediate flag (bit 5) to check if using immediate mode
  uint16_t imm_flag = (instr >> IMM_FLAG_SHIFT) & FLAG;

  if (imm_flag) {
    // Immediate mode: extract and sign-extend 5-bit immediate value
    uint16_t imm5 = sign_extend(instr & IMM_NUM, IMM_FLAG_SHIFT);
    reg[dest_reg] = reg[first_value_reg] + imm5;
  } else {
    // Register mode: extract second source register from bits [2:0]
    uint16_t second_value_reg = instr & REG;
    reg[dest_reg] = reg[first_value_reg] + reg[second_value_reg];
  }

  update_flags(dest_reg);
}

// LDI instruction: indirect load (memory address comes from memory)
void ldi_instr(const uint32_t instr) {
  uint16_t dest_reg = (instr >> DEST_REG_SHIFT) & REG;
  // Extract PC-relative offset and sign-extend it
  uint16_t pc_offset = sign_extend(instr & PC_OFFSET, PC_OFFSET_BIT_LEN);

  // reg[dest] = memory[memory[PC + offset]]
  reg[dest_reg] = mem_read(mem_read(reg[R_PC] + pc_offset));
  update_flags(dest_reg);
}

// AND instruction: bitwise AND between two registers or a register and an
// immediate
void and_instr(const uint32_t instr) {
  uint16_t dest_reg = (instr >> DEST_REG_SHIFT) & REG;
  uint16_t first_value_reg = (instr >> VALUE_REG_SHIFT) & REG;
  uint16_t imm_flag = (instr >> IMM_FLAG_SHIFT) & FLAG;

  if (imm_flag) {
    // Immediate mode: use 5-bit immediate
    uint16_t imm5 = sign_extend(instr & IMM_NUM, IMM_NUM_BIT_LEN);
    reg[dest_reg] = reg[first_value_reg] & imm5;
  } else {
    // Register mode: use second source register
    uint16_t second_value_reg = instr & REG;
    reg[dest_reg] = reg[first_value_reg] & reg[second_value_reg];
  }

  update_flags(dest_reg);
}

// NOT instruction: bitwise NOT of a register value
void not_instr(const uint32_t instr) {
  uint16_t dest_reg = (instr >> DEST_REG_SHIFT) & REG;
  uint16_t value_reg = (instr >> VALUE_REG_SHIFT) & REG;

  reg[dest_reg] = ~reg[value_reg];
  update_flags(dest_reg);
}

// BR instruction: conditional branch based on condition flags
void branch_instr(const uint32_t instr) {
  // Extract and sign-extend PC offset
  uint16_t pc_offset = sign_extend(instr & PC_OFFSET, PC_OFFSET_BIT_LEN);
  // Extract condition flags from bits [11:9]
  uint16_t cond_flag = (instr >> COND_FLAG_SHIFT) & REG;

  // Check if current condition matches
  if (cond_flag & reg[R_COND]) {
    reg[R_PC] += pc_offset;
  }
}

// JMP (and RET) instruction: jump to address in a register
void jump_instr(const uint32_t instr) {
  uint16_t val_reg = (instr >> VALUE_REG_SHIFT) & REG;
  reg[R_PC] = reg[val_reg];
}

// JSR/JSRR instruction: jump to a label or address in a register
void jump_register_instr(const uint32_t instr) {
  // Save current PC into R7
  uint16_t long_flag = (instr >> LONG_FLAG_SHIFT) & FLAG;
  reg[R_R7] = reg[R_PC];

  if (long_flag) {
    // Long flag set: JSR (PC-relative jump)
    uint16_t long_pc_offset =
        sign_extend(instr & LONG_PC_OFFSET, LONG_PC_OFFSET_BIT_LEN);
    reg[R_PC] += long_pc_offset;
  } else {
    // Otherwise: JSRR (register jump)
    uint16_t reg_val = (instr >> VALUE_REG_SHIFT) & REG;
    reg[R_PC] = reg[reg_val];
  }
}

// LD instruction: load from PC + offset
void load_instr(const uint32_t instr) {
  uint16_t dest_reg = (instr >> DEST_REG_SHIFT) & REG;
  uint16_t pc_offset = sign_extend(instr & PC_OFFSET, PC_OFFSET_BIT_LEN);

  reg[dest_reg] = mem_read(reg[R_PC] + pc_offset);
  update_flags(dest_reg);
}

// LDR instruction: load from (base register + offset)
void load_reg_instr(const uint32_t instr) {
  uint16_t dest_reg = (instr >> DEST_REG_SHIFT) & REG;
  uint16_t value_reg = (instr >> VALUE_REG_SHIFT) & REG;
  uint16_t offset = sign_extend(instr & OFFSET, OFFSET_BIT_LEN);

  reg[dest_reg] = mem_read(reg[value_reg] + offset);
  update_flags(dest_reg);
}

// LEA instruction: load effective address into register
void load_eff_addr_instr(const uint32_t instr) {
  uint16_t dest_reg = (instr >> DEST_REG_SHIFT) & REG;
  uint16_t pc_offset = sign_extend(instr & PC_OFFSET, PC_OFFSET_BIT_LEN);

  reg[dest_reg] = reg[R_PC] + pc_offset;
  update_flags(dest_reg);
}

// ST instruction: store register value to memory at (PC + offset)
void store_instr(const uint32_t instr) {
  uint16_t dest_reg = (instr >> DEST_REG_SHIFT) & REG;
  uint16_t pc_offset = sign_extend(instr & PC_OFFSET, PC_OFFSET_BIT_LEN);

  mem_write(reg[R_PC] + pc_offset, reg[dest_reg]);
}

// STI instruction: store register value indirectly through memory
void store_indirect_instr(const uint32_t instr) {
  uint16_t val_reg = (instr >> STORE_VALUE_REG_SHIFT) & REG;
  uint16_t pc_offset = sign_extend(instr & PC_OFFSET, PC_OFFSET_BIT_LEN);

  mem_write(mem_read(reg[R_PC] + pc_offset), reg[val_reg]);
}

// STR instruction: store register value to (base register + offset)
void store_reg_instr(const uint32_t instr) {
  uint16_t dest_reg = (instr >> DEST_REG_SHIFT) & REG;
  uint16_t offset_reg = (instr >> VALUE_REG_SHIFT) & REG;
  uint16_t offset = sign_extend(instr & OFFSET, OFFSET_BIT_LEN);

  mem_write(reg[offset_reg] + offset, reg[dest_reg]);
}

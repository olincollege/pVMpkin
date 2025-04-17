#include "instructions.h"

#include <stdint.h>

#include "memory.h"
#include "utils.h"

uint16_t sign_extend(uint16_t x, int bit_count) {
  if ((x >> (bit_count - 1)) & 1) {
    x |= (0xFFFF << bit_count);
  }
  return x;
}

void add_instr(uint16_t* instr) {
  uint16_t dest_reg = (*instr >> 9) & 0x7;
  uint16_t first_value_reg = (*instr >> 6) & 0x7;

  uint16_t imm_flag = (*instr >> 5) & 0x1;

  /* Check for immediate mode and handle the operation accordingly */
  if (imm_flag) {
    uint16_t imm5 = sign_extend(*instr & 0x1F, 5);
    reg[first_value_reg] = reg[first_value_reg] + imm5;
  } else {
    uint16_t second_value_reg = (*instr & 0x7);
    reg[dest_reg] = reg[first_value_reg] + reg[second_value_reg];
  }

  update_flags(dest_reg);
}

void ldi_instr(uint16_t* instr) {
  uint16_t dest_reg = (*instr >> 9) & 0x7;
  uint16_t pc_offset = sign_extend(*instr & 0x1FF, 9);

  /* Add pc_offset to the current PC, look at the memory location to get the
   * final address */
  reg[dest_reg] = mem_read(mem_read(reg[R_PC] + pc_offset));
  update_flags(dest_reg);
}

void and_instr(uint16_t* instr) {
  uint16_t dest_reg = (*instr >> 9) & 0x7;
  uint16_t first_value_reg = (*instr >> 6) & 0x7;
  uint16_t imm_flag = (*instr >> 5) & 0x1;

  if (imm_flag) {
    uint16_t imm5 = sign_extend(*instr & 0x1F, 5);
    reg[dest_reg] = reg[first_value_reg] & imm5;
  } else {
    uint16_t second_value_reg = (*instr & 0x7);
    reg[dest_reg] = reg[first_value_reg] & reg[second_value_reg];
  }

  update_flags(dest_reg);
}

void not_instr(uint16_t* instr) {
  uint16_t dest_reg = (*instr >> 9) & 0x7;
  uint16_t value_reg = (*instr >> 6) & 0x7;

  reg[dest_reg] = ~reg[value_reg];
  update_flags(dest_reg);
}

void branch_instr(uint16_t* instr) {
  uint16_t pc_offset = sign_extend(*instr & 0x1FF, 9);
  uint16_t cond_flag = (*instr >> 9) & 0x7;

  if (cond_flag & reg[R_COND]) {
    reg[R_PC] += pc_offset;
  }
}

void jump_instr(uint16_t* instr) {
  /* Also handles RET */
  uint16_t r1 = (*instr >> 6) & 0x7;
  reg[R_PC] = reg[r1];
}

void jump_register_instr(uint16_t* instr) {
  uint16_t long_flag = (*instr >> 11) & 1;
  reg[R_R7] = reg[R_PC];

  if (long_flag) {
    uint16_t long_pc_offset = sign_extend(*instr & 0x7FF, 11);
    reg[R_PC] += long_pc_offset; /* JSR - Uses the PC offset */
  } else {
    uint16_t reg_val = (*instr >> 6) & 0x7;
    reg[R_PC] =
        reg[reg_val]; /* JSRR - Uses the value from the register to jump */
  }
}

void load_instr(uint16_t* instr) {
  uint16_t dest_reg = (*instr >> 9) & 0x7;
  uint16_t pc_offset = sign_extend(*instr & 0x1FF, 9);

  reg[dest_reg] = mem_read(reg[R_PC] + pc_offset);
  update_flags(dest_reg);
}

void load_reg_instr(uint16_t* instr) {
  uint16_t dest_reg = (*instr >> 9) & 0x7;
  uint16_t value_reg = (*instr >> 6) & 0x7;
  uint16_t offset = sign_extend(*instr & 0x3F, 6);

  reg[dest_reg] = mem_read(reg[value_reg] += offset);
  update_flags(dest_reg);
}

void load_eff_addr_instr(uint16_t* instr) {
  uint16_t dest_reg = (*instr >> 9) & 0x7;
  uint16_t pc_offset = sign_extend(*instr & 0x1FF, 9);
  reg[dest_reg] = reg[R_PC] + pc_offset;
  update_flags(dest_reg);
}

void store_instr(uint16_t* instr) {
  uint16_t dest_reg = (*instr >> 9) & 0x7;
  uint16_t pc_offset = sign_extend(*instr & 0x1FF, 9);
  mem_write(reg[R_PC] + pc_offset, reg[dest_reg]);
}

void store_indirect_instr(uint16_t* instr) {
  uint16_t dest_reg = (*instr >> 9) & 0x7;
  uint16_t pc_offset = sign_extend(*instr & 0x1FF, 9);
  mem_write(mem_read(reg[R_PC] + pc_offset), reg[dest_reg]);
}

void store_reg_instr(uint16_t* instr) {
  uint16_t dest_reg = (*instr >> 9) & 0x7;
  uint16_t offset_reg = (*instr >> 6) & 0x7;
  uint16_t offset = sign_extend(*instr & 0x3F, 6);
  mem_write(reg[offset_reg] + offset, reg[dest_reg]);
}

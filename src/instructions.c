#include <stdint.h>

#include "utils.c"

/**
 * Extends a signed number with 'bit_count' bits to a 16-bit signed integer.
 *
 * @param x: The value to be sign-extended.
 * @param bit_count: The number of bits in the original value.
 *
 * @return The 16-bit sign-extended value.
 */
uint16_t sign_extend(uint16_t x, int bit_count) {
  if ((x >> (bit_count - 1)) & 1) {
    x |= (0xFFFF << bit_count);
  }
  return x;
}

/**
 * Implements the ADD instruction.
 * Performs addition with either two registers or a register and an immediate
 * value.
 *
 * Format:
 * - 0001 DR SR1 0 00 SR2 (Register mode)
 * - 0001 DR SR1 1 imm5   (Immediate mode)
 *
 * @param instr:  Pointer to the instruction.
 *
 * @return void
 */
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

/**
 * Implements the LDI (Load Indirect) instruction.
 * Loads a value from the address found at a PC-relative offset.
 *
 * Format:
 * - 1010 DR PCoffset9
 *
 * @param instr: Pointer to the instruction.
 *
 * @return void
 */
void ldi_instr(uint16_t* instr) {
  uint16_t dest_reg = (*instr >> 9) & 0x7;
  uint16_t pc_offset = sign_extend(*instr & 0x1FF, 9);

  /* Add pc_offset to the current PC, look at the memory location to get the
   * final address */
  reg[dest_reg] = mem_read(mem_read(reg[R_PC] + pc_offset));
  update_flags(dest_reg);
}

/**
 * Implements the AND instruction.
 * Performs bitwise AND between two registers or with an immediate value.
 *
 * Format:
 * - 0101 DR SR1 0 00 SR2
 * - 0101 DR SR1 1 imm5
 *
 * @param instr: Pointer to the instruction.
 *
 * @return void
 */
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

/**
 * Implements the NOT instruction.
 * Performs bitwise NOT on a register and stores the result.
 *
 * Format:
 * - 1001 DR SR 111111
 *
 * @param instr: Pointer to the instruction.
 *
 * @return void
 */
void not_instr(uint16_t* instr) {
  uint16_t dest_reg = (*instr >> 9) & 0x7;
  uint16_t value_reg = (*instr >> 6) & 0x7;

  reg[dest_reg] = ~reg[value_reg];
  update_flags(dest_reg);
}

/**
 * Implements the BR (Branch) instruction.
 * Updates PC if condition flags match.
 *
 * Format:
 * - 0000 n z p PCoffset9
 *
 * @param instr: Pointer to the instruction.
 *
 * @return void
 */
void branch_instr(uint16_t* instr) {
  uint16_t pc_offset = sign_extend(*instr & 0x1FF, 9);
  uint16_t cond_flag = (*instr >> 9) & 0x7;

  if (cond_flag & reg[R_COND]) {
    reg[R_PC] += pc_offset;
  }
}

/**
 * Implements the JMP or RET instruction.
 * Sets PC to the value in a register.
 *
 * Format:
 * - 1100 000 BaseR 000000
 *
 * @param instr: Pointer to the instruction.
 *
 * @return void
 */
void jump_instr(uint16_t* instr) {
  /* Also handles RET */
  uint16_t r1 = (*instr >> 6) & 0x7;
  reg[R_PC] = reg[r1];
}

/**
 * Implements the JSR and JSRR instructions.
 * Jumps to a subroutine and stores return address in R7.
 *
 * Format:
 * - 0100 1 PCoffset11    (JSR)
 * - 0100 0 00 BaseR ...  (JSRR)
 *
 * @param instr: Pointer to the instruction.
 *
 * @return void
 */
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

/**
 * Implements the LD (Load) instruction.
 * Loads a value from a PC-relative memory location into a register.
 *
 * Format:
 * - 0010 DR PCoffset9
 *
 * @param instr: Pointer to the instruction.
 *
 * @return void
 */
void load_instr(uint16_t* instr) {
  uint16_t dest_reg = (*instr >> 9) & 0x7;
  uint16_t pc_offset = sign_extend(*instr & 0x1FF, 9);

  reg[dest_reg] = mem_read(reg[R_PC] + pc_offset);
  update_flags(dest_reg);
}

/**
 * Implements the LDR (Load Register) instruction.
 * Loads a value using base register + signed 6-bit offset.
 *
 * Format:
 * - 0110 DR BaseR offset6
 *
 * @param instr: Pointer to the instruction.
 *
 * @return void
 */
void load_reg_instr(uint16_t* instr) {
  uint16_t dest_reg = (*instr >> 9) & 0x7;
  uint16_t value_reg = (*instr >> 6) & 0x7;
  uint16_t offset = sign_extend(*instr & 0x3F, 6);

  reg[dest_reg] = mem_read(reg[value_reg] += offset);
  update_flags(dest_reg);
}

/**
 * Implements the LEA (Load Effective Address) instruction.
 * Loads the address computed from PC + offset into a register.
 *
 * Format:
 * - 1110 DR PCoffset9
 *
 * @param instr: Pointer to the instruction.
 *
 * @return void
 */
void load_eff_addr_instr(uint16_t* instr) {
  uint16_t dest_reg = (*instr >> 9) & 0x7;
  uint16_t pc_offset = sign_extend(*instr & 0x1FF, 9);
  reg[dest_reg] = reg[R_PC] + pc_offset;
  update_flags(dest_reg);
}

/**
 * Implements the ST (Store) instruction.
 * Stores a register value into a PC-relative memory address.
 *
 * Format:
 * - 0011 SR PCoffset9
 *
 * @param instr: Pointer to the instruction.
 *
 * @return void
 */
void store_instr(uint16_t* instr) {
  uint16_t dest_reg = (*instr >> 9) & 0x7;
  uint16_t pc_offset = sign_extend(*instr & 0x1FF, 9);
  mem_write(reg[R_PC] + pc_offset, reg[dest_reg]);
}

/**
 * Implements the STI (Store Indirect) instruction.
 * Stores a value into the memory location found via a PC-relative pointer.
 *
 * Format:
 * - 1011 SR PCoffset9
 *
 * @param instr: Pointer to the instruction.
 *
 * @return void
 */
void store_indirect_instr(uint16_t* instr) {
  uint16_t dest_reg = (*instr >> 9) & 0x7;
  uint16_t pc_offset = sign_extend(*instr & 0x1FF, 9);
  mem_write(mem_read(reg[R_PC] + pc_offset), reg[dest_reg]);
}

/**
 * Implements the STR (Store Register) instruction.
 * Stores a register value into memory using base register + offset.
 *
 * Format:
 * - 0111 SR BaseR offset6
 *
 * @param instr: Pointer to the instruction.
 *
 * @return void
 */
void store_reg_instr(uint16_t* instr) {
  uint16_t dest_reg = (*instr >> 9) & 0x7;
  uint16_t offset_reg = (*instr >> 6) & 0x7;
  uint16_t offset = sign_extend(*instr & 0x3F, 6);
  mem_write(reg[offset_reg] + offset, reg[dest_reg]);
}

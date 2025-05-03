#pragma once

#include <stdint.h>

#include "utils.h"

#define REG 0x07U
#define FLAG 0x1U
#define IMM_NUM 0x1FU
#define OFFSET 0x3FU
#define PC_OFFSET 0x1FFU
#define LONG_PC_OFFSET 0x7FFU
#define COND_FLAG 0x7U
#define ONES 0xFFFFU
#define SIGN 0x1U

#define DEST_REG_SHIFT 9U
#define VALUE_REG_SHIFT 6U
#define STORE_VALUE_REG_SHIFT 9U
#define IMM_FLAG_SHIFT 5U
#define COND_FLAG_SHIFT 9U
#define LONG_FLAG_SHIFT 11U

#define PC_OFFSET_BIT_LEN 9
#define OFFSET_BIT_LEN 6
#define IMM_NUM_BIT_LEN 5
#define LONG_PC_OFFSET_BIT_LEN 11

/**
 * Extends a signed number with 'bit_count' bits to a 16-bit signed integer.
 *
 * @param x: The value to be sign-extended.
 * @param bit_count: The number of bits in the original value.
 *
 * @return The 16-bit sign-extended value.
 */
uint16_t sign_extend(uint16_t num, uint8_t bit_count);

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
void add_instr(const uint32_t instr);

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
void ldi_instr(const uint32_t instr);

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
void and_instr(const uint32_t instr);

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
void not_instr(const uint32_t instr);

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
void branch_instr(const uint32_t instr);

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
void jump_instr(const uint32_t instr);

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
void jump_register_instr(const uint32_t instr);

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
void load_instr(const uint32_t instr);

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
void load_reg_instr(const uint32_t instr);

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
void load_eff_addr_instr(const uint32_t instr);

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
void store_instr(const uint32_t instr);

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
void store_indirect_instr(const uint32_t instr);

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
void store_reg_instr(const uint32_t instr);

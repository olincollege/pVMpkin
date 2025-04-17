#include "utils.h"

// Registers Enum
enum {
  R_R0 = 0,
  R_R1,
  R_R2,
  R_R3,
  R_R4,
  R_R5,
  R_R6,
  R_R7,
  R_PC, /* Program Counter */
  R_COND,
  R_COUNT
};

// Opcodes for Instructions
enum {
  OP_BR = 0,
  OP_ADD,
  OP_LD,
  OP_ST,
  OP_JSR,
  OP_AND,
  OP_LDR,
  OP_STR,
  OP_RTI,
  OP_NOT,
  OP_LDI,
  OP_STI,
  OP_JMP,
  OP_RES,
  OP_LEA,
  OP_TRAP
};

enum {
  FL_POS = 1 << 0,
  FL_ZRO = 1 << 1,
  FL_NEG = 1 << 2 
};

enum {
  TRAP_GETC = 0x20,
  TRAP_OUT = 0x21,
  TRAP_PUTS = 0x22,
  TRAP_IN = 0x23,
  TRAP_PUTSP = 0x24,
  TRAP_HALT = 0x25
};

enum {
  MR_KBSR = 0xFE00, 
  MR_KBDR = 0xFE02, 
};

void update_flags(uint16_t r) {
  if (reg[r] == 0) {
    reg[R_COND] = FL_ZRO;
  } else if (reg[r] >> 15) {
    reg[R_COND] = FL_NEG;
  } else {
    reg[R_COND] = FL_POS;
  }
}

void mem_write(uint16_t address, uint16_t value) { memory[address] = value; }

uint16_t mem_read(uint16_t address) {
  if (address == MR_KBSR) {
    if (check_key()) {
      memory[MR_KBSR] = (1 << 15);
      memory[MR_KBDR] = getchar();
    } else {
      memory[MR_KBSR] = 0;
    }
  }

  return memory[address];
}

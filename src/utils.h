#pragma once
#include <stdint.h>
#define MEMORY_MAX (1 << 16)

uint16_t memory[MEMORY_MAX];

// Registers Enum
enum {
  R_R0,
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

// Register Storage
uint16_t reg[R_COUNT];

// Opcodes for Instructions
enum {
  OP_BR,     /* branch */
  OP_ADD,    /* add */
  OP_LD,     /* load */
  OP_ST,     /* store */
  OP_JSR,    /* jump register */
  OP_AND,    /* bitwise and */
  OP_LDR,    /* load register */
  OP_STR,    /* store register */
  OP_RTI,    /* unused */
  OP_NOT,    /* bitwise not */
  OP_LDI,    /* load indirect */
  OP_STI,    /* store indirect */
  OP_JMP,    /* jump */
  OP_RES,    /* reserved (unused) */
  OP_LEA,    /* load effective address */
  OP_TRAP    /* execute trap */
};

enum {
  FL_POS,   /* Positive */
  FL_ZRO,   /* Zero */
  FL_NEG    /* Negative */
};

enum {
  TRAP_GETC,    /* get character from keyboard, not echoed to terminal */
  TRAP_OUT,     /* output a character */
  TRAP_PUTS,    /* output a word string */
  TRAP_IN,      /* get character from keyboard, echoed onto the terminal */
  TRAP_PUTSP,   /* output a byte string */
  TRAP_HALT     /* halt the program */
};

// Allows to poll the keyboard state and avoid blocking the execution of the
// program,
enum {
  MR_KBSR, /* Keyboard Status */
  MR_KBDR  /* Keyboard data */
};

void update_flags(uint16_t r);

/**
 * Writes a 16-bit value to the specified memory address.
 *
 * This function simulates writing to memory by directly updating the
 * value at the given address in the virtual memory array.
 *
 * @param address The memory address to write to.
 * @param value The 16-bit value to store at the memory location.
 */
void mem_write(uint16_t address, uint16_t value) { memory[address] = value; }

/**
 * Reads a 16-bit value from the specified memory address.
 *
 * If the address is the memory-mapped keyboard status register (MR_KBSR),
 * this function checks for a key press. If a key is pressed, it sets the
 * high bit of MR_KBSR and stores the character in the keyboard data
 * register (MR_KBDR). Otherwise, it clears the MR_KBSR.
 *
 * @param address The memory address to read from.
 * @return The 16-bit value stored at the specified memory address.
 */

uint16_t mem_read(uint16_t address);

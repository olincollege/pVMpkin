#pragma once
#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/termios.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "memory.h"

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

// Register Storage
extern uint16_t reg[R_COUNT];

// Opcodes for Instructions
enum {
  OP_BR = 0, /* branch */
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
  FL_POS = 1 << 0, /* Positive */
  FL_ZRO = 1 << 1, /* Zero */
  FL_NEG = 1 << 2  /* Negative */
};

enum {
  TRAP_GETC =
      0x20, /* get character from keyboard, not echoed onto the terminal */
  TRAP_OUT = 0x21,   /* output a character */
  TRAP_PUTS = 0x22,  /* output a word string */
  TRAP_IN = 0x23,    /* get character from keyboard, echoed onto the terminal */
  TRAP_PUTSP = 0x24, /* output a byte string */
  TRAP_HALT = 0x25   /* halt the program */
};

// Allows to poll the keyboard state and avoid blocking the execution of the
// program,
enum {
  MR_KBSR = 0xFE00, /* Keyboard Status */
  MR_KBDR = 0xFE02, /* Keyboard data */
};

/**
 *
 */
void disable_input_buffering(void);
/**
 *
 */
void restore_input_buffering(void);
/**
 *
 */
uint16_t check_key(void);
/**
 *
 */
void handle_interrupt(int signal);

/**
 * Updates the flag according to the value stored in the provided register
 *
 * @param r a uint16_t representing the register to update flags for
 */
void update_flags(uint16_t r);

/**
 * Swap a u_int16t from little-endian to big-endian.
 *
 * Most modern computers are little-endian. However, the LC-3 architecture
 * program expects big-endian.
 *
 * @param x the uint16_t to swap from little to big-endian.
 */
uint16_t swap16(uint16_t x);

/**
 * Read an image file into memory from a file pointer.
 *
 * Copies contents right into an address of memory.
 *
 * @param file a FILE pointer representing the image.
 */
void read_image_file(FILE* file);

/**
 * Reads an image file into memory from a path.
 *
 * @param image_path a String representing the path to the image.
 */
int read_image(const char* image_path);

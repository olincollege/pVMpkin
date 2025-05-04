#pragma once
#include <SDL2/SDL.h>
#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdnoreturn.h>
#include <sys/mman.h>
#include <sys/termios.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "audio.h"
#include "memory.h"

#define BYTE_LEN 8U

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
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
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
  FL_POS = 1U << 0U, /* Positive */
  FL_ZRO = 1U << 1U, /* Zero */
  FL_NEG = 1U << 2U  /* Negative */
};

enum {
  TRAP_GETC = 0x20,  /* get character from keyboard, not echoed onto terminal */
  TRAP_OUT = 0x21,   /* output a character */
  TRAP_PUTS = 0x22,  /* output a word string */
  TRAP_IN = 0x23,    /* get character from keyboard, echoed onto terminal */
  TRAP_PUTSP = 0x24, /* output a byte string */
  TRAP_HALT = 0x25   /* halt the program */
};

// Allows to poll the keyboard state and avoid blocking the execution of the
// program,
enum {
  MR_KBSR = 0xFE00,       /* Keyboard Status */
  MR_KBDR = 0xFE02,       /* Keyboard data */
  MR_AUDIO_DATA = 0xFE04, /* Audio data */
};

/**
 * Print an error message and exit with a failure status code.
 *
 * Upon an error, print an error message with a desired prefix. The prefix
 * error_msg should describe the context in which the error occurred, followed
 * by a more specific message corresponding to errno set by whatever function or
 * system call that encountered the error. This function exits the program and
 * thus does not return.
 *
 * @param error_msg The error message to print.
 */
noreturn void error_and_exit(const char* error_msg);

/**
 * Disables input buffering to allow immediate keypress detection.
 */
void disable_input_buffering(void);

/**
 * Restores the default input buffering behavior.
 */
void restore_input_buffering(void);

/**
 * Checks if a key has been pressed and returns its value.
 *
 * @return The key pressed as a 16-bit unsigned integer, or 0 if no key is
 *         pressed.
 */
uint16_t check_key(void);

/**
 * Handles interrupt signals to perform cleanup or other actions.
 *
 * @param signal The signal number that triggered the interrupt.
 */
void handle_interrupt(int signal);

/**
 * Updates the flag according to the value stored in the provided register
 *
 * @param R_Rx a uint16_t representing the register to update flags for
 */
void update_flags(uint16_t R_Rx);

/**
 * Swap a u_int16t from little-endian to big-endian.
 *
 * Most modern computers are little-endian. However, the LC-3 architecture
 * program expects big-endian.
 *
 * @param bit the uint16_t to swap from little to big-endian.
 */
uint16_t swap16(uint16_t bit);

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

/**
 * Updates the given SDL_Texture with the values of the addresses stored in the
 * memory.
 *
 * @param texture A pointer to the SDL_Texture to be updated.
 */
void update_texture(SDL_Texture* texture);

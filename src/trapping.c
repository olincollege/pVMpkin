#include "trapping.h"

#include <stdint.h>
#include <stdio.h>

#include "memory.h"
#include "utils.h"

void trap_getc(void) {
  int input = getc(stdin);
  if (input == EOF) {
    error_and_exit("Failed to get character in GETC.");
  }

  else {
    reg[R_R0] = (uint16_t)input;
    update_flags(R_R0);
  }
}
void trap_out(void) {
  if (putc((char)reg[R_R0], stdout) == EOF) {
    error_and_exit("Failed to print in OUT");
  }
  if (fflush(stdout) == EOF) {
    error_and_exit("Failed to flush in OUT.");
  }
}
void trap_puts(void) {
  uint16_t* chr = memory + reg[R_R0];  // get char pointer
  while (*chr) {
    if (putc((char)*chr, stdout) == EOF) {
      error_and_exit("Failed to put in PUTS.");
    }
    ++chr;
  }
  if (fflush(stdout) == EOF) {
    error_and_exit("Failed to flush in PUTS.");
  }
}

void trap_in(void) {
  printf("Enter a character: ");
  int chr = getc(stdin);
  if (chr == EOF) {
    error_and_exit("Failed to get input character in IN");
  }
  if (putc((char)chr, stdout) == EOF) {
    error_and_exit("Failed to print input character in IN.");
  }
  if (fflush(stdout) == EOF) {
    error_and_exit("Failed to flush stdout in IN.");
  }
  reg[R_R0] = (uint16_t)chr;  // store as unsigned 16-bit value into R0
  update_flags(R_R0);
}

void trap_putsp(void) {
  uint16_t* chr = memory + reg[R_R0];  // get address in memory

  while (*chr) {
    uint8_t chr1 = (uint8_t)((*chr) & BIT_MASK_8);  // mask lower 8 bits
    if (putc(chr1, stdout) == EOF) {
      error_and_exit("Failed to print 1st char in PUTSP.");
    }

    uint8_t chr2 = (uint8_t)((*chr) >> BIT_SHIFT_8);  // shift to upper 8 bits
    if (chr2 != 0) {  // only print if second character exists
      if (putc(chr2, stdout) == EOF) {
        error_and_exit("Failed to print 2nd char in PUTSP.");
      }
    }

    ++chr;
  }

  if (fflush(stdout) == EOF) {
    error_and_exit("Failed to flush stdout in PUTSP.");
  }
}
void trap_halt(int* running) {
  printf("HALT");
  if (fflush(stdout) == EOF) {
    error_and_exit("Failed to flush stdout in HALT.");
  };
  *running = 0;
}

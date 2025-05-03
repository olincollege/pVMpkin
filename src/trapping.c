#include "trapping.h"
#include "memory.h"

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
  if (fflush(stdout) == EOF) error_and_exit("Failed to flush in OUT.");
}
void trap_puts(void) {
  uint16_t* chr = memory + reg[R_R0];  // get char pointer
  while (*chr) {
    if (putc((char)*chr, stdout) == EOF) {
      error_and_exit("Failed to put in PUTS.");
    }
    ++chr;
  }
  if (fflush(stdout) == EOF) error_and_exit("Failed to flush in PUTS.");
}

void trap_in(void) {
  printf("Enter a character: ");
  char chr = getc(stdin);
  if ((uint16_t)chr == EOF) {
    error_and_exit("Failed to get input character in IN");
  }
  if (putc(chr, stdout) == EOF) {
    error_and_exit("Failed to print input character in IN.");
  }
  if (fflush(stdout) == EOF) {
    error_and_exit("Failed to flush stdout in IN.");
  }
  reg[R_R0] = (uint16_t)chr;
  update_flags(R_R0);
}
void trap_putsp(void) {                  // double check that * instead of [] works
  uint16_t* chr = memory + reg[R_R0];  // get char pointer
  while (*chr) {
    char chr1 = (*chr) & 0xFF;  // 8 bits ; mask
    if (putc(chr1, stdout) == EOF) {
      error_and_exit("Failed to print 1st char in PUTSP.");
    }
    char chr2 = (*chr) >> 8;  // 8 bits ; right shift
    if (chr2) {             // if c2 exists
      if (putc(chr2, stdout) == EOF) {
        error_and_exit("Failed to print 2nd char in PUTSP.");
      }
    }
    ++chr;
  }
  if (fflush(stdout) == EOF) error_and_exit("Failed to flush stdout in PUTSP.");
}
void trap_halt(int* running) {
  printf("HALT");
  if (fflush(stdout) == EOF) error_and_exit("Failed to flush stdout in HALT.");
  *running = 0;
}

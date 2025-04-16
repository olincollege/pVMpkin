#include <stdint.h>
#include <stdio.h>
#include "utils.c"

#define MEMORY_MAX (1 << 16)
uint16_t memory[MEMORY_MAX];

#define PC_START 0x3000;


int main(int argc, const char* argv[]) {
  if (argc < 2) {
    /* show instructions on how to use */
    printf("lc3 [image-file1] ...\n");
    exit(2);
  }

  /* TODO: load the image given in the arguments */
  /* TODO: setup for specific platform */

  /* since one condition flag should be set at all times, set the Z flag*/
  reg[R_COND] = FL_ZRO;

  /* set the PC to starting position (0x3000 is default)*/
  reg[R_PC] = PC_START;

  int running = 1;
  while (running) {
    reg[R_R7] = reg[R_PC];
    uint16_t instr = mem_read(reg[R_PC]++);

    /* TODO */



    // case TRAP_OP:
      switch (instr & 0xFF) {
        case TRAP_GETC: /* DONE -- ready to test */
          reg[R_R0] = (uint16_t)getc(stdin);
          if (reg[R_R0] == EOF) {
            perror("Failed to get character in GETC.");
          }
          update_flags(R_R0);
          break;
        case TRAP_OUT: /* DONE -- ready to test */
          if (putc((char)reg[R_R0], stdout) == EOF) {
            perror("Failed to print in OUT");
          }
          if (fflush(stdout) == EOF) perror("Failed to flush in OUT.");
          break;
        case TRAP_PUTS: /* DONE -- ready to test */
          uint16_t* c = memory + reg[R_R0]; // get char pointer
          while (*c) {
            if (putc((char)*c, stdout) == EOF) {
              perror("Failed to put in PUTS.");
            }
            ++c;
          }
          if (fflush(stdout) == EOF) perror("Failed to flush in PUTS.");
          break;

        case TRAP_IN: /* DONE --  ready to test */
          printf("Enter a character: ");
          char c = getc(stdin);
          if ((uint16_t)c == EOF) {
            perror("Failed to get input character in IN");
          }
          if (putc(c, stdout) == EOF) {
            perror("Failed to print input character in IN.");
          }
          if (fflush(stdout) == EOF) {
            perror("Failed to flush stdout in IN.");
          }
          reg[R_R0] = (uint16_t)c;
          update_flags(R_R0);
          break;
        case TRAP_PUTSP: /* DONE -- ready to test */
          uint16_t* c = memory + reg[R_R0]; // get char pointer
          while (*c) {
            char c1 = (*c) & 0xFF; // 8 bits ; mask
            if (putc(c1, stdout) == EOF) {
              perror("Failed to print 1st char in PUTSP.");
            }
            char c2 = (*c) >> 8; // 8 bits ; right shift
            if (c2) { // if c2 exists
              if (putc(c2, stdout) == EOF) {
                perror("Failed to print 2nd char in PUTSP."); 
              }
            }
            ++c;
          }
          if (fflush(stdout) == EOF) perror("Failed to flush stdout in PUTSP.");
          break;

        case TRAP_HALT:
          printf("HALT");
          if (fflush(stdout) == EOF) perror("Failed to flush stdout in HALT.");
          running = 0;
          break;
      }

  }
}

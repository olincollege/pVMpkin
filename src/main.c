#include <stdint.h>
#include <stdio.h>
#include "utils.c"
#include "trapping.c"

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
      case TRAP_GETC:
          trap_getc();
          break;
      case TRAP_OUT:
          trap_out();
          break;
      case TRAP_PUTS:
          trap_puts(memory);
          break;
      case TRAP_IN:
          trap_in();
          break;
      case TRAP_PUTSP:
          trap_putsp(memory);
          break;
      case TRAP_HALT:
          trap_halt(&running);
          break;
    }


     

  }
}

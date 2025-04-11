#include <stdint.h>
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
    /* TODO */
  }
}

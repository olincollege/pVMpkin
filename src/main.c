#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "instructions.h"
#include "memory.h"
#include "trapping.h"
#include "utils.h"

#define PC_START 0x3000;

int main(int argc, const char* argv[]) {
  /* Ensure proper input handling from the terminal */
  signal(SIGINT, handle_interrupt);
  disable_input_buffering();
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
    uint16_t instr = mem_read(reg[R_PC]++);
    uint16_t op = instr >> 12;

    switch (op) {
      case OP_ADD:
        add_instr(&instr);
        break;
      case OP_AND:
        and_instr(&instr);
        break;
      case OP_NOT:
        not_instr(&instr);
        break;
      case OP_BR:
        branch_instr(&instr);
        break;
      case OP_JMP:
        jump_instr(&instr);
        break;
      case OP_JSR:
        jump_register_instr(&instr);
        break;
      case OP_LD:
        load_instr(&instr);
        break;
      case OP_LDI:
        ldi_instr(&instr);
        break;
      case OP_LDR:
        load_reg_instr(&instr);
        break;
      case OP_LEA:
        load_eff_addr_instr(&instr);
        break;
      case OP_ST:
        store_instr(&instr);
        break;
      case OP_STI:
        store_indirect_instr(&instr);
        break;
      case OP_STR:
        store_reg_instr(&instr);
        break;
      case OP_TRAP:
        reg[R_R7] = reg[R_PC];

        switch (instr & 0xFF) {
          case TRAP_GETC:
            trap_getc();
            break;
          case TRAP_OUT:
            trap_out();
            break;
          case TRAP_PUTS:
            trap_puts();
            break;
          case TRAP_IN:
            trap_in();
            break;
          case TRAP_PUTSP:
            trap_putsp();
            break;
          case TRAP_HALT:
            trap_halt(&running);
            break;
        }
    }
  }
  restore_input_buffering();  // restores the terminal back to normal
}

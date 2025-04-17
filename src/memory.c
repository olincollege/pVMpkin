#include "memory.h"

#include <stdint.h>

#include "utils.h"

uint16_t memory[MEMORY_MAX];

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

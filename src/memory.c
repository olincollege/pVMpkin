#include "memory.h"

#include <stdint.h>

#include "audio.h"
#include "utils.h"

uint16_t memory[MEMORY_MAX];

uint16_t (*check_key_func)(void) = check_key;
int (*get_char_func)(void) = getchar;

void mem_write(uint16_t address, uint16_t value) {
  if (address == MR_AUDIO_DATA) {
    audio_output(value);
  }
  memory[address] = value;
}

uint16_t mem_read(uint16_t address) {
  if (address == MR_KBSR) {
    if (check_key_func) {
      memory[MR_KBSR] = (1 << 15);
      memory[MR_KBDR] = (uint16_t)get_char_func();
    } else {
      memory[MR_KBSR] = 0;
    }
  }

  return memory[address];
}

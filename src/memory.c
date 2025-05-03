#include "memory.h"

#include <stdint.h>

#include "audio.h"
#include "utils.h"

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
uint16_t memory[MEMORY_MAX];

void mem_write(uint16_t address, uint16_t value) {
  if (address == MR_AUDIO_DATA) {
    if (reg[R_R0] == AUDIO_ADDRESS) {
      audio_reset_buffering();
    }
    audio_output(value);
  } else {
    memory[address] = value;
  }
}

uint16_t mem_read(uint16_t address) { return memory[address]; }

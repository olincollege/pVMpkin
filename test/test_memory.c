#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../src/memory.h"
#include "../src/utils.h"

// NOLINTBEGIN

// --- Basic mem_write to normal memory ---

Test(mem_write, writes_to_memory) {
  memset(memory, 0, sizeof(memory));
  uint16_t address = 0x1000;
  uint16_t value = 0x1234;

  mem_write(address, value);

  cr_assert(eq(u16, memory[address], value),
            "Memory write failed at normal address");
}

// --- Basic mem_read from normal memory ---

Test(mem_read, normal_memory_read) {
  memset(memory, 0, sizeof(memory));
  uint16_t address = 0x1000;
  uint16_t value = 0x5678;

  mem_write(address, value);
  uint16_t result = mem_read(address);

  cr_assert(eq(u16, result, value), "Memory read returned wrong value");
}

// --- Special case: write to MR_AUDIO_DATA ---

Test(mem_write, writes_to_audio_memory_triggers_audio_output) {
  // Setup: MR_AUDIO_DATA is a special address for audio
  memset(memory, 0, sizeof(memory));

  reg[R_R0] = AUDIO_ADDRESS;

  uint16_t value = 0xABCD;
  mem_write(MR_AUDIO_DATA, value);

  cr_assert(eq(u16, memory[MR_AUDIO_DATA], 0),
            "Memory at MR_AUDIO_DATA should not be directly modified");
}

// NOLINTEND

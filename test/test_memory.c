#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../src/memory.h"
#include "../src/utils.h"

// Declare the register and memory arrays if not already defined
extern uint16_t memory[MEMORY_MAX];

// Test writing a value to memory
Test(mem_write, writes_to_memory) {
  for (int i = 0; i < MEMORY_MAX; i++) {
    memory[i] = 0;
  }

  uint16_t address = 0x1000;
  uint16_t value = 0x1234;

  mem_write(address, value);

  cr_assert(eq(uint16_t, memory[address], value));
}

// Test reading a value from memory
Test(mem_read, normal_memory_read) {
  for (int i = 0; i < MEMORY_MAX; i++) {
    memory[i] = 0;
  }

  uint16_t address = 0x1000;
  uint16_t value = 0x5678;

  mem_write(address, value);

  uint16_t result = mem_read(address);

  cr_assert(eq(uint16_t, result, value));
}

// Test keyboard status register when no key is pressed
Test(mem_read, keyboard_status_register_no_key_pressed) {
  for (int i = 0; i < MEMORY_MAX; i++) {
    memory[i] = 0;
  }

  uint16_t address = MR_KBSR;

  uint16_t result = mem_read(address);

  cr_assert(eq(uint16_t, result, 0));
}

// Test keyboard status register when a key is pressed
Test(mem_read, keyboard_status_register_key_pressed) {
  for (int i = 0; i < MEMORY_MAX; i++) {
    memory[i] = 0;
  }

  uint16_t address = MR_KBSR;

  uint16_t result = mem_read(address);

  cr_assert(eq(uint16_t, result, (1 << 15)));

  cr_assert(eq(uint16_t, memory[MR_KBDR], 0x41));  // 'A' in ASCII
}

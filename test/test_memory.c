#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../src/memory.h"
#include "../src/utils.h"

uint16_t key_pressed = 0;

// Mock check_key function
uint16_t fake_check_key(void) { return key_pressed; }
int fake_get_char(void) {
  return 0x41;  // ASCII for 'A'
}

Test(mem_write, writes_to_memory) {
  memset(memory, 0, sizeof(memory));
  uint16_t address = 0x1000;
  uint16_t value = 0x1234;

  mem_write(address, value);
  cr_assert(eq(u16, memory[address], value));
}

Test(mem_read, normal_memory_read) {
  memset(memory, 0, sizeof(memory));
  uint16_t address = 0x1000;
  uint16_t value = 0x5678;

  mem_write(address, value);
  uint16_t result = mem_read(address);

  cr_assert(eq(u16, result, value));
}

Test(mem_read, keyboard_status_register_key_pressed) {
  memset(memory, 0, sizeof(memory));
  key_pressed = 1;
  check_key_func = fake_check_key;
  get_char_func = fake_get_char;

  uint16_t result = mem_read(MR_KBSR);

  cr_assert(eq(u16, result, (1 << 15)));
  cr_assert(eq(u16, memory[MR_KBDR], 0x41));  // ASCII for 'A'
}

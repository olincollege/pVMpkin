#pragma once

#include <stdint.h>

#define MEMORY_MAX (1 << 16)
extern uint16_t memory[MEMORY_MAX];

/**
 * Writes a 16-bit value to the specified memory address.
 *
 * This function simulates writing to memory by directly updating the
 * value at the given address in the virtual memory array.
 *
 * @param address The memory address to write to.
 * @param value The 16-bit value to store at the memory location.
 */
void mem_write(uint16_t address, uint16_t value);

/**
 * Reads a 16-bit value from the specified memory address.
 *
 * If the address is the memory-mapped keyboard status register (MR_KBSR),
 * this function checks for a key press. If a key is pressed, it sets the
 * high bit of MR_KBSR and stores the character in the keyboard data
 * register (MR_KBDR). Otherwise, it clears the MR_KBSR.
 *
 * @param address The memory address to read from.
 * @return The 16-bit value stored at the specified memory address.
 */
uint16_t mem_read(uint16_t address);

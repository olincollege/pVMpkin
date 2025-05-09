#pragma once

#include <stdint.h>

#include "audio.h"

// NOLINTNEXTLINE(cppcoreguidelines-macro-to-enum,modernize-macro-to-enum)
#define MEMORY_MAX 0xFFFFU

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
extern uint16_t memory[MEMORY_MAX];

/**
 * Writes a uint16_t value to the specified memory address.
 *
 * This function simulates writing to memory by directly updating the
 * value at the given address in the virtual memory array.
 *
 * @param address The memory address to write to.
 * @param value The uint16_t value to store at the memory location.
 */
void mem_write(uint16_t address, uint16_t value);

/**
 * Reads a uint16_t value from the specified memory address.
 *
 * If the address is the memory-mapped keyboard status register (MR_KBSR),
 * this function checks for a key press. If a key is pressed, it sets the
 * high bit of MR_KBSR and stores the character in the keyboard data
 * register (MR_KBDR). Otherwise, it clears the MR_KBSR.
 *
 * @param address The memory address to read from.
 * @return The uint16_t value stored at the specified memory address.
 */
uint16_t mem_read(uint16_t address);

#pragma once
#include <stdio.h>

#include "memory.h"
#include "utils.h"

/**
 * Trapping function for getting a char from keyboard, not echoed onto the
 * terminal.
 */
void trap_getc(void);
/**
 * Trapping function for outputting a char onto the terminal.
 */
void trap_out(void);
/**
 * Trapping function for outputting a char onto the terminal.
 */
void trap_puts(void);
/**
 * Trapping function for getting an input char from stdin, echoed onto the
 * terminal.
 */
void trap_in(void);
/**
 * Trapping function for outputting a byte string.
 */
void trap_putsp(void);
/**
 * Trapping function for halting the program.
 *
 * @param running An int pointer representing the status of the running loop.
 */
void trap_halt(int* running);

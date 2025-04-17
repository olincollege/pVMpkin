#include <stdio.h>
#include "utils.c"

void trap_getc(void) {
    reg[R_R0] = (uint16_t)getc(stdin);
    if (reg[R_R0] == EOF) {
        perror("Failed to get character in GETC.");
    }
    update_flags(R_R0);
}
void trap_out(void) {
    if (putc((char)reg[R_R0], stdout) == EOF) {
        perror("Failed to print in OUT");
    }
    if (fflush(stdout) == EOF) perror("Failed to flush in OUT.");
}
void trap_puts(uint16_t *memory) { // double check that * instead of [] works
    uint16_t* c = memory + reg[R_R0]; // get char pointer
    while (*c) {
    if (putc((char)*c, stdout) == EOF) {
        perror("Failed to put in PUTS.");
    }
    ++c;
    }
    if (fflush(stdout) == EOF) perror("Failed to flush in PUTS.");
}

void trap_in(void) { 
    printf("Enter a character: ");
    char c = getc(stdin);
    if ((uint16_t)c == EOF) {
    perror("Failed to get input character in IN");
    }
    if (putc(c, stdout) == EOF) {
    perror("Failed to print input character in IN.");
    }
    if (fflush(stdout) == EOF) {
    perror("Failed to flush stdout in IN.");
    }
    reg[R_R0] = (uint16_t)c;
    update_flags(R_R0);
}
void trap_putsp(uint16_t* memory) { // double check that * instead of [] works
    uint16_t* c = memory + reg[R_R0]; // get char pointer
    while (*c) {
    char c1 = (*c) & 0xFF; // 8 bits ; mask
    if (putc(c1, stdout) == EOF) {
        perror("Failed to print 1st char in PUTSP.");
    }
    char c2 = (*c) >> 8; // 8 bits ; right shift
    if (c2) { // if c2 exists
        if (putc(c2, stdout) == EOF) {
        perror("Failed to print 2nd char in PUTSP."); 
        }
    }
    ++c;
    }
    if (fflush(stdout) == EOF) perror("Failed to flush stdout in PUTSP.");
}
void trap_halt(int* running) {
    printf("HALT");
    if (fflush(stdout) == EOF) perror("Failed to flush stdout in HALT.");
    *running = 0;
}

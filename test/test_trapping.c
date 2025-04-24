#include <criterion/criterion.h>
#include <criterion/hooks.h>
#include "../src/trapping.h"
#include "memory.h"

Test(trapping, test_trap_getc) {
    reg[R_R0] = 0; // Reset register
    FILE* input = fmemopen("A", 1, "r");
    stdin = input;
    trap_getc();
    cr_assert_eq(reg[R_R0], 'A', "trap_getc should store 'A' in R_R0");
    fclose(input);
}

Test(trapping, test_trap_out) {
    FILE* output = tmpfile();
    FILE* old_stdout = stdout;
    stdout = output;
    reg[R_R0] = 'B';
    trap_out();
    fflush(output);
    fseek(output, 0, SEEK_SET);
    cr_assert_eq(fgetc(output), 'B');
    stdout = old_stdout;
    fclose(output);
}

Test(trapping, test_trap_puts) {
    FILE* output = tmpfile();
    FILE* old_stdout = stdout;
    stdout = output;

    // Store "Hi\0" in memory at 0x3000
    reg[R_R0] = 0x3000;
    memory[0x3000] = 'H';
    memory[0x3001] = 'i';
    memory[0x3002] = 0;

    trap_puts();

    fflush(output);
    fseek(output, 0, SEEK_SET);

    char buf[3];
    fread(buf, sizeof(char), 2, output);
    buf[2] = '\0';
    cr_assert_str_eq(buf, "Hi");

    stdout = old_stdout;
    fclose(output);
}

Test(trapping, test_trap_in) {
    FILE* input = fmemopen("C", 1, "r");
    stdin = input;
    FILE* output = fmemopen(NULL, 1, "w");
    stdout = output;
    trap_in();
    fflush(output);
    cr_assert_eq(reg[R_R0], 'C', "trap_in should store 'C' in R_R0");
    fclose(input);
    fclose(output);
}

Test(trapping, test_trap_putsp) {
    FILE* output = tmpfile();
    FILE* old_stdout = stdout;
    stdout = output;

    // Memory layout for "Hi!\n" -> 'H' + 'i' = 0x6948, '!' + '\n' = 0x0A21
    reg[R_R0] = 0x3000;
    memory[0x3000] = 0x6948; // 'H', 'i'
    memory[0x3001] = 0x0A21; // '!', '\n'
    memory[0x3002] = 0;      // null terminator

    trap_putsp();

    fflush(output);
    fseek(output, 0, SEEK_SET);

    char buf[5];
    fread(buf, sizeof(char), 4, output);
    buf[4] = '\0';

    cr_assert_str_eq(buf, "Hi!\n");

    stdout = old_stdout;
    fclose(output);
}

Test(trapping, test_trap_halt) {
    int running = 1;
    trap_halt(&running);
    cr_assert_eq(running, 0);
}

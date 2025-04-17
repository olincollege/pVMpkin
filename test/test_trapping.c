#include <criterion/criterion.h>
#include "../src/trapping.h"

Test(trapping, test_trap_getc) {
    reg[R_R0] = 0; // Reset register
    FILE* input = fmemopen("A", 1, "r");
    stdin = input;
    trap_getc();
    cr_assert_eq(reg[R_R0], 'A', "trap_getc should store 'A' in R_R0");
    fclose(input);
}

Test(trapping, test_trap_out) {
    reg[R_R0] = 'B';
    FILE* output = fmemopen(NULL, 1, "w");
    stdout = output;
    trap_out();
    fflush(output);
    cr_assert_eq(fgetc(output), 'B', "trap_out should output 'B'");
    fclose(output);
}

Test(trapping, test_trap_puts) {
    uint16_t test_memory[] = {'H', 'e', 'l', 'l', 'o', 0};
    reg[R_R0] = 0;
    memory = test_memory;
    FILE* output = fmemopen(NULL, 6, "w");
    stdout = output;
    trap_puts();
    fflush(output);
    char buffer[6];
    fread(buffer, 1, 5, output);
    buffer[5] = '\0';
    cr_assert_str_eq(buffer, "Hello", "trap_puts should output 'Hello'");
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
    uint16_t test_memory[] = {'H' | ('i' << 8), '!', 0};
    reg[R_R0] = 0;
    memory = test_memory;
    FILE* output = fmemopen(NULL, 4, "w");
    stdout = output;
    trap_putsp();
    fflush(output);
    char buffer[4];
    fread(buffer, 1, 3, output);
    buffer[3] = '\0';
    cr_assert_str_eq(buffer, "Hi!", "trap_putsp should output 'Hi!'");
    fclose(output);
}

Test(trapping, test_trap_halt) {
    int running = 1;
    FILE* output = fmemopen(NULL, 5, "w");
    stdout = output;
    trap_halt(&running);
    fflush(output);
    char buffer[5];
    fread(buffer, 1, 4, output);
    buffer[4] = '\0';
    cr_assert_str_eq(buffer, "HALT", "trap_halt should output 'HALT'");
    cr_assert_eq(running, 0, "trap_halt should set running to 0");
    fclose(output);
}

#include "../src/trapping.h"
#include "memory.h"

#include <criterion/criterion.h>
#include <criterion/hooks.h>

const uint16_t PC_START = 0x3000;
const size_t BUFFER_SIZE = 1024; // For fmemopen buffer
const uint16_t HI_PAIR = 0x6948; // 'H', 'i'
const uint16_t EXCL_NEWLINE_PAIR = 0x0A21; // '!', '\n'
const size_t OUTPUT_BUFFER_SIZE = 5; // For output buffer

Test(trapping, test_trap_getc) {
    reg[R_R0] = 0; // Reset register
    FILE* input = fmemopen("A", 1, "r");
    stdin = input;
    trap_getc();
    cr_assert_eq(reg[R_R0], 'A', "trap_getc should store 'A' in R_R0");
    if (fclose(input) != 0) {
        cr_assert_fail("Failed to close the input stream");
    }
}

Test(trapping, test_trap_out) {
    FILE* output = tmpfile();
    if (!output) {
        cr_assert_fail("Failed to create temporary file for output");
    }

    FILE* old_stdout = stdout;
    stdout = output;
    reg[R_R0] = 'B';
    trap_out();

    if (fflush(output) != 0) {
        cr_assert_fail("Failed to flush output stream");
    }

    if (fseek(output, 0, SEEK_SET) != 0) {
        cr_assert_fail("Failed to seek to the beginning of the output stream");
    }

    int chr = fgetc(output);
    if (chr == EOF) {
        cr_assert_fail("Failed to read character from output stream");
    }
    cr_assert_eq(chr, 'B');

    stdout = old_stdout;

    if (fclose(output) != 0) {
        cr_assert_fail("Failed to close the output stream");
    }
}

Test(trapping, test_trap_puts) {
    FILE* output = tmpfile();
    if (!output) {
        cr_assert_fail("Failed to create temporary file for output");
    }

    FILE* old_stdout = stdout;
    stdout = output;

    // Store "Hi\0" in memory at 0x3000
    reg[R_R0] = PC_START;
    memory[PC_START] = 'H';
    memory[PC_START+1] = 'i';
    memory[PC_START+2] = 0;

    trap_puts();

    if (fflush(output) != 0) {
        cr_assert_fail("Failed to flush output stream");
    }
    if (fseek(output, 0, SEEK_SET) != 0) {
        cr_assert_fail("Failed to seek to the beginning of the output stream");
    }

    char buf[3];
    size_t bytesRead = fread(buf, sizeof(char), 2, output);
    if (bytesRead != 2) {
        cr_assert_fail("Failed to read expected number of characters from output stream");
    }
    buf[2] = '\0';
    cr_assert_str_eq(buf, "Hi");

    stdout = old_stdout;

    if (fclose(output) != 0) {
        cr_assert_fail("Failed to close the output stream");
    }
}

Test(trapping, test_trap_in) {
    FILE* input = fmemopen("C", 1, "r");
    if (!input) {
        cr_assert_fail("Failed to create input stream");
    }
    stdin = input;

    FILE* output = fmemopen(NULL, BUFFER_SIZE, "w");
    if (!output) {
        if (fclose(input) != 0) {
            cr_assert_fail("Failed to close input file");
        }
        cr_assert_fail("Failed to create output stream");
    }
    stdout = output;

    trap_in();

    if (fflush(output) != 0) {
        if (fclose(input) != 0) {
            cr_assert_fail("Failed to close input file");
        }
        if (fclose(output) != 0) {
            cr_assert_fail("Failed to close output file");
        }
        cr_assert_fail("Failed to flush output stream");
    }

    cr_assert_eq(reg[R_R0], 'C', "trap_in should store 'C' in R_R0");

    if (fclose(input) != 0) {
        if (fclose(output) != 0) {
            cr_assert_fail("Failed to close output file");
        }
        cr_assert_fail("Failed to close the input stream");
    }

    if (fclose(output) != 0) {
        cr_assert_fail("Failed to close the output stream");
    }
}

Test(trapping, test_trap_putsp) {
    FILE* output = tmpfile();
    if (!output) {
        cr_assert_fail("Failed to create temporary file for output");
    }

    FILE* old_stdout = stdout;
    stdout = output;

    // Memory layout for "Hi!\n" -> 'H' + 'i' = 0x6948, '!' + '\n' = 0x0A21
    reg[R_R0] = PC_START;
    memory[PC_START] = HI_PAIR; // 'H', 'i'
    memory[PC_START+1] = EXCL_NEWLINE_PAIR; // '!', '\n'
    memory[PC_START+2] = 0;      // null terminator

    trap_putsp();

    if (fflush(output) != 0) {
        stdout = old_stdout;
        if (fclose(output) != 0) {
            cr_assert_fail("Failed to close output file");
        }
        cr_assert_fail("Failed to flush output stream");
    }

    if (fseek(output, 0, SEEK_SET) != 0) {
        stdout = old_stdout;
        if (fclose(output) != 0) {
            cr_assert_fail("Failed to close output file");
        }
        cr_assert_fail("Failed to seek to the beginning of the output stream");
    }

    char buf[OUTPUT_BUFFER_SIZE];
    size_t bytesRead = fread(buf, sizeof(char), 4, output);
    if (bytesRead != 4) {
        stdout = old_stdout;
        if (fclose(output) != 0) {
            cr_assert_fail("Failed to close output file");
        }
        cr_assert_fail("Failed to read expected number of characters from output stream");
    }
    buf[4] = '\0';

    cr_assert_str_eq(buf, "Hi!\n");

    stdout = old_stdout;

    if (fclose(output) != 0) {
        cr_assert_fail("Failed to close the output stream");
    }
}

Test(trapping, test_trap_halt) {
    int running = 1;
    trap_halt(&running);
    cr_assert_eq(running, 0, "trap_halt should set running to 0");
}

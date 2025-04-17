# Some compilers warn that the return value of cr_assert gets converted from an
# int to a unsigned char:1. To avoid this error in those compilers, turn off
# conversion warnings for test code.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-conversion")

list(APPEND CMAKE_CTEST_ARGUMENTS --verbose)

# If you need to test a library called foo, use lines like the following.
# add_executable(test_foo test_foo.c)
# target_link_libraries(test_foo
#     PRIVATE foo
#     PUBLIC ${CRITERION}
# )
# add_test(
#     NAME test_foo
#     COMMAND test_foo ${CRITERION_FLAGS}
# )

add_executable(test_instructions test_instructions.c)
target_link_libraries(test_instructions
    PRIVATE utils memory
    PUBLIC ${CRITERION}
)

add_test(
    NAME test_instructions
    COMMAND test_instructions ${CRITERION_FLAGS}
)

add_executable(test_trapping test_trapping.c)
target_link_libraries(test_trapping
    PRIVATE trapping
    PUBLIC ${CRITERION}
)
add_test(
    NAME test_trapping
    COMMAND test_trapping ${CRITERION_FLAGS}
)

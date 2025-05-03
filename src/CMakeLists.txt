# If you have a program that's just a .c file and it has a main method, define
# an executable. If it has both a .c and .h file, make it a library. Also, link
# libraries appropriately here. Targets should be added in dependency order - if
# executable bar needs library foo, then you should do
#   add_library(foo foo.c foo.h)
#   add_executable(bar bar.c)
#   target_link_libraries(bar PRIVATE foo)
# or similar. The PRIVATE/INTERFACE/PUBLIC keyword will depend on whether the
# library is used only in function bodies (PRIVATE), only in function
# signatures/types (INTERFACE), or both (PUBLIC).

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_library(trapping trapping.c trapping.h)
add_library(instructions instructions.c instructions.h)
add_library(utils utils.c utils.h)
add_library(memory memory.c memory.h)

add_executable(main main.c)

target_link_libraries(main PRIVATE ${SDL2_LIBRARIES})
target_link_libraries(instructions PRIVATE utils memory)
target_link_libraries(memory PRIVATE utils)
target_link_libraries(main PRIVATE memory utils instructions trapping)
target_link_libraries(trapping PRIVATE memory)

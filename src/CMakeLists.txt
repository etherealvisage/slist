add_definitions(-W -Wall -ansi -pedantic -Wextra -O0 -g -std=c99)

aux_source_directory(. sources)

add_executable(stest ${sources})

cmake_minimum_required(VERSION 2.6)

project("Tuto-Termcaps")

file(GLOB_RECURSE source_files src/*.c)
file(GLOB_RECURSE header_files src/*.h)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

set(EXE tetris.exe)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -W -Wall -Werror -Wextra -std=c89 -pedantic")

add_executable(${EXE} ${source_files})
target_link_libraries(${EXE} ${CURSES_LIBRARIES})


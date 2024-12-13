cmake_minimum_required(VERSION 3.10)

project(Praktika7_guess_game_number1 VERSION 1.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_executable(Praktika7_guess_game_number1 prog.c)
target_compile_options(Praktika7_guess_game_number1 PRIVATE -Wall -Werror)

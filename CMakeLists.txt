cmake_minimum_required(VERSION 4.1)
project(Hangman)

set(CMAKE_CXX_STANDARD 26)

add_executable(Hangman hangman.cpp
        helper_functions/helpers.cpp
        helper_functions/helpers.h)

target_include_directories(Hangman PUBLIC helper_functions)

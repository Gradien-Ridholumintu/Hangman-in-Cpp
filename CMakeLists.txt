cmake_minimum_required(VERSION 4.0)
project(Hangman)

set(CMAKE_CXX_STANDARD 26)

add_executable(hangman
        hangman.cpp
        helper_functions/helpers.cpp
        sqlite/sqlite3.c)

target_include_directories(hangman PUBLIC
        helper_functions
        sqlite)

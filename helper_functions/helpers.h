#pragma once // include the headers only once
#include <string>

// function declaration
std::string pilihKataAcak();
void printHangman(int& jumlahKesalahan);
void printTerminal(const std::string& progresTebakan,
                   const std::string& tebakanSalah, int& jumlahKesalahan);
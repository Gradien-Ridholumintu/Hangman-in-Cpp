#ifndef HANGMAN_HELPERS_H
#define HANGMAN_HELPERS_H
#pragma once // Mencegah file di-include lebih dari sekali

#include <string>

// --- Deklarasi Fungsi ---
std::string pilihKataAcak();
void printHangman(int jumlahKesalahan);
void printStatus(const std::string& progres, const std::string& tebakanSalah,
                 int jumlahKesalahan);

#endif // HANGMAN_HELPERS_H

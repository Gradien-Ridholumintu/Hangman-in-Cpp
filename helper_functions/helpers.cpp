#include "helpers.h"
#include <cctype>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

// function definition
std::string pilihKataAcak()
{
    std::vector<std::string> daftarKata;
    // open the daftar_kata.txt file
    std::ifstream fileDaftarKata("helper_functions/daftar_kata.txt");

    if (!fileDaftarKata.is_open()) {
        std::cerr
                << "Error: Gagal membuka 'helper_functions/daftar_kata.txt'.\n";
        return ""; // if unable to open, displays error and return empty string
    }

    std::string kata;
    while (std::getline(fileDaftarKata, kata)) {
        // remove the carriage returns character
        if (!kata.empty() && kata.back() == '\r') { kata.pop_back(); }
        // add each words to the daftarKata vector
        if (!kata.empty()) { daftarKata.push_back(kata); }
    }
    fileDaftarKata.close();

    if (daftarKata.empty()) {
        std::cerr << "Error: File 'daftar_kata.txt' kosong.\n";
        return ""; // if there are no words added, displays error and return
                   // empty string
    }

    // select random word
    std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<> distribution(0, daftarKata.size() - 1);
    return daftarKata[distribution(generator)];
}

void printHangman(int& jumlahKesalahan)
{
    std::cout << "  +---+\n";
    std::cout << "  |   |\n";
    std::cout << "  |   " << (jumlahKesalahan >= 1 ? "O" : "") << '\n';
    std::cout << "  |  " << (jumlahKesalahan >= 3 ? "/" : " ")
              << (jumlahKesalahan >= 2 ? "|" : "")
              << (jumlahKesalahan >= 4 ? "\\" : "") << '\n';
    std::cout << "  |  " << (jumlahKesalahan >= 5 ? "/" : "") << " "
              << (jumlahKesalahan >= 6 ? "\\" : "") << '\n';
    std::cout << "  |\n";
    std::cout << "=========\n";
}

void printTerminal(const std::string& progresTebakan,
                   const std::string& tebakanSalah, int& jumlahKesalahan)
{
// clear terminal on both windows and linux
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif

    std::cout << "\n--- Hangman ---\n";
    printHangman(jumlahKesalahan);
    std::cout << "\nKata Rahasia: ";
    // adjust the progresTebakan visual
    for (char c: progresTebakan) {
        if (c == ' ') { std::cout << "  "; }
        else {
            std::cout << c << ' ';
        }
    }
    std::cout << "\n\nTebakan Salah: " << tebakanSalah << std::endl;
    std::cout << "Kesempatan tersisa: " << 6 - jumlahKesalahan << std::endl;
}
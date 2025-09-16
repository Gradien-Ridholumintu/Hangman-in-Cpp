#include "helpers.h" // Menggunakan header yang sudah kita atur di CMake
#include <cctype>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

std::string pilihKataAcak()
{
    std::vector<std::string> daftarKata;
    std::ifstream file("helper_functions/daftar_kata.txt");

    if (!file.is_open()) {
        std::cerr
                << "Error: Gagal membuka 'helper_functions/daftar_kata.txt'.\n";
        return "";
    }

    std::string kata;
    while (std::getline(file, kata)) {
        // --- PERBAIKAN 1: Menghapus karakter '\r' dari Windows ---
        if (!kata.empty() && kata.back() == '\r') { kata.pop_back(); }

        if (!kata.empty()) { daftarKata.push_back(kata); }
    }
    file.close();

    if (daftarKata.empty()) {
        std::cerr << "Error: File 'daftar_kata.txt' kosong.\n";
        return "";
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, daftarKata.size() - 1);
    return daftarKata[distrib(gen)];
}

void printHangman(int jumlahKesalahan)
{
    std::cout << "  +---+\n";
    std::cout << "  |   |\n";
    std::cout << "  |   " << (jumlahKesalahan >= 1 ? "O" : "") << std::endl;
    std::cout << "  |  " << (jumlahKesalahan >= 3 ? "/" : " ")
              << (jumlahKesalahan >= 2 ? "|" : "")
              << (jumlahKesalahan >= 4 ? "\\" : "") << std::endl;
    std::cout << "  |  " << (jumlahKesalahan >= 5 ? "/" : "") << " "
              << (jumlahKesalahan >= 6 ? "\\" : "") << std::endl;
    std::cout << "  |\n";
    std::cout << "=========\n";
}

void printStatus(const std::string& progres, const std::string& tebakanSalah,
                 int jumlahKesalahan)
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    std::cout << "\n--- Hangman ---\n";
    printHangman(jumlahKesalahan);
    std::cout << "\nKata Rahasia: ";
    // Perbaikan tampilan agar spasi terlihat jelas
    for (char c: progres) {
        if (c == ' ') {
            std::cout << "  "; // Beri spasi ganda untuk visual
        }
        else {
            std::cout << c << ' ';
        }
    }
    std::cout << "\n\nTebakan Salah: " << tebakanSalah << std::endl;
    std::cout << "Kesempatan tersisa: " << 6 - jumlahKesalahan << std::endl;
}
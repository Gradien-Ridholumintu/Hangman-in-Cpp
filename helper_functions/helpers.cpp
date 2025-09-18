#include "helpers.h"
#include <cctype>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <vector>

// function definition
// clear terminal on both windows and linux
void clearTerminal()
{
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

void displayMainMenu()
{
    clearTerminal();
    std::cout << "\n=========================================\n";
    std::cout << "                 HANGMAN\n";
    std::cout << "=========================================\n\n";
    std::cout << "  1. Mulai Permainan\n";
    std::cout << "  2. Keluar\n\n";
    std::cout << "Pilihan Anda (1-2): ";
}

int selectLevel()
{
    clearTerminal();
    std::cout << "\n--- PILIH TINGKAT KESULITAN ---\n";
    std::cout << "  1. Mudah (4-6 huruf)\n";
    std::cout << "  2. Sedang (7-10 huruf)\n";
    std::cout << "  3. Sulit (>10 huruf)\n\n";

    while (true) {
        std::cout << "Pilihan Anda (1-3): ";
        int pilihan = 0;
        std::cin >> pilihan;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                        '\n'); // clear input buffer
        if (std::cin.fail()) {
            std::cin.clear(); // reset cin state
            std::cout << "Input tidak valid. Masukkan angka.\n";
            continue; // restart the loop if the user input non-int
        }
        if (pilihan >= 1 && pilihan <= 3) { return pilihan; }
        std::cout << "Pilihan tidak valid. Coba lagi.\n"; // prompt the user to
                                                          // input difficulty
                                                          // again if the user
                                                          // input is not valid
    }
}

bool displayEndScreen()
{
    std::cout << "\nTekan Enter untuk melanjutkan...";

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                    '\n'); // clear input buffer

    clearTerminal();
    std::cout << "\n--- PERMAINAN SELESAI ---\n";
    std::cout << "  1. Kembali ke Menu Utama\n";
    std::cout << "  2. Keluar\n\n";

    while (true) {
        std::cout << "Pilihan Anda (1-2): ";
        int pilihan;
        std::cin >> pilihan;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                        '\n'); // clear input buffer
        if (std::cin.fail()) {
            std::cin.clear(); // reset cin state
            std::cout << "Input tidak valid. Masukkan angka.\n";
            continue; // restart the loop if the user input non-int
        }
        if (pilihan == 1) { return true; }
        if (pilihan == 2) { return false; }
    }
}

std::string selectRandomWord(int level)
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

    // filter words according to difficulty
    std::vector<std::string> kataSesuaiLevel;
    for (const auto& element: daftarKata) {
        size_t panjangKata = 0;
        for (char c: element) {
            if (c != ' ') {
                panjangKata++;
            } // counting how many non-whitespace char
        }

        switch (level) {
        case 1: // mudah
            if (panjangKata <= 6) { kataSesuaiLevel.push_back(element); }
            break;
        case 2: // sedang
            if (panjangKata >= 7 && panjangKata <= 10) {
                kataSesuaiLevel.push_back(element);
            }
            break;
        case 3: // sulit
            if (panjangKata > 10) { kataSesuaiLevel.push_back(element); }
            break;
        default:
            break;
        }
    }

    if (kataSesuaiLevel.empty()) {
        std::cerr
                << "Error: Tidak ada kata yang sesuai dengan tingkat "
                   "kesulitan yang dipilih.\n"; // throws error if there are no
                                                // words filtered in accordance
                                                // to the selected difficulty
        return "";
    }

    // select random word
    std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<> distribution(0, kataSesuaiLevel.size() - 1);
    return kataSesuaiLevel[distribution(generator)];
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

void displayGame(const std::string& progresTebakan,
                 const std::string& tebakanSalah, int& jumlahKesalahan)
{
    clearTerminal();
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

void playGame(const int& level)
{
    // variable initialization
    const std::string kataRahasia = selectRandomWord(level);
    if (kataRahasia.empty()) {
        std::cin.get(); // wait for user input
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                        '\n'); // clear input buffer
        return;
    } // if there are no string detected, return error
    std::string progresTebakan = "";
    std::string tebakanSalah = "";
    std::string semuaTebakan = "";
    int jumlahKesalahan = 0;
    constexpr int maksKesalahan = 6;

    // game logic
    for (char c: kataRahasia) { progresTebakan += (c == ' ') ? ' ' : '_'; }
    while (jumlahKesalahan < maksKesalahan && progresTebakan != kataRahasia) {
        displayGame(progresTebakan, tebakanSalah, jumlahKesalahan);

        // prompt the user to guess a char
        std::cout << "\nTebak satu huruf: ";
        char tebakan = ' ';
        std::cin >> tebakan;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                        '\n'); // clear input buffer
        tebakan = std::tolower(tebakan); // ignore uppercase

        // input validation
        if (!std::isalpha(tebakan)
            || semuaTebakan.find(tebakan) != std::string::npos) {
            continue; // if it's not an alphabet or it's already inputted
                      // before, then ignore
        }
        semuaTebakan += tebakan; // add the user's guess

        // processing the user's guess
        if (kataRahasia.find(tebakan) != std::string::npos) {
            for (size_t i = 0; i < kataRahasia.length(); ++i) {
                if (kataRahasia[i] == tebakan) { progresTebakan[i] = tebakan; }
            }
        } // if the guess is right, displays the correct location
        else {
            jumlahKesalahan++;
            tebakanSalah += std::string(1, tebakan) + ' ';
        } // if the guess is wrong, increment the jumlahKesalahan and displays
          // the wrong guess
    }
    displayGame(progresTebakan, tebakanSalah, jumlahKesalahan);

    // check the loop termination condition
    if (progresTebakan == kataRahasia) {
        // if the loop terminated because the guesses are correct
        std::cout << "\nSelamat! Anda berhasil menebak katanya!\n";
    }
    else {
        // if the loop terminated because the guesses are incorrect
        std::cout << "\nGAME OVER! Anda gagal menebak katanya.\n";
        std::cout << "Kata yang benar adalah: " << kataRahasia << std::endl;
    }
}
#include "helper_functions/helpers.h"
#include <cctype>
#include <iostream>
#include <string>

int main()
{
    // variable initialization
    const std::string kataRahasia = pilihKataAcak();
    if (kataRahasia.empty()) {
        return 1;
    } // if there are no string detected, return error

    std::string progresTebakan = "";
    for (char c: kataRahasia) {
        if (c == ' ') {
            progresTebakan += ' ';
        } // add spacing for more than one word
        else {
            progresTebakan += '_';
        }
    }

    std::string tebakanSalah = "";
    std::string semuaTebakan = "";
    int jumlahKesalahan = 0;
    constexpr int maksKesalahan = 6;

    // game logic
    while (jumlahKesalahan < maksKesalahan && progresTebakan != kataRahasia) {
        printTerminal(progresTebakan, tebakanSalah, jumlahKesalahan);

        // prompt the user to guess a char
        std::cout << "\nTebak satu huruf: ";
        char tebakan = ' ';
        std::cin >> tebakan;
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
    printTerminal(progresTebakan, tebakanSalah, jumlahKesalahan);

    // check the loop termination condition
    if (progresTebakan == kataRahasia) {
        // if the loop terminated because the guesses are correct
        std::cout << "\nSelamat! Anda berhasil menebak katanya! 🎉\n";
    }
    else {
        // if the loop terminated because the guesses are incorrect
        std::cout << "\nGAME OVER! 🚹 Anda gagal menebak kata.\n";
        std::cout << "Kata yang benar adalah: " << kataRahasia << std::endl;
    }

    return 0;
}
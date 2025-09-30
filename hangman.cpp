#include "helper_functions/helpers.h"
#include <chrono>
#include <iostream>
#include <thread>

using namespace std::chrono_literals;

int main()
{
    initializeConsole();
    initializeDatabase();
    int gameState = 0; // 0 = Homescreen, 1 = Playing, 2 = Quitting

    while (gameState != 2) {
        if (gameState == 0) {
            std::vector<scoreEntry> leaderboard = getLeaderboard();
            displayMainMenu(leaderboard);

            int pilihanMainMenu;
            std::cin >> pilihanMainMenu;

            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                                '\n'); // clear input buffer
                std::cout << "Input tidak valid. Masukkan angka.\n";
                std::this_thread::sleep_for(1s); // jeda 1 deitk
                continue; // restart the loop if the user input non-int
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                            '\n'); // clear input buffer

            if (pilihanMainMenu == 1) {
                gameState = 1; // play the game
            }
            else if (pilihanMainMenu == 2) {
                gameState = 2; // quit the game
            }
            else {
                std::cout << "Pilihan tidak valid. Coba lagi.\n";
                std::this_thread::sleep_for(1s); // jeda 1 deitk
            }
        }

        if (gameState == 1) {
            std::string username = getUsername();
            bool mainLagi = true;

            while (mainLagi) { // Loop  jika user ingin bermain lagi
                const int level = selectLevel();
                const auto gameResult = playGame(level);
                const auto skorRonde = gameResult.first;
                const auto lastGameWon = gameResult.second;
                int totalSkor = 0;

                // update the score in the database
                if (skorRonde > 0) {
                    totalSkor = updateScore(username, skorRonde);
                }

                mainLagi = displayEndScreen(username, skorRonde, totalSkor,
                                            lastGameWon);
            }
            // back to main menu
            gameState = 0;
        }
    }

    std::cout << "\nTerima kasih telah bermain! Sampai jumpa!\n";
    return 0;
}
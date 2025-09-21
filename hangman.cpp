#include "helper_functions/helpers.h"
#include <cctype>
#include <iostream>

int main()
{
    initializeDatabase();
    int gameState = 0; // 0 = Homescreen, 1 = Playing, 2 = Quitting

    while (gameState != 2) {
        if (gameState == 0) {
            std::vector<scoreEntry> leaderboard = getLeaderboard();
            displayMainMenu(leaderboard);

            int pilihanMainMenu;
            std::cin >> pilihanMainMenu;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                            '\n'); // clear input buffer

            if (std::cin.fail()) {
                std::cin.clear();
                continue; // restart the loop if the user input non-int
            }

            if (pilihanMainMenu == 1) {
                gameState = 1; // play the game
            }
            else if (pilihanMainMenu == 2) {
                gameState = 2; // quit the game
            }
        }

        if (gameState == 1) {
            std::string username = getUsername();
            bool mainLagi = true;

            while (mainLagi) { // Loop  jika user ingin bermain lagi
                const int level = selectLevel();
                int skorRonde = playGame(level);
                int totalSkor = 0;

                // update the score in the database
                if (skorRonde > 0) {
                    totalSkor = updateScore(username, skorRonde);
                }

                int pilihanEndScreen
                        = displayEndScreen(username, skorRonde, totalSkor);
                if (pilihanEndScreen == 1) { mainLagi = true; }
                else {
                    mainLagi = false;
                }
            }
            // back to main menu
            gameState = 0;
        }
    }

    std::cout << "\nTerima kasih telah bermain! Sampai jumpa!\n";
    return 0;
}
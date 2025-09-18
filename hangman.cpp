#include "helper_functions/helpers.h"
#include <cctype>
#include <iostream>

int main()
{
    int gameState = 0;

    while (gameState != 2) {
        if (gameState == 0) { // main menu
            displayMainMenu();
            int pilihanUser;
            std::cin >> pilihanUser;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                            '\n'); // clear input buffer

            if (std::cin.fail()) {
                std::cin.clear();
                continue; // restart the loop if the user input non-int
            }

            if (pilihanUser == 1) {
                gameState = 1; // play the game
            }
            else if (pilihanUser == 2) {
                gameState = 2; // quit the game
            }
        }

        if (gameState == 1) { // playing state
            const int level = selectLevel();
            playGame(level);

            if (displayEndScreen()) {
                gameState = 0; // back to main menu
            }
            else {
                gameState = 2; // quit the game
            }
        }
    }

    std::cout << "\nTerima kasih telah bermain! Sampai jumpa!\n";
    return 0;
}
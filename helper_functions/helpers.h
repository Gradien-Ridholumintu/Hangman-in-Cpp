#pragma once // include the headers only once
#include <string>

// function declaration
// UI function
void clearTerminal();
void displayMainMenu();
int selectLevel();
bool displayEndScreen();
// main function
void playGame(const int& level);
std::string selectRandomWord();
void printHangman(int& jumlahKesalahan);
void displayGame(const std::string& progresTebakan,
                 const std::string& tebakanSalah, int& jumlahKesalahan);
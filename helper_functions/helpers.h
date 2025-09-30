#pragma once // include the headers only once
#include <string>
#include <vector>

// leaderboard struct
struct scoreEntry {
    std::string namaPemain;
    int skor;
};

// function declaration

// database function
void initializeDatabase();
std::vector<scoreEntry> getLeaderboard();
int updateScore(const std::string& nama, const int& skor);

// UI function
void initializeConsole();
void clearTerminal();
void displayMainMenu(const std::vector<scoreEntry>& leaderboard);
std::string getUsername();
int selectLevel();
int displayEndScreen(const std::string& username, const int& currentScore,
                     const int& totalScore, const bool& lastGameWon);

// main function
std::pair<int, bool> playGame(const int& level);
std::string selectRandomWord(const int& level);
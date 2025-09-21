#include "helpers.h"
#include "sqlite3.h"
#include <cctype>
#include <iostream>
#include <limits>
#include <random>
#include <vector>

// function definition

void initializeDatabase()
{
    sqlite3* db;
    sqlite3_open("sqlite/hangman.db", &db); // open/create the database

    const char* createDaftarKata = "CREATE TABLE IF NOT EXISTS DaftarKata("
                                   "ID INT PRIMARY KEY AUTOINCREMENT,"
                                   "Kata TEXT NOT NULL UNIQUE,"
                                   "Panjang INT NOT NULL);";
    sqlite3_exec(db, createDaftarKata, nullptr, nullptr, nullptr);

    const char* createLeaderboard = "CREATE TABLE IF NOT EXISTS Leaderboard("
                                    "Nama TEXT PRIMARY KEY,"
                                    "Skor INT NOT NULL);";
    sqlite3_exec(db, createLeaderboard, nullptr, nullptr, nullptr);

    sqlite3_close(db);
}

std::vector<scoreEntry> getLeaderboard()
{
    sqlite3* db;
    sqlite3_open("sqlite/hangman.db", &db);
    std::vector<scoreEntry> leaderboard;
    // sql query to select the top 6 entries from the descending leaderboard
    const char* sql
            = "SELECT Nama, Skor FROM Leaderboard ORDER BY Skor DESC LIMIT 6;";
    sqlite3_stmt* stmt; // prepared statement

    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    // loop through each row and returns SQLITE_ROW
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        // fetch the value of the 1st column
        std::string nama
                = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        // fetch the value of the 2nd column
        const int skor = sqlite3_column_int(stmt, 1);
        leaderboard.push_back({nama, skor});
    }
    // cleaning up the resource used
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return leaderboard;
}

int updateScore(const std::string& nama, const int& skor)
{
    sqlite3* db;
    sqlite3_open("sqlite/hangman.db", &db);
    sqlite3_stmt* stmt;

    // retrieve the already stored scores
    int skorLama = 0;
    sqlite3_prepare_v2(db, "SELECT Skor FROM Leaderboard WHERE Nama = ?;", -1,
                       &stmt, nullptr); // sql query to  retrieve the scores
    sqlite3_bind_text(stmt, 1, nama.c_str(), -1, SQLITE_STATIC);
    // if there are match for the player name, retrieve the score
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        skorLama = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);

    // accumulate the score
    const int skorBaru = skorLama + skor;
    sqlite3_prepare_v2(
            db,
            "INSERT OR REPLACE INTO Leaderboard (Nama, Skor) VALUES (?, ?);",
            -1, &stmt, nullptr); // sql query to insert a new name and score or
                                 // replace with the new one
    sqlite3_bind_text(stmt, 1, nama.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, skorBaru);
    sqlite3_step(stmt);

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return skorBaru;
}

void clearTerminal()
// clear terminal on both windows and linux
{
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

void displayMainMenu(const std::vector<scoreEntry>& leaderboard)
{
    clearTerminal();
    std::cout << "\n=========================================\n";
    std::cout << "                 HANGMAN\n";
    std::cout << "=========================================\n\n";

    if (!leaderboard.empty()) {
        std::cout << "--- Leaderboard (Top 6) ---\n";
        for (const auto& entry: leaderboard) {
            std::cout << "  " << entry.namaPemain << ": " << entry.skor
                      << std::endl;
        }
        std::cout << "---------------------------------\n\n";
    }

    std::cout << "  1. Mulai Permainan\n";
    std::cout << "  2. Keluar\n\n";
    std::cout << "Pilihan Anda (1-2): ";
}

std::string getUsername()
{
    clearTerminal();
    std::string nama;
    while (true) {
        std::cout << "\nMasukkan username (satu kata, tanpa spasi): ";
        std::cin >> nama;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                        '\n'); // clear input buffer
        if (!nama.empty()) { return nama; }
        std::cout << "Nama tidak boleh kosong. Coba lagi.\n";
    }
}

int selectLevel()
{
    clearTerminal();
    std::cout << "\n--- PILIH TINGKAT KESULITAN ---\n";
    std::cout << "  1. Mudah (<7 huruf)\n";
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
        // prompt the user to input difficulty again if the user input is not
        // valid
        std::cout << "Pilihan tidak valid. Coba lagi.\n";
    }
}

int displayEndScreen(const std::string& username, const int& currentScore,
                     const int& totalScore)
{
    std::cout << "\nTekan Enter untuk melanjutkan...";

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                    '\n'); // clear input buffer

    clearTerminal();
    std::cout << "\n--- PERMAINAN SELESAI ---\n";
    std::cout << "Pemain: " << username << "\n";
    std::cout << "Skor ronde ini: " << currentScore << '\n';
    std::cout << "Skor Total Anda: " << totalScore << "\n\n";
    std::cout << "  1. Main Lagi\n";
    std::cout << "  2. Kembali ke Menu Utama\n\n";

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

std::string selectRandomWord(const int& level)
{
    sqlite3* db;
    sqlite3_open("sqlite/hangman.db", &db);
    std::vector<std::string> kataSesuaiLevel;

    std::string sql_query;
    // create sql query based on the selected difficulty
    switch (level) {
    case 1:
        sql_query = "SELECT Kata FROM DaftarKata WHERE Panjang <= 6;";
        break;
    case 2:
        sql_query = "SELECT Kata FROM DaftarKata WHERE Panjang >= 7 AND "
                    "Panjang <= 10;";
        break;
    case 3:
        sql_query = "SELECT Kata FROM DaftarKata WHERE Panjang > 10;";
        break;
    }

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql_query.c_str(), -1, &stmt, nullptr);

    // loop the query and return the rows
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        kataSesuaiLevel.push_back(
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    if (kataSesuaiLevel.empty()) {
        std::cerr << "Error: Tidak ada kata yang sesuai dengan tingkat "
                     "kesulitan yang dipilih di database.\n";
        return ""; // throws error when there are no words filtered in
                   // accordance to the selected difficulty
    }

    // select random word
    std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<> distribution(0, kataSesuaiLevel.size() - 1);
    return kataSesuaiLevel[distribution(generator)];
}

void printHangman(const int& jumlahKesalahan)
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

int playGame(const int& level)
{
    // variable initialization
    const std::string kataRahasia = selectRandomWord(level);
    if (kataRahasia.empty()) {
        return 0;
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
        // if the loop terminated because the guesses are correct, return score
        std::cout << "\nSelamat! Anda berhasil menebak katanya!\n";
        return (level * 10) + (6 - jumlahKesalahan);
    }
    // if the loop terminated because the guesses are incorrect, return 0
    std::cout << "\nGAME OVER! Anda gagal menebak katanya.\n";
    std::cout << "Kata yang benar adalah: " << kataRahasia << std::endl;
    return 0;
}
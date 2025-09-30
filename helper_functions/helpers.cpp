#include "helpers.h"
#include "sqlite3.h"
#include <algorithm>
#include <cctype>
#include <chrono>
#include <iostream>
#include <limits>
#include <random>
#include <sstream>
#include <thread>
#include <vector>
// library for windows console handling
#ifdef _WIN32
#include <windows.h>
#endif

// for sleep
using namespace std::chrono_literals;

// anonymous namespace function
namespace {
    // color helpers
    constexpr auto RESET = "\x1b[0m";
    constexpr auto BOLD = "\x1b[1m";
    constexpr auto RED = "\x1b[31m";
    constexpr auto GREEN = "\x1b[32m";
    constexpr auto YELLOW = "\x1b[33m";
    constexpr auto MAGENTA = "\x1b[35m";
    constexpr auto CYAN = "\x1b[36m";
    constexpr auto WHITE = "\x1b[97m";

    // width for centering
    constexpr int WIDTH = 80;
    int consoleWidth()
    {
#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO info;
        if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),
                                       &info)) {
            return (info.srWindow.Right - info.srWindow.Left + 1);
        }
#endif
        return WIDTH;
    }

    // padding the string with spaces to center it
    std::string centerLine(const std::string& text, int width)
    {
        if (width <= 0) width = consoleWidth();
        if (static_cast<int>(text.size()) >= width) return text;
        const int totalPad = width - static_cast<int>(text.size());
        const int left = totalPad / 2;
        const int right = totalPad - left;
        return std::string(left, ' ') + text + std::string(right, ' ');
    }
    std::string centerLine(const std::string& text)
    {
        return centerLine(text, consoleWidth());
    }

    // compute the number of spaces needed to center a block of text
    int computeLeftPad(const std::vector<std::string>& lines, int width)
    {
        size_t maxLen = 0;
        for (const auto& l: lines) maxLen = std::max(maxLen, l.size());
        int pad = (width - static_cast<int>(maxLen));
        if (pad < 0) pad = 0;
        return pad / 2;
    }

    // print a block of text centered
    void printBlockUnitCentered(const std::vector<std::string>& lines,
                                const char* color = RESET, int width = -1)
    {
        if (width <= 0) width = consoleWidth();
        int left = computeLeftPad(lines, width);
        std::string indent(left, ' ');
        for (const auto& line: lines) {
            std::cout << color << indent << line << RESET << '\n';
        }
    }

    // split multiline string and prints it centered
    void printCenteredBlock(const std::string& block, const char* color = RESET,
                            int width = -1)
    {
        std::istringstream iss(block);
        std::string line;
        std::vector<std::string> lines;
        while (std::getline(iss, line)) lines.push_back(line);
        printBlockUnitCentered(lines, color, width);
    }

    // print a prompt centered
    void printPrompt(const std::string& text, const char* color = BOLD)
    {
        std::cout << color << centerLine(text) << RESET;
    }

    auto TITLE_ASCII =
        " _                                             \n"
        "| |                                             \n"
        "| |__   __ _ _ __   __ _ _ __ ___   __ _ _ __  \n"
        "| '_ \\ / _` | '_ \\ / _` | '_ ` _ \\ / _` | '_ \\ \n"
        "| | | | (_| | | | | (_| | | | | | | (_| | | | |\n"
        "|_| |_|\\__,_|_| |_|\\__, |_| |_| |_|\\__,_|_| |_|\n"
        "                    __/ |                      \n"
        "                   |___/                       ";

    // database helpers
    std::string path(const std::string& a, const std::string& b)
    {
#ifdef _WIN32
        const char sep = '\\';
#else
        const char sep = '/';
#endif
        if (a.empty()) return b;
        if (a.back() == sep) return a + b;
        return a + sep + b;
    }

    std::string executableDirectory()
    {
#ifdef _WIN32
        char buffer[MAX_PATH];
        DWORD len = GetModuleFileNameA(nullptr, buffer, MAX_PATH);
        if (len > 0) {
            std::string full(buffer, buffer + len);
            size_t pos = full.find_last_of("/\\");
            if (pos != std::string::npos) return full.substr(0, pos);
            return full;
        }
#endif
        return ".";
    }

    int openDb(sqlite3** db, std::string& outPath)
    {
        std::vector<std::string> candidates;
        const std::string ed = executableDirectory();
        candidates.push_back(
                path(path(ed, ".."), path("sqlite", "hangman.db")));
        candidates.push_back(path(ed, path("sqlite", "hangman.db")));
        candidates.push_back(path(".", path("sqlite", "hangman.db")));
        candidates.push_back(path("..", path("sqlite", "hangman.db")));

        for (const auto& path: candidates) {
            if (sqlite3_open(path.c_str(), db) == SQLITE_OK) {
                outPath = path;
                return SQLITE_OK;
            }
            if (*db) {
                sqlite3_close(*db);
                *db = nullptr;
            }
        }
        return SQLITE_CANTOPEN;
    }

    class DatabaseManager {
    public:
        DatabaseManager(): db(nullptr)
        {
            if (openDb(&db, path) != SQLITE_OK) {
                std::cerr << RED << "Cannot open database." << RESET
                          << std::endl;
                if (db) {
                    sqlite3_close(db);
                    db = nullptr;
                }
            }
        }

        ~DatabaseManager()
        {
            if (db) { sqlite3_close(db); }
        }

        sqlite3* get() const { return db; }
        bool isOpen() const { return db != nullptr; }

    private:
        sqlite3* db;
        std::string path;
    };

    void importDb(sqlite3* db)
    {
        const char* createDaftarKata = "CREATE TABLE IF NOT EXISTS DaftarKata("
                                       "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                                       "Kata TEXT NOT NULL UNIQUE,"
                                       "Panjang INT NOT NULL);";
        sqlite3_exec(db, createDaftarKata, nullptr, nullptr, nullptr);

        const char* createLeaderboard
                = "CREATE TABLE IF NOT EXISTS Leaderboard("
                  "Nama TEXT PRIMARY KEY,"
                  "Skor INT NOT NULL);";
        sqlite3_exec(db, createLeaderboard, nullptr, nullptr, nullptr);

        sqlite3_stmt* stmt = nullptr;
        bool hasPanjang = false;
        if (sqlite3_prepare_v2(db, "PRAGMA table_info(DaftarKata);", -1, &stmt,
                               nullptr)
            == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                const unsigned char* name = sqlite3_column_text(stmt, 1);
                if (name
                    && std::string(reinterpret_cast<const char*>(name))
                               == "Panjang") {
                    hasPanjang = true;
                    break;
                }
            }
        }
        sqlite3_finalize(stmt);
        if (!hasPanjang) {
            sqlite3_exec(db, "ALTER TABLE DaftarKata ADD COLUMN Panjang INT;",
                         nullptr, nullptr, nullptr);
            sqlite3_exec(db, "UPDATE DaftarKata SET Panjang = LENGTH(Kata);",
                         nullptr, nullptr, nullptr);
        }
    }

    void printHangman(const int& jumlahKesalahan)
{
    int s = jumlahKesalahan;
        if (s < 0) s = 0; if (s > 6) s = 6;

        static const std::vector<std::vector<std::string>> STAGES = {
            {
                R"(  ________    )",
                R"( |/      |    )",
                R"( |            )",
                R"( |            )",
                R"( |            )",
                R"( |            )",
                R"( |            )",
                R"( |            )",
                R"(_|___         )"
            },
            {
                R"(  ________    )",
                R"( |/      |    )",
                R"( |       |    )",
                R"( |      ( )   )",
                R"( |            )",
                R"( |            )",
                R"( |            )",
                R"( |            )",
                R"(_|___         )"
            },
            {
                R"(  ________    )",
                R"( |/      |    )",
                R"( |       |    )",
                R"( |      ( )   )",
                R"( |       |    )",
                R"( |       |    )",
                R"( |            )",
                R"( |            )",
                R"(_|___         )"
            },
            {
                R"(  ________    )",
                R"( |/      |    )",
                R"( |       |    )",
                R"( |      ( )   )",
                R"( |      \|    )",
                R"( |       |    )",
                R"( |            )",
                R"( |            )",
                R"(_|___         )"
            },
            {
                R"(  ________    )",
                R"( |/      |    )",
                R"( |       |    )",
                R"( |      ( )   )",
                R"( |      \|/   )",
                R"( |       |    )",
                R"( |            )",
                R"( |            )",
                R"(_|___         )"
            },
            {
                R"(  ________    )",
                R"( |/      |    )",
                R"( |       |    )",
                R"( |      ( )   )",
                R"( |      \|/   )",
                R"( |       |    )",
                R"( |      /     )",
                R"( |            )",
                R"(_|___         )"
            },
            {
                R"(  ________    )",
                R"( |/      |    )",
                R"( |       |    )",
                R"( |      ( )   )",
                R"( |      \|/   )",
                R"( |       |    )",
                R"( |      / \   )",
                R"( |            )",
                R"(_|___         )"
            }
        };

        printBlockUnitCentered(STAGES[s], WHITE);
}

    // function do draw the gameover hangman
    void printHangmanGameOver()
    {
        std::vector<std::string> lines = {
            R"(  ________    )",
            R"( |/      |    )",
            R"( |       |    )",
            R"( |     (x_x)  )",
            R"( |            )",
            R"( |      /|\   )",
            R"( |      / \   )",
            R"( |     .:'::. )",
            R"(_|___  ':' ':')",
        };

        int width = consoleWidth();
        int leftPad = computeLeftPad(lines, width);
        std::string indent(leftPad, ' ');
        for (size_t i = 0; i < lines.size(); ++i) {
            const char* color = (i <= 4) ? WHITE : RED;
            std::cout << color << indent << lines[i] << RESET << '\n';
        }
    }

    // function to draw the winning hangman
    void printHangmanWin()
    {
        std::vector<std::string> lines = {
            R"(  ________    )",
            R"( |/      |    )",
            R"( |       |    )",
            R"( |            )",
            R"( |            )",
            R"( |      ( )   )",
            R"( |      \|/   )",
            R"( |       |    )",
            R"(_|___   / \   )"
        };

        printBlockUnitCentered(lines, GREEN);
    }

    void displayGame(const std::string& progresTebakan,
                 const std::string& tebakanSalah, const int& jumlahKesalahan)
    {
        clearTerminal();
        printCenteredBlock(TITLE_ASCII, MAGENTA);
        std::cout << '\n';
        printHangman(jumlahKesalahan);
        std::cout << '\n' << CYAN << centerLine("Kata Rahasia:") << RESET << '\n';
        std::string visual;
        for (char c: progresTebakan) {
            visual += (c == ' ' ? "  " : std::string(1, c) + " ");
        }
        std::cout << BOLD << centerLine(visual) << RESET << '\n';
        std::cout << '\n'
                  << RED
                  << centerLine(std::string("Tebakan Salah: ") + tebakanSalah)
                  << RESET << '\n';
        std::ostringstream s;
        s << "Kesempatan tersisa: " << (6 - jumlahKesalahan);
        std::cout << YELLOW << centerLine(s.str()) << RESET << '\n';
        std::cout << '\n';
        printPrompt("Tebak satu huruf: ");
    }
} // namespace

// public function

void initializeConsole()
{
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    if (hIn != INVALID_HANDLE_VALUE) {
        DWORD inMode = 0;
        if (GetConsoleMode(hIn, &inMode)) { SetConsoleMode(hIn, inMode); }
    }
#endif
}

void initializeDatabase()
{
    DatabaseManager dbManager;
    if (!dbManager.isOpen()) {
        std::cerr << RED
                  << "Pastikan file 'hangman.db' ada di dalam folder 'sqlite/' "
                     "relatif terhadap executable."
                  << RESET << std::endl;
        std::this_thread::sleep_for(1s);
        return;
    }
    importDb(dbManager.get());
}

std::vector<scoreEntry> getLeaderboard()
{
    DatabaseManager dbManager;
    if (!dbManager.isOpen()) { return {}; }

    std::vector<scoreEntry> leaderboard;
    const char* sql
            = "SELECT Nama, Skor FROM Leaderboard ORDER BY Skor DESC LIMIT 6;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(dbManager.get(), sql, -1, &stmt, nullptr)
        == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string nama = reinterpret_cast<const char*>(
                    sqlite3_column_text(stmt, 0));
            const int skor = sqlite3_column_int(stmt, 1);
            leaderboard.push_back({nama, skor});
        }
    }
    sqlite3_finalize(stmt);
    return leaderboard;
}

int updateScore(const std::string& nama, const int& skor)
{
    DatabaseManager dbManager;
    if (!dbManager.isOpen()) { return 0; }
    sqlite3* db = dbManager.get();
    sqlite3_stmt* stmt = nullptr;

    int skorLama = 0;
    if (sqlite3_prepare_v2(db, "SELECT Skor FROM Leaderboard WHERE Nama = ?;",
                           -1, &stmt, nullptr)
        == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, nama.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            skorLama = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);

    const int skorBaru = skorLama + skor;
    if (sqlite3_prepare_v2(db,
                           "INSERT OR REPLACE INTO Leaderboard (Nama, Skor) "
                           "VALUES (?, ?);",
                           -1, &stmt, nullptr)
        == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, nama.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, skorBaru);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
    return skorBaru;
}

std::string selectRandomWord(const int& level)
{
    DatabaseManager dbManager;
    if (!dbManager.isOpen()) {
        std::this_thread::sleep_for(2s);
        return "";
    }
    sqlite3* db = dbManager.get();
    std::vector<std::string> words;
    std::string query;
    switch (level) {
    case 1:
        query = "SELECT Kata FROM DaftarKata WHERE Panjang <= 6;";
        break;
    case 2:
        query = "SELECT Kata FROM DaftarKata WHERE Panjang BETWEEN 7 AND 10;";
        break;
    case 3:
        query = "SELECT Kata FROM DaftarKata WHERE Panjang > 10;";
        break;
    default:
        query = "SELECT Kata FROM DaftarKata;";
        break;
    }

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr)
        != SQLITE_OK) {
        std::cerr << RED
                  << "Failed to prepare statement: " << sqlite3_errmsg(db)
                  << RESET << std::endl;
        return "";
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* txt = sqlite3_column_text(stmt, 0);
        if (txt != nullptr) {
            words.emplace_back(reinterpret_cast<const char*>(txt));
        }
    }
    sqlite3_finalize(stmt);

    if (words.empty()) {
        std::cerr << RED
                  << "Error: Tidak ada kata yang sesuai dengan tingkat "
                     "kesulitan yang dipilih di database."
                  << RESET << std::endl;
        std::this_thread::sleep_for(1s);
        return "";
    }

    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<size_t> dist(0, words.size() - 1);
    return words[dist(gen)];
}

void clearTerminal()
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
    printCenteredBlock(TITLE_ASCII, MAGENTA);
    std::cout << '\n';

    if (!leaderboard.empty()) {
        std::cout << centerLine("--- Leaderboard (Top 6) ---") << '\n';
        for (const auto& entry: leaderboard) {
            std::ostringstream row;
            row << "  " << entry.namaPemain << ": " << entry.skor;
            std::cout << GREEN << centerLine(row.str()) << RESET << '\n';
        }
        std::cout << centerLine("---------------------------------") << "\n\n";
    }

    std::cout << YELLOW << centerLine("1. Mulai Permainan") << RESET << '\n';
    std::cout << YELLOW << centerLine("2. Keluar") << RESET << "\n\n";
    printPrompt("Pilihan Anda (1-2): ");
}

std::string getUsername()
{
    clearTerminal();
    printCenteredBlock(TITLE_ASCII, MAGENTA);
    std::string nama;
    while (true) {
        std::cout << '\n';
        printPrompt("Masukkan username (satu kata, tanpa spasi): ");
        std::cin >> nama;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (!nama.empty()) { return nama; }
        std::cout << RED << centerLine("Nama tidak boleh kosong. Coba lagi.")
                  << RESET << '\n';
        std::this_thread::sleep_for(1s);
        clearTerminal();
        printCenteredBlock(TITLE_ASCII, CYAN);
    }
}

int selectLevel()
{
    while (true) {
        clearTerminal();
        printCenteredBlock(TITLE_ASCII, MAGENTA);
        std::cout << '\n';
        std::cout << centerLine("--- PILIH TINGKAT KESULITAN ---") << '\n';
        std::cout << YELLOW << centerLine("1. Mudah (<7 huruf)") << RESET
                  << '\n';
        std::cout << YELLOW << centerLine("2. Sedang (7-10 huruf)") << RESET
                  << '\n';
        std::cout << YELLOW << centerLine("3. Sulit (>10 huruf)") << RESET
                  << "\n\n";
        printPrompt("Pilihan Anda (1-3): ");

        int pilihan = 0;
        std::cin >> pilihan;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << '\n'
                      << RED << centerLine("Input tidak valid. Masukkan angka.")
                      << RESET << '\n';
            std::this_thread::sleep_for(1s);
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (pilihan >= 1 && pilihan <= 3) { return pilihan; }
        std::cout << '\n'
                  << RED << centerLine("Pilihan tidak valid. Coba lagi.")
                  << RESET << '\n';
        std::this_thread::sleep_for(1s);
    }
}

std::pair<int, bool> playGame(const int& level)
{
    const std::string kataRahasia = selectRandomWord(level);
    if (kataRahasia.empty()) { return {0, false}; }

    std::string progresTebakan;
    std::string tebakanSalah;
    std::string semuaTebakan;
    int jumlahKesalahan = 0;
    constexpr int maksKesalahan = 6;

    for (char c: kataRahasia) { progresTebakan += (c == ' ') ? ' ' : '_'; }
    while (jumlahKesalahan < maksKesalahan && progresTebakan != kataRahasia) {
        displayGame(progresTebakan, tebakanSalah, jumlahKesalahan);
        char tebakan = ' ';
        std::cin >> tebakan;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        tebakan = std::tolower(static_cast<unsigned char>(tebakan));
        if (!std::isalpha(static_cast<unsigned char>(tebakan))
            || semuaTebakan.find(tebakan) != std::string::npos) {
            continue;
        }
        semuaTebakan += tebakan;
        if (kataRahasia.find(tebakan) != std::string::npos) {
            for (size_t i = 0; i < kataRahasia.length(); ++i) {
                if (kataRahasia[i] == tebakan) { progresTebakan[i] = tebakan; }
            }
        }
        else {
            jumlahKesalahan++;
            tebakanSalah += std::string(1, tebakan) + ' ';
        }
    }
    displayGame(progresTebakan, tebakanSalah, jumlahKesalahan);

    if (progresTebakan == kataRahasia) {
        std::cout << '\n'
                  << GREEN
                  << centerLine("Selamat! Anda berhasil menebak katanya!")
                  << RESET << '\n';
        return {(level * 10) + (6 - jumlahKesalahan), true};
    }
    std::cout << '\n'
              << RED << centerLine("GAME OVER! Anda gagal menebak katanya.")
              << RESET << '\n';
    std::cout << centerLine(std::string("Kata yang benar adalah: ")
                            + kataRahasia)
              << std::endl;
    return {0, false};
}

int displayEndScreen(const std::string& username, const int& currentScore,
                     const int& totalScore, const bool& lastGameWon)
{
    std::cout << "\nTekan Enter untuk melanjutkan...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    while (true) {
        clearTerminal();
        printCenteredBlock(TITLE_ASCII, lastGameWon ? CYAN : RED);
        std::cout << '\n';

        if (!lastGameWon) {
            printHangmanGameOver();
            std::cout << '\n';
        }
        else {
            printHangmanWin();
            std::cout << '\n';
        }

        std::cout << centerLine("--- PERMAINAN SELESAI ---") << '\n';
        std::cout << centerLine(std::string("Pemain: ") + username) << '\n';
        std::cout << centerLine(std::string("Skor ronde ini: ")
                                + std::to_string(currentScore))
                  << '\n';
        std::cout << centerLine(std::string("Skor Total Anda: ")
                                + std::to_string(totalScore))
                  << "\n\n";
        std::cout << YELLOW << centerLine("1. Main Lagi") << RESET << '\n';
        std::cout << YELLOW << centerLine("2. Kembali ke Menu Utama") << RESET
                  << "\n\n";
        printPrompt("Pilihan Anda (1-2): ");

        int pilihan;
        std::cin >> pilihan;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << '\n'
                      << RED << centerLine("Input tidak valid. Masukkan angka.")
                      << RESET << '\n';
            std::this_thread::sleep_for(1s);
            continue;
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (pilihan == 1) { return true; }
        if (pilihan == 2) { return false; }

        std::cout << '\n'
                  << RED << centerLine("Pilihan tidak valid. Coba lagi.")
                  << RESET << '\n';
        std::this_thread::sleep_for(1s);
    }
}

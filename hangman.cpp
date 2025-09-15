#include <iostream>
#include <string>
#include <vector>
#include <fstream>   // Diperlukan untuk membaca file
#include <random>
#include <cctype>

// --- Fungsi Helper ---

// FUNGSI INI ADALAH SATU-SATUNYA BAGIAN YANG DIUBAH SECARA SIGNIFIKAN
std::string pilihKataAcak() {
    // Daftar kata sekarang dibaca dari file, bukan di-hard-code
    std::vector<std::string> daftarKata;
    std::ifstream file("daftar_kata.txt"); // Buka file

    if (!file.is_open()) {
        std::cerr << "Error: Gagal membuka 'daftar_kata.txt'. Pastikan file ada di direktori yang sama.\n";
        return ""; // Kembalikan string kosong jika gagal
    }

    std::string kata;
    // Baca setiap kata dari file dan masukkan ke dalam vector
    while (file >> kata) {
        daftarKata.push_back(kata);
    }
    file.close();

    if (daftarKata.empty()) {
        std::cerr << "Error: File 'daftar_kata.txt' kosong.\n";
        return "";
    }

    // Bagian ini sama seperti sebelumnya: pilih satu kata secara acak
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, daftarKata.size() - 1);
    return daftarKata[distrib(gen)];
}

// Fungsi untuk menampilkan gambar hangman (TIDAK BERUBAH)
void printHangman(int jumlahKesalahan) {
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

// Fungsi untuk menampilkan status permainan (TIDAK BERUBAH)
void printStatus(const std::string& progres, const std::string& tebakanSalah, int jumlahKesalahan) {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif

    std::cout << "\n--- Hangman ---\n";
    printHangman(jumlahKesalahan);
    std::cout << "\nKata Rahasia: ";
    for (char c : progres) {
        std::cout << c << ' ';
    }
    std::cout << "\n\nTebakan Salah: " << tebakanSalah << std::endl;
    std::cout << "Kesempatan tersisa: " << 6 - jumlahKesalahan << std::endl;
}

// --- Program Utama ---
int main() {
    // Panggil fungsi seperti biasa. Main tidak perlu tahu dari mana kata itu berasal.
    std::string kataRahasia = pilihKataAcak();
    // Tambahan kecil untuk menangani jika file tidak bisa dibuka
    if (kataRahasia.empty()) {
        return 1;
    }

    std::string progresTebakan(kataRahasia.length(), '_');
    std::string tebakanSalah = "";
    std::string semuaTebakan = "";
    int jumlahKesalahan = 0;
    const int maksKesalahan = 6;

    // Sisa dari game loop ini sama persis seperti versi sebelumnya
    while (jumlahKesalahan < maksKesalahan && progresTebakan != kataRahasia) {
        printStatus(progresTebakan, tebakanSalah, jumlahKesalahan);

        if (progresTebakan == kataRahasia) {
            std::cout << "\nSelamat! Anda berhasil menebak katanya! 🎉\n";
            break;
        }

        std::cout << "\nTebak satu huruf: ";
        char tebakan;
        std::cin >> tebakan;
        tebakan = std::tolower(tebakan);

        if (!std::isalpha(tebakan)) {
            std::cout << "Input tidak valid. Harap masukkan huruf.\n";
            continue;
        }
        if (semuaTebakan.find(tebakan) != std::string::npos) {
            std::cout << "Anda sudah menebak huruf '" << tebakan << "'. Coba lagi.\n";
            continue;
        }
        semuaTebakan += tebakan;

        if (kataRahasia.find(tebakan) != std::string::npos) {
            std::cout << "\nBenar! Huruf '" << tebakan << "' ada di dalam kata.\n";
            for (size_t i = 0; i < kataRahasia.length(); ++i) {
                if (kataRahasia[i] == tebakan) {
                    progresTebakan[i] = tebakan;
                }
            }
        } else {
            std::cout << "\nSalah! Huruf '" << tebakan << "' tidak ada.\n";
            jumlahKesalahan++;
            tebakanSalah += tebakan;
            tebakanSalah += ' ';
        }
    }

    if (jumlahKesalahan == maksKesalahan) {
        printStatus(progresTebakan, tebakanSalah, jumlahKesalahan);
        std::cout << "\nGAME OVER! 🚹 Anda gagal menebak kata.\n";
        std::cout << "Kata yang benar adalah: " << kataRahasia << std::endl;
    }

    return 0;
}


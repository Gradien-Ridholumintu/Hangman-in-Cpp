#include "helper_functions/helpers.h" // Sertakan file header Anda
#include <cctype>
#include <iostream>
#include <string>

// --- Program Utama ---
int main()
{
    std::string kataRahasia = pilihKataAcak();
    if (kataRahasia.empty()) { return 1; }

    // --- PERBAIKAN: Inisialisasi progres dengan mempertimbangkan spasi ---
    std::string progresTebakan = "";
    for (char c: kataRahasia) {
        if (c == ' ') { progresTebakan += ' '; }
        else {
            progresTebakan += '_';
        }
    }

    std::string tebakanSalah = "";
    std::string semuaTebakan = "";
    int jumlahKesalahan = 0;
    const int maksKesalahan = 6;

    // Loop berjalan selama permainan BELUM berakhir (belum menang DAN belum
    // kalah)
    while (jumlahKesalahan < maksKesalahan && progresTebakan != kataRahasia) {
        printStatus(progresTebakan, tebakanSalah, jumlahKesalahan);

        std::cout << "\nTebak satu huruf: ";
        char tebakan;
        std::cin >> tebakan;
        tebakan = std::tolower(tebakan);

        // Menambahkan spasi ke tebakan yang sudah ada agar tidak bisa ditebak
        if (!std::isalpha(tebakan)
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
            tebakanSalah += tebakan;
            tebakanSalah += ' ';
        }
    }

    // --- Logika Akhir Permainan (Setelah Loop Selesai) ---
    // Pertama, tampilkan status terakhir dari papan permainan
    printStatus(progresTebakan, tebakanSalah, jumlahKesalahan);

    // Sekarang, periksa MENGAPA loop berakhir
    if (progresTebakan == kataRahasia) {
        // Jika loop berakhir karena kata sudah benar
        std::cout << "\nSelamat! Anda berhasil menebak katanya! 🎉\n";
    }
    else {
        // Jika loop berakhir karena kesalahan sudah maksimal
        std::cout << "\nGAME OVER! 🚹 Anda gagal menebak kata.\n";
        std::cout << "Kata yang benar adalah: " << kataRahasia << std::endl;
    }

    return 0;
}

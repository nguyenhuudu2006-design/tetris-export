// ============================================================
// main.cpp — Điểm vào chương trình Tetris
// Môn: Kỹ Thuật Lập Trình Nâng Cao
// Ngôn ngữ: C++17 | Thư viện: SFML 2.5+
// ============================================================

#include "TroChoi.h"
#include "QuanLyDiem.h"
#include <stdexcept>
#include <iostream>

int main() {
    try {
        QuanLyDiem quanLyDiem("diem_cao.dat");
        TroChoi    troChoi(quanLyDiem);
        troChoi.chayGame();
    }
    catch (const std::exception& loi) {
        std::cerr << "[LOI NGHIEM TRONG] " << loi.what() << std::endl;
        return 1;
    }
    return 0;
}

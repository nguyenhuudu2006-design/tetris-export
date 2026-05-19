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

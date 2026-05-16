// ============================================================
// QuanLyDiem.cpp
// Quản lý điểm số — lưu/tải file binary
// ============================================================
#include "QuanLyDiem.h"
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <iostream>

QuanLyDiem::QuanLyDiem(const std::string& tenFile)
    : tenFileLuu_(tenFile) {
    docTuFile();
}

void QuanLyDiem::themDiem(const MucDiem& mucDiem) {
    danhSachDiem_.push_back(mucDiem);
    std::sort(danhSachDiem_.begin(), danhSachDiem_.end(),
        [](const MucDiem& a, const MucDiem& b) {
            return a.diemSo > b.diemSo;
        });
    if ((int)danhSachDiem_.size() > SO_MUC_TOI_DA)
        danhSachDiem_.resize(SO_MUC_TOI_DA);
    luuVaoFile();
}

bool QuanLyDiem::laKyLuc(int diemSo) const {
    if ((int)danhSachDiem_.size() < SO_MUC_TOI_DA) return true;
    return diemSo > danhSachDiem_.back().diemSo;
}

int QuanLyDiem::layDiemCaoNhat() const {
    if (danhSachDiem_.empty()) return 0;
    return danhSachDiem_.front().diemSo;
}

const std::vector<MucDiem>& QuanLyDiem::layDanhSach() const {
    return danhSachDiem_;
}

void QuanLyDiem::luuVaoFile() const {
    try {
        std::ofstream file(tenFileLuu_, std::ios::binary | std::ios::trunc);
        if (!file.is_open())
            throw std::runtime_error("Khong the mo file luu diem: " + tenFileLuu_);

        int soMuc = (int)danhSachDiem_.size();
        file.write(reinterpret_cast<const char*>(&soMuc), sizeof(soMuc));

        for (const auto& muc : danhSachDiem_) {
            int doDaiTen = (int)muc.tenNguoiChoi.size();
            file.write(reinterpret_cast<const char*>(&doDaiTen), sizeof(doDaiTen));
            file.write(muc.tenNguoiChoi.data(), doDaiTen);
            file.write(reinterpret_cast<const char*>(&muc.diemSo),  sizeof(muc.diemSo));
            file.write(reinterpret_cast<const char*>(&muc.capDo),   sizeof(muc.capDo));
            file.write(reinterpret_cast<const char*>(&muc.soHang),  sizeof(muc.soHang));
        }
    } catch (const std::exception& loi) {
        std::cerr << "[QuanLyDiem] Loi khi luu: " << loi.what() << "\n";
    }
}

void QuanLyDiem::docTuFile() {
    try {
        std::ifstream file(tenFileLuu_, std::ios::binary);
        if (!file.is_open()) return;

        int soMuc = 0;
        file.read(reinterpret_cast<char*>(&soMuc), sizeof(soMuc));
        if (soMuc < 0 || soMuc > SO_MUC_TOI_DA)
            throw std::runtime_error("File diem bi hong");

        danhSachDiem_.clear();
        for (int i = 0; i < soMuc; i++) {
            MucDiem muc;
            int doDaiTen = 0;
            file.read(reinterpret_cast<char*>(&doDaiTen), sizeof(doDaiTen));
            if (doDaiTen < 0 || doDaiTen > 50)
                throw std::runtime_error("Ten nguoi choi khong hop le");
            muc.tenNguoiChoi.resize(doDaiTen);
            file.read(muc.tenNguoiChoi.data(), doDaiTen);
            file.read(reinterpret_cast<char*>(&muc.diemSo), sizeof(muc.diemSo));
            file.read(reinterpret_cast<char*>(&muc.capDo),  sizeof(muc.capDo));
            file.read(reinterpret_cast<char*>(&muc.soHang), sizeof(muc.soHang));
            danhSachDiem_.push_back(muc);
        }
    } catch (const std::exception& loi) {
        std::cerr << "[QuanLyDiem] Loi khi doc: " << loi.what() << "\n";
        danhSachDiem_.clear();
    }
}

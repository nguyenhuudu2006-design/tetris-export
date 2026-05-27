#pragma once
#include "KhoiTetromino.h"
#include <memory>
#include <random>
#include <array>
#include <numeric>
#include <algorithm>
#include <stdexcept>

// ============================================================
// Template class: TuiNgauNhien<T, SoPhanTu>
// Đảm bảo mỗi 7 khối xuất hiện đủ 1 lần (7-bag shuffle)
// ============================================================
template<typename KieuDuLieu, int SoPhanTu>
class TuiNgauNhien {
public:
    TuiNgauNhien() : boSinhNgauNhien_(std::random_device{}()), viTriHienTai_(SoPhanTu) {}

    KieuDuLieu layPhanTuTiepTheo() {
        if (viTriHienTai_ >= SoPhanTu) napLayTui();
        return tuiChua_[viTriHienTai_++];
    }

private:
    void napLayTui() {
        std::iota(tuiChua_.begin(), tuiChua_.end(), KieuDuLieu{0});
        std::shuffle(tuiChua_.begin(), tuiChua_.end(), boSinhNgauNhien_);
        viTriHienTai_ = 0;
    }

    std::array<KieuDuLieu, SoPhanTu> tuiChua_;
    std::mt19937 boSinhNgauNhien_;
    int viTriHienTai_;
};

// ============================================================
// NhaMayKhoi: tạo các khối Tetromino ngẫu nhiên
// ============================================================
class NhaMayKhoi {
public:
    NhaMayKhoi() : tuiNgauNhien_() {}

    // Tạo khối tiếp theo — trả về con trỏ thông minh
    std::unique_ptr<KhoiTetromino> taoKhoiTiepTheo(int x, int y) {
        int loaiKhoi = tuiNgauNhien_.layPhanTuTiepTheo();
        return taoKhoiTheoLoai(loaiKhoi, x, y);
    }

    static std::unique_ptr<KhoiTetromino> taoKhoiTheoLoai(int loai, int x, int y) {
        switch (loai) {
            case 0: return std::make_unique<KhoiI>(x, y);
            case 1: return std::make_unique<KhoiO>(x, y);
            case 2: return std::make_unique<KhoiT>(x, y);
            case 3: return std::make_unique<KhoiS>(x, y);
            case 4: return std::make_unique<KhoiZ>(x, y);
            case 5: return std::make_unique<KhoiJ>(x, y);
            case 6: return std::make_unique<KhoiL>(x, y);
            default: throw std::invalid_argument("Loai khoi khong hop le: " + std::to_string(loai));
        }
    }

private:
    TuiNgauNhien<int, 7> tuiNgauNhien_;
};

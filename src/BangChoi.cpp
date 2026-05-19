#include "BangChoi.h"
#include <algorithm>

// Khởi tạo bảng trống
BangChoi::BangChoi() {
    xoaBang();
}

// Xóa toàn bộ bảng về trạng thái trống
void BangChoi::xoaBang() {
    luoi_.assign(SO_HANG, HangO(SO_COT, sf::Color::Transparent));
}

// Kiểm tra va chạm khi di chuyển/xoay
bool BangChoi::kiemTraVaCham(const KhoiTetromino& khoi, int gocXoay,
                              int dichX, int dichY) const {
    auto hinhDang = khoi.layHinhDangXoay(gocXoay);
    int viTriX = khoi.viTriX + dichX;
    int viTriY = khoi.viTriY + dichY;

    for (int h = 0; h < (int)hinhDang.size(); h++) {
        for (int c = 0; c < (int)hinhDang[h].size(); c++) {
            if (!hinhDang[h][c]) continue;
            int cotThuc = viTriX + c;
            int hangThuc = viTriY + h;

            if (cotThuc < 0 || cotThuc >= SO_COT) return true;
            if (hangThuc >= SO_HANG)               return true;
            if (hangThuc >= 0 && luoi_[hangThuc][cotThuc] != sf::Color::Transparent)
                return true;
        }
    }
    return false;
}

// Đặt khối vào bảng (gộp vào lưới)
void BangChoi::datKhoiVaoBang(const KhoiTetromino& khoi, int gocXoay) {
    auto hinhDang = khoi.layHinhDangXoay(gocXoay);
    for (int h = 0; h < (int)hinhDang.size(); h++) {
        for (int c = 0; c < (int)hinhDang[h].size(); c++) {
            if (hinhDang[h][c] && khoi.viTriY + h >= 0) {
                luoi_[khoi.viTriY + h][khoi.viTriX + c] = khoi.layMauSac();
            }
        }
    }
}

// Xóa các hàng đầy — trả về số hàng đã xóa
int BangChoi::xoaHangDay() {
    int soHangDaXoa = 0;
    for (int h = SO_HANG - 1; h >= 0; ) {
        bool hangDay = std::all_of(luoi_[h].begin(), luoi_[h].end(),
            [](const sf::Color& o) { return o != sf::Color::Transparent; });

        if (hangDay) {
            luoi_.erase(luoi_.begin() + h);
            luoi_.insert(luoi_.begin(), HangO(SO_COT, sf::Color::Transparent));
            soHangDaXoa++;
        } else {
            h--;
        }
    }
    return soHangDaXoa;
}

// Lấy danh sách các hàng đầy (trước khi xóa)
std::vector<int> BangChoi::layDanhSachHangDay() const {
    std::vector<int> danhSach;
    for (int h = 0; h < SO_HANG; h++) {
        if (std::all_of(luoi_[h].begin(), luoi_[h].end(),
            [](const sf::Color& o){ return o != sf::Color::Transparent; }))
            danhSach.push_back(h);
    }
    return danhSach;
}

// Vẽ bảng lên cửa sổ
void BangChoi::veLen(sf::RenderWindow& cuaSo, int toaDoX, int toaDoY,
                     int kichThuocO) const {
    // Vẽ nền bảng
    sf::RectangleShape nenBang(
        sf::Vector2f(SO_COT * kichThuocO, SO_HANG * kichThuocO));
    nenBang.setPosition((float)toaDoX, (float)toaDoY);
    nenBang.setFillColor(sf::Color(12, 12, 20));
    nenBang.setOutlineColor(sf::Color(70, 70, 90));
    nenBang.setOutlineThickness(2.f);
    cuaSo.draw(nenBang);

    // Vẽ đường lưới mờ
    sf::RectangleShape duongLuoi;
    duongLuoi.setFillColor(sf::Color(255, 255, 255, 10));
    for (int c = 1; c < SO_COT; c++) {
        duongLuoi.setSize({1.f, (float)(SO_HANG * kichThuocO)});
        duongLuoi.setPosition(toaDoX + c * kichThuocO, (float)toaDoY);
        cuaSo.draw(duongLuoi);
    }
    for (int h = 1; h < SO_HANG; h++) {
        duongLuoi.setSize({(float)(SO_COT * kichThuocO), 1.f});
        duongLuoi.setPosition((float)toaDoX, toaDoY + h * kichThuocO);
        cuaSo.draw(duongLuoi);
    }

    // Vẽ các ô đã có khối
    sf::RectangleShape oKhoi(sf::Vector2f(kichThuocO - 2, kichThuocO - 2));
    sf::RectangleShape anhSang(sf::Vector2f(kichThuocO - 4, 4.f));

    for (int h = 0; h < SO_HANG; h++) {
        for (int c = 0; c < SO_COT; c++) {
            if (luoi_[h][c] != sf::Color::Transparent) {
                oKhoi.setFillColor(luoi_[h][c]);
                oKhoi.setPosition(toaDoX + c * kichThuocO + 1,
                                  toaDoY + h * kichThuocO + 1);
                cuaSo.draw(oKhoi);

                // Viền sáng trên cùng
                anhSang.setFillColor(sf::Color(255, 255, 255, 55));
                anhSang.setPosition(toaDoX + c * kichThuocO + 2,
                                    toaDoY + h * kichThuocO + 2);
                cuaSo.draw(anhSang);
            }
        }
    }
}

const std::vector<BangChoi::HangO>& BangChoi::layLuoi() const { return luoi_; }
std::vector<BangChoi::HangO>& BangChoi::layLuoi()             { return luoi_; }

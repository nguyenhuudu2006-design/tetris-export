#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <random>

// Flash màu khi xóa hàng
struct FlashHang {
    int soHang; float thoiGianHienTai, thoiGianToiDa; sf::Color mauSac;
};
class HieuUngHangFlash {
public:
    void them(int soHang, sf::Color mauSac);
    void capNhat(float dt);
    void veLen(sf::RenderWindow& cuaSo, int toaDoX, int toaDoY,
               int kichThuocO, int soCot) const;
private:
    std::vector<FlashHang> danhSach_;
};

// Mảnh vỡ bay ra khi block nổ
struct ManhVo {
    sf::Vector2f viTri, vanToc;
    float tuoiHienTai, tuoiToiDa, kichThuoc;
    sf::Color mauSac;
};
class HieuUngManhVo {
public:
    void taoCuVo(int soHang, sf::Color mauSac, int soCot, int kichThuocO);
    void capNhat(float dt);
    void veLen(sf::RenderWindow& cuaSo, int toaDoX, int toaDoY) const;
private:
    std::vector<ManhVo> danhSach_;
    std::mt19937 boSinh_{std::random_device{}()};
};

// Chữ nổi (SINGLE, DOUBLE, TETRIS!, COMBO x3...)
struct ChuNoi {
    std::string chuoi; sf::Color mauSac;
    float viTriY, thoiGianHienTai, thoiGianToiDa, tyLeKichThuoc;
};
class HieuUngChuNoi {
public:
    void them(const std::string& chuoi, sf::Color mauSac,
              float viTriY, float tyLeKichThuoc = 1.f);
    void capNhat(float dt);
    void veLen(sf::RenderWindow& cuaSo, const sf::Font& font,
               int toaDoX, int chieuRongBang) const;
private:
    std::vector<ChuNoi> danhSach_;
};

// Banner "LÊN LEVEL!" xuất hiện giữa màn hình
class BannerLenLevel {
public:
    void kichHoat(int capDoMoi, sf::Color mauChuDe, const std::string& tenChuDe);
    bool dangChay() const;
    void capNhat(float dt);
    void veLen(sf::RenderWindow& cuaSo, const sf::Font& font,
               int toaDoX, int toaDoY, int chieuRong, int chieuCao) const;
private:
    static constexpr float THOI_GIAN_HIEN = 2.0f;
    bool  dangHienThi_ = false;
    float thoiGian_ = 0.f;
    int   capDoMoi_ = 1;
    sf::Color mauChuDe_;
    std::string tenChuDe_;
};

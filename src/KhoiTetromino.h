#pragma once
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

using LuoiKhoi = std::vector<std::vector<int>>;

// ============================================================
// Class trừu tượng: KhoiTetromino
// ============================================================
class KhoiTetromino {
public:
    int viTriX, viTriY;

    explicit KhoiTetromino(int x, int y) : viTriX(x), viTriY(y) {}
    virtual ~KhoiTetromino() = default;

    // Phương thức thuần ảo
    virtual LuoiKhoi layHinhDang()  const = 0;
    virtual sf::Color layMauSac()   const = 0;
    virtual std::string layTenKhoi() const = 0;

    // Xoay hình dạng 90 độ
    LuoiKhoi xoayHinhDang(const LuoiKhoi& hinhDang) const {
        int soHang = (int)hinhDang.size();
        int soCot  = (int)hinhDang[0].size();
        LuoiKhoi ketQua(soCot, std::vector<int>(soHang, 0));
        for (int h = 0; h < soHang; h++)
            for (int c = 0; c < soCot; c++)
                ketQua[c][soHang - 1 - h] = hinhDang[h][c];
        return ketQua;
    }

    // Lấy hình dạng sau khi xoay n lần
    LuoiKhoi layHinhDangXoay(int soLanXoay) const {
        LuoiKhoi hinhDang = layHinhDang();
        for (int i = 0; i < (soLanXoay % 4); i++)
            hinhDang = xoayHinhDang(hinhDang);
        return hinhDang;
    }
};

// ============================================================
// 7 loại khối kế thừa từ KhoiTetromino
// ============================================================
class KhoiI : public KhoiTetromino {
public:
    using KhoiTetromino::KhoiTetromino;
    LuoiKhoi layHinhDang()   const override { return {{1,1,1,1}}; }
    sf::Color layMauSac()    const override { return sf::Color(55,138,221); }
    std::string layTenKhoi() const override { return "Khoi-I"; }
};

class KhoiO : public KhoiTetromino {
public:
    using KhoiTetromino::KhoiTetromino;
    LuoiKhoi layHinhDang()   const override { return {{1,1},{1,1}}; }
    sf::Color layMauSac()    const override { return sf::Color(232,196,0); }
    std::string layTenKhoi() const override { return "Khoi-O"; }
};

class KhoiT : public KhoiTetromino {
public:
    using KhoiTetromino::KhoiTetromino;
    LuoiKhoi layHinhDang()   const override { return {{0,1,0},{1,1,1}}; }
    sf::Color layMauSac()    const override { return sf::Color(153,53,86); }
    std::string layTenKhoi() const override { return "Khoi-T"; }
};

class KhoiS : public KhoiTetromino {
public:
    using KhoiTetromino::KhoiTetromino;
    LuoiKhoi layHinhDang()   const override { return {{0,1,1},{1,1,0}}; }
    sf::Color layMauSac()    const override { return sf::Color(99,153,34); }
    std::string layTenKhoi() const override { return "Khoi-S"; }
};

class KhoiZ : public KhoiTetromino {
public:
    using KhoiTetromino::KhoiTetromino;
    LuoiKhoi layHinhDang()   const override { return {{1,1,0},{0,1,1}}; }
    sf::Color layMauSac()    const override { return sf::Color(226,75,74); }
    std::string layTenKhoi() const override { return "Khoi-Z"; }
};

class KhoiJ : public KhoiTetromino {
public:
    using KhoiTetromino::KhoiTetromino;
    LuoiKhoi layHinhDang()   const override { return {{1,0,0},{1,1,1}}; }
    sf::Color layMauSac()    const override { return sf::Color(24,95,165); }
    std::string layTenKhoi() const override { return "Khoi-J"; }
};

class KhoiL : public KhoiTetromino {
public:
    using KhoiTetromino::KhoiTetromino;
    LuoiKhoi layHinhDang()   const override { return {{0,0,1},{1,1,1}}; }
    sf::Color layMauSac()    const override { return sf::Color(216,90,48); }
    std::string layTenKhoi() const override { return "Khoi-L"; }
};

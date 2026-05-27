#pragma once
#include "KhoiTetromino.h"
#include <vector>
#include <SFML/Graphics.hpp>

constexpr int SO_HANG     = 20;
constexpr int SO_COT      = 10;

class BangChoi {
public:
    using HangO = std::vector<sf::Color>;

    BangChoi();
    void xoaBang();
    bool kiemTraVaCham(const KhoiTetromino& khoi, int gocXoay,
                       int dichX = 0, int dichY = 0) const;
    void datKhoiVaoBang(const KhoiTetromino& khoi, int gocXoay);
    int  xoaHangDay();
    std::vector<int> layDanhSachHangDay() const;
    void veLen(sf::RenderWindow& cuaSo, int toaDoX, int toaDoY,
               int kichThuocO) const;

    const std::vector<HangO>& layLuoi() const;
    std::vector<HangO>&       layLuoi();

private:
    std::vector<HangO> luoi_;
};

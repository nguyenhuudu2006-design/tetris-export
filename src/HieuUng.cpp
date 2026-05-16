// ============================================================
// HieuUng.cpp
// Hiệu ứng nổ block, combo text, particles
// ============================================================
#include "HieuUng.h"
#include <cmath>
#include <algorithm>

// ─── HieuUngHangFlash ───────────────────────────────────────
void HieuUngHangFlash::them(int soHang, sf::Color mauSac) {
    danhSach_.push_back({soHang, 0.f, 0.3f, mauSac});
}

void HieuUngHangFlash::capNhat(float thoiGianDelta) {
    for (auto& f : danhSach_) f.thoiGianHienTai += thoiGianDelta;
    danhSach_.erase(
        std::remove_if(danhSach_.begin(), danhSach_.end(),
            [](const FlashHang& f){ return f.thoiGianHienTai >= f.thoiGianToiDa; }),
        danhSach_.end());
}

void HieuUngHangFlash::veLen(sf::RenderWindow& cuaSo,
                              int toaDoX, int toaDoY, int kichThuocO,
                              int soCot) const {
    for (const auto& f : danhSach_) {
        float tyLe   = 1.f - f.thoiGianHienTai / f.thoiGianToiDa;
        sf::Color mau = f.mauSac;
        mau.a = (sf::Uint8)(tyLe * 160);
        sf::RectangleShape thanh(
            {(float)(soCot * kichThuocO), (float)kichThuocO});
        thanh.setPosition((float)toaDoX,
                          (float)(toaDoY + f.soHang * kichThuocO));
        thanh.setFillColor(mau);
        cuaSo.draw(thanh);

        sf::Color mauTrang(255, 255, 255, (sf::Uint8)(tyLe * 80));
        thanh.setFillColor(mauTrang);
        cuaSo.draw(thanh);
    }
}

// ─── HieuUngManhVo ──────────────────────────────────────────
void HieuUngManhVo::taoCuVo(int soHang, sf::Color mauSac, int soCot,
                              int kichThuocO) {
    std::uniform_real_distribution<float> gocNgauNhien(0.f, 6.28318f);
    std::uniform_real_distribution<float> tocDoNgauNhien(1.f, 4.f);
    std::uniform_real_distribution<float> tuoiSong(0.7f, 1.5f);
    std::uniform_real_distribution<float> kichThuocNgauNhien(2.f, 6.f);

    for (int c = 0; c < soCot; c++) {
        for (int k = 0; k < 3; k++) {
            float goc = gocNgauNhien(boSinh_);
            float tocDo = tocDoNgauNhien(boSinh_);
            ManhVo mv;
            mv.viTri  = { (float)(c * kichThuocO + kichThuocO/2),
                          (float)(soHang * kichThuocO + kichThuocO/2) };
            mv.vanToc = { std::cos(goc) * tocDo, std::sin(goc) * tocDo - 0.8f };
            mv.tuoiHienTai  = 0.f;
            mv.tuoiToiDa    = tuoiSong(boSinh_);
            mv.kichThuoc    = kichThuocNgauNhien(boSinh_);
            mv.mauSac       = mauSac;
            danhSach_.push_back(mv);
        }
    }
}

void HieuUngManhVo::capNhat(float dt) {
    for (auto& mv : danhSach_) {
        mv.viTri   += mv.vanToc * dt * 60.f;
        mv.vanToc.y += 0.12f;
        mv.tuoiHienTai += dt;
    }
    danhSach_.erase(
        std::remove_if(danhSach_.begin(), danhSach_.end(),
            [](const ManhVo& mv){ return mv.tuoiHienTai >= mv.tuoiToiDa; }),
        danhSach_.end());
}

void HieuUngManhVo::veLen(sf::RenderWindow& cuaSo,
                           int toaDoX, int toaDoY) const {
    for (const auto& mv : danhSach_) {
        float tyLe = 1.f - mv.tuoiHienTai / mv.tuoiToiDa;
        float kt   = mv.kichThuoc * (1.f - mv.tuoiHienTai / mv.tuoiToiDa * 0.5f);
        sf::RectangleShape hinh({kt, kt});
        sf::Color mau = mv.mauSac;
        mau.a = (sf::Uint8)(tyLe * 230);
        hinh.setFillColor(mau);
        hinh.setPosition(toaDoX + mv.viTri.x - kt/2,
                         toaDoY + mv.viTri.y - kt/2);
        cuaSo.draw(hinh);
    }
}

// ─── HieuUngChuNoi ──────────────────────────────────────────
void HieuUngChuNoi::them(const std::string& chuoi, sf::Color mauSac,
                          float viTriY, float tyLeKichThuoc) {
    ChuNoi cn;
    cn.chuoi          = chuoi;
    cn.mauSac         = mauSac;
    cn.viTriY         = viTriY;
    cn.thoiGianHienTai = 0.f;
    cn.thoiGianToiDa  = 1.1f;
    cn.tyLeKichThuoc  = tyLeKichThuoc;
    danhSach_.push_back(cn);
}

void HieuUngChuNoi::capNhat(float dt) {
    for (auto& cn : danhSach_) cn.thoiGianHienTai += dt;
    danhSach_.erase(
        std::remove_if(danhSach_.begin(), danhSach_.end(),
            [](const ChuNoi& cn){ return cn.thoiGianHienTai >= cn.thoiGianToiDa; }),
        danhSach_.end());
}

void HieuUngChuNoi::veLen(sf::RenderWindow& cuaSo, const sf::Font& font,
                           int toaDoX, int chieuRongBang) const {
    for (const auto& cn : danhSach_) {
        float tyLe = cn.thoiGianHienTai / cn.thoiGianToiDa;
        float alpha = tyLe < 0.2f ? tyLe/0.2f : 1.f-(tyLe-0.2f)/0.8f;
        float deChuoi = -tyLe * 40.f;

        sf::Text vanBan;
        vanBan.setFont(font);
        vanBan.setString(cn.chuoi);
        unsigned int coDoChu = (unsigned int)(20 * cn.tyLeKichThuoc *
                                              (1.f + (1.f - tyLe) * 0.15f));
        vanBan.setCharacterSize(std::max(10u, coDoChu));
        vanBan.setStyle(sf::Text::Bold);
        sf::Color mau = cn.mauSac;
        mau.a = (sf::Uint8)(std::max(0.f, alpha) * 255);
        vanBan.setFillColor(mau);

        float chieuRongChu = vanBan.getLocalBounds().width;
        vanBan.setPosition(
            toaDoX + (chieuRongBang - chieuRongChu) / 2.f,
            cn.viTriY + deChuoi);
        cuaSo.draw(vanBan);
    }
}

// ─── BannerLenLevel ─────────────────────────────────────────
void BannerLenLevel::kichHoat(int capDoMoi, sf::Color mauChuDe,
                               const std::string& tenChuDe) {
    dangHienThi_  = true;
    thoiGian_     = 0.f;
    capDoMoi_     = capDoMoi;
    mauChuDe_     = mauChuDe;
    tenChuDe_     = tenChuDe;
}

bool BannerLenLevel::dangChay() const { return dangHienThi_; }

void BannerLenLevel::capNhat(float dt) {
    if (!dangHienThi_) return;
    thoiGian_ += dt;
    if (thoiGian_ >= THOI_GIAN_HIEN) dangHienThi_ = false;
}

void BannerLenLevel::veLen(sf::RenderWindow& cuaSo, const sf::Font& font,
                           int toaDoX, int toaDoY,
                           int chieuRong, int chieuCao) const {
    if (!dangHienThi_) return;
    float tyLe = thoiGian_ / THOI_GIAN_HIEN;
    float alpha = tyLe < 0.15f ? tyLe/0.15f : tyLe > 0.75f ? 1.f-(tyLe-0.75f)/0.25f : 1.f;

    float bw = 220.f, bh = 100.f;
    float bx = toaDoX + (chieuRong - bw) / 2.f;
    float by = toaDoY + (chieuCao - bh) / 2.f;

    // Flash trắng lúc đầu
    if (thoiGian_ < 0.12f) {
        sf::RectangleShape flash({(float)chieuRong,(float)chieuCao});
        flash.setPosition((float)toaDoX,(float)toaDoY);
        float a = (1.f - thoiGian_/0.12f) * 0.4f;
        flash.setFillColor(sf::Color(255,255,255,(sf::Uint8)(a*255)));
        cuaSo.draw(flash);
    }

    // Hộp banner
    sf::RectangleShape hop({bw, bh});
    hop.setPosition(bx, by);
    sf::Color mauNen(0,0,0,(sf::Uint8)(alpha*210));
    hop.setFillColor(mauNen);
    sf::Color mauVien = mauChuDe_;
    mauVien.a = (sf::Uint8)(alpha*255);
    hop.setOutlineColor(mauVien);
    hop.setOutlineThickness(2.f);
    cuaSo.draw(hop);

    auto veChoChu = [&](const std::string& s, float y, unsigned sz,
                        sf::Color mau, bool in_dam = false) {
        sf::Text t; t.setFont(font); t.setString(s);
        t.setCharacterSize(sz);
        mau.a = (sf::Uint8)(alpha*255);
        t.setFillColor(mau);
        if (in_dam) t.setStyle(sf::Text::Bold);
        t.setPosition(bx + (bw - t.getLocalBounds().width)/2.f - 4.f, y);
        cuaSo.draw(t);
    };

    veChoChu("LEN LEVEL!", by + 14, 14, mauChuDe_);
    veChoChu(std::to_string(capDoMoi_), by + 32, 40, sf::Color::White, true);
    veChoChu(tenChuDe_, by + 78, 15, mauChuDe_, true);
}

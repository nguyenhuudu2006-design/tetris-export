#pragma once
// ============================================================
// ChuDe.h — Chủ đề màu sắc và hình nền cho 10 level
// ============================================================
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <cmath>
#include <random>

struct ThongTinChuDe {
    std::string tenChuDe;
    sf::Color   mauNenChinh;
    sf::Color   mauNoiBat;
    sf::Color   mauHatParticle;
};

static const ThongTinChuDe DANH_SACH_CHU_DE[10] = {
    {"VOID",    {10,10,20},   {55,138,221},  {150,180,255}},
    {"NEON",    {3,14,8},     {29,158,117},  {80,220,160}},
    {"LAVA",    {18,4,0},     {216,90,48},   {220,80,20}},
    {"OCEAN",   {2,10,18},    {24,95,165},   {60,140,220}},
    {"CYBER",   {13,0,16},    {153,53,86},   {200,80,130}},
    {"FOREST",  {2,11,2},     {99,153,34},   {100,180,60}},
    {"STORM",   {6,0,16},     {127,119,221}, {160,150,255}},
    {"DESERT",  {15,9,0},     {186,117,23},  {220,160,60}},
    {"AURORA",  {0,8,15},     {93,202,165},  {100,220,180}},
    {"INFERNO", {15,0,0},     {226,75,74},   {255,80,60}},
};

// Renderer hình nền động
class VeNenDong {
public:
    VeNenDong(int chieuRong, int chieuCao)
        : chieuRong_(chieuRong), chieuCao_(chieuCao), tick_(0.f) {
        boSinh_ = std::mt19937(42);
    }

    void capNhat(float dt) { tick_ += dt * 0.05f; }

    void veLen(sf::RenderWindow& cuaSo, int capDo, int toaDoX, int toaDoY) {
        const auto& cd = DANH_SACH_CHU_DE[capDo - 1];
        const auto& mp = cd.mauHatParticle;
        float W = (float)chieuRong_, H = (float)chieuCao_;

        // Nền đặc
        sf::RectangleShape nen({W, H});
        nen.setPosition((float)toaDoX, (float)toaDoY);
        nen.setFillColor(cd.mauNenChinh);
        cuaSo.draw(nen);

        // Pattern theo level
        switch (capDo) {
        case 1: veNenSaoTroi(cuaSo, toaDoX, toaDoY, W, H, mp); break;
        case 2: veNenLuoi(cuaSo, toaDoX, toaDoY, W, H, mp);    break;
        case 3: veNenNuocLava(cuaSo, toaDoX, toaDoY, W, H, mp);break;
        case 4: veNenSongBien(cuaSo, toaDoX, toaDoY, W, H, mp);break;
        case 5: veNenDuongCheo(cuaSo, toaDoX, toaDoY, W, H, mp);break;
        case 6: veNenCayCoiRung(cuaSo, toaDoX, toaDoY, W, H, mp);break;
        case 7: veNenTiaSam(cuaSo, toaDoX, toaDoY, W, H, mp);  break;
        case 8: veNenCatDune(cuaSo, toaDoX, toaDoY, W, H, mp); break;
        case 9: veNenAurora(cuaSo, toaDoX, toaDoY, W, H, mp);  break;
        case 10:veNenLuaFlame(cuaSo, toaDoX, toaDoY, W, H, mp);break;
        }
    }

private:
    int chieuRong_, chieuCao_;
    float tick_;
    std::mt19937 boSinh_;

    void veDuong(sf::RenderWindow& cua, float x1, float y1, float x2, float y2,
                 sf::Color mau) {
        sf::VertexArray duong(sf::Lines, 2);
        duong[0].position = {x1, y1}; duong[0].color = mau;
        duong[1].position = {x2, y2}; duong[1].color = mau;
        cua.draw(duong);
    }

    void veNenSaoTroi(sf::RenderWindow& cua, int ox, int oy,
                      float W, float H, const sf::Color& mp) {
        sf::CircleShape ngoi_sao(1.5f);
        for (int i = 0; i < 30; i++) {
            float x = std::fmod(i*67.f + tick_*15.f, W);
            float y = std::fmod(i*53.f + tick_*10.f, H);
            float do_sang = 0.2f + 0.15f * std::sin(tick_*0.05f + i);
            ngoi_sao.setFillColor(sf::Color(mp.r, mp.g, mp.b,
                                            (sf::Uint8)(do_sang * 200)));
            ngoi_sao.setPosition(ox + x, oy + y);
            cua.draw(ngoi_sao);
        }
    }

    void veNenLuoi(sf::RenderWindow& cua, int ox, int oy,
                   float W, float H, const sf::Color& mp) {
        sf::Color mauLuoi(mp.r, mp.g, mp.b, 20);
        for (int i = 0; i < 7; i++) {
            float y = std::fmod(i*70.f + tick_*20.f, H + 30) - 15;
            veDuong(cua, ox, oy+y, ox+W, oy+y, mauLuoi);
        }
        for (int i = 0; i < 6; i++) {
            float x = std::fmod(i*40.f + tick_*15.f, W + 10) - 5;
            veDuong(cua, ox+x, oy, ox+x, oy+H, mauLuoi);
        }
    }

    void veNenNuocLava(sf::RenderWindow& cua, int ox, int oy,
                       float W, float H, const sf::Color& mp) {
        for (int i = 0; i < 8; i++) {
            sf::VertexArray song(sf::LinesStrip, (int)(W/6)+1);
            int idx = 0;
            for (float x = 0; x <= W; x += 6, idx++) {
                float y = H - 50 + std::sin(x*0.08f + tick_*0.4f + i*0.5f) * 15;
                song[idx].position = {ox+x, oy+y};
                song[idx].color = sf::Color(mp.r, mp.g, mp.b, 18);
            }
            cua.draw(song);
        }
    }

    void veNenSongBien(sf::RenderWindow& cua, int ox, int oy,
                       float W, float H, const sf::Color& mp) {
        for (int i = 0; i < 6; i++) {
            sf::VertexArray song(sf::LinesStrip, (int)(W/7)+1);
            int idx = 0;
            for (float x = 0; x <= W; x += 7, idx++) {
                float y = 80 + i*60 + std::sin(x*0.05f + tick_*0.25f + i)*8;
                song[idx].position = {ox+x, oy+y};
                song[idx].color = sf::Color(mp.r, mp.g, mp.b, 20);
            }
            cua.draw(song);
        }
    }

    void veNenDuongCheo(sf::RenderWindow& cua, int ox, int oy,
                        float W, float H, const sf::Color& mp) {
        sf::Color mau(mp.r, mp.g, mp.b, 16);
        for (int i = 0; i < 14; i++) {
            float x = std::fmod(i*22.f + tick_*10.f, W + 20) - 10;
            veDuong(cua, ox+x, oy, ox+x-(i%2?18:-18), oy+H, mau);
        }
    }

    void veNenCayCoiRung(sf::RenderWindow& cua, int ox, int oy,
                         float W, float H, const sf::Color& mp) {
        sf::Color mau(mp.r, mp.g, mp.b, 40);
        for (int i = 0; i < 14; i++) {
            float x  = i * 22.f;
            float h  = 70 + std::sin(i*1.7f)*25;
            float sw = std::sin(tick_*0.016f + i) * 3;
            veDuong(cua, ox+x+sw, oy+H, ox+x+sw/2, oy+H-h, mau);
        }
    }

    void veNenTiaSam(sf::RenderWindow& cua, int ox, int oy,
                     float W, float H, const sf::Color& mp) {
        sf::Color mau(mp.r, mp.g, mp.b, 22);
        for (int i = 0; i < 6; i++) {
            float x = std::fmod(i*44.f + tick_*70.f, W+20) - 10;
            veDuong(cua, ox+x, oy, ox+x-8, oy+H, mau);
        }
    }

    void veNenCatDune(sf::RenderWindow& cua, int ox, int oy,
                      float W, float H, const sf::Color& mp) {
        sf::VertexArray dune(sf::LinesStrip, (int)(W/5)+1);
        int idx = 0;
        for (float x = 0; x <= W; x += 5, idx++) {
            float y = H - 80 + std::sin(x*0.04f + tick_*0.5f)*20;
            dune[idx].position = {ox+x, oy+y};
            dune[idx].color = sf::Color(mp.r, mp.g, mp.b, 20);
        }
        cua.draw(dune);
    }

    void veNenAurora(sf::RenderWindow& cua, int ox, int oy,
                     float W, float H, const sf::Color& mp) {
        for (int i = 0; i < 5; i++) {
            sf::VertexArray dai(sf::LinesStrip, (int)(W/5)+1);
            int idx = 0;
            for (float x = 0; x <= W; x += 5, idx++) {
                float y = 60 + i*32 + std::sin(x*0.028f + tick_*0.75f + i)*22;
                dai[idx].position = {ox+x, oy+y};
                dai[idx].color = sf::Color(mp.r, mp.g, mp.b,
                                           (sf::Uint8)(14+i*5));
            }
            cua.draw(dai);
        }
    }

    void veNenLuaFlame(sf::RenderWindow& cua, int ox, int oy,
                       float W, float H, const sf::Color& mp) {
        for (int i = 0; i < 16; i++) {
            float x  = std::fmod(i*16.f + std::sin(tick_*1.6f + i)*8, W);
            float fh = 25 + std::sin(tick_*3.f + i*0.5f) * 14;
            float al = 0.07f + std::sin(tick_*5.f + i)*0.05f;
            sf::VertexArray lua(sf::Triangles, 3);
            lua[0].position = {ox+x-4, oy+H};
            lua[1].position = {ox+x+4, oy+H};
            lua[2].position = {ox+x,   oy+H-fh};
            sf::Color mauLua(mp.r, mp.g, mp.b, (sf::Uint8)(al*220));
            lua[0].color = lua[1].color = lua[2].color = mauLua;
            cua.draw(lua);
        }
    }
};

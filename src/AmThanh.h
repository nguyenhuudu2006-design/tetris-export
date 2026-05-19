#pragma once
#include <SFML/Audio.hpp>
#include <array>
#include <string>
#include <iostream>
#include <algorithm>

// ─── Hieu ung am thanh (SFX) ─────────────────────────────────
class AmThanhHieuUng {
public:
    AmThanhHieuUng() : tatAm_(false), amLuong_(70.f) { napTatCaFile(); }

    void phatRoiBlock()          { phat(bufRoiBlock_);  }
    void phatXoayKhoi()          { phat(bufXoay_);      }
    void phatXoaHang(int soHang) {
        if (soHang >= 4) phat(bufTetris_);
        else             phat(bufXoaHang_);
    }
    void phatLenLevel()          { phat(bufLenLevel_);  }
    void phatRoiNhanh()          { phat(bufRoiNhanh_);  }
    void phatGameOver()          { phat(bufGameOver_);  }

    void  datTatAm(bool tat)   { tatAm_ = tat; }
    bool  layTatAm() const     { return tatAm_; }
    void  batTat()             { tatAm_ = !tatAm_; }
    float layAmLuong() const   { return amLuong_; }
    void  datAmLuong(float al) {
        amLuong_ = std::clamp(al, 0.f, 100.f);
        for (auto& s : pool_) s.setVolume(amLuong_);
    }

private:
    sf::SoundBuffer bufRoiBlock_, bufXoay_, bufXoaHang_,
                    bufTetris_,  bufLenLevel_, bufRoiNhanh_, bufGameOver_;
    std::array<sf::Sound, 8> pool_;
    int   viTriPool_ = 0;
    bool  tatAm_     = false;
    float amLuong_   = 70.f;

    void napFile(sf::SoundBuffer& buf, const std::string& ten) {
        std::string duong = std::string("assets/sounds/") + ten;
        if (!buf.loadFromFile(duong))
            std::cerr << "[SFX] Khong tim thay: " << duong << "\n";
    }

    void napTatCaFile() {
        napFile(bufRoiBlock_, "roi_block.wav");
        napFile(bufXoay_,     "xoay.wav");
        napFile(bufXoaHang_,  "xoa_hang.wav");
        napFile(bufTetris_,   "tetris.wav");
        napFile(bufLenLevel_, "len_level.wav");
        napFile(bufRoiNhanh_, "soft_drop.wav");
        napFile(bufGameOver_, "game_over.wav");
    }

    void phat(const sf::SoundBuffer& buf) {
        if (tatAm_ || buf.getSampleCount() == 0) return;
        pool_[viTriPool_].setBuffer(buf);
        pool_[viTriPool_].setVolume(amLuong_);
        pool_[viTriPool_].play();
        viTriPool_ = (viTriPool_ + 1) % (int)pool_.size();
    }
};

// ─── Nhac nen: phan biet nhac menu va nhac game ───────────────
class NhacNen {
public:
    NhacNen()
        : tatAm_(false), dangPhat_(false),
          coFileMenu_(false), coFileGame_(false),
          amLuong_(50.f), loaiDang_(MENU)
    {
        // Nap nhac menu
        const char* dsMenu[] = {
            "assets/sounds/nhac_menu.mp3",
            "assets/sounds/nhac_menu.ogg",
            "assets/sounds/nhac_menu.wav",
        };
        for (auto d : dsMenu) {
            if (nhacMenu_.openFromFile(d)) {
                coFileMenu_ = true;
                std::cout << "[Nhac] Menu: " << d << "\n";
                break;
            }
        }
        if (!coFileMenu_)
            std::cerr << "[Nhac] Khong tim thay nhac_menu!\n";

        // Nap nhac game
        const char* dsGame[] = {
            "assets/sounds/nhac_game.wav",
            "assets/sounds/nhac_game.ogg",
            "assets/sounds/nhac_game.mp3",
        };
        for (auto d : dsGame) {
            if (nhacGame_.openFromFile(d)) {
                coFileGame_ = true;
                std::cout << "[Nhac] Game: " << d << "\n";
                break;
            }
        }
        if (!coFileGame_)
            std::cerr << "[Nhac] Khong tim thay nhac_game!\n";
    }

    // Phat nhac menu (sanh cho)
    void batMenu() {
        if (dangPhat_ && loaiDang_ == MENU) return;
        dung();
        if (tatAm_ || !coFileMenu_) return;
        nhacMenu_.setLoop(true);
        nhacMenu_.setVolume(amLuong_);
        nhacMenu_.play();
        dangPhat_ = true;
        loaiDang_ = MENU;
    }

    // Phat nhac game (trong khi choi)
    void batGame() {
        if (dangPhat_ && loaiDang_ == GAME) return;
        dung();
        if (tatAm_ || !coFileGame_) return;
        nhacGame_.setLoop(true);
        nhacGame_.setVolume(amLuong_);
        nhacGame_.play();
        dangPhat_ = true;
        loaiDang_ = GAME;
    }

    void tat() { dung(); }

    void datTatAm(bool tatMoi) {
        tatAm_ = tatMoi;
        if (tatMoi) {
            dung();
        } else {
            if (loaiDang_ == MENU) batMenu();
            else                   batGame();
        }
    }
    bool  layTatAm()  const { return tatAm_;   }
    void  batTat()          { datTatAm(!tatAm_); }
    bool  dangChay()  const { return dangPhat_; }

    float layAmLuong() const { return amLuong_; }
    void  datAmLuong(float al) {
        amLuong_ = std::clamp(al, 0.f, 100.f);
        nhacMenu_.setVolume(amLuong_);
        nhacGame_.setVolume(amLuong_);
    }

private:
    enum LoaiNhac { MENU, GAME };

    sf::Music nhacMenu_, nhacGame_;
    bool      tatAm_, dangPhat_, coFileMenu_, coFileGame_;
    float     amLuong_;
    LoaiNhac  loaiDang_;

    void dung() {
        nhacMenu_.stop();
        nhacGame_.stop();
        dangPhat_ = false;
    }
};

// ─── Quan ly chung ────────────────────────────────────────────
class QuanLyAmThanh {
public:
    QuanLyAmThanh() = default;
    AmThanhHieuUng& hieuUng() { return hieuUng_; }
    NhacNen&        nhacNen() { return nhacNen_; }
    float layAmLuongHieuUng() const  { return hieuUng_.layAmLuong(); }
    float layAmLuongNhac()    const  { return nhacNen_.layAmLuong(); }
    void  datAmLuongHieuUng(float v) { hieuUng_.datAmLuong(v);       }
    void  datAmLuongNhac(float v)    { nhacNen_.datAmLuong(v);       }
    bool  layCaiAmHieuUng() const    { return hieuUng_.layTatAm();   }
    bool  layCaiAmNhac()    const    { return nhacNen_.layTatAm();   }
private:
    AmThanhHieuUng hieuUng_;
    NhacNen        nhacNen_;
};

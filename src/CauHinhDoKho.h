#pragma once
// ============================================================
// CauHinhDoKho.h — Cấu hình 3 mức độ khó
// ============================================================
#include <string>
#include <algorithm>

enum class MucDoKho { DE, VUA, KHO };

struct CauHinhDoKho {
    MucDoKho  mucDo;
    std::string tenHienThi;   // "DE", "VUA", "KHO"
    std::string moTa;
    int soHangLenLevel;       // bao nhiêu hàng thì lên 1 level
    int thoiGianRoiBanDau;    // ms
    int thoiGianRoiToiThieu;  // ms
    int heSoCombo;
};

inline CauHinhDoKho layCauHinh(MucDoKho mucDo) {
    switch (mucDo) {
    case MucDoKho::DE:
        return {MucDoKho::DE, "DE", "Roi cham - 4 hang/level",
                4, 1400, 400, 1};
    case MucDoKho::VUA:
        return {MucDoKho::VUA, "VUA", "Can bang - 7 hang/level",
                7, 900, 250, 2};
    case MucDoKho::KHO:
        return {MucDoKho::KHO, "KHO", "Roi nhanh - 10 hang/level",
                10, 600, 100, 3};
    }
    return layCauHinh(MucDoKho::VUA);
}

inline int tinhThoiGianRoi(const CauHinhDoKho& ch, int capDo) {
    int khoangCach = ch.thoiGianRoiBanDau - ch.thoiGianRoiToiThieu;
    int giamMoiLevel = khoangCach / 9;
    return std::max(ch.thoiGianRoiToiThieu,
                    ch.thoiGianRoiBanDau - (capDo - 1) * giamMoiLevel);
}

#pragma once
// ============================================================
// QuanLyDiem.h
// ============================================================
#include <string>
#include <vector>

struct MucDiem {
    std::string tenNguoiChoi;
    int diemSo = 0;
    int capDo  = 1;
    int soHang = 0;
};

class QuanLyDiem {
public:
    static constexpr int SO_MUC_TOI_DA = 10;

    explicit QuanLyDiem(const std::string& tenFile);
    void themDiem(const MucDiem& mucDiem);
    bool laKyLuc(int diemSo) const;
    int  layDiemCaoNhat() const;
    const std::vector<MucDiem>& layDanhSach() const;

private:
    void luuVaoFile() const;
    void docTuFile();

    std::string tenFileLuu_;
    std::vector<MucDiem> danhSachDiem_;
};

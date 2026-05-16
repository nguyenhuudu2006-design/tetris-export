#pragma once
// ============================================================
// TroChoi.h — Class chính
// + Âm thanh 8-bit (AmThanh.h)
// + Cửa sổ có thể resize, tự scale layout
// ============================================================
#include "BangChoi.h"
#include "NhaMayKhoi.h"
#include "QuanLyDiem.h"
#include "HieuUng.h"
#include "CauHinhDoKho.h"
#include "ChuDe.h"
#include "AmThanh.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

// ── Kích thước mặc định (sẽ tính lại khi resize) ───────────
static int CHIEU_RONG_CUA_SO = (int)sf::VideoMode::getDesktopMode().width;
static int CHIEU_CAO_CUA_SO  = (int)sf::VideoMode::getDesktopMode().height - 40;

// ── Các giá trị layout — tính lại qua hàm tinhLayout() ─────
static int KICH_THUOC_O    = 0;
static int CHIEU_RONG_BANG = 0;
static int CHIEU_CAO_BANG  = 0;
static int OFFSET_X_BANG   = 0;
static int OFFSET_Y_BANG   = 0;
static int OFFSET_X_PANEL  = 0;
static int CHIEU_RONG_PANEL= 0;

// Tính lại layout mỗi khi resize cửa sổ
static void tinhLayout(int chieuRong, int chieuCao) {
    CHIEU_RONG_CUA_SO = chieuRong;
    CHIEU_CAO_CUA_SO  = chieuCao;
    KICH_THUOC_O      = std::max(16, (chieuCao - 60) / SO_HANG);
    CHIEU_RONG_BANG   = SO_COT  * KICH_THUOC_O;
    CHIEU_CAO_BANG    = SO_HANG * KICH_THUOC_O;
    OFFSET_X_BANG     = 40;
    OFFSET_Y_BANG     = (chieuCao - CHIEU_CAO_BANG) / 2;
    OFFSET_X_PANEL    = OFFSET_X_BANG + CHIEU_RONG_BANG + 28;
    CHIEU_RONG_PANEL  = chieuRong - OFFSET_X_PANEL - 20;
}

// ── Trạng thái game ─────────────────────────────────────────
enum class TrangThaiGame {
    MENU, CHON_DO_KHO, DANG_CHOI, TAM_DUNG,
    GAME_OVER, BANG_DIEM_CAO, CAI_DAT_AM_THANH
};

// ── Thống kê trong khi chơi ─────────────────────────────────
struct ThongKeGame {
    int diemSo      = 0;
    int capDo       = 1;
    int soHangDaXoa = 0;
    int soKhoiDaRoi = 0;
    int combo       = 0;
};

// ── Class TroChoi ────────────────────────────────────────────
class TroChoi {
public:
    explicit TroChoi(QuanLyDiem& quanLyDiem);
    void chayGame();

private:
    QuanLyDiem& quanLyDiem_;

    // SFML
    sf::RenderWindow cuaSo_;
    sf::Font    font_;
    sf::Texture textureNenMenu_;
    sf::Sprite  spriteNenMenu_;
    bool        coAnhNen_ = false;

    // Âm thanh
    QuanLyAmThanh amThanh_;

    // Game objects
    BangChoi   bangChoi_;
    NhaMayKhoi nhaMay_;
    std::unique_ptr<KhoiTetromino> khoiHienTai_;
    std::unique_ptr<KhoiTetromino> khoiTiepTheo_;

    // Trạng thái
    TrangThaiGame trangThai_;
    TrangThaiGame trangThaiTruoc_; // để quay lại từ cài đặt
    ThongKeGame   thongKe_;
    int           gocXoay_;
    int           diemKyLuc_;

    // Cấu hình
    CauHinhDoKho  cauHinhDoKho_;
    MucDoKho      mucDoChon_;

    // Hiệu ứng
    VeNenDong        veNenDong_;
    BannerLenLevel   bannerLenLevel_;
    HieuUngHangFlash hieuUngFlash_;
    HieuUngManhVo    hieuUngManhVo_;
    HieuUngChuNoi    hieuUngChuNoi_;

    // Nhập tên
    std::string tenNguoiChoi_;

    // ── Phương thức ──────────────────────────────────────────
    void xuLySuKien();
    void xuLyPhimBam(sf::Keyboard::Key phim);
    void xuLyResize(int chieuRong, int chieuCao);

    // Logic game
    void batDauGame();
    void taoKhoiMoi();
    void diChuyenKhoi(int huong);
    void xoayKhoi(int huong);
    void khoiRoiXuong();
    void softDrop();
    void hardDrop();
    void khoaBang();
    void tamDung();
    int  tinhViTriBong() const;
    void chuyenDoKho(int huong);

    // Vẽ
    void veManHinh();
    void veManGame();
    void veManMenu();
    void veManChonDoKho();
    void veManTamDung();
    void veManGameOver();
    void veBangDiemCao();
    void veManCaiDatAmThanh();
    void vePanelBenPhai();
    void veKhoi(const LuoiKhoi& hinhDang, int x, int y,
                sf::Color mauSac, float alpha);

    // Helpers vẽ
    void veNutDep(float x, float y, float w, float h,
                  const std::string& nhan, const std::string& phim,
                  sf::Color mauNen, sf::Color mauChu);
    void veChoChu(const std::string& chuoi, float x, float y,
                  unsigned int coDoChu,
                  sf::Color mauSac = sf::Color::White, bool inDam = false);
    void veChuGiua(const std::string& chuoi, float y, unsigned int coDoChu,
                   sf::Color mauSac = sf::Color::White);
    void vePanel(float x, float y, float w, float h);
    void vePanelVienVang(float x, float y, float w, float h);
    void vePanelMauNoiBat(float x, float y, float w, float h, sf::Color mau);
    void veHopOverlay(float x, float y, float w, float h, sf::Color mauNen);
    void veThanhTruot(float x, float y, float w, float h,
                      float giaTri, float toiDa, sf::Color mauThanh);
};

#include "TroChoi.h"
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <cmath>

// ─── Khởi tạo ───────────────────────────────────────────────
TroChoi::TroChoi(QuanLyDiem& quanLyDiem)
    : quanLyDiem_(quanLyDiem),
      cuaSo_(sf::VideoMode(sf::VideoMode::getDesktopMode().width,
             sf::VideoMode::getDesktopMode().height - 40),
             "Tetris - Ky Thuat Lap Trinh Nang Cao",
             sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize),
      trangThai_(TrangThaiGame::MENU),
      trangThaiTruoc_(TrangThaiGame::MENU),
      gocXoay_(0),
      diemKyLuc_(quanLyDiem.layDiemCaoNhat()),
      cauHinhDoKho_(layCauHinh(MucDoKho::VUA)),
      mucDoChon_(MucDoKho::VUA),
      veNenDong_(0, 0),
      tenNguoiChoi_("NguoiChoi")
{
    cuaSo_.setFramerateLimit(60);

    // Tính layout lần đầu
    tinhLayout(cuaSo_.getSize().x, cuaSo_.getSize().y);
    veNenDong_ = VeNenDong(CHIEU_RONG_BANG, CHIEU_CAO_BANG);

    if (!font_.loadFromFile("assets/font.ttf"))
        throw std::runtime_error("Khong the tai font: assets/font.ttf");

    // Nap anh nen menu
    if (textureNenMenu_.loadFromFile("assets/menu_bg.jpg")) {
        coAnhNen_ = true;
        textureNenMenu_.setSmooth(true);
    } else {
        std::cerr << "[Menu] Khong tim thay assets/menu_bg.jpg\n";
    }

    // Nhạc menu lúc khởi động
    amThanh_.nhacNen().batMenu();

    taoKhoiMoi();
    taoKhoiMoi();
}

// ─── Vòng lặp chính ─────────────────────────────────────────
void TroChoi::chayGame() {
    sf::Clock dongHo;
    float tichLuyRoi = 0.f;

    while (cuaSo_.isOpen()) {
        float thoiGianDelta = dongHo.restart().asMilliseconds();

        xuLySuKien();

        if (trangThai_ == TrangThaiGame::DANG_CHOI) {
            float dt_giay = thoiGianDelta / 1000.f;
            veNenDong_.capNhat(dt_giay);
            bannerLenLevel_.capNhat(dt_giay);
            hieuUngFlash_.capNhat(dt_giay);
            hieuUngManhVo_.capNhat(dt_giay);
            hieuUngChuNoi_.capNhat(dt_giay);

            tichLuyRoi += thoiGianDelta;
            int thoiGianRoi = tinhThoiGianRoi(cauHinhDoKho_, thongKe_.capDo);
            while (tichLuyRoi >= thoiGianRoi) {
                tichLuyRoi -= thoiGianRoi;
                khoiRoiXuong();
            }
        }

        veManHinh();
    }
}

// ─── Xử lý sự kiện ──────────────────────────────────────────
void TroChoi::xuLySuKien() {
    sf::Event suKien;
    while (cuaSo_.pollEvent(suKien)) {
        if (suKien.type == sf::Event::Closed)
            cuaSo_.close();

        // Resize cửa sổ → tính lại layout
        if (suKien.type == sf::Event::Resized) {
            xuLyResize(suKien.size.width, suKien.size.height);
        }

        if (suKien.type == sf::Event::KeyPressed)
            xuLyPhimBam(suKien.key.code);

        // Nhập tên khi game over
        if (trangThai_ == TrangThaiGame::GAME_OVER &&
            suKien.type == sf::Event::TextEntered) {
            if (suKien.text.unicode == '\b') {
                if (!tenNguoiChoi_.empty()) tenNguoiChoi_.pop_back();
            } else if (suKien.text.unicode >= 32 && suKien.text.unicode < 128
                       && tenNguoiChoi_.size() < 12) {
                tenNguoiChoi_ += (char)suKien.text.unicode;
            }
        }
    }
}

// ─── Xử lý resize cửa sổ ────────────────────────────────────
void TroChoi::xuLyResize(int chieuRong, int chieuCao) {
    // Cập nhật viewport
    sf::FloatRect viewport(0.f, 0.f, (float)chieuRong, (float)chieuCao);
    sf::View view(viewport);
    cuaSo_.setView(view);

    // Tính lại layout theo kích thước mới
    tinhLayout(chieuRong, chieuCao);

    // Tái tạo VeNenDong với kích thước mới
    veNenDong_ = VeNenDong(CHIEU_RONG_BANG, CHIEU_CAO_BANG);
}

void TroChoi::xuLyPhimBam(sf::Keyboard::Key phim) {
    // Phím M: tắt/bật nhạc nền mọi lúc
    if (phim == sf::Keyboard::M) {
        amThanh_.nhacNen().batTat();
        return;
    }
    // Phím N: tắt/bật hiệu ứng âm thanh mọi lúc
    if (phim == sf::Keyboard::N) {
        amThanh_.hieuUng().batTat();
        return;
    }

    switch (trangThai_) {
    case TrangThaiGame::MENU:
        if (phim == sf::Keyboard::Enter || phim == sf::Keyboard::Space)
            trangThai_ = TrangThaiGame::CHON_DO_KHO;
        if (phim == sf::Keyboard::H)
            trangThai_ = TrangThaiGame::BANG_DIEM_CAO;
        if (phim == sf::Keyboard::A)  { // Am thanh settings
            trangThaiTruoc_ = TrangThaiGame::MENU;
            trangThai_ = TrangThaiGame::CAI_DAT_AM_THANH;
        }
        break;

    case TrangThaiGame::CHON_DO_KHO:
        if (phim == sf::Keyboard::Up   || phim == sf::Keyboard::W) chuyenDoKho(-1);
        if (phim == sf::Keyboard::Down || phim == sf::Keyboard::S) chuyenDoKho(1);
        if (phim == sf::Keyboard::Num1) mucDoChon_ = MucDoKho::DE;
        if (phim == sf::Keyboard::Num2) mucDoChon_ = MucDoKho::VUA;
        if (phim == sf::Keyboard::Num3) mucDoChon_ = MucDoKho::KHO;
        if (phim == sf::Keyboard::Enter || phim == sf::Keyboard::Space)
            batDauGame();
        if (phim == sf::Keyboard::Escape) trangThai_ = TrangThaiGame::MENU;
        break;

    case TrangThaiGame::DANG_CHOI:
        if (!khoiHienTai_) break;
        if (phim == sf::Keyboard::A || phim == sf::Keyboard::Left)
            diChuyenKhoi(-1);
        if (phim == sf::Keyboard::D || phim == sf::Keyboard::Right)
            diChuyenKhoi(1);
        if (phim == sf::Keyboard::S || phim == sf::Keyboard::Down)
            softDrop();
        if (phim == sf::Keyboard::W || phim == sf::Keyboard::Up)
            xoayKhoi(1);
        if (phim == sf::Keyboard::Q) xoayKhoi(-1);
        if (phim == sf::Keyboard::Space) hardDrop();
        if (phim == sf::Keyboard::P) tamDung();
        if (phim == sf::Keyboard::Escape) {
            trangThai_ = TrangThaiGame::MENU;
        }
        break;

    case TrangThaiGame::TAM_DUNG:
        if (phim == sf::Keyboard::P || phim == sf::Keyboard::Enter)
            trangThai_ = TrangThaiGame::DANG_CHOI;
        if (phim == sf::Keyboard::Escape)
            trangThai_ = TrangThaiGame::MENU;
        break;

    case TrangThaiGame::GAME_OVER:
        // R: Chơi lại ngay
        if (phim == sf::Keyboard::R) {
            quanLyDiem_.themDiem({tenNguoiChoi_, thongKe_.diemSo,
                                  thongKe_.capDo, thongKe_.soHangDaXoa});
            diemKyLuc_ = quanLyDiem_.layDiemCaoNhat();
            batDauGame();  // chơi lại với cùng độ khó
        }
        // Enter hoặc M: quay menu
        if (phim == sf::Keyboard::Enter) {
            quanLyDiem_.themDiem({tenNguoiChoi_, thongKe_.diemSo,
                                  thongKe_.capDo, thongKe_.soHangDaXoa});
            diemKyLuc_ = quanLyDiem_.layDiemCaoNhat();
            trangThai_ = TrangThaiGame::MENU;
            amThanh_.nhacNen().batMenu();
        }
        // Nhập tên (BackSpace)
        break;

    case TrangThaiGame::BANG_DIEM_CAO:
        if (phim == sf::Keyboard::Escape || phim == sf::Keyboard::Enter)
            trangThai_ = TrangThaiGame::MENU;
        break;

    case TrangThaiGame::CAI_DAT_AM_THANH:
        if (phim == sf::Keyboard::Escape || phim == sf::Keyboard::Enter)
            trangThai_ = trangThaiTruoc_;
        // Left/Right: chỉnh âm lượng nhạc nền
        if (phim == sf::Keyboard::Left)
            amThanh_.datAmLuongNhac(amThanh_.layAmLuongNhac() - 10.f);
        if (phim == sf::Keyboard::Right)
            amThanh_.datAmLuongNhac(amThanh_.layAmLuongNhac() + 10.f);
        // Up/Down: chỉnh âm lượng hiệu ứng
        if (phim == sf::Keyboard::Up)
            amThanh_.datAmLuongHieuUng(amThanh_.layAmLuongHieuUng() + 10.f);
        if (phim == sf::Keyboard::Down)
            amThanh_.datAmLuongHieuUng(amThanh_.layAmLuongHieuUng() - 10.f);
        // M: bật/tắt nhạc nền
        if (phim == sf::Keyboard::M) amThanh_.nhacNen().batTat();
        // N: bật/tắt hiệu ứng
        if (phim == sf::Keyboard::N) amThanh_.hieuUng().batTat();
        break;
    }
}

// ─── Logic game ─────────────────────────────────────────────
void TroChoi::batDauGame() {
    cauHinhDoKho_ = layCauHinh(mucDoChon_);
    bangChoi_.xoaBang();
    thongKe_ = ThongKeGame{};
    gocXoay_ = 0;
    tenNguoiChoi_ = "NguoiChoi";
    hieuUngFlash_ = HieuUngHangFlash{};
    hieuUngManhVo_ = HieuUngManhVo{};
    hieuUngChuNoi_ = HieuUngChuNoi{};
    khoiTiepTheo_.reset();
    taoKhoiMoi();
    taoKhoiMoi();
    trangThai_ = TrangThaiGame::DANG_CHOI;
    amThanh_.nhacNen().batGame();  // bật nhạc game
}

void TroChoi::taoKhoiMoi() {
    if (!khoiTiepTheo_) {
        khoiHienTai_ = nhaMay_.taoKhoiTiepTheo(SO_COT / 2 - 1, 0);
        khoiTiepTheo_ = nhaMay_.taoKhoiTiepTheo(SO_COT / 2 - 1, 0);
        gocXoay_ = 0;
        return;
    }
    khoiHienTai_ = std::move(khoiTiepTheo_);
    khoiHienTai_->viTriX = SO_COT / 2 - 1;
    khoiHienTai_->viTriY = 0;
    gocXoay_ = 0;
    khoiTiepTheo_ = nhaMay_.taoKhoiTiepTheo(SO_COT / 2 - 1, 0);
    thongKe_.soKhoiDaRoi++;

    if (bangChoi_.kiemTraVaCham(*khoiHienTai_, gocXoay_)) {
        trangThai_ = TrangThaiGame::GAME_OVER;
        amThanh_.nhacNen().batMenu();  // chuyen ve nhac menu khi game over
        amThanh_.hieuUng().phatGameOver();
    }
}

void TroChoi::diChuyenKhoi(int huong) {
    if (!khoiHienTai_) return;
    if (!bangChoi_.kiemTraVaCham(*khoiHienTai_, gocXoay_, huong, 0))
        khoiHienTai_->viTriX += huong;
}

void TroChoi::xoayKhoi(int huong) {
    if (!khoiHienTai_) return;
    int gocMoi = (gocXoay_ + huong + 4) % 4;
    if (!bangChoi_.kiemTraVaCham(*khoiHienTai_, gocMoi)) {
        gocXoay_ = gocMoi;
        amThanh_.hieuUng().phatXoayKhoi();
        return;
    }
    for (int dich : {1, -1, 2, -2}) {
        khoiHienTai_->viTriX += dich;
        if (!bangChoi_.kiemTraVaCham(*khoiHienTai_, gocMoi)) {
            gocXoay_ = gocMoi;
            amThanh_.hieuUng().phatXoayKhoi();
            return;
        }
        khoiHienTai_->viTriX -= dich;
    }
}

void TroChoi::khoiRoiXuong() {
    if (!khoiHienTai_) return;
    if (!bangChoi_.kiemTraVaCham(*khoiHienTai_, gocXoay_, 0, 1))
        khoiHienTai_->viTriY++;
    else
        khoaBang();
}

void TroChoi::softDrop() {
    if (!khoiHienTai_) return;
    if (!bangChoi_.kiemTraVaCham(*khoiHienTai_, gocXoay_, 0, 1)) {
        khoiHienTai_->viTriY++;
        thongKe_.diemSo += 1;
        amThanh_.hieuUng().phatRoiNhanh();  // âm rơi nhanh khi nhấn S
    } else khoaBang();
}

void TroChoi::hardDrop() {
    if (!khoiHienTai_) return;
    int viTriBong = tinhViTriBong();
    thongKe_.diemSo += 2 * (viTriBong - khoiHienTai_->viTriY);
    khoiHienTai_->viTriY = viTriBong;
    khoaBang();
}

void TroChoi::khoaBang() {
    bangChoi_.datKhoiVaoBang(*khoiHienTai_, gocXoay_);

    // Tìm hàng đầy TRƯỚC khi xóa để tạo hiệu ứng
    auto danhSachHangDay = bangChoi_.layDanhSachHangDay();
    if (!danhSachHangDay.empty()) {
        for (int h : danhSachHangDay) {
            hieuUngFlash_.them(h, khoiHienTai_->layMauSac());
            hieuUngManhVo_.taoCuVo(h, khoiHienTai_->layMauSac(),
                                   SO_COT, KICH_THUOC_O);
        }
    } else {
        amThanh_.hieuUng().phatRoiBlock(); // âm đặt block
    }

    int soHangDaXoa = bangChoi_.xoaHangDay();

    if (soHangDaXoa > 0) {
        thongKe_.combo++;
        thongKe_.soHangDaXoa += soHangDaXoa;

        // Âm thanh xóa hàng
        amThanh_.hieuUng().phatXoaHang(soHangDaXoa);

        // Tính điểm
        static const int BANG_DIEM[5] = {0, 100, 300, 500, 800};
        int diemCoBan = BANG_DIEM[soHangDaXoa] * thongKe_.capDo;
        int diemCombo = (thongKe_.combo > 1) ?
                        thongKe_.combo * 50 * cauHinhDoKho_.heSoCombo : 0;
        thongKe_.diemSo += diemCoBan + diemCombo;

        // Lên level
        int capDoMoi = std::min(
            thongKe_.soHangDaXoa / cauHinhDoKho_.soHangLenLevel + 1, 10);
        if (capDoMoi > thongKe_.capDo) {
            const auto& chuDe = DANH_SACH_CHU_DE[capDoMoi - 1];
            bannerLenLevel_.kichHoat(capDoMoi, chuDe.mauNoiBat, chuDe.tenChuDe);
            amThanh_.hieuUng().phatLenLevel(); // âm lên level
        }
        thongKe_.capDo = capDoMoi;

        // Cập nhật kỷ lục
        if (thongKe_.diemSo > diemKyLuc_) diemKyLuc_ = thongKe_.diemSo;

        // Hiệu ứng chữ nổi
        const std::string tenHangXoa[] = {"","DON","DOI","BA HANG","TETRIS!"};
        const sf::Color mauHangXoa[]   = {{},{150,150,150},{93,202,165},
                                           {255,200,60},{226,75,74}};
        std::string nhanHieu = tenHangXoa[soHangDaXoa];
        sf::Color mauNhan   = mauHangXoa[soHangDaXoa];
        if (thongKe_.combo > 1) {
            nhanHieu = "x" + std::to_string(thongKe_.combo) + " COMBO  " + nhanHieu;
            mauNhan  = sf::Color(226,75,74);
        }
        float viTriY = std::max(20.f, (float)(khoiHienTai_->viTriY * KICH_THUOC_O));
        hieuUngChuNoi_.them(nhanHieu, mauNhan, viTriY,
                            soHangDaXoa == 4 || thongKe_.combo > 2 ? 1.4f : 1.0f);
    } else {
        thongKe_.combo = 0;
    }

    taoKhoiMoi();
}

void TroChoi::tamDung() {
    trangThai_ = TrangThaiGame::TAM_DUNG;
}

int TroChoi::tinhViTriBong() const {
    int viTriY = khoiHienTai_->viTriY;
    while (!bangChoi_.kiemTraVaCham(*khoiHienTai_, gocXoay_, 0,
                                     viTriY - khoiHienTai_->viTriY + 1))
        viTriY++;
    return viTriY;
}

void TroChoi::chuyenDoKho(int huong) {
    int hienTai = (int)mucDoChon_;
    hienTai = (hienTai + huong + 3) % 3;
    mucDoChon_ = (MucDoKho)hienTai;
}

// ─── Vẽ màn hình ────────────────────────────────────────────
void TroChoi::veManHinh() {
    const auto& chuDe = DANH_SACH_CHU_DE[thongKe_.capDo - 1];
    cuaSo_.clear(sf::Color(chuDe.mauNenChinh.r / 3,
                           chuDe.mauNenChinh.g / 3,
                           chuDe.mauNenChinh.b / 3));

    switch (trangThai_) {
    case TrangThaiGame::MENU:         veManMenu();       break;
    case TrangThaiGame::CHON_DO_KHO:  veManChonDoKho();  break;
    case TrangThaiGame::DANG_CHOI:    veManGame();       break;
    case TrangThaiGame::TAM_DUNG:     veManGame(); veManTamDung();  break;
    case TrangThaiGame::GAME_OVER:    veManGame(); veManGameOver(); break;
    case TrangThaiGame::BANG_DIEM_CAO:    veBangDiemCao();       break;
    case TrangThaiGame::CAI_DAT_AM_THANH: veManCaiDatAmThanh(); break;
    }

    cuaSo_.display();
}

void TroChoi::veManGame() {
    // Hình nền động
    veNenDong_.veLen(cuaSo_, thongKe_.capDo, OFFSET_X_BANG, OFFSET_Y_BANG);

    // Bảng chơi
    bangChoi_.veLen(cuaSo_, OFFSET_X_BANG, OFFSET_Y_BANG, KICH_THUOC_O);

    // Bóng mờ (ghost piece)
    if (khoiHienTai_) {
        int yBong = tinhViTriBong();
        auto hinhDang = khoiHienTai_->layHinhDangXoay(gocXoay_);
        veKhoi(hinhDang, khoiHienTai_->viTriX, yBong,
               khoiHienTai_->layMauSac(), 0.12f);
        // Khối hiện tại
        veKhoi(hinhDang, khoiHienTai_->viTriX, khoiHienTai_->viTriY,
               khoiHienTai_->layMauSac(), 1.f);
    }

    // Hiệu ứng
    hieuUngFlash_.veLen(cuaSo_, OFFSET_X_BANG, OFFSET_Y_BANG, KICH_THUOC_O, SO_COT);
    hieuUngManhVo_.veLen(cuaSo_, OFFSET_X_BANG, OFFSET_Y_BANG);
    hieuUngChuNoi_.veLen(cuaSo_, font_, OFFSET_X_BANG, CHIEU_RONG_BANG);
    bannerLenLevel_.veLen(cuaSo_, font_, OFFSET_X_BANG, OFFSET_Y_BANG,
                          CHIEU_RONG_BANG, CHIEU_CAO_BANG);

    // Panel bên phải
    vePanelBenPhai();
}

void TroChoi::veKhoi(const LuoiKhoi& hinhDang, int x, int y,
                     sf::Color mauSac, float alpha) {
    sf::RectangleShape oKhoi({(float)KICH_THUOC_O - 2, (float)KICH_THUOC_O - 2});
    sf::RectangleShape vienSang({(float)KICH_THUOC_O - 4, 4.f});
    mauSac.a = (sf::Uint8)(255 * alpha);

    for (int h = 0; h < (int)hinhDang.size(); h++) {
        for (int c = 0; c < (int)hinhDang[h].size(); c++) {
            if (!hinhDang[h][c] || y + h < 0) continue;
            oKhoi.setFillColor(mauSac);
            oKhoi.setPosition(
                OFFSET_X_BANG + (x+c)*KICH_THUOC_O + 1.f,
                OFFSET_Y_BANG + (y+h)*KICH_THUOC_O + 1.f);
            cuaSo_.draw(oKhoi);
            if (alpha > 0.3f) {
                sf::Color mauSang(255,255,255,(sf::Uint8)(50*alpha));
                vienSang.setFillColor(mauSang);
                vienSang.setPosition(
                    OFFSET_X_BANG + (x+c)*KICH_THUOC_O + 2.f,
                    OFFSET_Y_BANG + (y+h)*KICH_THUOC_O + 2.f);
                cuaSo_.draw(vienSang);
            }
        }
    }
}

void TroChoi::vePanelBenPhai() {
    const auto& chuDe = DANH_SACH_CHU_DE[thongKe_.capDo - 1];
    float px  = (float)OFFSET_X_PANEL;
    float py  = (float)OFFSET_Y_BANG;
    float pw  = (float)CHIEU_RONG_PANEL;
    float ch  = (float)CHIEU_CAO_BANG;   // tổng chiều cao bảng làm chuẩn
    float cso = (float)KICH_THUOC_O;     // để tính font theo ô

    // Font scale theo kích thước ô
    unsigned fLon  = (unsigned)(cso * 0.75f);   // ~27 khi cso=36
    unsigned fVua  = (unsigned)(cso * 0.55f);   // ~20
    unsigned fNho  = (unsigned)(cso * 0.38f);   // ~14
    unsigned fNhoNhat = (unsigned)(cso * 0.30f);

    // Chiều cao mỗi panel tính theo tỉ lệ chiều cao bảng
    float h1 = ch * 0.11f;   // Điểm
    float h2 = ch * 0.09f;   // Kỷ lục
    float h3 = ch * 0.09f;   // Level + Lines
    float h4 = ch * 0.04f;   // Chủ đề
    float h5 = ch * 0.08f;   // Combo
    float h6 = ch * 0.05f;   // Độ khó
    float h7 = ch * 0.14f;   // Tiếp theo
    // Phím bấm: phần còn lại
    float gapY = ch * 0.015f;

    float y0 = py;
    float y1 = y0 + h1 + gapY;
    float y2 = y1 + h2 + gapY;
    float y3 = y2 + h3 + gapY;
    float y4 = y3 + h4 + gapY;
    float y5 = y4 + h5 + gapY;
    float y6 = y5 + h6 + gapY;
    float y7 = y6 + h7 + gapY;

    // ĐIỂM
    vePanel(px, y0, pw, h1);
    veChoChu("DIEM SO", px+14, y0+6, fNhoNhat, sf::Color(150,150,150));
    veChoChu(std::to_string(thongKe_.diemSo), px+14, y0+h1*0.28f, fLon, sf::Color::White, true);

    // KỶ LỤC
    vePanelVienVang(px, y1, pw, h2);
    veChoChu("KY LUC", px+14, y1+6, fNhoNhat, sf::Color(180,145,0));
    veChoChu(std::to_string(diemKyLuc_), px+14, y1+h2*0.28f, fVua, sf::Color(255,215,0), true);

    // LEVEL + LINES
    vePanel(px, y2, pw/2-3, h3);
    veChoChu("LEVEL", px+14, y2+6, fNhoNhat, sf::Color(150,150,150));
    veChoChu(std::to_string(thongKe_.capDo), px+14, y2+h3*0.28f, fVua, chuDe.mauNoiBat, true);
    vePanel(px+pw/2+3, y2, pw/2-3, h3);
    veChoChu("HANG",  px+pw/2+17, y2+6, fNhoNhat, sf::Color(150,150,150));
    veChoChu(std::to_string(thongKe_.soHangDaXoa), px+pw/2+17, y2+h3*0.28f, fVua, sf::Color(93,202,165), true);

    // Tên chủ đề
    vePanelMauNoiBat(px, y3, pw, h4, chuDe.mauNoiBat);
    veChoChu(chuDe.tenChuDe, px+14, y3+h4*0.22f, fNho, chuDe.mauNoiBat, true);

    // COMBO
    vePanel(px, y4, pw, h5);
    veChoChu("COMBO", px+14, y4+6, fNhoNhat, sf::Color(150,150,150));
    if (thongKe_.combo > 1)
        veChoChu("x"+std::to_string(thongKe_.combo), px+14, y4+h5*0.28f, fVua, sf::Color(226,75,74), true);
    else
        veChoChu("--", px+14, y4+h5*0.28f, fVua, sf::Color(50,50,50));

    // ĐỘ KHÓ
    vePanel(px, y5, pw, h6);
    veChoChu("Do kho: " + cauHinhDoKho_.tenHienThi, px+14, y5+h6*0.22f, fNho, sf::Color(170,170,170));

    // KHỐI TIẾP THEO
    vePanel(px, y6, pw, h7);
    veChoChu("TIEP THEO", px+14, y6+6, fNhoNhat, sf::Color(150,150,150));
    if (khoiTiepTheo_) {
        auto hinhDang = khoiTiepTheo_->layHinhDang();
        int oSize = (int)(cso * 0.72f);
        int oX = (int)(px + (pw - hinhDang[0].size()*oSize)/2);
        int oY = (int)(y6 + h7*0.25f + (h7*0.7f - hinhDang.size()*oSize)/2);
        sf::RectangleShape o({(float)oSize-2,(float)oSize-2});
        for (int h=0;h<(int)hinhDang.size();h++)
            for (int c=0;c<(int)hinhDang[h].size();c++)
                if (hinhDang[h][c]) {
                    o.setFillColor(khoiTiepTheo_->layMauSac());
                    o.setPosition((float)(oX+c*oSize+1),(float)(oY+h*oSize+1));
                    cuaSo_.draw(o);
                }
    }

    // PHÍM BẤM
    float yPhim = y7;
    float hPhim = py + ch - yPhim;
    if (hPhim > cso * 1.5f) {
        vePanel(px, yPhim, pw, hPhim);
        veChoChu("PHIM BAM", px+14, yPhim+6, fNhoNhat, sf::Color(150,150,150));
        const std::pair<std::string,std::string> phimBam[] = {
            {"A / D","Di chuyen"},{"W","Xoay phai"},
            {"Q","Xoay trai"},{"S","Roi nhanh"},
            {"Space","Hard drop"},{"P","Tam dung"},
        };
        float buocY = hPhim * 0.13f;
        for (int i = 0; i < 6; i++) {
            veChoChu(phimBam[i].first,  px+14, yPhim+cso*0.55f+i*buocY, fNhoNhat, chuDe.mauNoiBat);
            veChoChu(phimBam[i].second, px+pw*0.35f, yPhim+cso*0.55f+i*buocY, fNhoNhat, sf::Color(190,190,190));
        }
    }
}

// ── Helper: Vẽ nút bo tròn giả (dùng nhiều rect xếp chồng) ──
void TroChoi::veNutDep(float x, float y, float w, float h,
                       const std::string& nhan, const std::string& phim,
                       sf::Color mauNen, sf::Color mauChu) {
    float r = 14.f; // bán kính bo tròn giả

    // ── Bóng đổ ──────────────────────────────────────────────
    sf::RectangleShape bongH({w - r, h + 5.f});
    bongH.setPosition(x + r/2.f, y + 4.f);
    bongH.setFillColor(sf::Color(0,0,0,80));
    cuaSo_.draw(bongH);

    // ── Thân nút (3 rect tạo hình bo tròn) ──────────────────
    // Màu đậm (phần dưới)
    sf::Color mauDam(
        (sf::Uint8)(mauNen.r * 0.70f),
        (sf::Uint8)(mauNen.g * 0.70f),
        (sf::Uint8)(mauNen.b * 0.70f));

    // Rect ngang giữa (phần thân chính)
    sf::RectangleShape giua({w, h - r});
    giua.setPosition(x, y + r/2.f);
    giua.setFillColor(mauDam);
    cuaSo_.draw(giua);

    // Rect dọc trái/phải (che góc vuông)
    sf::RectangleShape trai({w - r, h});
    trai.setPosition(x + r/2.f, y);
    trai.setFillColor(mauDam);
    cuaSo_.draw(trai);

    // ── Shine (nửa trên sáng hơn) ────────────────────────────
    sf::RectangleShape shineG({w, h/2.f - r/2.f});
    shineG.setPosition(x, y + r/2.f);
    shineG.setFillColor(mauNen);
    cuaSo_.draw(shineG);

    sf::RectangleShape shineT({w - r, h/2.f});
    shineT.setPosition(x + r/2.f, y);
    shineT.setFillColor(mauNen);
    cuaSo_.draw(shineT);

    // ── Viền trắng mờ trên (highlight) ──────────────────────
    sf::RectangleShape hi({w - r, 2.5f});
    hi.setPosition(x + r/2.f, y + 2.f);
    hi.setFillColor(sf::Color(255,255,255,80));
    cuaSo_.draw(hi);

    // ── Viền ngoài mỏng ──────────────────────────────────────
    sf::Color mauVien(
        (sf::Uint8)std::min(255, (int)mauNen.r + 40),
        (sf::Uint8)std::min(255, (int)mauNen.g + 40),
        (sf::Uint8)std::min(255, (int)mauNen.b + 40));
    // 4 cạnh viền
    auto veVien = [&](float vx, float vy, float vw, float vh) {
        sf::RectangleShape v({vw, vh});
        v.setPosition(vx, vy);
        v.setFillColor(mauVien);
        cuaSo_.draw(v);
    };
    veVien(x + r/2.f, y,           w - r, 1.5f);          // trên
    veVien(x + r/2.f, y + h - 1.5f, w - r, 1.5f);        // dưới
    veVien(x,         y + r/2.f,   1.5f,  h - r);         // trái
    veVien(x + w - 1.5f, y + r/2.f, 1.5f, h - r);        // phải

    // ── Chữ căn giữa nút ────────────────────────────────────
    {
        sf::Text txt;
        txt.setFont(font_);
        txt.setString(nhan);
        txt.setCharacterSize(22);
        txt.setStyle(sf::Text::Bold);
        txt.setFillColor(mauChu);
        sf::FloatRect rb = txt.getLocalBounds();
        txt.setOrigin(rb.left + rb.width/2.f, rb.top + rb.height/2.f);
        txt.setPosition(x + w/2.f, y + h/2.f - 2.f);
        cuaSo_.draw(txt);
    }

    // ── Phím tắt nhỏ ────────────────────────────────────────
    if (!phim.empty()) {
        veChoChu(phim, x + w - (float)phim.size() * 7.f - 8.f,
                 y + h - 16.f, 11, sf::Color(255,255,255,120));
    }
}

void TroChoi::veManMenu() {
    amThanh_.nhacNen().batMenu();
    cuaSo_.clear(sf::Color(10, 15, 40));

    float W  = (float)CHIEU_RONG_CUA_SO;
    float H  = (float)CHIEU_CAO_CUA_SO;
    float cx = W / 2.f;

    // ── Ảnh nền kiểu "cover": giữ tỉ lệ, crop giữa ─────────
    if (coAnhNen_) {
        textureNenMenu_.setRepeated(false);
        spriteNenMenu_.setTexture(textureNenMenu_);
        sf::Vector2u texSize = textureNenMenu_.getSize();
        float scaleX = W / (float)texSize.x;
        float scaleY = H / (float)texSize.y;
        float scale  = std::max(scaleX, scaleY); // cover: lấy scale lớn hơn
        spriteNenMenu_.setScale(scale, scale);
        // Căn trên để giữ chữ TETRIS không bị crop
        float scaledW = texSize.x * scale;
        spriteNenMenu_.setPosition((W - scaledW) / 2.f, 0.f);
        spriteNenMenu_.setTextureRect(
            sf::IntRect(0, 0, (int)texSize.x, (int)texSize.y));
        cuaSo_.draw(spriteNenMenu_);
    }

    // Overlay nhẹ để nút dễ đọc (không làm mờ ảnh)
    sf::RectangleShape overlay({W, H});
    overlay.setFillColor(sf::Color(0, 0, 0, 60));
    cuaSo_.draw(overlay);

    // ── Panel nút bấm — nhỏ gọn, bên dưới chữ TETRIS ────────
    float nutW  = W * 0.18f;
    float nutH  = H * 0.07f;
    float nutGap = H * 0.015f;
    float panW  = nutW + 24.f;
    float panH  = 3 * nutH + 2 * nutGap + 20.f;
    float panX  = cx - panW / 2.f;
    float panY  = H * 0.68f;

    float nx  = panX + 12.f;
    float ny0 = panY + 10.f;

    // 3 nút dọc (không có panel nền tối)
    veNutDep(nx, ny0,                        nutW, nutH,
             "CHOI GAME",    "[Enter]", sf::Color(41,128,185),  sf::Color::White);
    veNutDep(nx, ny0 + nutH + nutGap,        nutW, nutH,
             "XEP HANG",     "[H]",     sf::Color(211,84,0),    sf::Color::White);
    veNutDep(nx, ny0 + (nutH+nutGap)*2,      nutW, nutH,
             "CAI DAT AM THANH", "[A]", sf::Color(39,174,96),   sf::Color::White);

    // ── Kỷ lục ───────────────────────────────────────────────
    if (diemKyLuc_ > 0) {
        float kyY = panY + panH + 14.f;
        sf::RectangleShape kyBox({panW, 36.f});
        kyBox.setPosition(panX, kyY);
        kyBox.setFillColor(sf::Color(40,28,0,200));
        kyBox.setOutlineColor(sf::Color(200,160,0,100));
        kyBox.setOutlineThickness(1.f);
        cuaSo_.draw(kyBox);
        veChuGiua("KY LUC:  " + std::to_string(diemKyLuc_),
                  kyY + 8.f, 18, sf::Color(255,215,0));
    }

    // ── Footer ───────────────────────────────────────────────
    veChuGiua("WASD / Arrows  |  Space  |  P", H - 26.f, 13, sf::Color(120,120,140,180));
}

void TroChoi::veManChonDoKho() {
    cuaSo_.clear(sf::Color(8,8,15));
    float giuaX = CHIEU_RONG_CUA_SO / 2.f;
    float giuaY = CHIEU_CAO_CUA_SO  / 2.f;

    // Kích thước hộp theo màn hình
    float hopW = std::min(700.f, (float)CHIEU_RONG_CUA_SO * 0.45f);
    float hopH = (float)CHIEU_CAO_CUA_SO * 0.15f;
    float gap  = (float)CHIEU_CAO_CUA_SO * 0.02f;
    float tongH = 3 * hopH + 2 * gap;
    float startY = giuaY - tongH / 2.f;

    // Tiêu đề
    veChuGiua("CHON DO KHO", startY - CHIEU_CAO_CUA_SO * 0.1f, 48, sf::Color(255,200,60));
    veChuGiua("W/S hoac 1-2-3 de chon, Enter de bat dau",
              startY - CHIEU_CAO_CUA_SO * 0.04f, 20, sf::Color(90,90,90));

    const MucDoKho cacMuc[3] = {MucDoKho::DE, MucDoKho::VUA, MucDoKho::KHO};
    const sf::Color mauMuc[3] = {{93,202,165},{255,200,60},{226,75,74}};

    for (int i = 0; i < 3; i++) {
        auto ch = layCauHinh(cacMuc[i]);
        bool duocChon = (cacMuc[i] == mucDoChon_);
        float bx = giuaX - hopW / 2.f;
        float by = startY + i * (hopH + gap);

        sf::RectangleShape hop({hopW, hopH});
        hop.setPosition(bx, by);
        sf::Color mauVien = duocChon ? mauMuc[i] : sf::Color(45,45,65);
        sf::Color mauNen  = duocChon ?
            sf::Color(mauMuc[i].r,mauMuc[i].g,mauMuc[i].b,35) :
            sf::Color(18,18,30,200);
        hop.setFillColor(mauNen);
        hop.setOutlineColor(mauVien);
        hop.setOutlineThickness(duocChon ? 3.f : 1.f);
        cuaSo_.draw(hop);

        // Font theo hopH
        unsigned fSo  = (unsigned)(hopH * 0.18f);
        unsigned fTen = (unsigned)(hopH * 0.40f);
        unsigned fMoTa= (unsigned)(hopH * 0.17f);

        veChoChu("["+std::to_string(i+1)+"]", bx+18, by+hopH*0.08f, fSo, sf::Color(90,90,90));
        veChoChu(ch.tenHienThi, bx+22, by+hopH*0.25f, fTen,
                 duocChon ? mauMuc[i] : sf::Color(150,150,150), true);
        veChoChu(ch.moTa, bx+22, by+hopH*0.72f, fMoTa, sf::Color(110,110,110));
    }
    veChuGiua("[Esc] Quay lai", (float)CHIEU_CAO_CUA_SO - 40, 18, sf::Color(70,70,70));
}

void TroChoi::veManTamDung() {
    float hopW = 380.f, hopH = 140.f;
    float bx = (CHIEU_RONG_CUA_SO - hopW)/2.f;
    float by = (CHIEU_CAO_CUA_SO  - hopH)/2.f;
    veHopOverlay(bx, by, hopW, hopH, sf::Color(0,0,0,200));
    veChuGiua("TAM DUNG", by + 30, 36, sf::Color(255,200,60));
    veChuGiua("[P] Tiep tuc   [Esc] Menu", by + 85, 20);
}

void TroChoi::veManGameOver() {
    float W = (float)CHIEU_RONG_CUA_SO;
    float H = (float)CHIEU_CAO_CUA_SO;
    float hopW = W * 0.38f;
    float hopH = H * 0.52f;
    float fS = hopW / 500.f;
    float bx = (W - hopW) / 2.f;
    float by = (H - hopH) / 2.f;

    // Nền mờ tối
    veHopOverlay(bx - 4, by - 4, hopW + 8, hopH + 8, sf::Color(0,0,0,170));

    // Hộp chính với gradient viền
    sf::RectangleShape hop({hopW, hopH});
    hop.setPosition(bx, by);
    hop.setFillColor(sf::Color(10, 8, 18, 250));
    hop.setOutlineColor(sf::Color(226, 75, 74));
    hop.setOutlineThickness(2.5f);
    cuaSo_.draw(hop);

    // Dải màu đỏ trên cùng
    sf::RectangleShape dai({hopW, 6.f});
    dai.setPosition(bx, by);
    dai.setFillColor(sf::Color(226, 75, 74));
    cuaSo_.draw(dai);

    // ── GAME OVER ──
    veChuGiua("GAME OVER", by + 16.f * fS, (int)(44 * fS), sf::Color(226, 75, 74));

    // ── Điểm số ──
    {
        veChuGiua("DIEM SO", by + 68.f * fS, (int)(14 * fS), sf::Color(120,120,140));
        veChuGiua(std::to_string(thongKe_.diemSo), by + 86.f * fS, (int)(46 * fS), sf::Color::White);
    }

    // ── Kỷ lục mới ──
    if (thongKe_.diemSo >= diemKyLuc_ && thongKe_.diemSo > 0) {
        float kyY = by + 140.f * fS;
        sf::RectangleShape kyBox({hopW - 80.f * fS, 30.f * fS});
        kyBox.setPosition(bx + 40.f * fS, kyY);
        kyBox.setFillColor(sf::Color(60,45,0,200));
        kyBox.setOutlineColor(sf::Color(255,215,0,150));
        kyBox.setOutlineThickness(1.f);
        cuaSo_.draw(kyBox);
        veChuGiua("* KY LUC MOI! *", kyY + 6.f * fS, (int)(16 * fS), sf::Color(255,215,0));
    }

    // ── Thống kê ──
    float tkY = by + 178.f * fS;
    veChuGiua("Level " + std::to_string(thongKe_.capDo) +
              "   |   " + std::to_string(thongKe_.soHangDaXoa) + " hang",
              tkY, (int)(15 * fS), sf::Color(140,140,160));

    // ── Nhập tên ──
    float tenY = tkY + 28.f * fS;
    {
        float tenBoxW = hopW - 80.f * fS;
        sf::RectangleShape tenBox({tenBoxW, 32.f * fS});
        tenBox.setPosition(bx + 40.f * fS, tenY);
        tenBox.setFillColor(sf::Color(20,20,40));
        tenBox.setOutlineColor(sf::Color(255,200,60,160));
        tenBox.setOutlineThickness(1.f);
        cuaSo_.draw(tenBox);
        veChoChu("Ten: " + tenNguoiChoi_ + "_", bx + 52.f * fS, tenY + 7.f * fS,
                 (int)(15 * fS), sf::Color(255,200,60));
    }

    // ── Đường kẻ ──
    sf::RectangleShape ke({hopW - 50.f * fS, 1.f});
    ke.setFillColor(sf::Color(60,60,85));
    ke.setPosition(bx + 25.f * fS, tenY + 44.f * fS);
    cuaSo_.draw(ke);

    // ── 2 nút ──
    float nutH = 54.f * fS;
    float nutY = tenY + 54.f * fS;

    float gapGo = 12.f * fS;
    float nutWGo = (hopW - 50.f * fS - gapGo) / 2.f;
    veNutDep(bx + 15.f * fS,                    nutY, nutWGo, nutH,
             "CHOI LAI", "[R]",     sf::Color(39,174,96),  sf::Color::White);
    veNutDep(bx + 15.f * fS + nutWGo + gapGo,  nutY, nutWGo, nutH,
             "MENU",     "[Enter]", sf::Color(41,128,185), sf::Color::White);
}

void TroChoi::veBangDiemCao() {
    cuaSo_.clear(sf::Color(8,8,15));
    veChuGiua("BANG DIEM CAO", 55, 32, sf::Color(255,215,0));
    auto& danhSach = quanLyDiem_.layDanhSach();
    if (danhSach.empty()) {
        veChuGiua("Chua co diem nao duoc luu.", 220, 20, sf::Color(100,100,100));
    } else {
        for (int i = 0; i < (int)danhSach.size(); i++) {
            sf::Color mau = (i==0)?sf::Color(255,215,0):(i==1)?
                sf::Color(200,200,200):sf::Color(175,135,75);
            veChuGiua(std::to_string(i+1)+".  "+danhSach[i].tenNguoiChoi+
                      "   "+std::to_string(danhSach[i].diemSo)+
                      "  (Lv"+std::to_string(danhSach[i].capDo)+")",
                      110.f+i*48, i<3?20:17, mau);
        }
    }
    veChuGiua("[Esc] Quay lai", 680, 14, sf::Color(80,80,80));
}

// ─── Helpers vẽ ─────────────────────────────────────────────
void TroChoi::veChoChu(const std::string& chuoi, float x, float y,
                        unsigned int coDoChu, sf::Color mauSac, bool inDam) {
    sf::Text vanBan;
    vanBan.setFont(font_);
    vanBan.setString(chuoi);
    vanBan.setCharacterSize(coDoChu);
    vanBan.setFillColor(mauSac);
    if (inDam) vanBan.setStyle(sf::Text::Bold);
    vanBan.setPosition(x, y);
    cuaSo_.draw(vanBan);
}

void TroChoi::veChuGiua(const std::string& chuoi, float y,
                          unsigned int coDoChu, sf::Color mauSac) {
    sf::Text vanBan;
    vanBan.setFont(font_);
    vanBan.setString(chuoi);
    vanBan.setCharacterSize(coDoChu);
    vanBan.setFillColor(mauSac);
    float chieuRongChu = vanBan.getLocalBounds().width;
    vanBan.setPosition((CHIEU_RONG_CUA_SO - chieuRongChu) / 2.f, y);
    cuaSo_.draw(vanBan);
}

void TroChoi::vePanel(float x, float y, float w, float h) {
    sf::RectangleShape panel({w, h});
    panel.setPosition(x, y);
    panel.setFillColor(sf::Color(20, 20, 35, 225));
    panel.setOutlineColor(sf::Color(55, 55, 75));
    panel.setOutlineThickness(1.f);
    cuaSo_.draw(panel);
}

void TroChoi::vePanelVienVang(float x, float y, float w, float h) {
    sf::RectangleShape panel({w, h});
    panel.setPosition(x, y);
    panel.setFillColor(sf::Color(30, 22, 0, 220));
    panel.setOutlineColor(sf::Color(160, 120, 0, 180));
    panel.setOutlineThickness(1.f);
    cuaSo_.draw(panel);
}

void TroChoi::vePanelMauNoiBat(float x, float y, float w, float h, sf::Color mau) {
    sf::RectangleShape panel({w, h});
    panel.setPosition(x, y);
    panel.setFillColor(sf::Color(mau.r, mau.g, mau.b, 28));
    panel.setOutlineColor(sf::Color(mau.r, mau.g, mau.b, 90));
    panel.setOutlineThickness(1.f);
    cuaSo_.draw(panel);
}

void TroChoi::veHopOverlay(float x, float y, float w, float h, sf::Color mauNen) {
    sf::RectangleShape hop({w, h});
    hop.setPosition(x, y);
    hop.setFillColor(mauNen);
    cuaSo_.draw(hop);
}

// ─── Vẽ thanh trượt âm lượng ────────────────────────────────
void TroChoi::veThanhTruot(float x, float y, float w, float h,
                           float giaTri, float toiDa, sf::Color mauThanh)
{
    // Nền thanh
    sf::RectangleShape nen({w, h});
    nen.setPosition(x, y);
    nen.setFillColor(sf::Color(40, 40, 60));
    nen.setOutlineColor(sf::Color(80, 80, 100));
    nen.setOutlineThickness(1.f);
    cuaSo_.draw(nen);

    // Phần đã điền
    float tyLe = std::clamp(giaTri / toiDa, 0.f, 1.f);
    if (tyLe > 0.f) {
        sf::RectangleShape dien({w * tyLe, h});
        dien.setPosition(x, y);
        sf::Color mauDien = mauThanh;
        dien.setFillColor(mauDien);
        cuaSo_.draw(dien);
    }

    // Nút kéo
    float nutX = x + w * tyLe - h / 2.f;
    sf::RectangleShape nut({h, h * 1.4f});
    nut.setPosition(nutX, y - h * 0.2f);
    nut.setFillColor(sf::Color::White);
    nut.setOutlineColor(mauThanh);
    nut.setOutlineThickness(1.5f);
    cuaSo_.draw(nut);
}

// ─── Màn cài đặt âm thanh ───────────────────────────────────
void TroChoi::veManCaiDatAmThanh() {
    cuaSo_.clear(sf::Color(5, 5, 12));

    float W    = (float)CHIEU_RONG_CUA_SO;
    float H    = (float)CHIEU_CAO_CUA_SO;
    float cx   = W / 2.f;
    float cy   = H / 2.f;
    float hopW = W * 0.52f;
    float hopH = H * 0.62f;
    float bx   = cx - hopW / 2.f;
    float by   = cy - hopH / 2.f;

    float fScale = hopW / 560.f; // hệ số scale font/spacing theo hộp

    // Hộp nền
    sf::RectangleShape hop({hopW, hopH});
    hop.setPosition(bx, by);
    hop.setFillColor(sf::Color(12, 12, 26, 245));
    hop.setOutlineColor(sf::Color(93, 202, 165));
    hop.setOutlineThickness(2.f);
    cuaSo_.draw(hop);

    // Tiêu đề
    veChuGiua("CAI DAT AM THANH", by + 18.f * fScale, (int)(26 * fScale), sf::Color(93, 202, 165));

    sf::RectangleShape ke({hopW - 40.f, 1.f});
    ke.setFillColor(sf::Color(50, 50, 80));

    float nhanX  = bx + 24.f * fScale;
    float thanhX = bx + hopW * 0.40f;
    float thanhW = hopW * 0.50f;
    float thanhH = 13.f * fScale;
    float pctX   = thanhX + thanhW + 8.f;

    // Hàm vẽ 1 hàng điều chỉnh âm thanh
    auto veHang = [&](float y, const std::string& nhan, bool tatAm,
                      float amLuong, sf::Color mauOn,
                      const std::string& phimTat, const std::string& phimChinh) {
        ke.setPosition(bx + 20.f, y - 6.f);
        cuaSo_.draw(ke);

        sf::Color mauChu = tatAm ? sf::Color(80,80,80) : mauOn;

        veChoChu(nhan, nhanX, y + 2.f, (int)(16 * fScale), mauChu, true);

        float nutW = 60.f * fScale, nutH = 24.f * fScale;
        float nutX = nhanX, nutY = y + 26.f * fScale;
        sf::RectangleShape nut({nutW, nutH});
        nut.setPosition(nutX, nutY);
        nut.setFillColor(tatAm ? sf::Color(55,20,20) : sf::Color(20,55,30));
        nut.setOutlineColor(tatAm ? sf::Color(160,50,50) : sf::Color(50,180,70));
        nut.setOutlineThickness(1.5f);
        cuaSo_.draw(nut);
        veChoChu(tatAm ? "TAT" : "BAT", nutX + 10.f * fScale, nutY + 4.f * fScale, (int)(13 * fScale),
                 tatAm ? sf::Color(190,60,60) : sf::Color(50,200,70), true);
        veChoChu(phimTat, nutX + nutW + 6.f, nutY + 5.f * fScale, (int)(11 * fScale), sf::Color(100,100,100));

        float sY = y + 8.f * fScale;
        veThanhTruot(thanhX, sY, thanhW, thanhH, amLuong, 100.f,
                     tatAm ? sf::Color(55,55,55) : mauOn);
        veChoChu(std::to_string((int)amLuong) + "%", pctX, sY - 1.f, (int)(15 * fScale),
                 tatAm ? sf::Color(75,75,75) : mauOn);
        veChoChu(phimChinh, thanhX, sY + thanhH + 5.f, (int)(11 * fScale), sf::Color(65,65,95));
    };

    veHang(by + 62.f * fScale,  "NHAC NEN",         amThanh_.nhacNen().layTatAm(),
           amThanh_.layAmLuongNhac(),   sf::Color(255,200,60),
           "[M]", "[<-] giam  [->] tang");

    veHang(by + 175.f * fScale, "HIEU UNG AM THANH", amThanh_.hieuUng().layTatAm(),
           amThanh_.layAmLuongHieuUng(), sf::Color(93,202,165),
           "[N]", "[Up] giam  [Dn] tang");

    ke.setPosition(bx + 20.f, by + 288.f * fScale);
    cuaSo_.draw(ke);

    struct { const char* p; const char* m; } hd[] = {
        {"[<-] [->]", "Am luong nhac nen"},
        {"[Up] [Dn]", "Am luong hieu ung"},
        {"[M] / [N]", "Bat / tat tung kenh"},
        {"[Enter]",   "Quay lai"},
    };
    float hdY = by + 300.f * fScale;
    float hdBuoc = (hopH - (hdY - by) - 16.f) / 4.f;
    for (int i = 0; i < 4; i++) {
        float yi = hdY + i * hdBuoc;
        veChoChu(hd[i].p, nhanX + 6.f,        yi, (int)(13 * fScale), sf::Color(255,200,60));
        veChoChu(hd[i].m, nhanX + 140.f * fScale, yi, (int)(13 * fScale), sf::Color(160,160,160));
    }

    veChuGiua("[Enter / Esc]  Quay lai", by + hopH + 12.f, (int)(14 * fScale), sf::Color(60,60,85));
}

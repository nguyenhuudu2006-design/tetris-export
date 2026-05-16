# Hướng Dẫn Thêm File Âm Thanh

Đặt file âm thanh vào thư mục: `assets/sounds/`

---

## Tên file cần có

| Tên file | Dùng khi nào |
|---|---|
| `nhac_nen.ogg` | Nhạc nền (loop liên tục) |
| `roi_block.wav` | Khối rơi xuống đặt vào bảng |
| `xoay.wav` | Xoay khối |
| `xoa_hang.wav` | Xóa 1, 2 hoặc 3 hàng |
| `tetris.wav` | Xóa 4 hàng cùng lúc (Tetris!) |
| `len_level.wav` | Lên level |
| `no_block.wav` | Hiệu ứng nổ khi xóa hàng |
| `game_over.wav` | Game kết thúc |

---

## Định dạng hỗ trợ

SFML hỗ trợ: **WAV**, **OGG**, **FLAC**
- Không hỗ trợ MP3 trực tiếp
- Khuyên dùng **OGG** cho nhạc nền (file nhỏ, chất lượng tốt)
- Khuyên dùng **WAV** cho hiệu ứng ngắn (load nhanh)

---

## Tìm file âm thanh miễn phí

### Hiệu ứng âm thanh (SFX)
- **freesound.org** — tìm "block drop", "game over", "level up"
- **zapsplat.com** — tìm "retro game sound"
- **mixkit.co/free-sound-effects** — có sẵn game SFX

### Nhạc nền
- **opengameart.org** — tìm "tetris music" hoặc "chiptune"
- **incompetech.com** — nhạc Kevin MacLeod miễn phí
- **freemusicarchive.org** — chiptune 8-bit

---

## Lưu ý

- Game vẫn chạy bình thường nếu thiếu file — chỉ không có âm thanh
- Mỗi lần thêm file mới không cần build lại — chỉ cần chạy lại `.exe`
- Có thể đổi tên file khác nhưng phải sửa lại trong `AmThanh.h`

---

## Cấu trúc thư mục

```
assets/
├── font.ttf
└── sounds/
    ├── nhac_nen.ogg       ← nhạc nền
    ├── roi_block.wav
    ├── xoay.wav
    ├── xoa_hang.wav
    ├── tetris.wav
    ├── len_level.wav
    ├── no_block.wav
    └── game_over.wav
```

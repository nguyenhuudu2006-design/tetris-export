cmake_minimum_required(VERSION 3.14)
project(TetrisKTLT CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows: trỏ vào SFML nếu dùng thủ công
# set(SFML_DIR "C:/SFML/lib/cmake/SFML")

find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED)

# ── Game chính ────────────────────────────────────────────────
add_executable(tetris
    src/main.cpp
    src/BangChoi.cpp
    src/QuanLyDiem.cpp
    src/HieuUng.cpp
    src/TroChoi.cpp
)
target_include_directories(tetris PRIVATE src)
target_link_libraries(tetris PRIVATE
    sfml-graphics sfml-window sfml-system sfml-audio
)
target_compile_options(tetris PRIVATE -Wall)

# ── Copy assets vào thư mục build ────────────────────────────
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

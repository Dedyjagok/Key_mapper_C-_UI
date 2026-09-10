<div align="center">

# ⌨️ Key Remapper (Broken Ctrl Fix)
### *Aplikasi Windows Native C++ Portabel untuk Mengatasi Tombol Ctrl Rusak pada Laptop (Acer Predator)*

[![Platform](https://img.shields.io/badge/Platform-Windows%2010%20%7C%20Windows%2011-0078D6?style=for-the-badge&logo=windows)](https://microsoft.com)
[![Language](https://img.shields.io/badge/Language-C%2B%2B20%20(Win32%20API)-00599C?style=for-the-badge&logo=c%2B%2B)](https://isocpp.org)
[![Build](https://img.shields.io/badge/Build-Single%20Portable%20Exe%20(~230%20KB)-brightgreen?style=for-the-badge)](https://github.com/Dedyjagok)
[![Author](https://img.shields.io/badge/Author-Dedyjagok-blue?style=for-the-badge&logo=github)](https://github.com/Dedyjagok)
[![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)](LICENSE)

<br/>

<p align="center">
  <img src="Assets/Icon.png" alt="Key Remapper Logo" width="128" height="128" />
</p>

**Key Remapper** adalah aplikasi desktop Windows mandiri (100% pure C++ Win32) yang dikembangkan secara independen oleh **[Dedyjagok](https://github.com/Dedyjagok)**. Aplikasi ini dirancang khusus untuk mengatasi kerusakan hardware keyboard laptop (terutama tombol **Left Ctrl** yang sering rusak pada laptop gaming seperti **Acer Predator**, ASUS ROG, Lenovo Legion, dsb.) dengan memanfaatkan tombol fisik **Copilot** Windows 11 atau tombol keyboard lainnya menjadi **Ctrl** secara mulus tanpa latency.

</div>

---

## 📌 Latar Belakang & Masalah

Pada banyak seri laptop modern (seperti Acer Predator Helios / Triton), keyboard internal sering kali mengalami kerusakan mekanik atau switch pada tombol **Left Ctrl**. Sementara itu, laptop generasi Windows 11 dilengkapi dengan tombol fisik **Copilot** di jajaran bawah keyboard yang sangat jarang dimanfaatkan.

Aplikasi ini hadir sebagai solusi biner mandiri yang:
- **100% Native C++**: Tidak memerlukan runtime eksternal, interpreter, atau instalasi software pihak ketiga apa pun.
- **Portabel**: Cukup unduh satu file executable (`KeyRemapper.exe`) dan langsung jalankan di laptop mana saja.

---

## 🔬 Rekayasa Intercept Hardware Tombol Copilot

Pada level hardware dan keyboard controller Windows 11, tombol fisik **Copilot** memancarkan tiga keystroke secara berurutan dalam hitungan milidetik:
```
VK_LWIN (0x5B)  +  VK_LSHIFT (0xA0)  +  VK_F23 (0x86)
```

### Mengapa Software Remap Biasa Gagal?
Jika software remap hanya memetakan `F23` ke `Ctrl`, penekanan tombol kombinasi seperti `Copilot + C` akan dibaca oleh sistem operasi sebagai:
`Win + Shift + Ctrl + C` (bukan copy teks biasa). Selain itu, ketika tombol Copilot dilepas, pelepasan `Win` dapat memicu Start Menu Windows terbuka secara tidak sengaja.

### Solusi Kernel-Hook C++ yang Diterapkan:
1. **Low-Level Keyboard Hook (`WH_KEYBOARD_LL`)**: Menangkap event `VK_F23` sebelum sampai ke subsistem Windows.
2. **Modifier Masking & Release**:
   - Saat `F23 KeyDown`: Aplikasi secara logis langsung mengirim event `KeyUp` untuk `VK_LWIN` dan `VK_LSHIFT`, mengirim unmasking code (`0xE8`), lalu menahan `LCtrl KeyDown`.
   - Saat `F23 KeyUp`: Aplikasi melepaskan `LCtrl KeyUp` dan menyaring trailing hardware event `Win/Shift Up` dalam jendela waktu 150ms agar Start Menu tidak pernah terpicu.
3. **Hasil**: Semua kombinasi shortcut penting (`Ctrl+C`, `Ctrl+V`, `Ctrl+Z`, `Ctrl+A`, `Ctrl+Shift+Esc`, `Ctrl+T`, dsb.) berfungsi 100% normal dan instan layaknya tombol Ctrl bawaan pabrik.

---

## ✨ Fitur Lengkap Aplikasi

- 🚀 **100% Pure C++ & Mandiri**: Tanpa dependensi pihak ketiga, framework berat, atau instalasi runtime. Ukuran file hanya ~230 KB (termasuk ikon multi-resolusi 16x16 hingga 256x256 pixel).
- 🎛️ **Antarmuka Grafis (Win32 GUI)**: Desain modern yang bersih, ringan, dan responsif dengan dukungan Common Controls v6 serta High-DPI scaling.
- 🎯 **Preset Bawaan**:
  - `Copilot Key -> Left Ctrl` (Langsung aktif sejak pertama kali dibuka).
  - `Caps Lock -> Left Ctrl` (Pilihan populer untuk kemudahan mengetik / coding).
- 🔍 **Fitur Deteksi Tombol Interaktif ("Deteksi...")**:
  - Ingin memetakan tombol lain? Cukup klik tombol **Deteksi...** dan tekan tombol fisik apa saja di keyboard Anda.
  - Pilihan ComboBox akan **otomatis memilih tombol yang baru saja Anda tekan**.
- 🕒 **Run on Windows Startup**:
  - Cukup centang opsi *"Jalankan Otomatis saat Windows Menyala"* di UI.
  - Aplikasi otomatis mendaftarkan dirinya ke registry Windows pengguna (`HKCU\...\Run`), aktif tiap booting tanpa perlu hak akses administrator.
- 🔕 **Minimize ke System Tray**:
  - Jendela dapat disembunyikan ke area notifikasi taskbar (dekat jam) agar tidak mengganggu layar kerja atau gaming.
  - Menu klik kanan tray: Buka UI, Toggle On/Off Remapper, Akses Profil GitHub, dan Keluar.
- 💾 **Konfigurasi Lokal (`key_remapper.ini`)**:
  - Semua pengaturan dan aturan remap kustom disimpan otomatis di file `.ini` lokal portable.

---

## 🚀 Cara Menjalankan

1. Salin atau unduh file **`KeyRemapper.exe`**.
2. Klik dua kali untuk membuka.
3. Aturan **Copilot Key -> Left Ctrl** sudah otomatis aktif.
4. **Tes Fungsi**:
   - Tahan tombol **Copilot** di keyboard Anda, lalu tekan **C** (Copy).
   - Tahan tombol **Copilot**, lalu tekan **V** (Paste).
5. Klik tanda silang **(X)** atau tombol **Sembunyikan ke Tray** untuk membiarkannya berjalan tenang di background.

---

## 🛠️ Kompilasi dari Source Code

Jika ingin melakukan kompilasi mandiri dari kode sumber (`main.cpp`):

### Prasyarat:
- Kompiler GCC / MinGW-w64 (`g++` dan `windres`).

### Perintah Build:
Cukup jalankan file batch:
```cmd
build.bat
```
Atau ketikkan perintah berikut di Command Prompt / Terminal:
```cmd
windres resource.rc -o resource.o
g++ -O2 -mwindows main.cpp resource.o -o KeyRemapper.exe -lcomctl32 -lshell32 -luser32 -ladvapi32
del resource.o
```

---

## 📂 Struktur Proyek

```
AutoHotKey_keybind_swap/
├── Assets/
│   ├── Icon.png         # Logo grafis asli aplikasi
│   └── app.ico          # Multi-resolution icon Windows (16x16 - 256x256)
├── app.manifest         # Windows Application Manifest (DPI Aware & Common Controls v6)
├── resource.rc          # Windows Resource Script (Ikon & Manifest)
├── main.cpp             # Source Code Lengkap (C++ Win32 GUI & Kernel Hook)
├── build.bat            # Script otomatis untuk build executable
├── key_remapper.ini     # File konfigurasi portable (otomatis dibuat)
├── KeyRemapper.exe      # Executable mandiri (~230 KB)
├── LICENSE              # Lisensi resmi MIT
└── README.md            # Dokumentasi lengkap proyek
```

---

## 👤 Author & Developer

- **Developer**: **[Dedyjagok](https://github.com/Dedyjagok)**
  - GitHub: [@Dedyjagok](https://github.com/Dedyjagok)
  - Proyek: Solusi Keyboard Hardware Remapping untuk Laptop Acer Predator & Windows 11.

---

## 📄 Lisensi

Proyek ini dirilis di bawah lisensi [MIT License](LICENSE). Bebas digunakan, dimodifikasi, dan didistribusikan untuk keperluan pribadi maupun komersial oleh siapa pun.

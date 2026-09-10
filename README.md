<div align="center">

# ⌨️ Key Remapper (Broken Ctrl Fix)
### *Aplikasi Windows Native C++ Portabel untuk Menyelamatkan Tombol Ctrl yang "Pensiun Dini" (Khususnya Korban Laptop Acer Predator)*

[![Platform](https://img.shields.io/badge/Platform-Windows%2010%20%7C%20Windows%2011-0078D6?style=for-the-badge&logo=windows)](https://microsoft.com)
[![Language](https://img.shields.io/badge/Language-C%2B%2B20%20(Win32%20API)-00599C?style=for-the-badge&logo=c%2B%2B)](https://isocpp.org)
[![Build](https://img.shields.io/badge/Build-Single%20Portable%20Exe%20(~230%20KB)-brightgreen?style=for-the-badge)](https://github.com/Dedyjagok)
[![Author](https://img.shields.io/badge/Author-Dedyjagok-blue?style=for-the-badge&logo=github)](https://github.com/Dedyjagok)
[![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)](LICENSE)

<br/>

<p align="center">
  <img src="Assets/Icon.png" alt="Key Remapper Logo" width="128" height="128" />
</p>

**Key Remapper** adalah aplikasi desktop Windows mandiri (100% pure C++ Win32) yang dikembangkan oleh **[Dedyjagok](https://github.com/Dedyjagok)**. Aplikasi ini lahir dari jeritan hati pengguna laptop gaming (terutama lini **Acer Predator / Nitro** yang terkenal dengan "penyakit langganan" keyboard-nya) yang mendapati tombol **Left Ctrl** mendadak mogok kerja padahal cicilan laptop mungkin belum lunas. Daripada keluar jutaan rupiah ganti satu set *top-case* ke service center demi sebuah switch membrane yang rapuh, aplikasi ini membajak tombol gimmick **Copilot** Windows 11 yang menganggur dan mengubahnya menjadi tombol **Ctrl** pengganti yang responsif, zero-latency, dan 100% fungsional.

</div>

---

## 📌 Latar Belakang: Ketika Laptop Acer Kembali Berulah

Bukan rahasia umum lagi di kalangan pejuang laptop bahwa produk **Acer** (mulai dari seri Aspire, Nitro, hingga Predator yang garang di brosur tapi manja di tombol) kerap menghadirkan kejutan berupa tombol keyboard yang pensiun sebelum waktunya. Korban nomor satu biasanya adalah **Left Ctrl**—tombol paling vital untuk ngoding, kerja, dan gaming (*crouch/prone*).

Ketika tombol Ctrl di laptop Acer Anda tewas, pilihan resmi yang tersedia biasanya sangat tidak masuk akal:
1. 💸 **Bawa ke Service Center**: Menunggu berminggu-minggu, bayar ganti satu set modul keyboard utuh hanya karena **satu tombol** rusak.
2. 🎒 **Bawa Keyboard Eksternal**: Beli laptop gaming tipis dengan harga selangit, tapi kemana-mana tas tetap berat karena harus bawa mechanical keyboard cadangan.
3. 🧘 **Pasrah & Menerima Nasib**: Mencoba membiasakan diri menekan `Right Ctrl` yang letaknya di ujung dunia.

Sementara itu di keyboard Anda, Microsoft dan pabrikan laptop dengan bangganya menyematkan **tombol fisik Copilot** di jajaran tombol bawah—sebuah tombol yang kira-kira 99% dari kita tidak pernah sentuh seumur hidup.

Aplikasi ini hadir sebagai **solusi cerdas pembangkangan terhadap kualitas keyboard Acer**: manfaatkan tombol Copilot yang nganggur tersebut, jadikan dia **Ctrl sejati**!

- **100% Native C++**: Tanpa Python, tanpa runtime eksternal, tanpa framework bloated, dan bebas dari dependensi pihak ketiga.
- **Single File & Portabel**: Cukup unduh satu file `.exe` (~230 KB), simpan di mana saja, langsung jalan tanpa instalasi ribet.

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
  - Misi Pribadi: Menyelamatkan produktivitas sesama pejuang laptop Acer dari penderitaan tombol Ctrl yang mokel / pensiun dini.

---

## 📄 Lisensi

Proyek ini dirilis di bawah lisensi [MIT License](LICENSE). Bebas digunakan, dimodifikasi, dan didistribusikan untuk keperluan pribadi maupun komersial oleh siapa pun.

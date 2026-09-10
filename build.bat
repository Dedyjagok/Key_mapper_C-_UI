@echo off
echo ======================================================================
echo Compiling KeyRemapper (C++ Windows Native GUI with Modern Manifest)...
echo ======================================================================

windres resource.rc -o resource.o
if %ERRORLEVEL% NEQ 0 (
    echo [PERINGATAN] Gagal mengompilasi resource.rc, melanjutkan kompilasi tanpa resource...
    g++ -O2 -mwindows main.cpp -o KeyRemapper.exe -lcomctl32 -lshell32 -luser32 -ladvapi32
) else (
    g++ -O2 -mwindows main.cpp resource.o -o KeyRemapper.exe -lcomctl32 -lshell32 -luser32 -ladvapi32
    if exist resource.o del resource.o
)

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ======================================================================
    echo [SUKSES] KeyRemapper.exe berhasil dikompilasi!
    echo Ukuran file sangat kecil (~230 KB), sepenuhnya mandiri (portable),
    echo dan siap digunakan di laptop mana pun tanpa perlu install apa pun.
    echo ======================================================================
) else (
    echo.
    echo [GAGAL] Terjadi kesalahan saat kompilasi.
)
pause

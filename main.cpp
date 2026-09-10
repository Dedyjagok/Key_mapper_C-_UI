#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0601

#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

// Enable visual styles
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// -----------------------------------------------------------------------------
// CONSTANTS & IDS
// -----------------------------------------------------------------------------
#define IDI_APP_ICON 101

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_OPEN    2001
#define ID_TRAY_TOGGLE  2002
#define ID_TRAY_EXIT    2003
#define ID_TRAY_GITHUB  2004
#define ID_TRAY_ABOUT   2005

#define IDC_BTN_TOGGLE_HOOK    1001
#define IDC_BTN_PRESET_COPILOT 1002
#define IDC_BTN_PRESET_CAPS    1003
#define IDC_BTN_ADD_CUSTOM     1004
#define IDC_BTN_REMOVE_RULE    1005
#define IDC_BTN_TOGGLE_RULE    1006
#define IDC_CHK_STARTUP        1007
#define IDC_CHK_MIN_TO_TRAY    1008
#define IDC_BTN_HIDE_TRAY      1009
#define IDC_LIST_RULES         1010
#define IDC_STATUS_LABEL       1011
#define IDC_BTN_GITHUB         1012
#define IDC_BTN_ABOUT          1013

// Dialog Add Custom Rule IDs
#define IDC_COMBO_SRC_KEY    1101
#define IDC_COMBO_DST_KEY    1102
#define IDC_BTN_DETECT_SRC   1103
#define IDC_BTN_DETECT_DST   1104
#define IDC_STATIC_DETECT    1105

// -----------------------------------------------------------------------------
// DATA STRUCTURES
// -----------------------------------------------------------------------------
struct KeyRule {
    int id;
    DWORD srcVk;
    DWORD dstVk;
    bool isCopilot;
    bool enabled;
    std::wstring name;
};

// -----------------------------------------------------------------------------
// GLOBAL STATE
// -----------------------------------------------------------------------------
HINSTANCE g_hInstance = NULL;
HWND g_hWndMain = NULL;
HWND g_hListRules = NULL;
HWND g_hStatusLabel = NULL;
HWND g_hChkStartup = NULL;
HWND g_hChkMinToTray = NULL;
HWND g_hBtnToggle = NULL;

NOTIFYICONDATAW g_nid = { 0 };
HHOOK g_hKeyboardHook = NULL;

bool g_bHookActive = true;
bool g_bCopilotActive = false;
DWORD g_dwLastCopilotReleaseTime = 0;
bool g_bMinToTrayOnClose = true;
int g_nextRuleId = 1;

std::vector<KeyRule> g_rules;

// Key Name Mapping
struct KeyInfo {
    DWORD vk;
    const wchar_t* name;
};

static const KeyInfo KEY_DATABASE[] = {
    { 0x86, L"Copilot Key (Win+Shift+F23)" },
    { VK_LCONTROL, L"Left Ctrl" },
    { VK_RCONTROL, L"Right Ctrl" },
    { VK_CONTROL,  L"Ctrl (Universal)" },
    { VK_CAPITAL,  L"Caps Lock" },
    { VK_LMENU,    L"Left Alt" },
    { VK_RMENU,    L"Right Alt (AltGr)" },
    { VK_LSHIFT,   L"Left Shift" },
    { VK_RSHIFT,   L"Right Shift" },
    { VK_LWIN,     L"Left Windows" },
    { VK_RWIN,     L"Right Windows" },
    { VK_APPS,     L"Menu / Apps Key" },
    { VK_TAB,      L"Tab" },
    { VK_ESCAPE,   L"Escape" },
    { VK_BACK,     L"Backspace" },
    { VK_RETURN,   L"Enter" },
    { VK_SPACE,    L"Space" },
    { VK_INSERT,   L"Insert" },
    { VK_DELETE,   L"Delete" },
    { VK_HOME,     L"Home" },
    { VK_END,      L"End" },
    { VK_PRIOR,    L"Page Up" },
    { VK_NEXT,     L"Page Down" },
    { VK_UP,       L"Up Arrow" },
    { VK_DOWN,     L"Down Arrow" },
    { VK_LEFT,     L"Left Arrow" },
    { VK_RIGHT,    L"Right Arrow" },
    { VK_SNAPSHOT, L"Print Screen" },
    { VK_SCROLL,   L"Scroll Lock" },
    { VK_PAUSE,    L"Pause / Break" },
    { VK_OEM_3,    L"` ~ (Tilde)" },
    { 'A', L"A" }, { 'B', L"B" }, { 'C', L"C" }, { 'D', L"D" },
    { 'E', L"E" }, { 'F', L"F" }, { 'G', L"G" }, { 'H', L"H" },
    { 'I', L"I" }, { 'J', L"J" }, { 'K', L"K" }, { 'L', L"L" },
    { 'M', L"M" }, { 'N', L"N" }, { 'O', L"O" }, { 'P', L"P" },
    { 'Q', L"Q" }, { 'R', L"R" }, { 'S', L"S" }, { 'T', L"T" },
    { 'U', L"U" }, { 'V', L"V" }, { 'W', L"W" }, { 'X', L"X" },
    { 'Y', L"Y" }, { 'Z', L"Z" },
    { '0', L"0" }, { '1', L"1" }, { '2', L"2" }, { '3', L"3" },
    { '4', L"4" }, { '5', L"5" }, { '6', L"6" }, { '7', L"7" },
    { '8', L"8" }, { '9', L"9" },
    { VK_F1,  L"F1" },  { VK_F2,  L"F2" },  { VK_F3,  L"F3" },  { VK_F4,  L"F4" },
    { VK_F5,  L"F5" },  { VK_F6,  L"F6" },  { VK_F7,  L"F7" },  { VK_F8,  L"F8" },
    { VK_F9,  L"F9" },  { VK_F10, L"F10" }, { VK_F11, L"F11" }, { VK_F12, L"F12" },
    { VK_F13, L"F13" }, { VK_F14, L"F14" }, { VK_F15, L"F15" }, { VK_F16, L"F16" },
    { VK_F17, L"F17" }, { VK_F18, L"F18" }, { VK_F19, L"F19" }, { VK_F20, L"F20" },
    { VK_F21, L"F21" }, { VK_F22, L"F22" }, { VK_F23, L"F23" }, { VK_F24, L"F24" }
};

std::wstring GetVkName(DWORD vk) {
    for (const auto& item : KEY_DATABASE) {
        if (item.vk == vk) return item.name;
    }
    wchar_t buf[32];
    wsprintfW(buf, L"Key (0x%02X)", vk);
    return buf;
}

// -----------------------------------------------------------------------------
// LOW-LEVEL KEYBOARD INJECTION & HOOK
// -----------------------------------------------------------------------------
void SendKeyInput(WORD vk, bool isDown) {
    INPUT ip;
    ZeroMemory(&ip, sizeof(INPUT));
    ip.type = INPUT_KEYBOARD;
    ip.ki.wVk = vk;
    ip.ki.wScan = (WORD)MapVirtualKeyW(vk, MAPVK_VK_TO_VSC);
    ip.ki.dwFlags = (isDown ? 0 : KEYEVENTF_KEYUP);
    if (vk == VK_RCONTROL || vk == VK_RMENU || vk == VK_RWIN ||
        vk == VK_INSERT || vk == VK_DELETE || vk == VK_HOME ||
        vk == VK_END || vk == VK_PRIOR || vk == VK_NEXT ||
        vk == VK_UP || vk == VK_DOWN || vk == VK_LEFT || vk == VK_RIGHT ||
        vk == VK_APPS) {
        ip.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
    }
    SendInput(1, &ip, sizeof(INPUT));
}

// Send dummy key to mask Windows Start menu when releasing LWin
void SendStartMenuMask() {
    INPUT ip[2];
    ZeroMemory(ip, sizeof(ip));
    ip[0].type = INPUT_KEYBOARD;
    ip[0].ki.wVk = 0xE8; // Unassigned VK used as Windows menu mask
    ip[0].ki.dwFlags = 0;

    ip[1].type = INPUT_KEYBOARD;
    ip[1].ki.wVk = 0xE8;
    ip[1].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(2, ip, sizeof(INPUT));
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0 || !g_bHookActive) {
        return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
    }

    KBDLLHOOKSTRUCT* pKey = (KBDLLHOOKSTRUCT*)lParam;

    // Ignore synthetic keystrokes injected by SendInput to prevent recursion
    if (pKey->flags & LLKHF_INJECTED) {
        return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
    }

    bool isKeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
    bool isKeyUp   = (wParam == WM_KEYUP   || wParam == WM_SYSKEYUP);

    // 1. Check if Copilot Key (VK_F23 = 0x86) is pressed
    if (pKey->vkCode == 0x86) {
        // Look for enabled Copilot rule
        for (const auto& rule : g_rules) {
            if (rule.enabled && rule.isCopilot) {
                if (isKeyDown) {
                    if (!g_bCopilotActive) {
                        g_bCopilotActive = true;
                        // Release hardware Win and Shift logically so Ctrl shortcuts work cleanly
                        SendKeyInput(VK_LWIN, false);
                        SendKeyInput(VK_LSHIFT, false);
                        SendStartMenuMask();
                        // Press down destination key (default LCtrl)
                        SendKeyInput((WORD)rule.dstVk, true);
                    }
                    return 1; // Intercept & block F23
                } else if (isKeyUp) {
                    if (g_bCopilotActive) {
                        g_bCopilotActive = false;
                        // Release destination key
                        SendKeyInput((WORD)rule.dstVk, false);
                        SendStartMenuMask();
                        g_dwLastCopilotReleaseTime = GetTickCount();
                    }
                    return 1; // Intercept & block F23
                }
            }
        }
    }

    // Suppress trailing Win/Shift UP events within 150ms of Copilot release
    if (g_dwLastCopilotReleaseTime != 0 && (GetTickCount() - g_dwLastCopilotReleaseTime) < 150) {
        if (pKey->vkCode == VK_LWIN || pKey->vkCode == VK_LSHIFT) {
            if (isKeyUp) {
                return 1; // Swallow to prevent side effects
            }
        }
    }

    // 2. Check other custom rules
    for (const auto& rule : g_rules) {
        if (rule.enabled && !rule.isCopilot && rule.srcVk == pKey->vkCode) {
            if (isKeyDown) {
                SendKeyInput((WORD)rule.dstVk, true);
                return 1;
            } else if (isKeyUp) {
                SendKeyInput((WORD)rule.dstVk, false);
                return 1;
            }
        }
    }

    return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
}

void StartHook() {
    if (!g_hKeyboardHook) {
        g_hKeyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, g_hInstance, 0);
    }
}

void StopHook() {
    if (g_hKeyboardHook) {
        UnhookWindowsHookEx(g_hKeyboardHook);
        g_hKeyboardHook = NULL;
    }
}

// -----------------------------------------------------------------------------
// CONFIGURATION (INI FILE) & REGISTRY STARTUP
// -----------------------------------------------------------------------------
std::wstring GetIniPath() {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    wchar_t* lastSlash = wcsrchr(path, L'\\');
    if (lastSlash) *(lastSlash + 1) = L'\0';
    return std::wstring(path) + L"key_remapper.ini";
}

bool IsRunAtStartup() {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        wchar_t buf[MAX_PATH];
        DWORD bufSize = sizeof(buf);
        LONG res = RegQueryValueExW(hKey, L"KeyRemapper", NULL, NULL, (LPBYTE)buf, &bufSize);
        RegCloseKey(hKey);
        return (res == ERROR_SUCCESS);
    }
    return false;
}

void SetRunAtStartup(bool enable) {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
        if (enable) {
            wchar_t path[MAX_PATH];
            GetModuleFileNameW(NULL, path, MAX_PATH);
            std::wstring cmd = L"\"" + std::wstring(path) + L"\" -tray";
            RegSetValueExW(hKey, L"KeyRemapper", 0, REG_SZ, (const BYTE*)cmd.c_str(), (DWORD)((cmd.length() + 1) * sizeof(wchar_t)));
        } else {
            RegDeleteValueW(hKey, L"KeyRemapper");
        }
        RegCloseKey(hKey);
    }
}

void SaveConfig() {
    std::wstring ini = GetIniPath();
    WritePrivateProfileStringW(L"General", L"HookActive", g_bHookActive ? L"1" : L"0", ini.c_str());
    WritePrivateProfileStringW(L"General", L"MinToTrayOnClose", g_bMinToTrayOnClose ? L"1" : L"0", ini.c_str());

    wchar_t countBuf[16];
    wsprintfW(countBuf, L"%d", (int)g_rules.size());
    WritePrivateProfileStringW(L"Rules", L"Count", countBuf, ini.c_str());

    for (size_t i = 0; i < g_rules.size(); ++i) {
        std::wstring sec = L"Rule_" + std::to_wstring(i);
        WritePrivateProfileStringW(sec.c_str(), L"Name", g_rules[i].name.c_str(), ini.c_str());
        WritePrivateProfileStringW(sec.c_str(), L"IsCopilot", g_rules[i].isCopilot ? L"1" : L"0", ini.c_str());
        WritePrivateProfileStringW(sec.c_str(), L"SrcVk", std::to_wstring(g_rules[i].srcVk).c_str(), ini.c_str());
        WritePrivateProfileStringW(sec.c_str(), L"DstVk", std::to_wstring(g_rules[i].dstVk).c_str(), ini.c_str());
        WritePrivateProfileStringW(sec.c_str(), L"Enabled", g_rules[i].enabled ? L"1" : L"0", ini.c_str());
    }
}

void LoadConfig() {
    std::wstring ini = GetIniPath();
    g_bHookActive = (GetPrivateProfileIntW(L"General", L"HookActive", 1, ini.c_str()) != 0);
    g_bMinToTrayOnClose = (GetPrivateProfileIntW(L"General", L"MinToTrayOnClose", 1, ini.c_str()) != 0);

    int count = GetPrivateProfileIntW(L"Rules", L"Count", -1, ini.c_str());
    g_rules.clear();

    if (count <= 0) {
        // Default initial rule: Copilot Key -> Left Ctrl
        KeyRule r;
        r.id = g_nextRuleId++;
        r.isCopilot = true;
        r.srcVk = 0x86;
        r.dstVk = VK_LCONTROL;
        r.enabled = true;
        r.name = L"Copilot Key -> Left Ctrl (Broken Ctrl Fix)";
        g_rules.push_back(r);
        SaveConfig();
    } else {
        for (int i = 0; i < count; ++i) {
            std::wstring sec = L"Rule_" + std::to_wstring(i);
            wchar_t nameBuf[256];
            GetPrivateProfileStringW(sec.c_str(), L"Name", L"Rule", nameBuf, 256, ini.c_str());

            KeyRule r;
            r.id = g_nextRuleId++;
            r.name = nameBuf;
            r.isCopilot = (GetPrivateProfileIntW(sec.c_str(), L"IsCopilot", 0, ini.c_str()) != 0);
            r.srcVk = (DWORD)GetPrivateProfileIntW(sec.c_str(), L"SrcVk", 0, ini.c_str());
            r.dstVk = (DWORD)GetPrivateProfileIntW(sec.c_str(), L"DstVk", VK_LCONTROL, ini.c_str());
            r.enabled = (GetPrivateProfileIntW(sec.c_str(), L"Enabled", 1, ini.c_str()) != 0);
            g_rules.push_back(r);
        }
    }
}

// -----------------------------------------------------------------------------
// UI UPDATE HELPERS
// -----------------------------------------------------------------------------
void UpdateStatusUI() {
    if (g_bHookActive) {
        SetWindowTextW(g_hStatusLabel, L"Status: AKTIF (Keyboard Intercept Berjalan)");
        SetWindowTextW(g_hBtnToggle, L"Matikan Remapper");
    } else {
        SetWindowTextW(g_hStatusLabel, L"Status: NONAKTIF (Remapper Dimatikan)");
        SetWindowTextW(g_hBtnToggle, L"Nyalakan Remapper");
    }

    if (g_nid.cbSize) {
        wcsncpy(g_nid.szTip, g_bHookActive ? L"Key Remapper (Aktif)" : L"Key Remapper (Mati)", 128);
        Shell_NotifyIconW(NIM_MODIFY, &g_nid);
    }
}

void ShowTrayBalloon(const wchar_t* title, const wchar_t* msg) {
    if (g_nid.cbSize) {
        g_nid.uFlags |= NIF_INFO;
        wcsncpy(g_nid.szInfoTitle, title, 64);
        wcsncpy(g_nid.szInfo, msg, 256);
        g_nid.dwInfoFlags = NIIF_INFO;
        Shell_NotifyIconW(NIM_MODIFY, &g_nid);
        g_nid.uFlags &= ~NIF_INFO;
    }
}

void RefreshListView() {
    ListView_DeleteAllItems(g_hListRules);
    for (size_t i = 0; i < g_rules.size(); ++i) {
        const auto& r = g_rules[i];
        LVITEMW item;
        ZeroMemory(&item, sizeof(item));
        item.mask = LVIF_TEXT | LVIF_PARAM;
        item.iItem = (int)i;
        item.iSubItem = 0;
        item.lParam = r.id;

        wchar_t numBuf[16];
        wsprintfW(numBuf, L"%d", (int)(i + 1));
        item.pszText = numBuf;
        ListView_InsertItem(g_hListRules, &item);

        std::wstring srcStr = r.isCopilot ? L"Copilot Key" : GetVkName(r.srcVk);
        ListView_SetItemText(g_hListRules, (int)i, 1, (LPWSTR)srcStr.c_str());

        std::wstring dstStr = GetVkName(r.dstVk);
        ListView_SetItemText(g_hListRules, (int)i, 2, (LPWSTR)dstStr.c_str());

        std::wstring statStr = r.enabled ? L"✓ Aktif" : L"✗ Nonaktif";
        ListView_SetItemText(g_hListRules, (int)i, 3, (LPWSTR)statStr.c_str());

        ListView_SetItemText(g_hListRules, (int)i, 4, (LPWSTR)r.name.c_str());
    }
}

// -----------------------------------------------------------------------------
// ADD CUSTOM RULE DIALOG
// -----------------------------------------------------------------------------
static DWORD s_customSrcVk = VK_CAPITAL;
static DWORD s_customDstVk = VK_LCONTROL;
static bool s_isDetectingSrc = false;
static bool s_isDetectingDst = false;
static HHOOK s_hDetectHook = NULL;
static HWND s_hDlgCustom = NULL;

void SelectOrAddKeyToCombo(HWND hCombo, DWORD vk) {
    if (!hCombo) return;
    int count = (int)SendMessageW(hCombo, CB_GETCOUNT, 0, 0);
    for (int i = 0; i < count; ++i) {
        DWORD itemVk = (DWORD)SendMessageW(hCombo, CB_GETITEMDATA, i, 0);
        if (itemVk == vk) {
            SendMessageW(hCombo, CB_SETCURSEL, i, 0);
            InvalidateRect(hCombo, NULL, TRUE);
            UpdateWindow(hCombo);
            return;
        }
    }
    // If not found in combobox, add it and select it
    std::wstring name = GetVkName(vk);
    int newIdx = (int)SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)name.c_str());
    if (newIdx != CB_ERR) {
        SendMessageW(hCombo, CB_SETITEMDATA, newIdx, (LPARAM)vk);
        SendMessageW(hCombo, CB_SETCURSEL, newIdx, 0);
        InvalidateRect(hCombo, NULL, TRUE);
        UpdateWindow(hCombo);
    }
}

LRESULT CALLBACK DetectKeyHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        KBDLLHOOKSTRUCT* pKey = (KBDLLHOOKSTRUCT*)lParam;
        if (s_isDetectingSrc) {
            s_customSrcVk = pKey->vkCode;
            s_isDetectingSrc = false;
            if (s_hDetectHook) { UnhookWindowsHookEx(s_hDetectHook); s_hDetectHook = NULL; }
            HWND hComboSrc = GetDlgItem(s_hDlgCustom, IDC_COMBO_SRC_KEY);
            SelectOrAddKeyToCombo(hComboSrc, s_customSrcVk);
            SetWindowTextW(GetDlgItem(s_hDlgCustom, IDC_BTN_DETECT_SRC), L"Deteksi...");
            SetWindowTextW(GetDlgItem(s_hDlgCustom, IDC_STATIC_DETECT), (L"Terdeteksi Tombol Asal: " + GetVkName(s_customSrcVk)).c_str());
            return 1;
        } else if (s_isDetectingDst) {
            s_customDstVk = pKey->vkCode;
            s_isDetectingDst = false;
            if (s_hDetectHook) { UnhookWindowsHookEx(s_hDetectHook); s_hDetectHook = NULL; }
            HWND hComboDst = GetDlgItem(s_hDlgCustom, IDC_COMBO_DST_KEY);
            SelectOrAddKeyToCombo(hComboDst, s_customDstVk);
            SetWindowTextW(GetDlgItem(s_hDlgCustom, IDC_BTN_DETECT_DST), L"Deteksi...");
            SetWindowTextW(GetDlgItem(s_hDlgCustom, IDC_STATIC_DETECT), (L"Terdeteksi Tombol Tujuan: " + GetVkName(s_customDstVk)).c_str());
            return 1;
        }
    }
    return CallNextHookEx(s_hDetectHook, nCode, wParam, lParam);
}

INT_PTR CALLBACK CustomRuleDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_INITDIALOG: {
        s_hDlgCustom = hDlg;
        HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

        // Create child controls dynamically
        HWND hL1 = CreateWindowW(L"STATIC", L"Tombol Asal (Yang Rusak/Ingin Ditukar):", WS_CHILD | WS_VISIBLE, 15, 12, 320, 18, hDlg, NULL, g_hInstance, NULL);
        HWND hComboSrc = CreateWindowW(L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL, 15, 32, 220, 200, hDlg, (HMENU)IDC_COMBO_SRC_KEY, g_hInstance, NULL);
        HWND hBtnDetSrc = CreateWindowW(L"BUTTON", L"Deteksi...", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 245, 31, 80, 24, hDlg, (HMENU)IDC_BTN_DETECT_SRC, g_hInstance, NULL);

        HWND hL2 = CreateWindowW(L"STATIC", L"Gantikan Fungsinya Menjadi:", WS_CHILD | WS_VISIBLE, 15, 65, 320, 18, hDlg, NULL, g_hInstance, NULL);
        HWND hComboDst = CreateWindowW(L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL, 15, 85, 220, 200, hDlg, (HMENU)IDC_COMBO_DST_KEY, g_hInstance, NULL);
        HWND hBtnDetDst = CreateWindowW(L"BUTTON", L"Deteksi...", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 245, 84, 80, 24, hDlg, (HMENU)IDC_BTN_DETECT_DST, g_hInstance, NULL);

        HWND hStaticDet = CreateWindowW(L"STATIC", L"Pilih tombol dari daftar atau klik 'Deteksi...'", WS_CHILD | WS_VISIBLE, 15, 120, 320, 18, hDlg, (HMENU)IDC_STATIC_DETECT, g_hInstance, NULL);

        HWND hBtnOk = CreateWindowW(L"BUTTON", L"Simpan Aturan", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 130, 150, 100, 28, hDlg, (HMENU)IDOK, g_hInstance, NULL);
        HWND hBtnCancel = CreateWindowW(L"BUTTON", L"Batal", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 240, 150, 85, 28, hDlg, (HMENU)IDCANCEL, g_hInstance, NULL);

        SendMessageW(hL1, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessageW(hComboSrc, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessageW(hBtnDetSrc, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessageW(hL2, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessageW(hComboDst, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessageW(hBtnDetDst, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessageW(hStaticDet, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessageW(hBtnOk, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessageW(hBtnCancel, WM_SETFONT, (WPARAM)hFont, TRUE);

        for (size_t i = 0; i < sizeof(KEY_DATABASE)/sizeof(KEY_DATABASE[0]); ++i) {
            SendMessageW(hComboSrc, CB_ADDSTRING, 0, (LPARAM)KEY_DATABASE[i].name);
            SendMessageW(hComboSrc, CB_SETITEMDATA, i, (LPARAM)KEY_DATABASE[i].vk);

            SendMessageW(hComboDst, CB_ADDSTRING, 0, (LPARAM)KEY_DATABASE[i].name);
            SendMessageW(hComboDst, CB_SETITEMDATA, i, (LPARAM)KEY_DATABASE[i].vk);
        }
        SendMessageW(hComboSrc, CB_SETCURSEL, 4, 0); // CapsLock default
        SendMessageW(hComboDst, CB_SETCURSEL, 1, 0); // LCtrl default
        s_customSrcVk = VK_CAPITAL;
        s_customDstVk = VK_LCONTROL;
        return TRUE;
    }
    case WM_COMMAND: {
        WORD id = LOWORD(wParam);
        if (id == IDC_BTN_DETECT_SRC) {
            s_isDetectingSrc = true;
            s_isDetectingDst = false;
            SetWindowTextW(GetDlgItem(hDlg, IDC_BTN_DETECT_SRC), L"Merekam...");
            SetWindowTextW(GetDlgItem(hDlg, IDC_BTN_DETECT_DST), L"Deteksi...");
            SetWindowTextW(GetDlgItem(hDlg, IDC_STATIC_DETECT), L"TEKAN TOMBOL ASAL DI KEYBOARD SEKARANG...");
            if (!s_hDetectHook) {
                s_hDetectHook = SetWindowsHookExW(WH_KEYBOARD_LL, DetectKeyHookProc, g_hInstance, 0);
            }
        } else if (id == IDC_BTN_DETECT_DST) {
            s_isDetectingDst = true;
            s_isDetectingSrc = false;
            SetWindowTextW(GetDlgItem(hDlg, IDC_BTN_DETECT_DST), L"Merekam...");
            SetWindowTextW(GetDlgItem(hDlg, IDC_BTN_DETECT_SRC), L"Deteksi...");
            SetWindowTextW(GetDlgItem(hDlg, IDC_STATIC_DETECT), L"TEKAN TOMBOL PENGGANTI DI KEYBOARD SEKARANG...");
            if (!s_hDetectHook) {
                s_hDetectHook = SetWindowsHookExW(WH_KEYBOARD_LL, DetectKeyHookProc, g_hInstance, 0);
            }
        } else if (id == IDC_COMBO_SRC_KEY && HIWORD(wParam) == CBN_SELCHANGE) {
            HWND hComboSrc = GetDlgItem(hDlg, IDC_COMBO_SRC_KEY);
            int idx = (int)SendMessageW(hComboSrc, CB_GETCURSEL, 0, 0);
            if (idx != CB_ERR) {
                s_customSrcVk = (DWORD)SendMessageW(hComboSrc, CB_GETITEMDATA, idx, 0);
                SetWindowTextW(GetDlgItem(hDlg, IDC_STATIC_DETECT), (L"Pilihan Tombol Asal: " + GetVkName(s_customSrcVk)).c_str());
            }
        } else if (id == IDC_COMBO_DST_KEY && HIWORD(wParam) == CBN_SELCHANGE) {
            HWND hComboDst = GetDlgItem(hDlg, IDC_COMBO_DST_KEY);
            int idx = (int)SendMessageW(hComboDst, CB_GETCURSEL, 0, 0);
            if (idx != CB_ERR) {
                s_customDstVk = (DWORD)SendMessageW(hComboDst, CB_GETITEMDATA, idx, 0);
                SetWindowTextW(GetDlgItem(hDlg, IDC_STATIC_DETECT), (L"Pilihan Tombol Tujuan: " + GetVkName(s_customDstVk)).c_str());
            }
        } else if (id == IDOK) {
            HWND hComboSrc = GetDlgItem(hDlg, IDC_COMBO_SRC_KEY);
            HWND hComboDst = GetDlgItem(hDlg, IDC_COMBO_DST_KEY);

            int idxSrc = (int)SendMessageW(hComboSrc, CB_GETCURSEL, 0, 0);
            int idxDst = (int)SendMessageW(hComboDst, CB_GETCURSEL, 0, 0);

            DWORD srcVk = (idxSrc != CB_ERR) ? (DWORD)SendMessageW(hComboSrc, CB_GETITEMDATA, idxSrc, 0) : s_customSrcVk;
            DWORD dstVk = (idxDst != CB_ERR) ? (DWORD)SendMessageW(hComboDst, CB_GETITEMDATA, idxDst, 0) : s_customDstVk;

            if (srcVk == dstVk) {
                MessageBoxW(hDlg, L"Tombol asal dan pengganti tidak boleh sama!", L"Peringatan", MB_ICONWARNING);
                return TRUE;
            }

            KeyRule r;
            r.id = g_nextRuleId++;
            r.isCopilot = (srcVk == 0x86);
            r.srcVk = srcVk;
            r.dstVk = dstVk;
            r.enabled = true;
            r.name = GetVkName(srcVk) + L" -> " + GetVkName(dstVk);
            g_rules.push_back(r);

            s_isDetectingSrc = false;
            s_isDetectingDst = false;
            if (s_hDetectHook) { UnhookWindowsHookEx(s_hDetectHook); s_hDetectHook = NULL; }
            EndDialog(hDlg, IDOK);
            return TRUE;
        } else if (id == IDCANCEL) {
            s_isDetectingSrc = false;
            s_isDetectingDst = false;
            if (s_hDetectHook) { UnhookWindowsHookEx(s_hDetectHook); s_hDetectHook = NULL; }
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        break;
    }
    case WM_CLOSE: {
        s_isDetectingSrc = false;
        s_isDetectingDst = false;
        if (s_hDetectHook) { UnhookWindowsHookEx(s_hDetectHook); s_hDetectHook = NULL; }
        EndDialog(hDlg, IDCANCEL);
        return TRUE;
    }
    }
    return FALSE;
}

void ShowAddCustomRuleDialog(HWND hWndParent) {
    std::vector<BYTE> buffer(2048, 0);
    LPDLGTEMPLATE pDlg = (LPDLGTEMPLATE)buffer.data();
    pDlg->style = DS_MODALFRAME | DS_CENTER | WS_POPUP | WS_CAPTION | WS_SYSMENU;
    pDlg->cdit = 0;
    pDlg->x = 0; pDlg->y = 0; pDlg->cx = 180; pDlg->cy = 110;

    BYTE* ptr = (BYTE*)(pDlg + 1);
    *(WORD*)ptr = 0; ptr += 2; // Menu
    *(WORD*)ptr = 0; ptr += 2; // Class
    const wchar_t* title = L"Tambah Aturan Remap Kustom";
    memcpy(ptr, title, (wcslen(title) + 1) * sizeof(wchar_t));

    DialogBoxIndirectParamW(g_hInstance, (LPCDLGTEMPLATEW)buffer.data(), hWndParent, CustomRuleDlgProc, 0);
}

// -----------------------------------------------------------------------------
// MAIN WINDOW PROCEDURE
// -----------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: {
        HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

        // Header Title
        HWND hTitle = CreateWindowW(L"STATIC", L"Key Remapper - Solusi Tombol Ctrl Rusak",
            WS_CHILD | WS_VISIBLE | SS_LEFT, 20, 15, 450, 22, hWnd, NULL, g_hInstance, NULL);

        // Status Label
        g_hStatusLabel = CreateWindowW(L"STATIC", L"Status: AKTIF (Keyboard Intercept Berjalan)",
            WS_CHILD | WS_VISIBLE | SS_LEFT, 20, 40, 360, 20, hWnd, (HMENU)IDC_STATUS_LABEL, g_hInstance, NULL);

        // Master Switch Button
        g_hBtnToggle = CreateWindowW(L"BUTTON", L"Matikan Remapper",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 390, 32, 185, 30, hWnd, (HMENU)IDC_BTN_TOGGLE_HOOK, g_hInstance, NULL);

        // Group Box: Rules
        HWND hGroupRules = CreateWindowW(L"BUTTON", L"Daftar Aturan Remap Tombol",
            WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 20, 75, 555, 240, hWnd, NULL, g_hInstance, NULL);

        // ListView for Rules
        g_hListRules = CreateWindowW(WC_LISTVIEWW, L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SINGLESEL,
            35, 100, 525, 165, hWnd, (HMENU)IDC_LIST_RULES, g_hInstance, NULL);
        ListView_SetExtendedListViewStyle(g_hListRules, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

        // Setup ListView Columns
        LVCOLUMNW col;
        ZeroMemory(&col, sizeof(col));
        col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        col.cx = 40;  col.pszText = (LPWSTR)L"No";      col.iSubItem = 0; ListView_InsertColumn(g_hListRules, 0, &col);
        col.cx = 140; col.pszText = (LPWSTR)L"Tombol Asal"; col.iSubItem = 1; ListView_InsertColumn(g_hListRules, 1, &col);
        col.cx = 120; col.pszText = (LPWSTR)L"Fungsi Baru"; col.iSubItem = 2; ListView_InsertColumn(g_hListRules, 2, &col);
        col.cx = 80;  col.pszText = (LPWSTR)L"Status";    col.iSubItem = 3; ListView_InsertColumn(g_hListRules, 3, &col);
        col.cx = 140; col.pszText = (LPWSTR)L"Keterangan"; col.iSubItem = 4; ListView_InsertColumn(g_hListRules, 4, &col);

        // List Action Buttons
        HWND hBtnPresetCopilot = CreateWindowW(L"BUTTON", L"+ Copilot -> Ctrl",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 35, 275, 125, 26, hWnd, (HMENU)IDC_BTN_PRESET_COPILOT, g_hInstance, NULL);
        HWND hBtnPresetCaps = CreateWindowW(L"BUTTON", L"+ CapsLock -> Ctrl",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 168, 275, 135, 26, hWnd, (HMENU)IDC_BTN_PRESET_CAPS, g_hInstance, NULL);
        HWND hBtnAddCustom = CreateWindowW(L"BUTTON", L"+ Kustom...",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 311, 275, 85, 26, hWnd, (HMENU)IDC_BTN_ADD_CUSTOM, g_hInstance, NULL);
        HWND hBtnToggleRule = CreateWindowW(L"BUTTON", L"On/Off",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 404, 275, 75, 26, hWnd, (HMENU)IDC_BTN_TOGGLE_RULE, g_hInstance, NULL);
        HWND hBtnRemoveRule = CreateWindowW(L"BUTTON", L"Hapus",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 485, 275, 75, 26, hWnd, (HMENU)IDC_BTN_REMOVE_RULE, g_hInstance, NULL);

        // Bottom Options
        g_hChkStartup = CreateWindowW(L"BUTTON", L"Jalankan Otomatis saat Windows Menyala (Startup)",
            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 20, 328, 380, 22, hWnd, (HMENU)IDC_CHK_STARTUP, g_hInstance, NULL);
        if (IsRunAtStartup()) {
            SendMessageW(g_hChkStartup, BM_SETCHECK, BST_CHECKED, 0);
        }

        g_hChkMinToTray = CreateWindowW(L"BUTTON", L"Sembunyikan ke System Tray saat tombol Tutup (X) diklik",
            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 20, 353, 400, 22, hWnd, (HMENU)IDC_CHK_MIN_TO_TRAY, g_hInstance, NULL);
        SendMessageW(g_hChkMinToTray, BM_SETCHECK, g_bMinToTrayOnClose ? BST_CHECKED : BST_UNCHECKED, 0);

        HWND hBtnHide = CreateWindowW(L"BUTTON", L"Sembunyikan ke Tray",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 420, 335, 155, 35, hWnd, (HMENU)IDC_BTN_HIDE_TRAY, g_hInstance, NULL);

        // Footer & Credits
        HWND hFooter = CreateWindowW(L"STATIC", L"Created by Dedyjagok (github.com/Dedyjagok)  |  Pure C++ Win32 Edition",
            WS_CHILD | WS_VISIBLE | SS_LEFT, 20, 390, 390, 20, hWnd, NULL, g_hInstance, NULL);
        HWND hBtnGit = CreateWindowW(L"BUTTON", L"GitHub",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 420, 386, 75, 26, hWnd, (HMENU)IDC_BTN_GITHUB, g_hInstance, NULL);
        HWND hBtnAbt = CreateWindowW(L"BUTTON", L"Credits",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 500, 386, 75, 26, hWnd, (HMENU)IDC_BTN_ABOUT, g_hInstance, NULL);

        // Apply GUI font to all controls
        HWND controls[] = { hTitle, g_hStatusLabel, g_hBtnToggle, hGroupRules, g_hListRules,
                            hBtnPresetCopilot, hBtnPresetCaps, hBtnAddCustom, hBtnToggleRule, hBtnRemoveRule,
                            g_hChkStartup, g_hChkMinToTray, hBtnHide, hFooter, hBtnGit, hBtnAbt };
        for (HWND ctrl : controls) {
            SendMessageW(ctrl, WM_SETFONT, (WPARAM)hFont, TRUE);
        }

        // Set Window & Taskbar Icon
        HICON hAppIcon = LoadIconW(g_hInstance, MAKEINTRESOURCEW(IDI_APP_ICON));
        if (!hAppIcon) {
            hAppIcon = LoadIconW(NULL, IDI_APPLICATION);
        }
        SendMessageW(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hAppIcon);
        SendMessageW(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hAppIcon);

        // Initialize Tray Icon
        ZeroMemory(&g_nid, sizeof(NOTIFYICONDATAW));
        g_nid.cbSize = sizeof(NOTIFYICONDATAW);
        g_nid.hWnd = hWnd;
        g_nid.uID = 1;
        g_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        g_nid.uCallbackMessage = WM_TRAYICON;
        g_nid.hIcon = hAppIcon;
        wcsncpy(g_nid.szTip, L"Key Remapper (Aktif)", 128);
        Shell_NotifyIconW(NIM_ADD, &g_nid);

        UpdateStatusUI();
        RefreshListView();
        return 0;
    }

    case WM_COMMAND: {
        WORD id = LOWORD(wParam);
        switch (id) {
        case IDC_BTN_TOGGLE_HOOK: {
            g_bHookActive = !g_bHookActive;
            UpdateStatusUI();
            SaveConfig();
            break;
        }
        case IDC_BTN_PRESET_COPILOT: {
            bool exists = false;
            for (auto& r : g_rules) {
                if (r.isCopilot) { r.enabled = true; exists = true; break; }
            }
            if (!exists) {
                KeyRule r;
                r.id = g_nextRuleId++;
                r.isCopilot = true;
                r.srcVk = 0x86;
                r.dstVk = VK_LCONTROL;
                r.enabled = true;
                r.name = L"Copilot Key -> Left Ctrl";
                g_rules.push_back(r);
            }
            RefreshListView();
            SaveConfig();
            break;
        }
        case IDC_BTN_PRESET_CAPS: {
            bool exists = false;
            for (auto& r : g_rules) {
                if (!r.isCopilot && r.srcVk == VK_CAPITAL) { r.enabled = true; exists = true; break; }
            }
            if (!exists) {
                KeyRule r;
                r.id = g_nextRuleId++;
                r.isCopilot = false;
                r.srcVk = VK_CAPITAL;
                r.dstVk = VK_LCONTROL;
                r.enabled = true;
                r.name = L"Caps Lock -> Left Ctrl";
                g_rules.push_back(r);
            }
            RefreshListView();
            SaveConfig();
            break;
        }
        case IDC_BTN_ADD_CUSTOM: {
            ShowAddCustomRuleDialog(hWnd);
            RefreshListView();
            SaveConfig();
            break;
        }
        case IDC_BTN_TOGGLE_RULE: {
            int sel = ListView_GetNextItem(g_hListRules, -1, LVNI_SELECTED);
            if (sel >= 0 && sel < (int)g_rules.size()) {
                g_rules[sel].enabled = !g_rules[sel].enabled;
                RefreshListView();
                SaveConfig();
            } else {
                MessageBoxW(hWnd, L"Pilih salah satu aturan di tabel terlebih dahulu.", L"Pemberitahuan", MB_ICONINFORMATION);
            }
            break;
        }
        case IDC_BTN_REMOVE_RULE: {
            int sel = ListView_GetNextItem(g_hListRules, -1, LVNI_SELECTED);
            if (sel >= 0 && sel < (int)g_rules.size()) {
                g_rules.erase(g_rules.begin() + sel);
                RefreshListView();
                SaveConfig();
            } else {
                MessageBoxW(hWnd, L"Pilih aturan yang ingin dihapus dari tabel.", L"Pemberitahuan", MB_ICONINFORMATION);
            }
            break;
        }
        case IDC_CHK_STARTUP: {
            bool chk = (SendMessageW(g_hChkStartup, BM_GETCHECK, 0, 0) == BST_CHECKED);
            SetRunAtStartup(chk);
            break;
        }
        case IDC_CHK_MIN_TO_TRAY: {
            g_bMinToTrayOnClose = (SendMessageW(g_hChkMinToTray, BM_GETCHECK, 0, 0) == BST_CHECKED);
            SaveConfig();
            break;
        }
        case IDC_BTN_HIDE_TRAY: {
            ShowWindow(hWnd, SW_HIDE);
            static bool s_shownTrayNotice1 = false;
            if (!s_shownTrayNotice1) {
                s_shownTrayNotice1 = true;
                ShowTrayBalloon(L"Key Remapper Berjalan di Tray", L"Aplikasi disembunyikan ke area tray (dekat jam). Klik ikon untuk membuka kembali.");
            }
            break;
        }
        case IDC_BTN_GITHUB: {
            ShellExecuteW(NULL, L"open", L"https://github.com/Dedyjagok", NULL, NULL, SW_SHOWNORMAL);
            break;
        }
        case IDC_BTN_ABOUT: {
            MessageBoxW(hWnd,
                L"Key Remapper (Broken Ctrl Fix)\n"
                L"Versi 2.0 - Pure C++ Edition\n\n"
                L"Author / Developer:\n"
                L"• Dedyjagok (https://github.com/Dedyjagok)\n\n"
                L"100% Pure Native C++ Windows Application.\n"
                L"Dibangun mandiri menggunakan Win32 API & Low-Level Kernel Hook.\n"
                L"Didesain khusus untuk laptop ACER mungkin & Windows 11.",
                L"Tentang & Credits", MB_ICONINFORMATION | MB_OK);
            break;
        }
        }
        return 0;
    }

    case WM_TRAYICON: {
        if (lParam == WM_LBUTTONDBLCLK || lParam == WM_LBUTTONUP) {
            ShowWindow(hWnd, SW_SHOW);
            SetForegroundWindow(hWnd);
        } else if (lParam == WM_RBUTTONUP) {
            POINT pt;
            GetCursorPos(&pt);
            HMENU hMenu = CreatePopupMenu();
            AppendMenuW(hMenu, MF_STRING, ID_TRAY_OPEN, L"Buka Key Remapper");
            AppendMenuW(hMenu, MF_STRING, ID_TRAY_TOGGLE, g_bHookActive ? L"Status: AKTIF (Klik untuk Matikan)" : L"Status: NONAKTIF (Klik untuk Nyalakan)");
            AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
            AppendMenuW(hMenu, MF_STRING, ID_TRAY_GITHUB, L"Kunjungi GitHub Dedyjagok");
            AppendMenuW(hMenu, MF_STRING, ID_TRAY_ABOUT, L"Tentang & Credits");
            AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
            AppendMenuW(hMenu, MF_STRING, ID_TRAY_EXIT, L"Keluar");

            SetForegroundWindow(hWnd);
            int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, hWnd, NULL);
            DestroyMenu(hMenu);

            if (cmd == ID_TRAY_OPEN) {
                ShowWindow(hWnd, SW_SHOW);
                SetForegroundWindow(hWnd);
            } else if (cmd == ID_TRAY_TOGGLE) {
                g_bHookActive = !g_bHookActive;
                UpdateStatusUI();
                SaveConfig();
            } else if (cmd == ID_TRAY_GITHUB) {
                ShellExecuteW(NULL, L"open", L"https://github.com/Dedyjagok", NULL, NULL, SW_SHOWNORMAL);
            } else if (cmd == ID_TRAY_ABOUT) {
                SendMessageW(hWnd, WM_COMMAND, MAKEWPARAM(IDC_BTN_ABOUT, 0), 0);
            } else if (cmd == ID_TRAY_EXIT) {
                DestroyWindow(hWnd);
            }
        }
        return 0;
    }

    case WM_CLOSE: {
        if (g_bMinToTrayOnClose) {
            ShowWindow(hWnd, SW_HIDE);
            static bool s_shownTrayNotice2 = false;
            if (!s_shownTrayNotice2) {
                s_shownTrayNotice2 = true;
                ShowTrayBalloon(L"Key Remapper Berjalan di Tray", L"Aplikasi disembunyikan ke area tray (dekat jam). Klik ikon untuk membuka kembali.");
            }
        } else {
            DestroyWindow(hWnd);
        }
        return 0;
    }

    case WM_DESTROY: {
        Shell_NotifyIconW(NIM_DELETE, &g_nid);
        StopHook();
        SaveConfig();
        PostQuitMessage(0);
        return 0;
    }
    }
    return DefWindowProcW(hWnd, message, wParam, lParam);
}

// -----------------------------------------------------------------------------
// APPLICATION ENTRY POINT
// -----------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    g_hInstance = hInstance;

    // Initialize common controls (ListView, etc.)
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_LISTVIEW_CLASSES | ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icex);

    // Load saved configurations and rules
    LoadConfig();

    // Load Application Icon
    HICON hAppIcon = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_APP_ICON));
    if (!hAppIcon) {
        hAppIcon = LoadIconW(NULL, IDI_APPLICATION);
    }

    // Register Window Class
    const wchar_t CLASS_NAME[] = L"KeyRemapperMainWindowClass";
    WNDCLASSEXW wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = CLASS_NAME;
    wc.hIcon = hAppIcon;
    wc.hIconSm = (HICON)LoadImageW(hInstance, MAKEINTRESOURCEW(IDI_APP_ICON), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);

    if (!RegisterClassExW(&wc)) {
        MessageBoxW(NULL, L"Gagal mendaftarkan Window Class!", L"Error", MB_ICONERROR);
        return 1;
    }

    // Window dimensions
    int winWidth = 610;
    int winHeight = 465;
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int posX = (screenWidth - winWidth) / 2;
    int posY = (screenHeight - winHeight) / 2;

    g_hWndMain = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        CLASS_NAME,
        L"Key Remapper (Broken Ctrl Fix) - Khusus terutama laptop ACER YA& Windows 11",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        posX, posY, winWidth, winHeight,
        NULL, NULL, hInstance, NULL
    );

    if (!g_hWndMain) {
        MessageBoxW(NULL, L"Gagal membuat jendela utama!", L"Error", MB_ICONERROR);
        return 1;
    }

    // Start keyboard hook
    StartHook();

    // Check if started with "-tray" argument (e.g. from Windows Startup)
    bool startMinimized = (strstr(lpCmdLine, "-tray") != NULL);
    if (!startMinimized) {
        ShowWindow(g_hWndMain, nCmdShow);
        UpdateWindow(g_hWndMain);
    }

    // Main Message Loop
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return (int)msg.wParam;
}

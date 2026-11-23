// 星球大战：前线（2004）简体中文包 安装器  v1.0
// Win32 / 无第三方依赖 / 单文件（payload 附加在 exe 末尾）
#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")

#define ID_PATH     101
#define ID_BROWSE   102
#define ID_DETECT   103
#define ID_INSTALL  110
#define ID_RESTORE  111
#define ID_PROG     120
#define ID_STATUS   121
#define ID_RESULT   122
#define ID_INFO     123
#define ID_OPT_FULL 180
#define ID_OPT_UI   181
#define ID_OPT_MOV  182
#define ID_BAKMOV   171

static HWND g_hMain, g_hPath, g_hProg, g_hStatus, g_hResult, g_hInstall, g_hRestore;
static HINSTANCE g_hInst;
static int g_dpi = 96;
static HFONT g_font = 0;
static int S(int v) { return MulDiv(v, g_dpi, 96); }
static void set_font(HWND h) { if (g_font) SendMessageW(h, WM_SETFONT, (WPARAM)g_font, TRUE); }
static WCHAR g_self[MAX_PATH];

// ---------- 工具 ----------
static void str_copy(WCHAR* d, const WCHAR* s, int cap) {
    int i = 0; while (s[i] && i < cap - 1) { d[i] = s[i]; i++; } d[i] = 0;
}
static void str_cat(WCHAR* d, const WCHAR* s, int cap) {
    int n = lstrlenW(d), i = 0;
    while (s[i] && n + i < cap - 1) { d[n + i] = s[i]; i++; }
    d[n + i] = 0;
}
static int file_exists(const WCHAR* p) {
    DWORD a = GetFileAttributesW(p);
    return a != INVALID_FILE_ATTRIBUTES && !(a & FILE_ATTRIBUTE_DIRECTORY);
}
static unsigned __int64 file_size(const WCHAR* p) {
    WIN32_FILE_ATTRIBUTE_DATA d;
    if (!GetFileAttributesExW(p, GetFileExInfoStandard, &d)) return 0;
    return ((unsigned __int64)d.nFileSizeHigh << 32) | d.nFileSizeLow;
}
static void make_dirs(const WCHAR* path) {
    WCHAR buf[MAX_PATH]; str_copy(buf, path, MAX_PATH);
    for (int i = 3; buf[i]; i++) {
        if (buf[i] == L'\\') {
            buf[i] = 0;
            CreateDirectoryW(buf, NULL);
            buf[i] = L'\\';
        }
    }
}
static void log_line(const WCHAR* text) {
    WCHAR cur[4096]; GetWindowTextW(g_hResult, cur, 4096);
    WCHAR buf[4600];
    wsprintfW(buf, L"%s\r\n%s", cur, text);
    SetWindowTextW(g_hResult, buf);
}

// ---------- 内置 payload ----------
typedef struct {
    unsigned __int64 off, size;
    const WCHAR* rel;      // 目标相对路径
    const WCHAR* bak;      // 备份文件名
    const char*  label;
} PayloadFile;

static PayloadFile g_files[2] = {
    {0, 0, L"data\\_lvl_pc\\core.lvl",  L"data\\_lvl_pc\\core.lvl.original",  "core.lvl"},
    {0, 0, L"data\\_lvl_pc\\movies\\shell.mvs", L"data\\_lvl_pc\\movies\\shell.mvs.original", "shell.mvs"}
};
static int g_fileCount = 0;
static unsigned __int64 g_coreSize = 0;

static WCHAR g_payDir[MAX_PATH];

static int load_payload() {
    WCHAR d[MAX_PATH];
    str_copy(d, g_self, MAX_PATH);
    for (int i = (int)lstrlenW(d) - 1; i > 0; i--) {
        if (d[i] == L'\\') { d[i] = 0; break; }
    }
    str_copy(g_payDir, d, MAX_PATH);
    str_cat(g_payDir, L"\\payload", MAX_PATH);

    WCHAR p1[MAX_PATH], p2[MAX_PATH];
    str_copy(p1, g_payDir, MAX_PATH); str_cat(p1, L"\\core.lvl", MAX_PATH);
    str_copy(p2, g_payDir, MAX_PATH); str_cat(p2, L"\\shell.mvs", MAX_PATH);

    g_fileCount = 0;
    if (file_exists(p1)) { g_files[0].size = file_size(p1); g_coreSize = g_files[0].size; g_fileCount = 1; }
    if (file_exists(p2)) { g_files[1].size = file_size(p2); g_fileCount = 2; }
    return g_fileCount > 0;
}

// ---------- 路径与检测 ----------
static int join_path(const WCHAR* dir, const WCHAR* rel, WCHAR* out) {
    str_copy(out, dir, MAX_PATH);
    str_cat(out, L"\\", MAX_PATH);
    str_cat(out, rel, MAX_PATH);
    return file_exists(out);
}
static int valid_gamedata(const WCHAR* dir) {
    WCHAR p[MAX_PATH];
    return join_path(dir, L"data\\_lvl_pc\\core.lvl", p);
}
static void detect(const WCHAR* dir, WCHAR* out) {
    WCHAR core[MAX_PATH];
    if (!join_path(dir, L"data\\_lvl_pc\\core.lvl", core)) {
        str_copy(out, L"路径无效：找不到 data\\_lvl_pc\\core.lvl\r\n\r\n请选择 ...\\Star Wars - Battlefront\\GameData", 2048);
        return;
    }
    unsigned __int64 coreSz = file_size(core);
    WCHAR coreState[64];
    if (coreSz == g_files[0].size) str_copy(coreState, L"已汉化", 64);
    else str_copy(coreState, L"原版", 64);

    WCHAR movies[MAX_PATH];
    int hasMovies = join_path(dir, L"data\\_lvl_pc\\movies\\shell.mvs", movies);
    unsigned __int64 mvSz = hasMovies ? file_size(movies) : 0;
    int mvOk = (g_fileCount > 1 && mvSz == g_files[1].size);

    WCHAR bakCore[MAX_PATH], bakMv[MAX_PATH];
    int hasBakCore = join_path(dir, L"data\\_lvl_pc\\core.lvl.original", bakCore);
    int hasBakMv = join_path(dir, L"data\\_lvl_pc\\movies\\shell.mvs.original", bakMv);

    wsprintfW(out, L"目录：%s\r\n界面 core.lvl：%u 字节（%s）\r\n过场 shell.mvs：%u 字节%s\r\n备份：%s / %s",
              dir, (unsigned int)coreSz, coreState,
              (unsigned int)mvSz, hasMovies ? (mvOk ? L"（已汉化）" : L"（原版）") : L"（未找到）",
              hasBakCore ? L"有" : L"无", hasBakMv ? L"有" : L"无");
}

static void auto_detect_path() {
    const WCHAR* cands[] = {
        L"E:\\GOG Games\\Star Wars - Battlefront\\GameData",
        L"D:\\GOG Games\\Star Wars - Battlefront\\GameData",
        L"C:\\GOG Games\\Star Wars - Battlefront\\GameData",
        L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\Star Wars Battlefront (Classic 2004)\\GameData",
        L"D:\\SteamLibrary\\steamapps\\common\\Star Wars Battlefront (Classic 2004)\\GameData",
        L"E:\\SteamLibrary\\steamapps\\common\\Star Wars Battlefront (Classic 2004)\\GameData",
        L"C:\\Program Files\\LucasArts\\Star Wars Battlefront\\GameData",
        L"C:\\Program Files (x86)\\LucasArts\\Star Wars Battlefront\\GameData",
        L"E:\\LucasArts\\Star Wars Battlefront\\GameData",
    };
    for (int i = 0; i < (int)(sizeof(cands) / sizeof(cands[0])); i++) {
        if (valid_gamedata(cands[i])) { SetWindowTextW(g_hPath, cands[i]); return; }
    }
}

// ---------- 安装 / 还原 ----------
static void set_progress(int pct) {
    SendMessageW(g_hProg, PBM_SETPOS, (WPARAM)pct, 0);
}
static void pump() {
    MSG msg;
    while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) { TranslateMessage(&msg); DispatchMessageW(&msg); }
}
static int copy_file(const WCHAR* src, const WCHAR* dst, const WCHAR* label) {
    HANDLE hs = CreateFileW(src, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hs == INVALID_HANDLE_VALUE) return 0;
    make_dirs(dst);
    HANDLE hd = CreateFileW(dst, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    if (hd == INVALID_HANDLE_VALUE) { CloseHandle(hs); return 0; }
    unsigned __int64 total = file_size(src), done = 0;
    static unsigned char buf[1 << 18];
    DWORD rd = 0, wr = 0;
    int ok = 1;
    while (ReadFile(hs, buf, sizeof(buf), &rd, NULL) && rd > 0) {
        if (!WriteFile(hd, buf, rd, &wr, NULL) || wr != rd) { ok = 0; break; }
        done += rd;
        if (total) {
            set_progress((int)(done * 100 / total));
            SetWindowTextW(g_hStatus, label);
            pump();
        }
    }
    CloseHandle(hs); CloseHandle(hd);
    return ok;
}
static void refresh_buttons(const WCHAR* dir) {
    int ok = valid_gamedata(dir);
    EnableWindow(g_hInstall, ok ? TRUE : FALSE);
    int hasBak = 0;
    for (int i = 0; i < g_fileCount; i++) {
        WCHAR p[MAX_PATH];
        if (ok && join_path(dir, g_files[i].bak, p)) hasBak = 1;
    }
    EnableWindow(g_hRestore, hasBak ? TRUE : FALSE);
}

// 从自身 exe 的指定偏移流式写出，返回 1 成功
static int write_embedded(int idx, const WCHAR* dst, const WCHAR* label) {
    WCHAR srcp[MAX_PATH];
    str_copy(srcp, g_payDir, MAX_PATH);
    if (idx == 0) {
        unsigned __int64 cur = file_size(dst);
        WCHAR p12[MAX_PATH];
        str_copy(p12, g_payDir, MAX_PATH);
        str_cat(p12, L"\\core.lvl.12", MAX_PATH);
        // 1.2 原版 = 4,895,408；1.3 原版 = 4,899,596
        if (cur == 4895408ULL && file_exists(p12))
            str_cat(srcp, L"\\core.lvl.12", MAX_PATH);
        else
            str_cat(srcp, L"\\core.lvl", MAX_PATH);
    } else {
        str_cat(srcp, L"\\shell.mvs", MAX_PATH);
    }
    HANDLE hs = CreateFileW(srcp, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hs == INVALID_HANDLE_VALUE) return 0;
    make_dirs(dst);
    HANDLE hd = CreateFileW(dst, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    if (hd == INVALID_HANDLE_VALUE) { CloseHandle(hs); return 0; }
    static unsigned char buf[1 << 18];
    unsigned __int64 remain = g_files[idx].size, total = g_files[idx].size;
    DWORD rd = 0, wr = 0;
    int ok = 1;
    while (remain > 0) {
        DWORD want = remain > sizeof(buf) ? (DWORD)sizeof(buf) : (DWORD)remain;
        if (!ReadFile(hs, buf, want, &rd, NULL) || rd == 0) { ok = 0; break; }
        if (!WriteFile(hd, buf, rd, &wr, NULL) || wr != rd) { ok = 0; break; }
        remain -= rd;
        if (total) {
            set_progress((int)((total - remain) * 100 / total));
            SetWindowTextW(g_hStatus, label);
            pump();
        }
    }
    FlushFileBuffers(hd);
    CloseHandle(hs); CloseHandle(hd);
    return ok && remain == 0;
}

static void do_install(HWND hwnd) {
    WCHAR dir[MAX_PATH]; GetWindowTextW(g_hPath, dir, MAX_PATH);
    if (!valid_gamedata(dir)) { MessageBoxW(hwnd, L"请先选择正确的 GameData 目录。", L"提示", MB_ICONWARNING); return; }
    if (g_fileCount == 0) { MessageBoxW(hwnd, L"安装包数据损坏（找不到内置数据）。", L"错误", MB_ICONERROR); return; }

    int wantCore = 1, wantMovies = 1;
    if (SendMessageW(GetDlgItem(hwnd, ID_OPT_UI), BM_GETCHECK, 0, 0) == BST_CHECKED) wantMovies = 0;
    if (SendMessageW(GetDlgItem(hwnd, ID_OPT_MOV), BM_GETCHECK, 0, 0) == BST_CHECKED) wantCore = 0;
    int doBackup = (SendMessageW(GetDlgItem(hwnd, ID_BAKMOV), BM_GETCHECK, 0, 0) == BST_CHECKED);

    for (int i = 0; i < g_fileCount; i++) {
        if (i == 0 && !wantCore) continue;
        if (i == 1 && !wantMovies) continue;
        WCHAR dst[MAX_PATH], bak[MAX_PATH];
        if (!join_path(dir, g_files[i].rel, dst)) {
            WCHAR msg[512];
            wsprintfW(msg, L"找不到目标文件：%s", g_files[i].rel);
            MessageBoxW(hwnd, msg, L"错误", MB_ICONERROR);
            return;
        }
        // 备份（若不存在同名 .original）
        if (doBackup && !join_path(dir, g_files[i].bak, bak)) {
            WCHAR label[128];
            wsprintfW(label, L"正在备份 %S ...", g_files[i].label);
            SetWindowTextW(g_hStatus, label);
            set_progress(0); pump();
            if (!copy_file(dst, bak, label)) {
                MessageBoxW(hwnd, L"备份失败：可能没有写入权限，请用管理员身份运行。", L"错误", MB_ICONERROR);
                return;
            }
            log_line(L"已备份原版文件");
        }
        WCHAR label[128];
        wsprintfW(label, L"正在写入 %S ...", g_files[i].label);
        SetWindowTextW(g_hStatus, label);
        set_progress(0); pump();
        if (!write_embedded(i, dst, label)) {
            MessageBoxW(hwnd, L"写入失败：请用管理员身份运行。", L"错误", MB_ICONERROR);
            return;
        }
    }
    set_progress(100);
    SetWindowTextW(g_hStatus, L"安装完成");
    log_line(L"安装完成：界面 + 过场字幕");
    refresh_buttons(dir);
    PostMessageW(hwnd, WM_COMMAND, (WPARAM)ID_DETECT, 0);
    MessageBoxW(hwnd, L"汉化安装完成！\r\n\r\n界面已汉化，过场动画带中文字幕。\r\n如需还原，点击「还原原版」。", L"完成", MB_ICONINFORMATION);
}

static void do_restore(HWND hwnd) {
    WCHAR dir[MAX_PATH]; GetWindowTextW(g_hPath, dir, MAX_PATH);
    int any = 0;
    for (int i = 0; i < g_fileCount; i++) {
        WCHAR b[MAX_PATH];
        if (join_path(dir, g_files[i].bak, b)) any = 1;
    }
    if (!any) { MessageBoxW(hwnd, L"该目录下没有备份文件，无法还原。", L"提示", MB_ICONWARNING); return; }
    if (MessageBoxW(hwnd, L"将用备份覆盖当前文件，恢复原版界面与影片。\r\n确定继续吗？", L"还原原版", MB_YESNO | MB_ICONQUESTION) != IDYES) return;

    for (int i = 0; i < g_fileCount; i++) {
        WCHAR dst[MAX_PATH], bak[MAX_PATH];
        if (!join_path(dir, g_files[i].bak, bak)) continue;
        if (!join_path(dir, g_files[i].rel, dst)) continue;
        WCHAR label[128];
        wsprintfW(label, L"正在还原 %S ...", g_files[i].label);
        SetWindowTextW(g_hStatus, label);
        set_progress(0); pump();
        if (!copy_file(bak, dst, label)) {
            MessageBoxW(hwnd, L"还原失败：请用管理员身份运行。", L"错误", MB_ICONERROR);
            return;
        }
    }
    set_progress(100);
    SetWindowTextW(g_hStatus, L"已还原为原版");
    log_line(L"已还原为原版文件");
    refresh_buttons(dir);
    PostMessageW(hwnd, WM_COMMAND, (WPARAM)ID_DETECT, 0);
}

// ---------- 窗口 ----------
static void browse_folder(HWND hwnd) {
    BROWSEINFOW bi; ZeroMemory(&bi, sizeof(bi));
    bi.hwndOwner = hwnd;
    bi.lpszTitle = L"请选择 GameData 文件夹";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);
    if (pidl) {
        WCHAR p[MAX_PATH] = {0};
        if (SHGetPathFromIDListW(pidl, p)) {
            SetWindowTextW(g_hPath, p);
            refresh_buttons(p);
            PostMessageW(hwnd, WM_COMMAND, (WPARAM)ID_DETECT, 0);
        }
        CoTaskMemFree(pidl);
    }
}
static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE: {
        g_dpi = 96;
        g_font = CreateFontW(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
                             CLEARTYPE_QUALITY, DEFAULT_PITCH, L"SimSun");
        if (!g_font) g_font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

        HWND h;
        h = CreateWindowW(L"STATIC", L"游戏目录（GameData）：", WS_CHILD | WS_VISIBLE, S(18), S(14), S(300), S(18), hwnd, NULL, g_hInst, NULL); set_font(h);
        g_hPath = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                                S(18), S(34), S(420), S(24), hwnd, (HMENU)ID_PATH, g_hInst, NULL); set_font(g_hPath);
        h = CreateWindowW(L"BUTTON", L"浏览…", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, S(446), S(34), S(70), S(24), hwnd, (HMENU)ID_BROWSE, g_hInst, NULL); set_font(h);
        h = CreateWindowW(L"BUTTON", L"检测版本", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, S(522), S(34), S(96), S(24), hwnd, (HMENU)ID_DETECT, g_hInst, NULL); set_font(h);

        h = CreateWindowW(L"STATIC", L"检测结果：", WS_CHILD | WS_VISIBLE, S(18), S(68), S(200), S(18), hwnd, NULL, g_hInst, NULL); set_font(h);
        g_hResult = CreateWindowW(L"EDIT", L"（点击右侧「检测版本」查看游戏状态）",
                                  WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_READONLY | WS_VSCROLL,
                                  S(18), S(88), S(600), S(74), hwnd, (HMENU)ID_RESULT, g_hInst, NULL); set_font(g_hResult);

        h = CreateWindowW(L"STATIC", L"安装范围：", WS_CHILD | WS_VISIBLE, S(18), S(172), S(200), S(18), hwnd, NULL, g_hInst, NULL); set_font(h);
        h = CreateWindowW(L"BUTTON", L"完整安装（界面 + 过场字幕）", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
                          S(18), S(192), S(230), S(20), hwnd, (HMENU)ID_OPT_FULL, g_hInst, NULL); set_font(h);
        h = CreateWindowW(L"BUTTON", L"仅界面汉化", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
                          S(18), S(216), S(150), S(20), hwnd, (HMENU)ID_OPT_UI, g_hInst, NULL); set_font(h);
        h = CreateWindowW(L"BUTTON", L"仅过场字幕", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
                          S(178), S(216), S(150), S(20), hwnd, (HMENU)ID_OPT_MOV, g_hInst, NULL); set_font(h);
        CheckRadioButton(hwnd, ID_OPT_FULL, ID_OPT_MOV, ID_OPT_FULL);


        h = CreateWindowW(L"BUTTON", L"安装前备份原版文件（约 +895 MB 磁盘）", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                          S(18), S(248), S(360), S(20), hwnd, (HMENU)ID_BAKMOV, g_hInst, NULL); set_font(h);
        SendMessageW(GetDlgItem(hwnd, ID_BAKMOV), BM_SETCHECK, BST_CHECKED, 0);

        g_hInstall = CreateWindowW(L"BUTTON", L"开始安装", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, S(18), S(280), S(110), S(30), hwnd, (HMENU)ID_INSTALL, g_hInst, NULL); set_font(g_hInstall);
        g_hRestore = CreateWindowW(L"BUTTON", L"还原原版", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, S(138), S(280), S(110), S(30), hwnd, (HMENU)ID_RESTORE, g_hInst, NULL); set_font(g_hRestore);
        g_hStatus = CreateWindowW(L"STATIC", L"请选择游戏目录后点击「检测版本」", WS_CHILD | WS_VISIBLE, S(268), S(286), S(400), S(20), hwnd, (HMENU)ID_STATUS, g_hInst, NULL); set_font(g_hStatus);

        g_hProg = CreateWindowExW(0, PROGRESS_CLASSW, NULL, WS_CHILD | WS_VISIBLE | PBS_SMOOTH | WS_BORDER, S(18), S(320), S(600), S(16), hwnd, (HMENU)ID_PROG, g_hInst, NULL);
        h = CreateWindowW(L"STATIC", L"v1.0  ·  界面 / 字体 / 过场字幕  ·  Windows XP SP3+", WS_CHILD | WS_VISIBLE, S(18), S(348), S(500), S(18), hwnd, NULL, g_hInst, NULL); set_font(h);

        auto_detect_path();
        {
            WCHAR dir[MAX_PATH]; GetWindowTextW(g_hPath, dir, MAX_PATH);
            refresh_buttons(dir);
        }
        return 0;
    }
    case WM_COMMAND:
        if (LOWORD(wp) == ID_BROWSE) { browse_folder(hwnd); return 0; }
        if (LOWORD(wp) == ID_DETECT) {
            WCHAR dir[MAX_PATH], out[2048];
            GetWindowTextW(g_hPath, dir, MAX_PATH);
            detect(dir, out);
            SetWindowTextW(g_hResult, out);
            refresh_buttons(dir);
            return 0;
        }
        if (LOWORD(wp) == ID_INSTALL) { do_install(hwnd); return 0; }
        if (LOWORD(wp) == ID_RESTORE) { do_restore(hwnd); return 0; }
        return 0;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, int) {
    g_hInst = hInst;

    GetModuleFileNameW(NULL, g_self, MAX_PATH);
    INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_PROGRESS_CLASS | ICC_STANDARD_CLASSES };
    InitCommonControlsEx(&icc);
    CoInitialize(NULL);
    load_payload();

    WNDCLASSEXW wc; ZeroMemory(&wc, sizeof(wc));
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = L"SWBF1CNInstaller";
    RegisterClassExW(&wc);

    g_dpi = 96;
    int w = S(650), h = S(432);
    int x = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
    int y = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;
    g_hMain = CreateWindowExW(0, L"SWBF1CNInstaller", L"星球大战：前线 简体中文包 安装器 v1.0",
                              WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                              x, y, w, h, NULL, NULL, hInst, NULL);
    SetWindowPos(g_hMain, NULL, 120, 100, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    ShowWindow(g_hMain, SW_SHOW);
    UpdateWindow(g_hMain);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0) { TranslateMessage(&msg); DispatchMessageW(&msg); }
    CoUninitialize();
    return 0;
}
#include "inputswitcher.h"
#include <QCoreApplication>
#include <QDebug>
#include <Psapi.h>
#include <chrono>

InputSwitcher *InputSwitcher::instance = nullptr;

InputSwitcher::InputSwitcher(QObject *parent)
    : QObject(parent)
    , mouseHook(nullptr)
    , lastClickTime(0)
    , clickCount(0)
{
    instance = this;

    qDebug() << "========================================";
    qDebug() << "输入法自动切换程序已启动";
    qDebug() << "箭头光标 -> 自动切换英语";
    qDebug() << "I型光标+双击 -> 切换中文";
    qDebug() << "排除进程: obsidian.exe, firefox.exe";
    qDebug() << "========================================";

    installMouseHook();
}

InputSwitcher::~InputSwitcher()
{
    uninstallMouseHook();
}

void InputSwitcher::installMouseHook()
{
    mouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseProc, GetModuleHandle(nullptr), 0);

    if (!mouseHook) {
        qCritical() << "错误: 无法安装鼠标钩子！";
        QCoreApplication::quit();
    } else {
        qDebug() << "鼠标钩子安装成功";
    }
}

void InputSwitcher::uninstallMouseHook()
{
    if (mouseHook) {
        UnhookWindowsHookEx(mouseHook);
        mouseHook = nullptr;
        qDebug() << "鼠标钩子已卸载";
    }
}

LRESULT CALLBACK InputSwitcher::mouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0) {
        if (wParam == WM_MOUSEMOVE || wParam == WM_LBUTTONDOWN) {
            if (instance) {
                instance->handleMouseEvent(wParam);
            }
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

void InputSwitcher::handleMouseEvent(WPARAM eventType)
{
    // 获取当前进程名
    QString processName = getCurrentProcessName();

    // 检查是否为排除的进程
    if (isExcludedProcess(processName)) {
        return;
    }

    // 获取光标类型
    int cursorType = getCursorType();

    static int lastCursorType = 0;

    // 箭头光标：鼠标移动时自动切换
    if (eventType == WM_MOUSEMOVE && cursorType == 32512) { // IDC_ARROW
        if (cursorType != lastCursorType) {
            switchToEnglish();
            lastCursorType = cursorType;
        }
    }
    // I型光标：双击鼠标左键时切换
    else if (eventType == WM_LBUTTONDOWN && cursorType == 32513) { // IDC_IBEAM
        // 获取当前时间（毫秒）
        auto now = std::chrono::steady_clock::now();
        auto currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                               now.time_since_epoch()).count();

        // Windows 双击时间间隔（默认500ms）
        UINT doubleClickTime = GetDoubleClickTime();

        // 判断是否为双击
        if (currentTime - lastClickTime <= doubleClickTime) {
            clickCount++;
            if (clickCount >= 2) {
                switchToChinese();
                clickCount = 0;
            }
        } else {
            clickCount = 1;
        }

        lastClickTime = currentTime;
        lastCursorType = cursorType;
    }
    // 其他情况更新光标类型
    else if (eventType == WM_MOUSEMOVE && cursorType != lastCursorType) {
        lastCursorType = cursorType;
    }
}

QString InputSwitcher::getCurrentProcessName()
{
    HWND hwnd = GetForegroundWindow();
    if (!hwnd) {
        return QString();
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (!hProcess) {
        return QString();
    }

    wchar_t processPath[MAX_PATH];
    DWORD size = MAX_PATH;

    if (QueryFullProcessImageNameW(hProcess, 0, processPath, &size)) {
        QString fullPath = QString::fromWCharArray(processPath);
        QString processName = fullPath.split('\\').last().toLower();
        CloseHandle(hProcess);
        return processName;
    }

    CloseHandle(hProcess);
    return QString();
}

bool InputSwitcher::isExcludedProcess(const QString &processName)
{
    static QStringList excludedProcesses = {"obsidian.exe", "firefox.exe"};
    return excludedProcesses.contains(processName.toLower());
}

int InputSwitcher::getCursorType()
{
    CURSORINFO cursorInfo;
    cursorInfo.cbSize = sizeof(CURSORINFO);

    if (!GetCursorInfo(&cursorInfo)) {
        return 0;
    }

    HCURSOR hCursor = cursorInfo.hCursor;
    HCURSOR hArrow = LoadCursor(nullptr, IDC_ARROW);
    HCURSOR hIBeam = LoadCursor(nullptr, IDC_IBEAM);

    if (hCursor == hArrow) {
        return 32512; // IDC_ARROW
    } else if (hCursor == hIBeam) {
        return 32513; // IDC_IBEAM
    }

    return 0;
}

void InputSwitcher::switchToEnglish()
{
    HWND hwnd = GetForegroundWindow();
    if (hwnd) {
        // 英语(美国) 键盘布局标识符
        HKL hkl = (HKL) 0x04090409;
        PostMessage(hwnd, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM) hkl);
    }
}

void InputSwitcher::switchToChinese()
{
    HWND hwnd = GetForegroundWindow();
    if (hwnd) {
        // 中文(简体) 键盘布局标识符
        HKL hkl = (HKL) 0x08040804;
        PostMessage(hwnd, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM) hkl);
    }
}

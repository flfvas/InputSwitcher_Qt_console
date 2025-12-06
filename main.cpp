// 这个版本是切换为中文需要点击, 切换为英文不需要点击
// Console 版本，运行时不显示控制台窗口
#include <QCoreApplication>
#include "inputswitcher.h"

#ifdef _WIN32
#include <Windows.h>
#endif

int main(int argc, char *argv[])
{
#ifdef _WIN32
    // 隐藏控制台窗口（备用方案，主要依靠 CMake 配置）
    HWND hwnd = GetConsoleWindow();
    if (hwnd != nullptr) {
        ShowWindow(hwnd, SW_HIDE);
    }
#endif

    QCoreApplication app(argc, argv);

    InputSwitcher switcher;

    return app.exec();
}

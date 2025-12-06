#ifndef INPUTSWITCHER_H
#define INPUTSWITCHER_H

#include <QObject>
#include <Windows.h>

class InputSwitcher : public QObject
{
    Q_OBJECT

public:
    explicit InputSwitcher(QObject *parent = nullptr);
    ~InputSwitcher();

private:
    long long lastClickTime;  // 上次点击时间
    int clickCount;           // 点击计数

private:
    void installMouseHook();
    void uninstallMouseHook();
    static LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam);

    void handleMouseEvent(WPARAM eventType);
    QString getCurrentProcessName();
    bool isExcludedProcess(const QString &processName);
    int getCursorType();
    void switchToEnglish();
    void switchToChinese();

private:
    static InputSwitcher *instance;
    HHOOK mouseHook;
};

#endif // INPUTSWITCHER_H

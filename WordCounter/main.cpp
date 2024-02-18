#include "counter.h"
#include "history.h"

#include <QApplication>
#include<QGuiApplication>
#include <QLocale>
#include <Windows.h>
#include <iostream>

Counter* counter;
HHOOK _hook;
KBDLLHOOKSTRUCT kbdStruct;
LRESULT __stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0)
    {
        // the action is valid: HC_ACTION.
        if (wParam == WM_KEYDOWN)
        {
            // lParam is the pointer to the struct containing the data needed, so cast and assign it to kdbStruct.
            kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);
            // a key (non-system) is pressed.

            if(!counter->in_word && kbdStruct.vkCode >= 0x30 && kbdStruct.vkCode <= 0x5A)
                counter->in_word = true;

            if (kbdStruct.vkCode == VK_SPACE || kbdStruct.vkCode == VK_RETURN)
            {
                if(counter && counter->in_word)
                    counter->increment();
                counter->in_word = false;
            }
        }
    }

    // call the next hook in the hook chain. This is nessecary or your hook chain will break and the hook stops
    return CallNextHookEx(_hook, nCode, wParam, lParam);
}
void SetHook()
{
    if (!(_hook = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, NULL, 0)))
    {
        std::cout << "ERROR\n";
    }
}

void ReleaseHook()
{
    UnhookWindowsHookEx(_hook);
}


int main (int argc, char *argv[])
{
    SetHook();
    QApplication a(argc, argv);

    counter = new Counter();
    History* hist = new History();

    counter->hist = hist;
    hist->counter = counter;

    counter->show();
    hist->LoadHist();
    hist->LoadChart();

    return a.exec();
}

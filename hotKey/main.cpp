// hotKey.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "hotkeys.h"
#include "functions.h"


int main(int argc, char* argv[])
{
    bool isPause = false;
    std::vector<CHotkey> hKeys;
    std::vector<CHotkey> systemHotkeys;
    audiere::AudioDevicePtr device = audiere::OpenDevice();

    if(!device)
        return writeErrorMessage("Error creating AudioDevice", 1);
    if((!readTimers(systemHotkeys, "manageKeys.txt")) || (!readTimers(hKeys, "items.txt")))
        return writeErrorMessage("", 2);
    if(!checkSoundFile(systemHotkeys) || (!checkSoundFile(hKeys)))
        return writeErrorMessage("", 3);
    if((!loadSounds(systemHotkeys, device)) || (!loadSounds(hKeys, device)))
        return writeErrorMessage("Sounds incorrect", 4);
    if(!registerTimers(systemHotkeys) || (!registerTimers(hKeys)))
        return writeErrorMessage("Not all buttons registered successfully", 5);

    MSG msg = {0};
    std::cout << "\nTimers loaded, have fun ;)\n";

    while(GetMessage(&msg, nullptr, 0, 0) != 0)
    {
        if(msg.message == WM_HOTKEY)
        {

            for(auto& k : hKeys)
            {
                if(k.id == msg.wParam)
                    k.action(isPause, false);
            }
            for(auto& sysKey : systemHotkeys)
                if(msg.wParam == sysKey.id)
                    if (sysKey.systemAction(isPause, hKeys))
                        return writeErrorMessage("System stopped, good bye :)\n", 0);

        }
    }
    return 0;
}
#pragma once
#include "stdafx.h"
#include "hotkeys.h"
#include "functions.h"

void timerF(CHotkey* hKey)
{
    unsigned int curThread = hKey->threads;
    while(1)
    {
        if(curThread < hKey->threads)
            break;
        if(hKey->curSec >= hKey->spawnTime)
        {
            hKey->curSec = 0.0;
            hKey->isWarned = false;
            hKey->activeSound->play();
            continue;
        }
        if((hKey->curSec >= (hKey->spawnTime - hKey->warningSeconds)) && (!hKey->isWarned) && (hKey->warning))
        {
            hKey->warningSound->play();
            hKey->isWarned = true;
        }
        Sleep(99);
        hKey->curSec += 0.1;

    }
}

void CHotkey::action(bool isPause, bool fromPause)
{
    if(!isPause)
    {
        if(!fromPause)
        {
            this->curSec = 0.0;
            this->activeSound->play();
            this->isWarned = false;
        }
        this->isInit = true;
        ++threads;
        std::thread t(timerF, this);
        t.detach();
    }
}

bool CHotkey::systemAction(bool& isPause, std::vector<CHotkey>& hotkeys)
{
    if(name == "[systemPause]\n")
        if(isPause)
        {
            isPause = false;
            this->activeSound->play();
            if(registerTimers(hotkeys))
            {
                for(auto& i : hotkeys)
                    if(i.isInit)
                        i.action(isPause, true);
                std::cout << "Unpaused\n";
            }
            else
                return true;
        }
        else
        {
            isPause = true;
            this->warningSound->play();
            int unRegisteredCount = 0;
            for(auto& hk : hotkeys)
            {
                ++hk.threads;
                if(UnregisterHotKey(nullptr, hk.id))
                    ++unRegisteredCount;
            }
            if(unRegisteredCount == hotkeys.size())
                std::cout << "Paused\n";
            else
                return true;
        }
    else if(name == "[systemTimersClear]\n")
    {
        this->activeSound->play();
        for(auto& hk : hotkeys)
        {
            ++hk.threads;
            hk.curSec = 0.0;
            hk.isInit = false;
        }
    }
    else if(name == "[systemQuiteClose]\n")
    {
        for(auto& hk : hotkeys)
        {
            ++hk.threads;
            UnregisterHotKey(nullptr, hk.id);
        }
        return true;
    }
    return false;
}
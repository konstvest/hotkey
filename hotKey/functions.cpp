#pragma once
#include "stdafx.h"
#include "functions.h"

#define MODS(alt, shift, ctrl) (alt)|(shift)|(ctrl)

/// <summary>
/// Fills the timer.
/// </summary>
/// <param name="hk"> in out. Current hot-key structure </param>
/// <param name="before"> in. The string before '=' </param>
/// <param name="after"> in. The string after '=' </param>
/// <remarks> Author e-mail: konstvest@gmail.com </remarks>
void fillTimer(CHotkey& hk, const char* before, const char* after)
{
    if(strcmp("Shift", before) == 0)
        if(strcmp("true", after) == 0)
            hk.shift = true;
        else
            hk.shift = false;
    else if(strcmp("Ctrl", before) == 0)
        if(strcmp("true", after) == 0)
            hk.ctrl = true;
        else
            hk.ctrl = false;
    else if(strcmp("Alt", before) == 0)
        if(strcmp("true", after) == 0)
            hk.alt = true;
        else
            hk.alt = false;
    else if(strcmp("Button", before) == 0)
        hk.code = VkKeyScanA(after[0]);
    else if(strcmp("SpawnTime(seconds)", before) == 0)
        hk.spawnTime = atof(after);
    else if(strcmp("ActivationSoundPath", before) == 0)
        hk.activePath = after;
    else if(strcmp("ActivationVolume", before) == 0)
        hk.activationVolume = atof(after);
    else if(strcmp("WarningVolume", before) == 0)
        hk.warningVolume     = atof(after);
    else if(strcmp("Warning", before) == 0)
        if(strcmp("true", after) == 0)
            hk.warning = true;
        else
            hk.warning = false;
    else if(strcmp("Warning(seconds)", before) == 0)
        hk.warningSeconds = atof(after);
    else if(strcmp("WarningSoundPath", before) == 0)
        hk.warningPath = after;
}

/// <summary>
/// Parse configuration file by line
/// </summary>
/// <param name="aHotkey"> in. array of hot-keys </param>
/// <returns> False if file not found </returns>
/// <remarks> Author e-mail: konstvest@gmail.com </remarks>
bool readTimers(std::vector<CHotkey>& aHotkey, const char* filename)
{
    FILE* pFile = nullptr;
    fopen_s(&pFile, filename, "r");
    if(!pFile)
    {
        std::cout << filename << " file not found\n";
        return false;
    }

    char str[2550];
    std::ostringstream parseBefore;
    std::ostringstream parseAfter;
    char* pEqual = nullptr;
    int pos;
    while(!feof(pFile))
    {
        fgets(str, sizeof(str), pFile);
        pEqual = strchr(str, '#');
        if(pEqual)
            continue;
        if(str[0] == '[')
        {
            CHotkey hotkey;
            hotkey.name = str;
            if(strstr(str, "system"))
                hotkey.id = aHotkey.size() + 2;
            else
                hotkey.id = aHotkey.size() + 50;
            hotkey.curSec = 0.0;
            aHotkey.push_back(hotkey);
            continue;
        }
        pEqual = strchr(str, '=');
        if(pEqual)
        {
            pos = pEqual - str;
            int j = 0;
            while(str[j] != '\n')
            {
                if(str[j] == '=')
                {
                    ++j;
                    continue;
                }
                if(j < pos)
                    parseBefore << str[j];
                else
                    parseAfter << str[j];
                ++j;
            }
            fillTimer(aHotkey.back(), parseBefore.str().c_str(), parseAfter.str().c_str());
            parseBefore.str(std::string());
            parseAfter.str(std::string());
        }
    }
    std::cout << "file " << filename << " is OK\n";
    return true;
}

bool readSoundFile(const char* path, const char* timerName, const char* typeSound)
{
    if(strcmp(path, "empty"))
    {
        FILE* pFile = nullptr;
        pFile = fopen(path, "r");
        if(!pFile)
        {
            std::cout << "can't read " << typeSound << " sound file: "
                << path << " in timer: " << timerName << std::endl;
            return false;
        }
        fclose(pFile);
    }
    return true;
}

/// <summary>
/// Checks the sound files using in timers.
/// </summary>
/// <param name="aHotkey"> in. array of hot-keys.</param>
/// <returns> True if all files readable </returns>
/// <remarks> Author e-mail: konstvest@gmail.com </remarks>
bool checkSoundFile(std::vector<CHotkey>& aHotkey)
{

    for(auto& i : aHotkey)
    {
        if(!readSoundFile(i.activePath.c_str(), i.name.c_str(), "activation"))
            return false;
        if(!readSoundFile(i.activePath.c_str(), i.name.c_str(), "warning"))
            return false;
    }
    return true;
}

/// <summary>
/// Loads the sounds in hot-key structure
/// </summary>
/// <param name="aHotkey"> in. Hot-key array </param>
/// <param name="device"> in. The sound device </param>
/// <returns> True if successfully loaded </returns>
/// <remarks> Author e-mail: konstvest@gmail.com </remarks>
bool loadSounds(std::vector<CHotkey>& aHotkey, audiere::AudioDevicePtr& device)
{
    for(auto& i : aHotkey)
    {
        if(!strcmp(i.activePath.c_str(), "empty"))
            continue;
        i.activeSound = audiere::OpenSound(device, i.activePath.c_str(), false);
        i.activeSound->setVolume(i.activationVolume);
        if(!strcmp(i.warningPath.c_str(), "empty"))
            continue;
        i.warningSound = audiere::OpenSound(device, i.warningPath.c_str(), false);
        i.warningSound->setVolume(i.warningVolume);
        if(!i.activeSound || !i.warningSound)
            return false;
    }
    return true;
}

/// <summary>
/// Registers the timers hot-key
/// </summary>
/// <param name="aHotkey"> in. Array of hot-keys </param>
/// <returns> True if all buttons registered </returns>
/// <remarks> Author e-mail: konstvest@gmail.com </remarks>
bool registerTimers(std::vector<CHotkey>& aHotkey)
{
    int registeredCount = 0;
    for(auto& i : aHotkey)
    {
        if(RegisterHotKey(nullptr, i.id, MODS(i.alt ? MOD_ALT : 0, i.shift ? MOD_SHIFT : 0, i.ctrl ? MOD_CONTROL : 0), i.code))
        {
            std::cout << '\n' << i.name << char(i.code & 0xff) << " registered. modes: alt:";
            i.alt ? std::cout << "TRUE" << " shift:" : std::cout << "FALSE" << " shift:";
            i.shift ? std::cout << "TRUE" << " ctrl:" : std::cout << "FALSE" << " ctrl:";
            i.ctrl ? std::cout << "TRUE\n" : std::cout << "FALSE\n";
            ++registeredCount;
        }
    }
    if(registeredCount == aHotkey.size())
        return true;
    else
        return false;
}

int writeErrorMessage(const char* errString, const int errCode)
{
    std::cout << errString << std::endl;
    getchar();
    return errCode;
}
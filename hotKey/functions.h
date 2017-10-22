#pragma once
#include "stdafx.h"
#include "hotkeys.h"

void fillTimer(CHotkey& hk, const char* before, const char* after);
bool readTimers(std::vector<CHotkey>& aHotkey, const char* filename);
bool readSoundFile(const char* path, const char* timerName, const char* typeSound);
bool checkSoundFile(std::vector<CHotkey>& aHotkey);
bool loadSounds(std::vector<CHotkey>& aHotkey, audiere::AudioDevicePtr& device);
bool registerTimers(std::vector<CHotkey>& aHotkey);
int writeErrorMessage(const char* errString, const int errCode);
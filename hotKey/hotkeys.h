#pragma once
#include "stdafx.h"

struct CHotkey
{
    std::string name;
    int id;
    bool shift;
    bool ctrl;
    bool alt;
    short code;
    std::string activePath = "empty";
    std::string warningPath = "empty";
    audiere::OutputStreamPtr activeSound;
    audiere::OutputStreamPtr warningSound;
    float spawnTime;
    float activationVolume;
    float warningVolume;
    bool warning;
    float warningSeconds;
    float curSec;
    void action(bool isPause, bool fromPause);
    bool systemAction(bool& isPause, std::vector<CHotkey>& hotkeys);
    unsigned int threads = 0;
    bool isInit = false;
    bool isWarned = false;
};


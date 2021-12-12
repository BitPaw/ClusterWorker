#pragma once

#include "UserInterface.h"

class DeployWorkInfo
{
    public:
    char folderPath[260];
    BF::Server* ServerSystem = nullptr;
    UserInterface* UI = nullptr;
};
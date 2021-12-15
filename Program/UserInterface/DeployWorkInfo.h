#pragma once

#include "UserInterface.h"

class DeployWorkInfo
{
    public:
    char FolderPathSearchChunk[260];
    char FolderPath[260];
    BF::Server* ServerSystem = nullptr;
    ClientList* Clients = nullptr;    
    UserInterface* UI = nullptr;
};
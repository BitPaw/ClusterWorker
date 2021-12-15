#pragma once

#include "UserInterface.h"

class DeployWorkInfo
{
    public:
    char FolderPath[260];
    BF::Server* ServerSystem = nullptr;
    ClientList* Clients = nullptr;
};
#include <iostream>
#include <Network/Client.h>

#include "VSClientSystem.h"

using namespace BF;

//---<Variables>---
VSClientSystem _clientSystem;
//-----------------

int main(int parameterListSize, char** parameterList)
{
    // Setup
    _clientSystem.Initialisize();

    const char* ip = "127.0.0.1";
    const char* port = "25565";

    if (parameterListSize == 3)
    {
        
    }

    _clientSystem.StartConnectingLoop(ip, port);
  
    while (true)
    {    
        // Wait for input
    }    

    return EXIT_SUCCESS;
}
#include <iostream>

#include "VSClient.h"

using namespace BF;


//---<Makros>--------------------------
#define VSDefaultIP "localhost"
#define VSDefaultPort 25666

#define VSInvalidParameters -1
//-------------------------------------


//---<Variables>-----------------------
VS::VSClient _client;
//-------------------------------------

using namespace VS;

int main(int parameterListSize, char** parameterList)
{
    //---<Setup>---------------------------------------------------------------
    
    //-------------------------------------------------------------------------  

    //---<Parse Parameter>-----------------------------------------------------
    switch (parameterListSize)
    {
        case 1: // No Parameters
        {
            _client.IPSet(VSDefaultIP);
            _client.PortSet(VSDefaultPort);
            break;
        }
        case 3: // Parameter IP + Port
        {
            _client.IPSet(parameterList[1]);
            _client.PortSet(parameterList[2]);
            break;
        }

        case 0:
        default:
            return VSInvalidParameters;
    }
    //-------------------------------------------------------------------------  

    _client.StateChange(VSClientState::IDLE);

    do
    {
        switch (_client.State)
        {
            case VSClientState::IDLE:
            {
                // Try Connecting
                _client.StartConnectingLoop();
                break;                
            }            
            default:
            {
                // Do nothing at all
                break;
            }               
        }      

        // *Connected*

        // Send Data

    }
    while (_client.State != VS::VSClientState::Stop);
       
    return EXIT_SUCCESS;
}
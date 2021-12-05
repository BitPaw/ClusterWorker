#pragma once

#define VSClientIPLength 32

#include "VSClientState.h"
#include "VSMessageToken.h"

#include <ErrorCode.h>
#include <Network/Client.h>
#include <Network/ISocketListener.h>
#include <File/Program.h>
#include <ErrorCode.h>
#include <stdlib.h>
#include <stdio.h>

namespace VS
{
	struct VSClient : public BF::ISocketListener, public BF::ProgramExecuteResultListener
	{

		private:
		BF::Client _client;

		public:
		VSClientState State;
		char IP[VSClientIPLength];
		unsigned short Port;

		char TargetExecutableFilePath[260];
		FILE* _fileBuffer = nullptr;
		size_t FileBufferSize = 0;

		VSClient();

		void IPSet(const char *ip);
		void PortSet(const char* port);
		void PortSet(unsigned short port);

		bool IsRunning() { return State != VS::VSClientState::Stop; }
		void StateChange(VSClientState clientState);

		void StartConnectingLoop();
		void StartConnectingLoop(const char* ipString, const char* portString);


		//---<Events>----------------------------------------------------------
		void OnConnectionLinked(int socketID);
		void OnConnectionListening(int socketID);
		void OnConnectionEstablished(int socketID);
		void OnConnectionTerminated(int socketID);
		void OnMessageSend(BF::IOSocketMessage socketMessage);
		void OnMessageReceive(BF::IOSocketMessage socketMessage);
		//---------------------------------------------------------------------

		//---<Tokens>----------------------------------------------------------
		void SentMessageToken(VSMessageToken messageToken);
		void SentMessageToken(VSMessageToken* messageTokenList, size_t messageTokenListSize);
		void OnMessageTokenRecived(VSMessageToken messageToken, int socketID);
		//---------------------------------------------------------------------

		//---<External Execute>----------------------
		void ProgramExecute();
	
		void ExecutableFilePathSet(char* targetExecutableFilePath);

		void OnProgramExecuted(bool succesful, size_t returnResult, BF::ErrorCode errorCode);	
		//---------------------------------------------------------------------
	};
}
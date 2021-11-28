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

		VSClient();

		void IPSet(const char *ip);
		void PortSet(const char* port);
		void PortSet(unsigned short port);

		void StartConnectingLoop();
		void StartConnectingLoop(const char* ipString, const char* portString);

		void OnConnectionLinked(int socketID);
		void OnConnectionListening(int socketID);
		void OnConnectionEstablished(int socketID);
		void OnConnectionTerminated(int socketID);
		void OnMessageSend(int socketID, const char* message, size_t messageSize);
		void OnMessageReceive(int socketID, const char* message, size_t messageSize);

		bool IsRunning() { return State != VS::VSClientState::Stop; }

		void StateChange(VSClientState clientState);		


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
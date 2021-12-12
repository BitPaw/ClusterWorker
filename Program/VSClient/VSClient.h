#pragma once

#define VSClientIPLength 32

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
	struct VSClient :	public BF::ISocketListener, 
						public BF::IClientListener, 
						public BF::ProgramExecuteResultListener
	{
		private:
		BF::Client _client;

		public:
		VSMessageType State;
		char IP[VSClientIPLength];
		unsigned short Port;

		char TargetExecutableFilePath[260];
		char TargetExecutableInputFilePath[260];

		FILE* _fileBuffer = nullptr;
		size_t FileBufferSize = 0;

		VSClient();

		void IPSet(const char *ip);
		void PortSet(const char* port);
		void PortSet(unsigned short port);

		bool IsRunning() { return State != VS::VSMessageType::ShuttingDown && State != VS::VSMessageType::Invalid; }
		void StateChange(VSMessageType state);

		void StartConnectingLoop();
		void StartConnectingLoop(const char* ipString, const char* portString);


		bool RecieveFileChunk(char* input, size_t inputSize, FILE* file, size_t& currentSize);

		//---<Events>----------------------------------------------------------
		void OnConnectionLinked(const BF::IPAdressInfo& adressInfo);
		void OnConnectionListening(const BF::IPAdressInfo& adressInfo);
		void OnConnectionEstablished(const BF::IPAdressInfo& adressInfo);
		void OnConnectionTerminated(const BF::IPAdressInfo& adressInfo);
		void OnMessageSend(BF::IOSocketMessage socketMessage);
		void OnMessageReceive(BF::IOSocketMessage socketMessage);
		void OnConnectedToServer(BF::IPAdressInfo& adressInfo);
		void OnSocketCreating(const BF::IPAdressInfo& adressInfo, bool& use);
		void OnSocketCreated(const BF::IPAdressInfo& adressInfo, bool& use);
		//---------------------------------------------------------------------

		//---<Tokens>----------------------------------------------------------
		void SentMessageToken(VSMessageToken messageToken);
		void SentMessageToken(VSMessageToken* messageTokenList, size_t messageTokenListSize);
		void OnMessageTokenRecived(VSMessageToken messageToken, int socketID);
		//---------------------------------------------------------------------

		//---<External Execute>----------------------
		void ProgramExecute();
	
		void ExecutableFilePathSet(char* targetExecutableFilePath);

		void OnProgramExecuted(bool succesful, intptr_t returnResult, BF::ErrorCode errorCode);	
		//---------------------------------------------------------------------
	};
}
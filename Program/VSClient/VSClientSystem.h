#pragma once

#include <Network/ISocketListener.h>
#include <Network/Client.h>

class VSClientSystem : public BF::ISocketListener
{
	private: 
	BF::Client _client;

	public:
	void Initialisize();
	void StartConnectingLoop(const char* ipString, const char* portString);


	virtual void OnConnectionLinked(int socketID) override;
	virtual void OnConnectionListening(int socketID) override;
	virtual void OnConnectionEstablished(int socketID) override;
	virtual void OnConnectionTerminated(int socketID) override;
	virtual void OnMessageSend(int socketID, const char* message, size_t messageSize) override;
	virtual void OnMessageReceive(int socketID, const char* message, size_t messageSize) override;
};


#pragma once

enum class ServerState
{
	IDLE,
	ServerOpening,
	ServerOpened,
	DataDeploying,
	DataDeployed,
	Starting,
	Started
};
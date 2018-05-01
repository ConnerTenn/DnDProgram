
class NetworkManager;

#pragma once

#include <ws2tcpip.h>
#include <winsock2.h>
//#include <windows.h>
#include <iphlpapi.h>
#pragma comment(lib, "Ws2_32.lib")

#include "DnDProgram.h"
#include "World.h"
#include <vector>
#include <thread>



class NetworkManager
{
public:
	DnD::DnDProgram *Program;
	std::string NetIP = "192.168.0.118";
	std::string NetPort = "27015";
	std::thread ListenThread;
	bool Run = false;
	
	SOCKET ListenSocket = INVALID_SOCKET;
	//std::vector<DnD::Player *> PendingClients;

	NetworkManager(DnD::DnDProgram *program);
	~NetworkManager();

	void Initialize();
	void Shutdown();
	void ShutdownSocket(SOCKET socket);
	inline void ShutdownClientSocket(DnD::Player *player);
	static void Listen(NetworkManager *network, DnD::DnDProgram *program);
	static void ConnectToClient(NetworkManager *network, DnD::DnDProgram *program);

	void Heartbeat();

	//each have their own send and receive thread


	bool Receive(SOCKET socket, char *buffer, int size);
	bool ReceiveIdentifier(SOCKET socket, std::string &identifier, int &dataSize);

	bool Send(SOCKET socket, char *buffer, int size);
	bool Send(SOCKET socket, std::string identifier, char *buffer, int size);

};

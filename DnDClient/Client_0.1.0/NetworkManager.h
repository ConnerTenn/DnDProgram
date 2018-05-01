
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

#define NW_CLOSE 0xAA02


class NetworkManager
{
public:
	DnD::DnDProgram *Program;
	std::string NetIP = "192.168.0.118";
	std::string NetPort = "27015";
	std::thread ListenThread;
	bool Run = false;
	
	SOCKET ConnectSocket = INVALID_SOCKET;

	NetworkManager(DnD::DnDProgram *program);
	~NetworkManager();

	void Initialize();
	void Shutdown();
	void ShutdownSocket(SOCKET socket);
	static void Listen(NetworkManager *network, DnD::DnDProgram *program);


	bool Receive(SOCKET socket, char *buffer, int size);
	bool ReceiveIdentifier(SOCKET socket, std::string &identifier, int &dataSize);

	bool Send(SOCKET socket, char *buffer, int size);
	bool Send(SOCKET socket, std::string identifier, char *buffer, int size);
	bool Send2(SOCKET socket, std::string identifier, char *buffer, int size);
	//void Send(DnD::Player *player, std::string identifier, char * buffer, int size);

};

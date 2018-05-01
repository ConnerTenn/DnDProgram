
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

#define NW_RECEIVE 0xAA03
#define NW_MESSAGE 0xAA04

struct MessageString
{
	std::string *String = 0;

	void Alloc(std::string str);
	void Free();
};

class NetworkManager
{
public:
	DnD::DnDProgram *Program;
	std::string NetIP = "192.168.0.118";
	std::string NetPort = "27015";
	//std::thread ListenThread;
	std::thread NetThread;
	bool Run = false;
	
	SOCKET ListenSocket = INVALID_SOCKET;
	//std::vector<DnD::Player *> PendingClients;


	WSAEVENT EventArray[WSA_MAXIMUM_WAIT_EVENTS];
	struct SocketInfo
	{
		SOCKET Socket;
		DnD::Player *Player;
		std::string Identifier;
		int IdentifierSize = 0;
		int DataSize = 0;
		int RecieveState = 0;
		char *Buf = 0;

		void AllocBuf(int size);
		void FreeBuf();
	} SocketArray[WSA_MAXIMUM_WAIT_EVENTS];
	DWORD EventTotal = 0;

	int RecieveState = 0;
	int IdentifierSize = 0;
	int DataSize = 0;


	NetworkManager(DnD::DnDProgram *program);
	~NetworkManager();

	void Initialize();
	void Shutdown();
	//void ShutdownSocket(SOCKET socket);
	//inline void ShutdownClientSocket(DnD::Player *player);
	//static void Listen(NetworkManager *network, DnD::DnDProgram *program);
	static void NetHandler(NetworkManager *network, DnD::DnDProgram *program);

	void Receive(int eventIndex, std::string identifier, char *data, int size);
	//static void ConnectToClient(SOCKET socket, NetworkManager *network, DnD::DnDProgram *program);

	//void Heartbeat();

	//each have their own send and receive thread
	//static void ClientRecieveThread(DnD::Player *player, NetworkManager *network, DnD::DnDProgram *program);

	bool RedoHash = false;
	bool HashInProgress = false;
	void UpdateWorldState();
	void SyncWorldState(DnD::Player *player, SOCKET socket);

	void WriteMessage(std::string message);

	bool CreateSocketInformation(SOCKET socket, DnD::Player *player);
	void FreeSocketInformation(DWORD Event);

	bool Receive(SOCKET socket, char *buffer, int size);
	bool ReceiveIdentifier(SOCKET socket, std::string &identifier, int &dataSize);
	bool Receive2(SOCKET socket, char *buffer, int size);
	bool ReceiveIdentifier2(SOCKET socket, std::string &identifier, int &dataSize);

	bool Send(SOCKET socket, char *buffer, int size);
	bool Send(SOCKET socket, std::string identifier, char *buffer, int size);
	bool Send2(SOCKET socket, std::string identifier, char *buffer, int size);

};


#include "NetworkManager.h"


NetworkManager::NetworkManager(DnD::DnDProgram *program)
{
	Program = program;
}

NetworkManager::~NetworkManager()
{
	//ShutdownClientSockets();
	Run = false;

	for (int i = 0; i < Program->GraphicWin.GameWorld->PlayerList.size(); i++)
	{
		ShutdownClientSocket(Program->GraphicWin.GameWorld->PlayerList[i]);
	}

	Shutdown();


	if (ListenThread.joinable()) { ListenThread.join(); }

	Program->ConsoleWin.CommandLine->Write("Network successfully shutdown\n");
}


void NetworkManager::Initialize()
{
	int iResult;
	WSADATA Wsdata;
	struct addrinfo *result = NULL, hints;

	//iResult = InitNetwork("192.168.0.162", "27015"); 

	iResult = WSAStartup(MAKEWORD(2, 2), &Wsdata);
	if (iResult != 0)
	{
		Program->ConsoleWin.CommandLine->Write("ERROR::WSAStartup Failed\n");
		return;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	Program->ConsoleWin.CommandLine->Write("Starting server: " + NetIP + ":" + NetPort + "\n");
	// Resolve the local address and port to be used by the server
	iResult = getaddrinfo(NetIP.c_str(), NetPort.c_str(), &hints, &result);
	if (iResult != 0)
	{
		Program->ConsoleWin.CommandLine->Write("ERROR::Failed to setup IP\n");
		WSACleanup();
		return;
	}

	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (iResult == INVALID_SOCKET)
	{
		Program->ConsoleWin.CommandLine->Write("ERROR::Faild to creaete listen socket\n");
		freeaddrinfo(result); 
		WSACleanup();
		return;
	}

	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		Program->ConsoleWin.CommandLine->Write("ERROR::Faild to bind socket:" + std::to_string(WSAGetLastError()) + "\n");
		closesocket(ListenSocket);
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	freeaddrinfo(result);

	Program->ConsoleWin.CommandLine->Write("Network successfully initilized\n");

	Run = true;
	ListenThread = std::thread(&Listen, this, Program);
}



void NetworkManager::Shutdown()
{
	int iResult;

	iResult = shutdown(ListenSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		Program->ConsoleWin.CommandLine->Write("ERROR::Faild to shutdown listen socket:" + std::to_string(WSAGetLastError()) + "\n");
	}

	// cleanup
	closesocket(ListenSocket);

	WSACleanup();
}

void NetworkManager::ShutdownSocket(SOCKET socket)
{
	int iResult;

	// shutdown the send half of the connection since no more data will be sent
	iResult = shutdown(socket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		Program->ConsoleWin.CommandLine->Write("ERROR::Faild to shutdown socket:" + std::to_string(WSAGetLastError()) + "\n");
	}

	// cleanup
	closesocket(socket);
}

void NetworkManager::ShutdownClientSocket(DnD::Player * player)
{
	ShutdownSocket(player->ClientSocket);
}


void NetworkManager::Listen(NetworkManager *network, DnD::DnDProgram *program)
{
	while (network->Run)
	{
		if (listen(network->ListenSocket, SOMAXCONN) == SOCKET_ERROR)
		{
			program->ConsoleWin.CommandLine->Write("ERROR::Listen failed with error:" + std::to_string(WSAGetLastError()) + "\n");
			closesocket(network->ListenSocket);
			WSACleanup();
			return;
		}

		ConnectToClient(network, program);
	}
}

void NetworkManager::ConnectToClient(NetworkManager *network, DnD::DnDProgram *program)
{
	SOCKET socket;
	
	socket = accept(network->ListenSocket, NULL, NULL);
	if (socket == INVALID_SOCKET)
	{
		program->ConsoleWin.CommandLine->Write("ERROR::Client accept failed:" + std::to_string(WSAGetLastError()) + "\n");
		network->ShutdownSocket(socket);
	}
	else
	{
		std::string name;

		//getname
		network->Send(socket, "GetName", 0, 0);
		std::string identifier; int size;
		network->ReceiveIdentifier(socket, identifier, size);

		char *buf = new char[size];
		network->Receive(socket, buf, size);
		name = CharToString(buf, size);
		delete[] buf;
		
		if (name != "All")
		{
			if (!program->GraphicWin.GameWorld->ConnectPlayer(name))
			{
				DnD::Player *player = new DnD::Player();
				player->Name = name;
				player->Connected = true;
				player->ClientSocket = socket;
				program->GraphicWin.GameWorld->AddPlayer(player);
			}

			program->ConsoleWin.CommandLine->Write("Connected to player:" + name + "\n");
			program->GraphicWin.WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
		}
		else
		{
			network->ShutdownSocket(socket);
		}
	}
}

void NetworkManager::Heartbeat()
{
	for (int i = 1; i < Program->GraphicWin.GameWorld->PlayerList.size(); i++)
	{
		DnD::Player *player = Program->GraphicWin.GameWorld->PlayerList[i];

		if (player->Connected)
		{
			Send(player->ClientSocket, "Heartbeat", 0, 0);

			std::string identifier; int size = 0;
			if (!(ReceiveIdentifier(player->ClientSocket, identifier, size) && identifier == "Heartbeat"))
			{
				player->Connected = false;
				player->ClientSocket = INVALID_SOCKET;
				Program->ConsoleWin.CommandLine->Write("Player disconnected:" + player->Name + "\n");
				Program->GraphicWin.WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
			}
		}
	}
}


bool NetworkManager::Receive(SOCKET socket, char *buffer, int size)
{
	int iResult;
	iResult = recv(socket, buffer, size, 0);
	if (iResult == SOCKET_ERROR)
	{
		Program->ConsoleWin.CommandLine->Write("ERROR::Recieve failed:" + std::to_string(WSAGetLastError()) + "\n");
		//ShutdownSocket(socket);
		//Run = false;
		//NetworkManager::~NetworkManager();
		//Program->ConsoleWin.WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
		return false;
	}
	return true;
}

bool NetworkManager::ReceiveIdentifier(SOCKET socket, std::string &identifier, int &dataSize)
{
	//receive size of identifier
	if (!Receive(socket, (char *)&dataSize, sizeof(int))) { return false; }
	char *buffer = new char[dataSize];

	//receive identifier
	if (!Receive(socket, buffer, dataSize)) { return false; }
	identifier = CharToString(buffer, dataSize);

	//receive size of data
	if (!Receive(socket, (char *)&dataSize, sizeof(int))) { return false; }

	delete[] buffer;

	return true;
}

bool NetworkManager::Send(SOCKET socket, char *buffer, int size)
{
	int iResult;

	iResult = send(socket, buffer, size, 0);
	if (iResult == SOCKET_ERROR)
	{
		Program->ConsoleWin.CommandLine->Write("ERROR::Send failed:" + std::to_string(WSAGetLastError()) + "\n");
		//ShutdownSocket(socket);
		//Run = false;
		//NetworkManager::~NetworkManager();
		return false;
	}
	return true;
}

bool NetworkManager::Send(SOCKET socket, std::string identifier, char *buffer, int size)
{
	int len = (int)identifier.size();
	if (!Send(socket, (char *)&len, sizeof(int))) { return false; }

	char *buf = new char[identifier.size()];
	StringToChar(identifier, &buf);
	if (!Send(socket, buf, (int)identifier.size())) { return false; }

	if (!Send(socket, (char *)&size, sizeof(int))) { return false; }
	if (size)
	{
		Send(socket, buffer, size);
	}

	return true;
}

/*
void NetworkManager::Send(DnD::Player *player, std::string identifier, char * buffer, int size)
{
	int iResult;

	char *buf = new char[sizeof(int) + sizeof(int) + identifier.size() + size];
	*((int *)buf) = (int)identifier.size();
	*((int *)buf + sizeof(int)) = size;
	for (int i = 0; i < identifier.size(); i++)
	{
		*(buf + sizeof(int) + sizeof(int) + i) = identifier[i];
	}
	for (int i = 0; i < size; i++)
	{
		*(buf + sizeof(int) + sizeof(int) + identifier.size() + i) = buffer[i];
	}

	iResult = send(player->ClientSocket, buf, sizeof(int) + sizeof(int) + (int)identifier.size() + size, 0);
}*/


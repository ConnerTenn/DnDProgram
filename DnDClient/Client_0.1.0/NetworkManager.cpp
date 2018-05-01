
#include "NetworkManager.h"


NetworkManager::NetworkManager(DnD::DnDProgram *program)
{
	Program = program;
}

NetworkManager::~NetworkManager()
{
	//Run = false;

	Shutdown();

	//if (ListenThread.joinable()) { ListenThread.join(); }

	//Program->ConsoleWin.CommandLine->Write("Network successfully shutdown\n");
	//Program->GraphicWin.WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
}


void NetworkManager::Initialize()
{
	/*int iResult;
	WSADATA Wsdata;
	struct addrinfo *result = NULL, hints;

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
	
	Program->ConsoleWin.CommandLine->Write("Connecting to: " + NetIP + ":" + NetPort + "\n");
	// Resolve the local address and port to be used by the server
	iResult = getaddrinfo(NetIP.c_str(), NetPort.c_str(), &hints, &result);
	if (iResult != 0)
	{
		Program->ConsoleWin.CommandLine->Write("ERROR::Failed to setup IP\n");
		WSACleanup();
		return;
	}

	ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (iResult == INVALID_SOCKET)
	{
		Program->ConsoleWin.CommandLine->Write("ERROR::Faild to create connect socket\n");
		freeaddrinfo(result); 
		WSACleanup();
		return;
	}

	iResult = connect(ConnectSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		Program->ConsoleWin.CommandLine->Write("ERROR::Faild to connect socket:" + std::to_string(WSAGetLastError()) + "\n");
		closesocket(ConnectSocket);
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	freeaddrinfo(result);

	Program->ConsoleWin.CommandLine->Write("Network successfully initilized\n");*/

	Run = false;
	Shutdown();
	if (ListenThread.joinable())
	{
		ListenThread.join();
	}

	ListenThread = std::thread(&Listen, this, Program);
}



void NetworkManager::Shutdown()
{
	Run = false;

	ShutdownSocket(ConnectSocket);

	WSACleanup();

	if (ListenThread.joinable()) { ListenThread.join(); }

	Program->ConsoleWin.CommandLine->Write("Network successfully shutdown\n");
	Program->GraphicWin.WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
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
	//shutdown(socket, 0);
}


void NetworkManager::Listen(NetworkManager *network, DnD::DnDProgram *program)
{
	int iResult;
	WSADATA Wsdata;
	struct addrinfo *result = NULL, hints;

	iResult = WSAStartup(MAKEWORD(2, 2), &Wsdata);
	if (iResult != 0)
	{
		program->ConsoleWin.CommandLine->Write("ERROR::WSAStartup Failed\n");
		return;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	program->ConsoleWin.CommandLine->Write("Connecting to: " + network->NetIP + ":" + network->NetPort + "\n");
	// Resolve the local address and port to be used by the server
	iResult = getaddrinfo(network->NetIP.c_str(), network->NetPort.c_str(), &hints, &result);
	if (iResult != 0)
	{
		program->ConsoleWin.CommandLine->Write("ERROR::Failed to setup IP\n");
		WSACleanup();
		return;
	}

	network->ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (iResult == INVALID_SOCKET)
	{
		program->ConsoleWin.CommandLine->Write("ERROR::Faild to create connect socket\n");
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	iResult = connect(network->ConnectSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		program->ConsoleWin.CommandLine->Write("ERROR::Faild to connect socket:" + std::to_string(WSAGetLastError()) + "\n");
		closesocket(network->ConnectSocket);
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	freeaddrinfo(result);

	program->ConsoleWin.CommandLine->Write("Network successfully initilized\n");

	network->Run = true;
	program->GraphicWin.WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);


	char *buffer = 0;
	int size;
	std::string identifier;

	while (network->Run)
	{
		if (network->ReceiveIdentifier(network->ConnectSocket, identifier, size))
		{

			if (identifier == "GetName")
			{
				buffer = new char[program->Name.size()];
				StringToChar(program->Name, buffer);
				network->Send2(network->ConnectSocket, "Name", buffer, (int)program->Name.size());
				delete[] buffer;
			}
			if (identifier == "Heartbeat")
			{
				network->Send2(network->ConnectSocket, "Heartbeat", 0, 0);
			}
			if (identifier == "Hash")
			{
				network->Send2(network->ConnectSocket, "Hash", 0, 0);
			}
		}
		else
		{
			network->Run = false;
			program->ConsoleWin.WindowPtr->AddMessage(MessageData({ NW_CLOSE, 0, 0 }), true);
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
		Program->ConsoleWin.WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
		return false;
	}
	return true;
}

bool NetworkManager::ReceiveIdentifier(SOCKET socket, std::string &identifier, int &dataSize)
{
	//receive size of identifier
	if (!Receive(socket, (char *)&dataSize, sizeof(int)) || dataSize < 0) { return false; }
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
	StringToChar(identifier, buf);
	if (!Send(socket, buf, (int)identifier.size())) { return false; }

	if (!Send(socket, (char *)&size, sizeof(int))) { return false; }
	if (size)
	{
		Send(socket, buffer, size);
	}

	return true;
}

bool NetworkManager::Send2(SOCKET socket, std::string identifier, char *buffer, int size)
{
	char *buf;
	
	buf = new char[sizeof(int) * 2];
	
	*(int *)buf = (int)identifier.size();
	*(int *)(buf + sizeof(int)) = size;

	if (!Send(socket, buf, sizeof(int) * 2)) { return false; }


	buf = new char[(int)identifier.size() + size];

	StringToChar(identifier, buf);
	for (int i = 0; i < size; i++)
	{
		(buf + (int)identifier.size())[i] = buffer[i];
	}

	if (!Send(socket, buf, (int)identifier.size() + size)) { return false; }

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


#include "NetworkManager.h"

/*
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
	///*int iResult;
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

	Program->ConsoleWin.CommandLine->Write("Network successfully initilized\n");/ * /

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
*/

void MessageString::Alloc(std::string str)
{
	String = new std::string;
	*String = str;
}

void MessageString::Free()
{
	if (String) { delete String; String = 0; }
}

void NetworkManager::SocketInfo::AllocBuf(int size)
{
	Buf = new char[size];
}

void NetworkManager::SocketInfo::FreeBuf()
{
	delete[] Buf;
}

NetworkManager::NetworkManager(DnD::DnDProgram *program)
{
	Program = program;
}

NetworkManager::~NetworkManager()
{
	//ShutdownClientSockets();
	Run = false;

	WSASetEvent(Event);
	if (NetThread.joinable()) { NetThread.join(); }

	Shutdown();

	Program->ConsoleWin.CommandLine->Write("Network successfully shutdown\n");
}


void NetworkManager::Initialize()
{
	Run = false;
	Shutdown();
	if (NetThread.joinable())
	{
		NetThread.join();
	}


	//int iResult;
	WSADATA Wsdata;
	struct addrinfo *info = NULL, hints;

	//iResult = InitNetwork("192.168.0.162", "27015"); 

	if (WSAStartup(MAKEWORD(2, 2), &Wsdata) != 0)
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
	//Program->ConsoleWin.CommandLine->Write("Starting server:  Port:" + NetPort + "\n");
	// Resolve the local address and port to be used by the server
	if (getaddrinfo(NetIP.c_str(), NetPort.c_str(), &hints, &info) != 0)
	{
		Program->ConsoleWin.CommandLine->Write("ERROR::Failed to setup IP\n");
		WSACleanup();
		return;
	}

	SocketData.Socket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
	if (SocketData.Socket == INVALID_SOCKET)
	{
		Program->ConsoleWin.CommandLine->Write("ERROR::Faild to create socket\n");
		freeaddrinfo(info);
		WSACleanup();
		return;
	}

	if (connect(SocketData.Socket, info->ai_addr, (int)info->ai_addrlen) == SOCKET_ERROR)
	{
		Program->ConsoleWin.CommandLine->Write("ERROR::Faild to connect socket:" + std::to_string(WSAGetLastError()) + "\n");
		closesocket(SocketData.Socket);
		freeaddrinfo(info);
		WSACleanup();
		return;
	}

	CreateSocketInformation();

	if (WSAEventSelect(SocketData.Socket, Event, FD_READ | FD_WRITE | FD_CLOSE | FD_CONNECT) == SOCKET_ERROR)
	{
		Program->ConsoleWin.CommandLine->Write("ERROR::WSAEventSelect() failed with error:" + std::to_string(WSAGetLastError()) + "\n");
	}

	freeaddrinfo(info);

	Program->ConsoleWin.CommandLine->Write("Network successfully initilized\n");

	
	NetThread = std::thread(&NetHandler, this, Program);
}

void NetworkManager::Shutdown()
{
	Run = false;
	WSASetEvent(Event);
	if (NetThread.joinable()) { NetThread.join(); }

	FreeSocketInformation();

	SocketData.Identifier = "";
	SocketData.IdentifierSize = 0;
	SocketData.DataSize = 0;
	SocketData.RecieveState = 0;
	SocketData.Buf = 0;

	WSACleanup();
}

void NetworkManager::NetHandler(NetworkManager *network, DnD::DnDProgram *program)
{
	network->Run = true;

	DWORD eventIndex;
	WSANETWORKEVENTS networkEvents;

	while (network->Run)
	{
		eventIndex = WSAWaitForMultipleEvents(1, &network->Event, FALSE, WSA_INFINITE, FALSE);
		if (eventIndex == WSA_WAIT_FAILED)
		{
			//program->ConsoleWin.CommandLine->Write("ERROR::WSA_WAIT_FAILED\n");
			network->WriteMessage("ERROR::WSA_WAIT_FAILED\n");
		}
		if (WSAEnumNetworkEvents(network->SocketData.Socket, network->Event, &networkEvents) == SOCKET_ERROR)
		{
			//program->ConsoleWin.CommandLine->Write("ERROR::WSAEnumNetworkEvents failed\n");
			network->WriteMessage("ERROR::WSAEnumNetworkEvents failed\n");
		}

		if (networkEvents.lNetworkEvents & FD_READ)
		{
			//program->ConsoleWin.CommandLine->Write("Read\n");
			network->WriteMessage("Read\n");

			SocketInfo &socketInfo = network->SocketData;
			DWORD received, flags = 0;
			WSABUF dataBuf;

			if (socketInfo.RecieveState == 0 && socketInfo.IdentifierSize == 0 && socketInfo.DataSize == 0)
			{
				char *buffer = new char[2 * sizeof(int)];
				dataBuf.buf = buffer;
				dataBuf.len = 2 * sizeof(int);
				WSARecv(socketInfo.Socket, &dataBuf, 1, &received, &flags, NULL, NULL);
				socketInfo.IdentifierSize = *(int *)(buffer);
				socketInfo.DataSize = *(int *)(buffer + sizeof(int));

				delete[] buffer;

				socketInfo.RecieveState = 1;

			}
			else if (socketInfo.RecieveState == 1)
			{
				char *buffer = new char[socketInfo.IdentifierSize + socketInfo.DataSize];
				socketInfo.AllocBuf(socketInfo.DataSize);

				dataBuf.buf = buffer;
				dataBuf.len = socketInfo.IdentifierSize + socketInfo.DataSize;
				WSARecv(socketInfo.Socket, &dataBuf, 1, &received, &flags, NULL, NULL);

				//network->Receive(eventIndex, CharToString(buffer, network->IdentifierSize), (buffer + network->IdentifierSize), network->DataSize);
				socketInfo.Identifier = CharToString(buffer, socketInfo.IdentifierSize);
				for (int i = 0; i < socketInfo.DataSize; i++)
				{
					socketInfo.Buf[i] = (buffer + socketInfo.IdentifierSize)[i];
				}

				delete[] buffer;

				socketInfo.RecieveState = 0;
				program->ConsoleWin.WindowPtr->AddMessage(MessageData({ NW_RECEIVE, 0, 0 }));
			}
			//program->ConsoleWin.CommandLine->Write(CharToString(buffer, received) + "\n");

		}
		if (networkEvents.lNetworkEvents & FD_WRITE)
		{
			//program->ConsoleWin.CommandLine->Write("DEBUG::Write\n");
			network->WriteMessage("DEBUG::Write\n");
		}
		if (networkEvents.lNetworkEvents & FD_CLOSE)
		{
			if (networkEvents.iErrorCode[FD_CLOSE_BIT] != 0)
			{
				//program->ConsoleWin.CommandLine->Write("ERROR::FD_CLOSE failed with error:" + std::to_string(networkEvents.iErrorCode[FD_CLOSE_BIT]) + "\n");
				network->WriteMessage("ERROR::FD_CLOSE failed with error:" + std::to_string(networkEvents.iErrorCode[FD_CLOSE_BIT]) + "\n");
			}

			//program->ConsoleWin.CommandLine->Write("Closing Socket\n");
			network->WriteMessage("Closing Socket\n");
			network->FreeSocketInformation();

			program->GraphicWin.WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
		}
	}
}

void NetworkManager::Receive(int eventIndex, std::string identifier, char *data, int size)
{
	char *buffer = 0;

	if (identifier == "GetName")
	{
		buffer = new char[Program->Name.size()];
		StringToChar(Program->Name, buffer);
		Send2(SocketData.Socket, "Name", buffer, (int)Program->Name.size());
		delete[] buffer;
	}
	if (identifier == "Heartbeat")
	{
		Send2(SocketData.Socket, "Heartbeat", 0, 0);
	}
	if (identifier == "Hash")
	{
		Send2(SocketData.Socket, "Hash", 0, 0);
	}
}

void NetworkManager::WriteMessage(std::string message)
{
	MessageString str; str.Alloc(message);
	//char *buf = new char[sizeof(MessageString)];
	//*(MessageString *)buf = str;
	WPARAM param = *(WPARAM *)&str; //*(WPARAM *)buf;
									//delete[] buf;
	Program->ConsoleWin.WindowPtr->AddMessage(MessageData({ NW_MESSAGE, param, 0 }), true);
}


bool NetworkManager::CreateSocketInformation()
{
	Event = WSACreateEvent();
	if (Event == WSA_INVALID_EVENT)
	{
		//Program->ConsoleWin.CommandLine->Write("ERROR::WSACreateEvent failed with error:" + std::to_string(WSAGetLastError()) + "\n");
		WriteMessage("ERROR::WSACreateEvent failed with error:" + std::to_string(WSAGetLastError()) + "\n");
		return false;
	}

	return true;
}

void NetworkManager::FreeSocketInformation()
{
	if (SocketData.Socket) { closesocket(SocketData.Socket); }

	//if (Event && !WSACloseEvent(Event))
	{
		//Program->ConsoleWin.CommandLine->Write("ERROR::WSACloseEvent failed\n");
	}
}

bool NetworkManager::Receive(SOCKET socket, char *buffer, int size)
{
	int iResult;
	iResult = recv(socket, buffer, size, 0);
	if (iResult == SOCKET_ERROR)
	{
		//Program->ConsoleWin.CommandLine->Write("ERROR::Recieve failed:" + std::to_string(WSAGetLastError()) + "\n");
		WriteMessage("ERROR::Recieve failed:" + std::to_string(WSAGetLastError()) + "\n");
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
		//Program->ConsoleWin.CommandLine->Write("ERROR::Send failed:" + std::to_string(WSAGetLastError()) + "\n");
		WriteMessage("ERROR::Send failed:" + std::to_string(WSAGetLastError()) + "\n");
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

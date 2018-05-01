
#include "NetworkManager.h"


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

	Shutdown();

}


void NetworkManager::Initialize()
{
	//int iResult;
	WSADATA Wsdata;
	struct addrinfo *info = NULL, hints;

	//iResult = InitNetwork("192.168.0.162", "27015"); 

	//iResult = WSAStartup(MAKEWORD(2, 2), &Wsdata);
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

	Program->ConsoleWin.CommandLine->Write("Starting server: " + NetIP + ":" + NetPort + "\n");
	//Program->ConsoleWin.CommandLine->Write("Starting server:  Port:" + NetPort + "\n");
	// Resolve the local address and port to be used by the server
	if (getaddrinfo(NetIP.c_str(), NetPort.c_str(), &hints, &info) != 0)
	{
		Program->ConsoleWin.CommandLine->Write("ERROR::Failed to setup IP\n");
		WSACleanup();
		return;
	}

	ListenSocket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
	if (ListenSocket == INVALID_SOCKET)
	{
		Program->ConsoleWin.CommandLine->Write("ERROR::Faild to creaete listen socket\n");
		freeaddrinfo(info);
		WSACleanup();
		return;
	}

	CreateSocketInformation(ListenSocket, 0);

	if (WSAEventSelect(ListenSocket, EventArray[EventTotal - 1], FD_ACCEPT|FD_CLOSE) == SOCKET_ERROR)
	{
		Program->ConsoleWin.CommandLine->Write("ERROR::WSAEventSelect failed with error:" + std::to_string(WSAGetLastError()) + "\n");
	}

	if (bind(ListenSocket, info->ai_addr, (int)info->ai_addrlen) == SOCKET_ERROR)
	{
		Program->ConsoleWin.CommandLine->Write("ERROR::Faild to bind socket:" + std::to_string(WSAGetLastError()) + "\n");
		closesocket(ListenSocket);
		freeaddrinfo(info);
		WSACleanup();
		return;
	}

	freeaddrinfo(info);

	if (listen(ListenSocket, 5) == SOCKET_ERROR)
	{
		Program->ConsoleWin.CommandLine->Write("ERROR::Listen failed with error:" + std::to_string(WSAGetLastError()) + "\n");
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}


	Program->ConsoleWin.CommandLine->Write("Network successfully initilized\n");

	Run = true;
	NetThread = std::thread(&NetHandler, this, Program);
}



void NetworkManager::Shutdown()
{
	Run = false;

	WSASetEvent(EventArray[0]);
	if (NetThread.joinable()) { NetThread.join(); }

	while(EventTotal)
	{
		FreeSocketInformation(EventTotal - 1);
		//EventTotal is decremented
		SocketArray[EventTotal].Identifier = "";
		SocketArray[EventTotal].IdentifierSize = 0;
		SocketArray[EventTotal].DataSize = 0;
		SocketArray[EventTotal].RecieveState = 0;
		SocketArray[EventTotal].Buf = 0;
	}

	WSACleanup();

	//Program->ConsoleWin.CommandLine->Write("Network successfully shutdown\n");
	WriteMessage("Network successfully shutdown\n");
	Program->GraphicWin.WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
}

void NetworkManager::NetHandler(NetworkManager *network, DnD::DnDProgram *program)
{
	DWORD eventIndex;
	WSANETWORKEVENTS networkEvents;

	while (network->Run)
	{
		eventIndex = WSAWaitForMultipleEvents(network->EventTotal, network->EventArray, FALSE, WSA_INFINITE, FALSE);
		if (eventIndex == WSA_WAIT_FAILED)
		{
			//program->ConsoleWin.CommandLine->Write("ERROR::WSA_WAIT_FAILED\n");
			network->WriteMessage("ERROR::WSA_WAIT_FAILED\n");
		}
		if (WSAEnumNetworkEvents(network->SocketArray[eventIndex - WSA_WAIT_EVENT_0].Socket, network->EventArray[eventIndex - WSA_WAIT_EVENT_0], &networkEvents) == SOCKET_ERROR)
		{
			//program->ConsoleWin.CommandLine->Write("ERROR::WSAEnumNetworkEvents failed\n");
			network->WriteMessage("ERROR::WSAEnumNetworkEvents failed\n");
		}

		if (networkEvents.lNetworkEvents & FD_ACCEPT)
		{
			if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
			{
				//program->ConsoleWin.CommandLine->Write("ERROR::Listen failed with error:" + std::to_string(networkEvents.iErrorCode[FD_ACCEPT_BIT]) + "\n");
				network->WriteMessage("ERROR::Listen failed with error:" + std::to_string(networkEvents.iErrorCode[FD_ACCEPT_BIT]) + "\n");
			}
			else
			{
				SOCKET newSocket = accept(network->SocketArray[eventIndex - WSA_WAIT_EVENT_0].Socket, NULL, NULL);
				if (newSocket == INVALID_SOCKET)
				{
					//program->ConsoleWin.CommandLine->Write("ERROR::accept failed with error:" + std::to_string(WSAGetLastError()) + "\n");
					network->WriteMessage("ERROR::accept failed with error:" + std::to_string(WSAGetLastError()) + "\n");
				}
				else
				{
					//ConnectToClient(newSocket, network, program);

					if (network->EventTotal > WSA_MAXIMUM_WAIT_EVENTS)
					{
						//program->ConsoleWin.CommandLine->Write("ERROR::Too many connections; closing socket\n");
						network->WriteMessage("ERROR::Too many connections; closing socket\n");
						closesocket(newSocket);
					}
					else
					{
						network->CreateSocketInformation(newSocket, 0);
						if (WSAEventSelect(newSocket, network->EventArray[network->EventTotal - 1], FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
						{
							//program->ConsoleWin.CommandLine->Write("ERROR::WSAEventSelect() failed with error:" + std::to_string(WSAGetLastError()) + "\n");
							network->WriteMessage("ERROR::WSAEventSelect() failed with error:" + std::to_string(WSAGetLastError()) + "\n");
						}
						//program->ConsoleWin.CommandLine->Write("Socket Connected\n");
						network->WriteMessage("Socket Connected\n");

						network->Send2(newSocket, "GetName", 0, 0);
					}
				}
			}
		}

		if (networkEvents.lNetworkEvents & FD_READ)
		{
			//program->ConsoleWin.CommandLine->Write("Read\n");
			//network->WriteMessage("Read\n");

			SocketInfo &socketInfo = network->SocketArray[eventIndex - WSA_WAIT_EVENT_0];
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
				program->ConsoleWin.WindowPtr->AddMessage(MessageData({ NW_RECEIVE, eventIndex - WSA_WAIT_EVENT_0, 0 }));
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

			network->FreeSocketInformation(eventIndex - WSA_WAIT_EVENT_0);

			//program->ConsoleWin.CommandLine->Write("Closing Socket\n");
			network->WriteMessage("Closing Socket\n");
			program->GraphicWin.WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
		}
	}
}

void NetworkManager::Receive(int eventIndex, std::string identifier, char *data, int size)
{
	WriteMessage("Recieve:" + identifier + "\n");

	if (identifier == "Name")
	{
		std::string name = CharToString(data, size);

		SocketArray[eventIndex].Player = 0;

		if (name != "All")
		{
			DnD::Player *player = Program->GraphicWin.GameWorld->ConnectPlayer(name);
			if (!player)
			{
				player = new DnD::Player();
				player->Name = name;
				//player->ClientSocket = socket;
				//player->RecieveThread = std::thread(&ClientRecieveThread, player, program->Network, program);
				Program->GraphicWin.GameWorld->AddPlayer(player);
			}

			player->Connected = true;
			SocketArray[eventIndex].Player = player;

			//Program->ConsoleWin.CommandLine->Write("Retrieved Player Name\n");
			Program->GraphicWin.WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
		}
		else
		{
			Program->ConsoleWin.CommandLine->Write("Failed to connect; Player refused\n");
			closesocket(SocketArray[eventIndex].Socket);
			return;
		}
	}
	if (identifier == "Sync")
	{
		SyncWorldState(SocketArray[eventIndex].Player, SocketArray[eventIndex].Socket);
	}
	if (identifier == "SelfDataComplete")
	{
		SocketArray[eventIndex].Player->SyncState = 1;
		SyncWorldState(SocketArray[eventIndex].Player, SocketArray[eventIndex].Socket);
	}
	if (identifier == "EnvironmentHashes")
	{
		DnD::Player *player = SocketArray[eventIndex].Player;

		player->SyncEnvironments.clear();
		int count = *(int *)data;
		for (int i = 0; i < count; i++)
		{
			player->SyncEnvironments.push_back(*(int *)(data + sizeof(int) + sizeof(int) * i));
		}

		player->SyncState = 2;
		SyncWorldState(player, SocketArray[eventIndex].Socket);
	}
	if (identifier == "SyncComplete")
	{
		WriteMessage("Sync Complete\n");
		SocketArray[eventIndex].Player->SyncState = 0;

		bool AllComplete = true;
		for (int i = 0; i < Program->GraphicWin.GameWorld->PlayerList.size() && AllComplete; i++)
		{
			DnD::Player *player = Program->GraphicWin.GameWorld->PlayerList[i];

			AllComplete = (player->SyncState == 0);
		}
		if (AllComplete)
		{
			SyncInProgress = false;
			if (RedoSync)
			{
				WriteMessage("Redoing Sync\n");
				UpdateWorldState();
			}
		}
	}
}



void NetworkManager::UpdateWorldState(bool force)
{
	if (!SyncInProgress || force)
	{
		RedoSync = false;
		SyncInProgress = true;
		for (int i = 1; i < (int)EventTotal; i++)
		{
			SocketArray[i].Player->SyncState = 0;
			SocketArray[i].Player->SyncEnvironments.clear();
			Send2(SocketArray[i].Socket, "Sync", 0, 0);
		}
	}
	else
	{
		RedoSync = true;
		WriteMessage("Warning::Sync already in progress\n");
	}
}

/*void NetworkManager::SyncWorldState(DnD::Player *player, SOCKET socket)
{
	DnD::World *world = Program->GraphicWin.GameWorld;

	if (!player->HashSelfState)
	{
		char *buf = new char[sizeof(Int2) + player->Name.size()];
		*(Int2 *)buf = player->Position;
		StringToChar(player->Name, buf + sizeof(Int2));
		Send2(socket, "SelfData", buf, sizeof(int));
		delete[] buf;
	}	
	
	if (player->HashEnvironmentIndex == -1)
	{
		//calculate number of environments that player has
		int EnvironmentSize = 0;
		for (int i = 0; i < world->EnvironmentList.size(); i++)
		{
			for (int p = 0; p < world->EnvironmentList[i]->PlayerList.size(); p++)
			{
				if (world->EnvironmentList[i]->PlayerList[p] == player)
				{
					EnvironmentSize++;
				}
			}
		}

		char *buf = new char[sizeof(int)];
		*(int *)buf = EnvironmentSize;
		Send2(socket, "EnvironmentSize", buf, sizeof(int));
		delete[] buf;

		player->HashEnvironmentIndex = 0;
	}
	else
	{

	}
	//players have to be hashed as well

	//send expected environmentlist size
	//resize environmentlist to correct size
	//check environment hashes
	//for bad environment hashes
		//send expected layerlist size
		//resize layerlist to correct size
		//check layer hashes
		//for bad layer hashes
			//send expected objectlist size
			//resize objectlist to correct size
			//check bad object hashes
			//resend bad objects
		
	


}*/

void NetworkManager::SyncWorldState(DnD::Player *player, SOCKET socket)
{
	DnD::World *world = Program->GraphicWin.GameWorld;

	if (player->SyncState == 0)
	{
		//Send Player State

		int offset = 0;
		char *buf = new char[sizeof(Int2) + sizeof(int) + sizeof(int) * (int)player->Name.size()];
		*(Int2 *)buf = player->Position; offset += sizeof(Int2);
		*(int *)(buf + offset) = (int)player->Name.size(); offset += sizeof(int);
		StringToChar(player->Name, buf + offset);
		Send2(socket, "SelfData", buf, sizeof(Int2) + sizeof(int) * (int)player->Name.size());
		delete[] buf;
	}
	else if (player->SyncState == 1)
	{
		//Send Environment Hashes
		int size = 0;
		int count = 0;
		size += sizeof(int);
		for (int i = 0; i < world->EnvironmentList.size(); i++)
		{
			if (world->EnvironmentList[i]->ContainsPlayer(player))
			{
				size += sizeof(uint32_t);
				count++;
			}
		}
		char *buf = new char[size];
		*(int *)buf = count;

		for (int i = 0; i < world->EnvironmentList.size(); i++)
		{
			if (world->EnvironmentList[i]->ContainsPlayer(player))
			{
				*(uint32_t *)(buf + sizeof(uint32_t) * i + sizeof(int)) = world->EnvironmentList[i]->CalculateHash();
			}
		}

		Send2(socket, "EnvironmentHashes", buf, size);

		delete[] buf;

	}
	else if (player->SyncState == 2)
	{
		int size = 0;
		size += sizeof(int);
		for (int i = 0; i < player->SyncEnvironments.size(); i++)
		{
			size += sizeof(int); //index
			size += world->EnvironmentList[player->SyncEnvironments[i]]->GetDataSize(player);
		}

		int offset = 0;
		char *buf = new char[size];

		*(int *)buf = (int)player->SyncEnvironments.size(); offset += sizeof(int);
		for (int i = 0; i < player->SyncEnvironments.size(); i++)
		{
			*(int *)(buf + offset) = player->SyncEnvironments[i]; offset += sizeof(int); //index
			offset += world->EnvironmentList[player->SyncEnvironments[i]]->GetData(buf + offset,player);
		}

		Send2(socket, "EnvironmentData", buf, size);

		delete[] buf;
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










bool NetworkManager::CreateSocketInformation(SOCKET socket, DnD::Player *player)
{
	SocketInfo socketInfo;

	EventArray[EventTotal] = WSACreateEvent();
	if (EventArray[EventTotal] == WSA_INVALID_EVENT)
	{
		//Program->ConsoleWin.CommandLine->Write("ERROR::WSACreateEvent failed with error:" + std::to_string(WSAGetLastError()) + "\n");
		WriteMessage("ERROR::WSACreateEvent failed with error:" + std::to_string(WSAGetLastError()) + "\n");
		return false;
	}

	// Prepare SocketInfo structure for use
	socketInfo.Socket = socket;
	socketInfo.Player = player;

	SocketArray[EventTotal] = socketInfo;
	EventTotal++;
	return true;
}

void NetworkManager::FreeSocketInformation(DWORD Event)
{
	SocketInfo socketInfo = SocketArray[Event];

	closesocket(socketInfo.Socket);
	if (socketInfo.Player) { socketInfo.Player->Connected = false; }

	if (!WSACloseEvent(EventArray[Event]))
	{
		//Program->ConsoleWin.CommandLine->Write("ERROR::WSACloseEvent failed\n");
		WriteMessage("ERROR::WSACloseEvent failed\n");
	}

	// Squash the socket and event arrays
	for (DWORD i = Event; i < EventTotal; i++)
	{
		EventArray[i] = EventArray[i + 1];
		SocketArray[i] = SocketArray[i + 1];
	}
	EventTotal--;
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
		//Program->ConsoleWin.WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
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

bool NetworkManager::Receive2(SOCKET socket, char *buffer, int size)
{
	DWORD received, flags = 0;
	WSABUF dataBuf;

	dataBuf.buf = buffer;
	dataBuf.len = size;
	if (WSARecv(socket, &dataBuf, 1, &received, &flags, NULL, NULL) != 0)
	{
		//Program->ConsoleWin.CommandLine->Write("ERROR::Recieve failed:" + std::to_string(WSAGetLastError()) + "\n");
		WriteMessage("ERROR::Recieve failed:" + std::to_string(WSAGetLastError()) + "\n");
		//ShutdownSocket(socket);
		//Run = false;
		//NetworkManager::~NetworkManager();
		//Program->ConsoleWin.WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
		return false;
	}
	return true;
}

bool NetworkManager::ReceiveIdentifier2(SOCKET socket, std::string &identifier, int &dataSize)
{
	//receive size of identifier
	if (!Receive2(socket, (char *)&dataSize, sizeof(int)) || dataSize < 0) { return false; }
	char *buffer = new char[dataSize];

	//receive identifier
	if (!Receive2(socket, buffer, dataSize)) { return false; }
	identifier = CharToString(buffer, dataSize);
	
	//receive size of data
	if (!Receive2(socket, (char *)&dataSize, sizeof(int))) { return false; }

	delete[] buffer;

	WriteMessage("Recieve:" + identifier + "\n");

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

	WriteMessage("Send:" + identifier + "\n");
	//Sleep(10);

	return true;
}


#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <string>

#pragma comment(lib, "ws2_32.lib")
using namespace std;
SOCKET Connections[512];
SOCKET Connect;

SOCKET Listen;
SOCKET RECV;

int ClientCount = 0;



struct Data
{
	int ID;
	char * Name;
	
};

vector<Data> Players;
vector<string> ReadyMessage;

void ReadMessage(char * buffer);
void SetClientName(char * name);
void DeleteClient(int getID);
bool IsConnected(int getID);
void Work(int getID);



void SetClientName(char * name)
{

	return;
}

void DeleteClient(int getID)
{
	std::cout << "EZ DISCONNECT!" << std::endl;
	int size = Players.size();
	for (int i = 0; i < size; i++)
	{
		if (Players[i].ID == getID)
		{
			Players.erase(Players.begin() + i);
		}
	}
	Connections[getID] = NULL;
	closesocket(getID);
	return;
}

bool IsConnected(int getID)
{
	char * buffer = new char[1];
	int nSendBytes = send(getID, buffer, strlen(buffer), NULL);
	if (nSendBytes == SOCKET_ERROR)
	{
		DeleteClient(getID);
		return false;
	}
	else return true;
}

void Work(int getID)
{
	std::cout << "Thread created" << std::endl;
	char * buffer = new char[512];
	int result;
	while (IsConnected(getID) == true)
	{
		Sleep(50);

		result = recv(getID, buffer, sizeof(buffer), NULL);
		
		if (result > 0)
		{
			buffer[result] = '\0';
			std::cout << "Message from " << getID << ": " << buffer << std::endl;
			int size = Players.size();
			for (int i = 0; i < size; i++)
			{
				send(Players[i].ID, buffer, strlen(buffer), NULL);
			}
			ZeroMemory(buffer, sizeof(buffer));
			//std::cout << result << std::endl;
		}


	}

	closesocket(getID);
	return;
}



int main()
{
	setlocale(LC_ALL, "RU");
	WSADATA wsa;
	SOCKET s, new_socket;
	struct sockaddr_in server, client;
	int c;
	char *message;

	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		return 1;
	}

	printf("Initialised.\n");

	//Create a socket
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}

	printf("Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(2372);

	//Bind
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
	}

	puts("Bind done");

	//Listen to incoming connections
	listen(s, 3);

	//Accept and incoming connection
	puts("Waiting for incoming connections...");

	c = sizeof(struct sockaddr_in);

	while (true)
	{
		Sleep(50);
		new_socket = accept(s, NULL, NULL);
		if (new_socket == INVALID_SOCKET)
		{
			printf("accept failed with error code : %d", WSAGetLastError());
		}
		else
		{
			Data newClient;
			newClient.ID = new_socket;
			//closesocket(new_socket);
			cout << "Client with ID " << newClient.ID << " connected!" << endl;
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Work, (LPVOID)newClient.ID, 0, NULL);
			Players.push_back(newClient);
			
		}
	}
	getchar();

	closesocket(s);
	WSACleanup();


}





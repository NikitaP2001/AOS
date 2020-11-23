#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>;
#include "winsock2.h";
#include <ws2def.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <Ws2tcpip.h>
#include <string.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#include <fstream>
#include<cstdlib> 
#include<ctime> 
using namespace std;

void main() {

	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
		printf("Error at WSAStartup()\n");
	//----------------------
	// Create a SOCKET for connecting to server
	SOCKET ListenSocket;
	ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ListenSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
		return;
	}

	// The sockaddr_in structure specifies the address family,
	// IP address, and port for the socket that is being bound.
	sockaddr_in service;
	hostent* localHost;
	char* localIP;
	// Get the local host information
	localHost = gethostbyname("");
	localIP = inet_ntoa(*(struct in_addr*)*localHost->h_addr_list);
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr(localIP);
	service.sin_port = htons(1038);

	//----------------------
	// Bind the socket.
	if (bind(ListenSocket,
		(SOCKADDR*) &service,
		sizeof(service)) == SOCKET_ERROR) {
		printf("bind() failed.\n");
		closesocket(ListenSocket);
		return;
	}
	// Listen for incoming connection requests
	if (listen(ListenSocket, 1) == SOCKET_ERROR)
		printf("Error listening on socket.\n");

	SOCKET AcceptSocket;
	printf("Waiting for client to connect...\n");

	// Accept the connection.
	while (1) {
		AcceptSocket = SOCKET_ERROR;
		while (AcceptSocket == SOCKET_ERROR) {
			AcceptSocket = accept(ListenSocket, NULL, NULL);
		}
		printf("Client connected.\n");
		ListenSocket = AcceptSocket;
		break;
	}

	// Declare and initialize variables.
	int bytesSent;
	int bytesRecv = 0;
	char recvbuf[32] = "";
	char sendbuf[32] = "";
	srand(time(NULL));
	static const double fraction = 1.0 / (static_cast<double>(999999) + 1.0);
	int pass = static_cast<int>(rand() * fraction * (999999 - 100000 + 1) + 100000);
	const int len = 6;
	//----------------------
	// Receive data.
	bool flag;
	char chpass[32] = "";
	sprintf(chpass, "%d", pass);
	while (bytesRecv != SOCKET_ERROR) {
        //Pass length info
		strcpy_s(sendbuf, 32, "");
		sprintf(sendbuf, "%d", len);
		bytesSent = send(ListenSocket, sendbuf, strlen(sendbuf), 0);
		//----------------------------
		strcpy_s(sendbuf, 32, "");
		strcpy_s(recvbuf, 32, "");
		flag = true;
		bytesRecv = recv(ListenSocket, recvbuf, 32, 0);
		if (strcmp(recvbuf,"who") == 0) {
			strcpy_s(sendbuf, 32, "Nikita Piatigorskiy 13 password");
			bytesSent = send(ListenSocket, sendbuf, strlen(sendbuf), 0);
			flag = false;
		}
		if (strcmp(recvbuf, "try") == 0) {
			strcpy_s(sendbuf, 32, "wait pass");
			bytesSent = send(ListenSocket, sendbuf, strlen(sendbuf), 0);
			bytesRecv = recv(ListenSocket, recvbuf, 32, 0);
			if (strcmp(recvbuf, chpass) == 0) { strcpy_s(sendbuf, 32, "Succsess"); }
			else { strcpy_s(sendbuf, 32, "Failure"); }
			bytesSent = send(ListenSocket, sendbuf, strlen(sendbuf), 0);
			flag = false;
		}

		if (flag) {
			strcpy_s(sendbuf, 32, "server dont know this command");
			bytesSent = send(ListenSocket, sendbuf, strlen(sendbuf), 0);
		}
	}
	WSACleanup();
}
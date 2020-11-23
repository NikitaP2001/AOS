#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>;
#include "winsock2.h";
#include <ws2def.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <Ws2tcpip.h>
#include <string.h>
#pragma comment(lib, "ws2_32.lib")
#include <fstream>
using namespace std;

int main()
{
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
		return 0;
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

	// Connect to server.
	if (connect(ListenSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
		cout << "failed to connect the server" << endl;
		WSACleanup();
		return 0;
	}
	cout << "connected to server" << endl;
	ofstream myfile;
	myfile.open("log.txt");
	// Declare and initialize variables.
	int bytesSent;
	int bytesRecv;
	char sendbuf[32] = "";
	char recvbuf[32] = "";
	char length[32] = "";
	char clpass[32] = "";
		//----------------------
	// Send and receive data.
	time_t t;
	time(&t);
	tm* tme = localtime(&t);
	bool flag;
	int attempt = 0;
	string command = "";
	for (;;) {
		//get pass and attempt info from server
		bytesRecv = recv(ListenSocket, length, 32, 0);
		cout << "Attempt: " << attempt << " pass lenght " << length << endl;
		//-----------------------------------
		strcpy_s(sendbuf, 32, "");
		strcpy_s(recvbuf, 32, "");
		cout << "command>";
		cin >> command;
		flag = true;
		if (command == "who") {
			strcpy_s(sendbuf, 32, "who");
			bytesSent = send(ListenSocket, sendbuf, strlen(sendbuf), 0);
			bytesRecv = recv(ListenSocket, recvbuf, 32, 0);
			cout << recvbuf << endl;
			flag = false;
			myfile << tme->tm_hour << ":" << tme->tm_min << " Command: " << sendbuf << endl;
		}
		if (command == "try") {
			strcpy_s(sendbuf, 32, "try");
			bytesSent = send(ListenSocket, sendbuf, strlen(sendbuf), 0);
			cin >> clpass;
			bytesRecv = recv(ListenSocket, recvbuf, 32, 0);
			bytesSent = send(ListenSocket, clpass, strlen(clpass), 0);
			bytesRecv = recv(ListenSocket, recvbuf, 32, 0);
			cout << recvbuf << endl;
			attempt++;
			flag = false;
			myfile << tme->tm_hour << ":" << tme->tm_min << " Command: " << sendbuf << endl;
		}
		if (command == "autotry") {
			strcpy_s(sendbuf, 32, "try");
			int time;
			cin >> time;
			clock_t start = clock();
			clock_t now = clock();
			int n = 0;
			while ((start + time*1000) > now) {
				bytesSent = send(ListenSocket, sendbuf, strlen(sendbuf), 0);
				sprintf(clpass, "%d", n);
				bytesRecv = recv(ListenSocket, recvbuf, 32, 0);
				bytesSent = send(ListenSocket, clpass, strlen(clpass), 0);
				bytesRecv = recv(ListenSocket, recvbuf, 32, 0);
				if (strcmp(recvbuf, "Succsess") == 0) { break; }
				attempt++;
				n++;
				now = clock();
			}
			cout << recvbuf << endl;
			flag = false;
			myfile << tme->tm_hour << ":" << tme->tm_min << " Command: " << sendbuf << endl;
		}
		if (flag) cout << "Unknown command" << endl;
	}

	bytesSent = send(ListenSocket, sendbuf, strlen(sendbuf), 0);
	myfile << "Bytes Sent :" << bytesSent << "bufer " << recvbuf;
	WSACleanup();
	myfile.close();
}
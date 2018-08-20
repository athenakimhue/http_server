#include "stdafx.h"
#include <windows.h>
#include "..\mysocket.h"
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")
#define SRV_PORT	44000
#define buf 512
#define DEFAULT_BUFLEN 512
using namespace std;

SOCKET ConnectToServer(char *ip, USHORT port)
{

	//Init winsock
	WORD	wVersionRequested = MAKEWORD(2, 0);//version
	WSADATA wsaData;
	int err = WSAStartup(wVersionRequested, &wsaData); //WSAStartup
	if (err) return INVALID_SOCKET;

	//Create socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0); //socket
	if (sock == INVALID_SOCKET) return INVALID_SOCKET;

	//Connect to the server
	struct sockaddr_in saServer;
	saServer.sin_family = AF_INET;
	saServer.sin_addr.s_addr = inet_addr(ip);
	saServer.sin_port = htons(port);

	err = connect(sock, (sockaddr*)&saServer, sizeof(saServer));//connect
	if (err) return INVALID_SOCKET;
	return sock;
}

int __cdecl main(int argc, char **argv)
{

	// Validate the parameters
	if (argc != 2) {
		printf("usage: %s server-ip\n", argv[0]);
		return 1;
	}

	SOCKET sock;
	sock = ConnectToServer(argv[1], SRV_PORT);
	if (sock == INVALID_SOCKET)
	{
		printf("Failed\n");
	}
	else
		printf("Connected. Leaving.\n");

	char string[buf];

	//send
	CLIENT_URL client;
	printf("\nInput URL : ");
	gets_s(string, buf);
	sprintf_s(client.msg, sizeof(CLIENT_URL::msg), "%s", string);
	sendall((char*)&client, sizeof(CLIENT_URL), sock);
	
	////recv
	printf("\n\nHTTP RESPONSE :");
	int recvbuflen = DEFAULT_BUFLEN;
	char recvbuf[DEFAULT_BUFLEN] =" ";
	client_Request clientreq;
	recvall((char*)&clientreq, sizeof(client_Request),sock);
	recv(sock, recvbuf, recvbuflen, 0);
	printf("\n\t%s %s\n\t%s %d bytes\n\t%s\n\t%s\n-----------------------------------\n",
		clientreq.version, clientreq.code, clientreq.lengh, (int)strlen(recvbuf), clientreq.type, clientreq.connection);
	printf("%s", recvbuf);
	
	

	closesocket(sock);
	WSACleanup();
	getchar();
	return 0;
}



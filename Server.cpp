#include "stdafx.h"
#include <winsock.h>
#include <string.h>
#include <strsafe.h>
#include "..\mysocket.h"
#define bufer 512
#define buf_max 0x200
#define DEFAULT_BUFLEN 512
typedef struct
{
	SOCKET		sock;
	SOCKADDR_IN saClient;
	DWORD		id;
}THREAD_PARAM;



SOCKET StartServerListeningSocket(USHORT port)
{

	//Init winsock
	WORD	wVersionRequested = MAKEWORD(2, 0);
	WSADATA wsaData;
	int err = WSAStartup(wVersionRequested, &wsaData); //WSAStartup
	if (err)return INVALID_SOCKET;

	//Create socket
	SOCKET sock;
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //socket
	if (sock == INVALID_SOCKET) return INVALID_SOCKET;

	//Bind the socket
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);
	err = bind(sock, (sockaddr*)&addr, sizeof(addr));  //bind
	if (err)
	{
		closesocket(sock);
		return INVALID_SOCKET;
	}

	//Start listening
	err = listen(sock, SOMAXCONN);
	if (err)
	{
		closesocket(sock);
		return INVALID_SOCKET;
	}
	return sock;


}
DWORD WINAPI ServerThread(void *pParam)
{

	char aszIPAddresses[16];
	THREAD_PARAM *pClientInfo = (THREAD_PARAM*)pParam;
	printf("Client No %d connected\n", pClientInfo->id);
	strcpy(aszIPAddresses, inet_ntoa(pClientInfo->saClient.sin_addr));
	printf("IP Address : %s :%d \n\n\n", aszIPAddresses, ntohs(pClientInfo->saClient.sin_port));

	//recv
	char recvbuf[512];
	recv(pClientInfo->sock, recvbuf, bufer, 0);
	printf("HTTP Request : \n");

	char *ptr, *host, *path;
	ptr = strtok(recvbuf, "////");
	ptr = strtok(NULL, "//");
	host = ptr;
	ptr = strtok(NULL, " ");
	path = ptr;

	printf("\tGET %s HTTP/1.1\n", path);
	printf("\tHOST : %s", host);
	char save[MAX_PATH];
	strcpy(save, "F:\\");
	strcat(save, path);
	
	
	HANDLE hFile;
	hFile = CreateFileA(save,
	GENERIC_READ,
	NULL,
	NULL,
	OPEN_EXISTING,
	FILE_ATTRIBUTE_NORMAL,
	NULL);

	//Not found
	if (hFile == INVALID_HANDLE_VALUE)
	{
	printf("\nHTTP response :");

	client_Request clientreq;
	char b[buf_max] = { 0 };
	DWORD nByte;
	HANDLE hFile;
	char save1[MAX_PATH]= "F:\\laptrinh\\tailieu\\NotF.html";
	hFile = CreateFileA(save1,
		GENERIC_READ,
		NULL,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	ReadFile(hFile, b, buf_max, &nByte, NULL);
	sprintf_s(clientreq.version, sizeof(client_Request::version), "%s", "HTTP/1.1");
	sprintf_s(clientreq.code, sizeof(client_Request::code),"%s", "404 Not Found");
	sprintf_s(clientreq.lengh, sizeof(client_Request::lengh),"%s","Content - Length : ");
	sprintf_s(clientreq.type, sizeof(client_Request::type), "%s", "Content - Type : text / html");
	sprintf_s(clientreq.connection, sizeof(client_Request::connection), "%s", "Connection : Closed");

	printf("\n\t%s %s\n\t%s %d bytes\n\t%s\n\t%s\n-----------------------------------\n",
		clientreq.version, clientreq.code, clientreq.lengh, (int)strlen(b), clientreq.type, clientreq.connection);
	printf("%s", b);
	//-----------send
	sendall((char*)&clientreq, sizeof(client_Request), pClientInfo->sock);
	send(pClientInfo->sock, b, (int)strlen(b), 0);

	return 1;
	}
	else
	{
		printf("\n\nHTTP response :");
		client_Request clientreq;
		char b[buf_max] = { 0 };
		DWORD nByte;
		ReadFile(hFile, b, buf_max, &nByte, NULL);

		sprintf_s(clientreq.version, sizeof(client_Request::version), "%s","HTTP/1.1");
		sprintf_s(clientreq.code, sizeof(client_Request::code), "%s", "200 OK");
		sprintf_s(clientreq.lengh, sizeof(client_Request::lengh), "%s", "Content - Length : ");
		sprintf_s(clientreq.type, sizeof(client_Request::type), "%s", "Content - Type : text / html");
		sprintf_s(clientreq.connection, sizeof(client_Request::connection), "%s", "Connection : Closed");
		
		printf("\n\t%s %s\n\t%s %d bytes\n\t%s\n\t%s\n-----------------------------------\n",
			clientreq.version,clientreq.code,clientreq.lengh,(int)strlen(b) ,clientreq.type,clientreq.connection);
		printf("%s", b);
		
		//-----------send
		sendall((char*)&clientreq, sizeof(client_Request), pClientInfo->sock);
		send(pClientInfo->sock, b, (int)strlen(b), 0);
	}
		
	CloseHandle(hFile);
	return 0;
}

int main()
{
	SOCKET sListeningSocket, sWorkingSocket;
	SOCKADDR_IN addrClient;
	int nClientAddressLen;
	THREAD_PARAM tp;
	DWORD dwClientCount = 0;
	printf("Starting the server socket...");
	sListeningSocket = StartServerListeningSocket(SRV_PORT);

	if (sListeningSocket != INVALID_SOCKET) printf("OK\n");
	else printf("FAILED\n");

	while (1)
	{
		printf("Waiting for a client...\n");
		nClientAddressLen = sizeof(addrClient);
		sWorkingSocket = accept(sListeningSocket, (sockaddr*)&addrClient, &nClientAddressLen); //accept

		if (sWorkingSocket == INVALID_SOCKET) { printf("Failed!!"); break; }

		tp.sock = sWorkingSocket;
		tp.saClient.sin_addr = addrClient.sin_addr;
		tp.id = ++dwClientCount;

		HANDLE hServerSocket = CreateThread(NULL, 0, ServerThread, &tp, 0, NULL);
	}

	WSACleanup();
	getchar();
	return 0;
}

//http://127.0.0.1/laptrinh/tailieu/hoc.html
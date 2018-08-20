#ifndef  MYSOCKET_HEADER
#define MYSOCKET_HEADER 
//----------------------------------
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#define SRV_PORT			44000
//----------------------------------
#endif
#define buff 1000

typedef  struct
{
	char msg[1000];
	
}CLIENT_URL;


typedef  struct
{
	char version[buff];
	char code[buff];
	char type[buff];
	char lengh[buff];
	char connection[buff];

}client_Request;

typedef  struct
{
	char msg[1000];

}Server_Response;



int sendall(char *msg, int len, SOCKET sock)
{
	char *buf = msg;
	int remained = len;		//chua gui
	int error = 0, count;	//cout -> da gui
	while (remained && !error)
	{
		count = send(sock, buf, remained, 0);
		error = (count == 0);
		remained -= count;
		buf += count;
	}
	return error;
}
int recvall(char *msg, int len, SOCKET sock)
{
	char *buf = msg;
	int remained = len;
	int error = 0, count;
	while (remained && !error)
	{
		count = recv(sock, buf, remained, 0);
		error = (count == 0);
		remained -= count;
		buf += count;
	}

	return error;
}
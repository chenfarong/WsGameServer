
#ifndef server_h__
#define server_h__

#include <stdlib.h>
#include <stdio.h>
#include <iostream>


#define DATA_BUFSIZE 8192

#ifdef _MSC_VER

#define __WINDOWS__ 1

#ifndef  WIN32
#define  WIN32 1
#endif

//#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")

typedef SOCKET  XSOCKET;

typedef struct _SOCKET_INFORMATION {
	CHAR Buffer[DATA_BUFSIZE];
	WSABUF DataBuf;
	SOCKET Socket;
	OVERLAPPED Overlapped;
	DWORD BytesSEND;
	DWORD BytesRECV;
} SOCKET_INFORMATION, *LPSOCKET_INFORMATION;

// Prototypes
BOOL CreateSocketInformation(SOCKET s);
void FreeSocketInformation(DWORD Index);



#else
typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;

typedef int XSOCKET;



#endif

int ServerInit();
int ServerListen(std::string ipAddr, int port);
int ServerStep();
void ServerClean();
int ServerCore();
void OnShutdown();





#endif // server_h__






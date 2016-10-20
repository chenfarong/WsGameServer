

#include "server.h"
#include "mcs.h"

// Global var
DWORD TotalSockets = 0;
LPSOCKET_INFORMATION SocketArray[FD_SETSIZE];

XSOCKET ListenSocket;
u_long NonBlock = 1;
timeval TimeOut = {0,200};

#if defined(__WINDOWS__)


BOOL CreateSocketInformation(SOCKET s)
{
	LPSOCKET_INFORMATION SI;

	printf("Accepted socket number %d\n", (int)s);

	if ((SI = (LPSOCKET_INFORMATION)GlobalAlloc(GPTR, sizeof(SOCKET_INFORMATION))) == NULL)
	{
		printf("GlobalAlloc() failed with error %d\n", GetLastError());
		return FALSE;
	}
	else
		printf("GlobalAlloc() for SOCKET_INFORMATION is OK!\n");

	// Prepare SocketInfo structure for use
	SI->Socket = s;
	SI->BytesSEND = 0;
	SI->BytesRECV = 0;

	SocketArray[TotalSockets] = SI;
	TotalSockets++;

	subscriber_add(s);

	return(TRUE);
}

void FreeSocketInformation(DWORD Index)
{
	LPSOCKET_INFORMATION SI = SocketArray[Index];
	DWORD i;
	
	subscriber_remove(SI->Socket);

	closesocket(SI->Socket);
	printf("Closing socket number %d\n", (int)SI->Socket);
	GlobalFree(SI);

	// Squash the socket array
	for (i = Index; i < TotalSockets; i++)
	{
		SocketArray[i] = SocketArray[i + 1];
	}

	TotalSockets--;
}



int ServerInit()
{
	int Ret = 0;
	WSADATA wsaData;
	if ((Ret = WSAStartup(0x0202, &wsaData)) != 0)
	{
		printf("WSAStartup() failed with error %d\n", Ret);
		WSACleanup();
		return Ret;
	}
	return Ret;
}

int ServerListen(std::string ipAddr, int port)
{
	SOCKADDR_IN InternetAddr;
	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	InternetAddr.sin_port = htons(port);

	// Prepare a socket to listen for connections
	if ((ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		printf("WSASocket() failed with error %d\n", WSAGetLastError());
		return 1;
	}
	else
		printf("WSASocket() is OK!\n");

	if (bind(ListenSocket, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr)) == SOCKET_ERROR)
	{
		printf("bind() failed with error %d\n", WSAGetLastError());
		return 1;
	}
	else
		printf("bind() is OK!\n");

	if (listen(ListenSocket, 5))
	{
		printf("listen() failed with error %d\n", WSAGetLastError());
		return 1;
	}
	else
		printf("listen() is OK!\n");

	// Change the socket mode on the listening socket from blocking to
	// non-block so the application will not block waiting for requests
	NonBlock = 1;
	if (ioctlsocket(ListenSocket, FIONBIO, &NonBlock) == SOCKET_ERROR)
	{
		printf("ioctlsocket() failed with error %d\n", WSAGetLastError());
		return 1;
	}
	else
		printf("ioctlsocket() is OK!\n");

	return 0;

}

int ServerStep()
{
	SOCKET AcceptSocket;
	FD_SET WriteSet;
	FD_SET ReadSet;
	FD_SET ExceptSet;
	DWORD i;
	DWORD Total;
	DWORD Flags;
	DWORD SendBytes;
	DWORD RecvBytes;

	//while (TRUE)
	{
		// Prepare the Read and Write socket sets for network I/O notification
		FD_ZERO(&ReadSet);
		FD_ZERO(&WriteSet);
		FD_ZERO(&ExceptSet);

		// Always look for connection attempts
		FD_SET(ListenSocket, &ReadSet);

		// Set Read and Write notification for each socket based on the
		// current state the buffer.  If there is data remaining in the
		// buffer then set the Write set otherwise the Read set
		for (i = 0; i < TotalSockets; i++)
			if (SocketArray[i]->BytesRECV > SocketArray[i]->BytesSEND)
				FD_SET(SocketArray[i]->Socket, &WriteSet);
			else
				FD_SET(SocketArray[i]->Socket, &ReadSet);

		if ((Total = select(0, &ReadSet, &WriteSet, &ExceptSet, &TimeOut)) == SOCKET_ERROR)
		{
			printf("select() returned with error %d\n", WSAGetLastError());
			return 1;
		}
		//else
		//	printf("select() is OK!\n");

		// Check for arriving connections on the listening socket.
		if (FD_ISSET(ListenSocket, &ReadSet))
		{
			Total--;
			if ((AcceptSocket = accept(ListenSocket, NULL, NULL)) != INVALID_SOCKET)
			{
				// Set the accepted socket to non-blocking mode so the server will
				// not get caught in a blocked condition on WSASends
				NonBlock = 1;
				if (ioctlsocket(AcceptSocket, FIONBIO, &NonBlock) == SOCKET_ERROR)
				{
					printf("ioctlsocket(FIONBIO) failed with error %d\n", WSAGetLastError());
					return 1;
				}
				else
					printf("ioctlsocket(FIONBIO) is OK!\n");

				if (CreateSocketInformation(AcceptSocket) == FALSE)
				{
					printf("CreateSocketInformation(AcceptSocket) failed!\n");
					return 1;
				}
				else {
					printf("CreateSocketInformation() is OK!\n");
					//加入到订阅
					
				}

			}
			else
			{
				if (WSAGetLastError() != WSAEWOULDBLOCK)
				{
					printf("accept() failed with error %d\n", WSAGetLastError());
					return 1;
				}
				else
					printf("accept() is fine!\n");
			}
		}

		// Check each socket for Read and Write notification until the number
		// of sockets in Total is satisfied
		for (i = 0; Total > 0 && i < TotalSockets; i++)
		{
			LPSOCKET_INFORMATION SocketInfo = SocketArray[i];

			// If the ReadSet is marked for this socket then this means data
			// is available to be read on the socket
			if (FD_ISSET(SocketInfo->Socket, &ReadSet))
			{
				Total--;

				SocketInfo->DataBuf.buf = SocketInfo->Buffer;
				SocketInfo->DataBuf.len = DATA_BUFSIZE;

				Flags = 0;
				if (WSARecv(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &RecvBytes, &Flags, NULL, NULL) == SOCKET_ERROR)
				{
					if (WSAGetLastError() != WSAEWOULDBLOCK)
					{
						printf("WSARecv() failed with error %d\n", WSAGetLastError());
						FreeSocketInformation(i);
					}
					else
						printf("WSARecv() is OK!\n");
					continue;
				}
				else
				{
					SocketInfo->BytesRECV = RecvBytes;

					// If zero bytes are received, this indicates the peer closed the connection.
					if (RecvBytes == 0)
					{
						FreeSocketInformation(i);
						continue;
					}
					else {
						xmsg_command(SocketInfo->Socket, SocketInfo->DataBuf.buf, (int)RecvBytes);
						printf("%lu", SocketInfo->DataBuf.len);
						//if(RecvBytes<DATA_BUFSIZE-1) SocketInfo->DataBuf.buf[RecvBytes]=0;
						memset(SocketInfo->DataBuf.buf, 0, DATA_BUFSIZE); //不加这行 后面的脏数据还在
					}
				}
			}

			// If the WriteSet is marked on this socket then this means the internal
			// data buffers are available for more data
			if (FD_ISSET(SocketInfo->Socket, &WriteSet))
			{
				Total--;

				SocketInfo->DataBuf.buf = SocketInfo->Buffer + SocketInfo->BytesSEND;
				SocketInfo->DataBuf.len = SocketInfo->BytesRECV - SocketInfo->BytesSEND;

				if (WSASend(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &SendBytes, 0, NULL, NULL) == SOCKET_ERROR)
				{
					if (WSAGetLastError() != WSAEWOULDBLOCK)
					{
						printf("WSASend() failed with error %d\n", WSAGetLastError());
						FreeSocketInformation(i);
					}
					else
						printf("WSASend() is OK!\n");

					continue;
				}
				else
				{
					SocketInfo->BytesSEND += SendBytes;

					if (SocketInfo->BytesSEND == SocketInfo->BytesRECV)
					{
						SocketInfo->BytesSEND = 0;
						SocketInfo->BytesRECV = 0;
					}
				}
			}
		}
	}

	return 0;
}

void ServerClean()
{
	WSACleanup();
}

int ServerCore()
{
	return 0;
}

void OnShutdown()
{

}

#else


#endif


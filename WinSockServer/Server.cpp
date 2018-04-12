#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <queue>
#include <string>
using namespace std;
#define DEFAULT_BUFLEN 3000
#define MAX_SERVICES 10

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

bool InitializeWindowsSockets();


int waitForSocketAccept(SOCKET acceptedSocket, bool isSend, int sleepTime);


typedef struct Queue {
 	SOCKET clientSock;
	int lenOutgoingbuff;
	char* messageBuffer;
} Queue;

int i;
int SendData(std::queue<Queue> queue, int dataSize);

int waitForSocketAccept(SOCKET acceptedSocket, bool isSend, int sleepTime)
{
	while (1)
	{
		FD_SET set;
		timeval timeVal;
		FD_ZERO(&set);
		FD_SET(acceptedSocket, &set);
		int iResult;
		timeVal.tv_sec = 0;
		timeVal.tv_usec = 0;

		if (!isSend)
			iResult = select(0, &set, NULL, NULL, &timeVal);
		else
			iResult = select(0, NULL, &set, NULL, &timeVal);

		if (iResult == SOCKET_ERROR)
		{
			fprintf(stderr, "select failed with error: %ld\n", WSAGetLastError());
			return SOCKET_ERROR;
		}

		if (iResult == 0)
		{
			Sleep(sleepTime);
			continue;
		}
		break;
	}
	return 0;
}


int id;
char messageToSend[DEFAULT_BUFLEN];
std::queue<Queue> my_queue;

int __cdecl main(int argc, char *argv[]) 
{

	if(atoi(argv[3])==1)
	{
	
		SOCKET connectSocket = INVALID_SOCKET;
		int iResult;

		if(InitializeWindowsSockets() == false)
		{
			return 1;
		}

		connectSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

		if (connectSocket == INVALID_SOCKET)
		{
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}
	
			char temp;
			printf("Unesite (broj) primaoca poruke: ");
			scanf("%d",&id);

			printf("Unesite sadrzaj poruke: ");
			scanf("%c",&temp);
			scanf("%[^\n]",messageToSend);
			

			sockaddr_in serverAddress;
			serverAddress.sin_family = AF_INET;

			for(int i = 0; i<MAX_SERVICES;i++)
			{
				if(i==id)
				{
					char buff[] = "127.0.0.x";
					buff[8] = i + '0';
					serverAddress.sin_addr.s_addr = inet_addr(buff);
					serverAddress.sin_port = htons(15000+i);
					break;
				}
			}
			

			if (connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
			{
				printf("\nClient>> Unable to connect to server.\n");
				WSACleanup();
			}
			 
			    Queue queue;

				queue.clientSock = connectSocket;
				queue.lenOutgoingbuff = strlen(messageToSend);
				queue.messageBuffer =  (char*)&messageToSend;
	
				my_queue.push(queue);

			    unsigned long int nonBlockingMode = 1;
				iResult = ioctlsocket(connectSocket, FIONBIO, &nonBlockingMode);
			
				iResult = SendData(my_queue,queue.lenOutgoingbuff);
						
				printf("\n\n\nClient>> Poslao sam: %ld bytes\n", iResult);
			
				if (iResult == SOCKET_ERROR)
				{
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(connectSocket);
					WSACleanup();
					return 1;
				}
				
			
			    closesocket(connectSocket);
				WSACleanup();
				Sleep(50000);
					
			
	}
	else
	{
		
				SOCKET listenSocket = INVALID_SOCKET;
				SOCKET acceptedSocket = INVALID_SOCKET;
				int iResult;
			 
				char recvbuf[DEFAULT_BUFLEN];
    
				if(InitializeWindowsSockets() == false)
				{
					return 1;
				}
    
				addrinfo *resultingAddress = NULL;
				addrinfo hints;
				memset(&hints, 0, sizeof(hints));
				hints.ai_family = AF_INET;       // IPv4 address
				hints.ai_socktype = SOCK_STREAM; // Provide reliable data streaming
				hints.ai_protocol = IPPROTO_TCP; // Use TCP protocol
				hints.ai_flags = AI_PASSIVE;     // 

				iResult = getaddrinfo(NULL, argv[2], &hints, &resultingAddress);//argv[2]
				if ( iResult != 0 )
				{
				printf("getaddrinfo failed with error: %d\n", iResult);
				return 1;
				}
	
				listenSocket = socket(AF_INET,      // IPv4 address famly
									  SOCK_STREAM,  // stream socket
									  IPPROTO_TCP); // TCP
	
				if (listenSocket == INVALID_SOCKET)
				{
					printf("socket failed with error: %ld\n", WSAGetLastError());
					freeaddrinfo(resultingAddress);
					WSACleanup();
					return 1;
				}
	
				iResult = bind( listenSocket, resultingAddress->ai_addr, (int)resultingAddress->ai_addrlen);
	
				if (iResult == SOCKET_ERROR)
				{
					printf("bind failed with error: %d\n", WSAGetLastError());
					freeaddrinfo(resultingAddress);
					closesocket(listenSocket);
					WSACleanup();
					return 1;
				}

			

				freeaddrinfo(resultingAddress);
	
				iResult = listen(listenSocket, SOMAXCONN);
	
				if (iResult == SOCKET_ERROR)
				{
					printf("listen failed with error: %d\n", WSAGetLastError());
					closesocket(listenSocket);
					WSACleanup();
					return 1;
				}
	
		
				printf("Server>>Zdravo, ja sam server broj:%s",argv[3]);
				printf("\nCekam poruku od klijenta...");

				do{
						
			 						acceptedSocket = accept(listenSocket, NULL, NULL);
		
									if (acceptedSocket == INVALID_SOCKET)
									{
										printf("accept failed with error: %d\n", WSAGetLastError());
										closesocket(listenSocket);
										WSACleanup();
										return 1;
									}
									
									unsigned long int nonBlockingMode = 1;
									iResult = ioctlsocket( acceptedSocket, FIONBIO, &nonBlockingMode );		
							
						
									FD_SET set;
									timeval timeVal;

									FD_ZERO( &set );
					     			// Add socket we will wait to read from
									FD_SET( acceptedSocket, &set );
									timeVal.tv_sec = 0;
									timeVal.tv_usec = 0;
									iResult = select( 0 /* ignored */, &set, NULL, NULL, &timeVal );
									

									iResult = recv(acceptedSocket,recvbuf-iResult, strlen(recvbuf)+iResult , 0);
								 

									if (iResult > 0)
									{   
										
										printf("\n\nServer>>Primio sam poruku sa sadrzajem: %.*s", (int)iResult,  recvbuf);	
										printf("\nServer>> Primio sam %d bytes", (int)iResult);
										Sleep(20000);

									}
									else if (iResult == 0)
									{
										// connection was closed gracefully
									
										printf("Server>>Zatvorena konekcija sa klijentom.\n");
										closesocket(acceptedSocket);
									}
									else
									{
										// there was an error during recv
										printf("recv failed with error: %d\n", WSAGetLastError());
										closesocket(acceptedSocket);
									}
							
					} while (iResult>0);

					iResult = shutdown(acceptedSocket, SD_SEND);

					if (iResult == SOCKET_ERROR)
					{
						printf("shutdown failed with error: %d\n", WSAGetLastError());
						closesocket(acceptedSocket);
						WSACleanup();
						return 1;
					}

   
					closesocket(listenSocket);
					closesocket(acceptedSocket);
					WSACleanup();

					return 0;


		}}
		
bool InitializeWindowsSockets()
{
    WSADATA wsaData;
	// Initialize windows sockets library for this process
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
    {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return false;
    }
	return true;
}
int SendData(std::queue<Queue> queue,  int dataSize)
{
	
	int iResult = SOCKET_ERROR;
	int i = 0;

	Queue queue1 = queue.front();	
	SOCKET clientSocket = queue1.clientSock;
	char *data =queue1.messageBuffer;
	int length = queue1.lenOutgoingbuff;

	while (i < dataSize)
	{
		if (waitForSocketAccept(clientSocket, true, 50000) == SOCKET_ERROR)
		{
			return SOCKET_ERROR;
		}

		iResult = send(clientSocket, data+i, dataSize-i, 0);
		i += iResult;

		if (iResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(clientSocket);
			return SOCKET_ERROR;
		}
		else if (iResult == 0)
		{
			printf("gracefull shutdown");
			closesocket(clientSocket);
			return 0;
		}

	}
	
	return iResult;
}


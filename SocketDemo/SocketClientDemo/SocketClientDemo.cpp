// SocketClientDemo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <WinSock2.h>

#pragma comment(lib,"Ws2_32.lib")

//#define RECV_BUFFER_SIZE 8192
const unsigned RECV_BUFFER_SIZE = 8192;


//int _tmain(int argc, _TCHAR* argv[])
int _tmain(int argc, char* argv[])
{

	SOCKADDR_IN clientService;
	SOCKET connectSocket;
	WSADATA wsaData;
	char* recvbuf;
	int bytesSent;
	int bytesRecv = 0;
	char sendbuf[32] = "get information";
	//初始化库，保证ws2_32.dll已经加载
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR){
		printf("Eroor at WSAStartup()\n");
	}
	//创建Socket
	connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (connectSocket == INVALID_SOCKET){
		printf("Error at socket():%ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	//
	clientService.sin_family = AF_INET;
	clientService.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	clientService.sin_port = htons(1000);
	//
	if (connect(connectSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR){
		printf("failed to connect(%d)\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	//
	if (argc == 2 && (!strcmp(argv[1], "-d"))){
		strcpy(sendbuf, "down file");		
	}
	bytesSent = send(connectSocket, sendbuf, strlen(sendbuf) + 1, 0);
	if (bytesSent == SOCKET_ERROR){
		printf("send error %d\n", WSAGetLastError());
		closesocket(connectSocket);
		return 1;
	}
	printf("bytes sent: %d\n", bytesSent);
	//接收
	recvbuf = (char*)malloc(RECV_BUFFER_SIZE);
	while (bytesRecv != SOCKET_ERROR){
		bytesRecv = recv(connectSocket, recvbuf, RECV_BUFFER_SIZE, 0);
		if (bytesRecv == 0){
			printf("connection closed\n");
			break;
		}
		printf("bytes recv:&d\n", bytesRecv);
	}
	free(recvbuf);
	WSACleanup();
	return 0;



	std::cout << "aaa" << std::endl;
	return 0;
}


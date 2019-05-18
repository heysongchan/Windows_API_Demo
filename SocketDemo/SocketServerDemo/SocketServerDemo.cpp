// SocketServerDemo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib,"Ws2_32.lib")

//#define DEFAULT_PORT "10000"
const char* const DEFAULT_PORT = "10000";
const unsigned MAX_REQUEST = 1024;
const unsigned BUF_SIZE = 4096;

//具体交互逻辑实现
DWORD doCommunicate(SOCKET& socket){
	DWORD dwTid = GetCurrentThreadId();
	LPSTR szRequest = (LPSTR)malloc(MAX_REQUEST);
	int iResult, bytesSent;
	//接收数据
	iResult = recv(socket, szRequest, MAX_REQUEST, 0);
	if (iResult == 0){
		std::cout << "Connection closing..." << std::endl;
		free(szRequest);
		closesocket(socket);
		return 1;
	}
	else if (iResult == SOCKET_ERROR){
		printf("recv failed:%d\n", WSAGetLastError());
		free(szRequest);
		closesocket(socket);
		return 1;
	}
	else if (iResult > 0){
		printf("%d \t bytes received:%d\n", dwTid, iResult);
		printf("%d \t request string is %s\n", dwTid, szRequest);
		//download
		if (!strcmp(szRequest, "download file")){
			HANDLE hFile;
			LPVOID lpReadBuf;
			DWORD dwBytesRead;
			DWORD dwFileSize;
			DWORD dwSendFile = 0;
			hFile = CreateFileA("zz.txt", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE){
				printf("Could not open file %d\n", GetLastError());
				send(socket, "error", 6, 0);
				closesocket(socket);
				return 1;
			}
			lpReadBuf = malloc(BUF_SIZE);
			dwFileSize = GetFileSize(hFile, NULL);
			while (1){
				if (!ReadFile(hFile, lpReadBuf, BUF_SIZE, &dwBytesRead, NULL)){
					printf("read file error %d\n", GetLastError());
					closesocket(socket);
					CloseHandle(hFile);
					return 1;
				}
				bytesSent = send(socket, (char*)lpReadBuf, dwBytesRead, 0);
				if (bytesSent = SOCKET_ERROR){
					printf("sent error %d\n", WSAGetLastError());
					closesocket(socket);
					CloseHandle(hFile);
					return 1;
				}
				printf("thread %d send %d bytes \n", dwTid, bytesSent);
				dwSendFile += dwBytesRead;
				if (dwSendFile = dwFileSize){
					printf("download finish!\n");
					break;
				}
			}
			free(lpReadBuf);
			CloseHandle(hFile);
			closesocket(socket);
		}
		else if (!strcmp(szRequest, "get information")){
			char* msg = "this is infomation!";
			bytesSent = send(socket, msg, strlen(msg) + 1, 0);
			if (bytesSent == SOCKET_ERROR){
				printf("send error %d\n", WSAGetLastError());
				closesocket(socket);
				return 1;
			}
			printf("thread %d  send %d bytes\n", dwTid, bytesSent);
		}
		else{
			printf("unreferenced request\n");
		}
	}
	free(szRequest);
	closesocket(socket);
	return 0;
}

DWORD WINAPI CommunicationThread(LPVOID lpParameter){
	SOCKET socket = (SOCKET)lpParameter;
	return doCommunicate(socket);
	
}


int startLocalService(const char* port){
	WSADATA wsadata;
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo* result = NULL, hints;
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (iResult != 0){
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}
	//ZeroMemory(&hints, sizeof(hints));
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	//
	iResult = getaddrinfo(NULL, port, &hints, &result);
	if (iResult != 0){
		printf("getaddrinfo error failed:%d \n", iResult);
		WSACleanup();
		return 1;
	}
	//创建socket
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET){
		printf("socket failed:%d\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	//
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR){
		printf("bind error :%d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	printf("bind \n");
	freeaddrinfo(result);
	//
	iResult = listen(ListenSocket, SOMAXCONN);
	printf("start listening...\n");
	if (iResult == SOCKET_ERROR){
		printf("listen failed:%d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	while (1){
		printf("ready to accept\n");
		struct sockaddr sa;
		memset(&sa, 0, sizeof(sa));

		ClientSocket = accept(ListenSocket, &sa, NULL);
		printf("accept a connection\n");
		if (ClientSocket == INVALID_SOCKET){
			printf("accept failed:%d\n", WSAGetLastError());
			closesocket(ListenSocket);
			break;
		}
		HANDLE ht = CreateThread(NULL, 0, CommunicationThread, (LPVOID)ClientSocket, 0, NULL);
		if (!ht){
			printf("create thread error %d\n", GetLastError());
			break;
		}
	}
	WSACleanup();
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	int ret = startLocalService( DEFAULT_PORT);
	return ret;
}


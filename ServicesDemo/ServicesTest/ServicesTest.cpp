// ServicesTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>



SERVICE_STATUS SqlSrvServiceStatus;
SERVICE_STATUS_HANDLE SqlSrvServiceStatusHandle;


int lastIndexOf(char* str, char c){
	unsigned size = strlen(str);
	if (size < 1) return 0;
	for (int i = size - 1; i >= 0; i--){
		char cc = str[i];
		if (c == cc) return i;
	}
}

void subString(char* str, unsigned i){
	unsigned size = strlen(str);
	if (size<1 || i>size - 1) return;
	char* tmp = str + i + 1;
	memset(tmp, 0, size);
}



VOID SvcDebugOut(LPSTR String, DWORD Status);
void WINAPI SqlSrvServiceCtrlHandler(DWORD opcode);//服务管理控制程序，回调方法
void WINAPI SqlSrvServiceStart(DWORD ARGC, LPTSTR* argv);//服务主程序
DWORD SqlSrvServiceInitiallization(DWORD argc, LPTSTR* argv, DWORD* specificError);


VOID WINAPI SqlSrvServiceStart(DWORD argc, LPTSTR* argv){
	DWORD status;
	DWORD specificError;

	SqlSrvServiceStatus.dwServiceType = SERVICE_WIN32;
	SqlSrvServiceStatus.dwCurrentState = SERVICE_START_PENDING;//服务在运行
	SqlSrvServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;
	SqlSrvServiceStatus.dwWin32ExitCode = 0;
	SqlSrvServiceStatus.dwServiceSpecificExitCode = 0;
	SqlSrvServiceStatus.dwCheckPoint = 0;
	SqlSrvServiceStatus.dwWaitHint = 0;

	SqlSrvServiceStatusHandle = RegisterServiceCtrlHandlerA("A", SqlSrvServiceCtrlHandler);

	if (SqlSrvServiceStatusHandle == (SERVICE_STATUS_HANDLE)0){
		SvcDebugOut(" failed %d \n", GetLastError());
	}
	printf("%d", GetLastError());

	status = SqlSrvServiceInitiallization(argc, argv, &specificError);
	if (status != NO_ERROR){
		SqlSrvServiceStatus.dwCurrentState = SERVICE_STOPPED;
		SqlSrvServiceStatus.dwCheckPoint = 0;
		SqlSrvServiceStatus.dwWaitHint = 0;
		SqlSrvServiceStatus.dwWin32ExitCode = status;
		SqlSrvServiceStatus.dwServiceSpecificExitCode = specificError;
		SetServiceStatus(SqlSrvServiceStatusHandle, &SqlSrvServiceStatus);
		return;
	}
	SqlSrvServiceStatus.dwCurrentState = SERVICE_RUNNING;
	SqlSrvServiceStatus.dwCheckPoint = 0;
	SqlSrvServiceStatus.dwWaitHint = 0;
	if(!SetServiceStatus(SqlSrvServiceStatusHandle, &SqlSrvServiceStatus)){
		SvcDebugOut(" failed %d\n", GetLastError());
	}

	SvcDebugOut(" ok", 0);
	return;
}


DWORD SqlSrvServiceInitiallization(DWORD argc, LPTSTR* argv, DWORD* specificError){
	return 0;
}


void WINAPI SqlSrvServiceCtrlHandler(DWORD opcode){
//	DWORD status;
	FILE* f = NULL;
	char* newpath = NULL;
	int index = -1;
	switch (opcode)
	{
	case SERVICE_CONTROL_PAUSE:
		SqlSrvServiceStatus.dwCurrentState = SERVICE_PAUSED;
		break;
	case SERVICE_CONTROL_CONTINUE:
		SqlSrvServiceStatus.dwCurrentState = SERVICE_RUNNING;
		break;
	case SERVICE_CONTROL_STOP:
		SqlSrvServiceStatus.dwWin32ExitCode = 0;
		SqlSrvServiceStatus.dwCurrentState = SERVICE_STOPPED;
		SqlSrvServiceStatus.dwCheckPoint = 0;
		SqlSrvServiceStatus.dwWaitHint = 0;
		if (!SetServiceStatus(SqlSrvServiceStatusHandle, &SqlSrvServiceStatus)){
			SvcDebugOut(" failed %d\n", GetLastError());
		}
		SvcDebugOut(" ok", 0);
		break;
	case SERVICE_CONTROL_INTERROGATE:
		MessageBeep(MB_OK);	
		char path[1024];
		GetModuleFileNameA(NULL, path, sizeof(path));
		index = lastIndexOf(path, '\\');
		subString(path, index);
		strcat(path, "bb.txt");
		fopen_s(&f, path, "a+");
		fputs("a", f);
		fclose(f);
		break;
	default:
		SvcDebugOut(" unrecognized", 0);
		break;
	}
	//当前状态
	if (!SetServiceStatus(SqlSrvServiceStatusHandle, &SqlSrvServiceStatus)){
		SvcDebugOut(" failed %d\n", GetLastError());
	}
	return;
}


void SvcDebugOut(LPSTR str, DWORD status){
	char buf[1024];
	if (strlen(str) < 1000){
		sprintf(buf, str, status);
		OutputDebugStringA(buf);
	}
}




int _tmain1(int argc, _TCHAR* argv[]){
	char path[1024];
	GetModuleFileNameA(NULL, path, sizeof(path));
	int index = lastIndexOf(path, '\\');
	subString(path, index);
	strcat(path, "bb.txt");
	printf("%s\n", path);
	return 0;
}


int _tmain(int argc, _TCHAR* argv[])
{
	SC_HANDLE scManager;
	scManager = OpenSCManagerA(NULL, SERVICES_ACTIVE_DATABASEA, SC_MANAGER_ALL_ACCESS);
	printf("%d\n", GetLastError());


	SERVICE_TABLE_ENTRYA DispatchTable[] = {
		{ "A", (LPSERVICE_MAIN_FUNCTIONA)SqlSrvServiceStart }, { NULL, NULL }
	};
	if (!StartServiceCtrlDispatcherA(DispatchTable)){
		printf("%d\n", GetLastError());
	}

	char path[1024];
	GetModuleFileNameA(NULL, path, sizeof(path));	

	CreateServiceA(scManager, "A", "AA", \
		SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, path, NULL, NULL, NULL, NULL, NULL);
	printf("%d\n", GetLastError());

	

	
	return 0;
}


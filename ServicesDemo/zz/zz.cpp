// zz.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>


int _tmain(int argc, _TCHAR* argv[])
{

	char path[1024];
	DWORD len = 0;
	GetModuleFileNameA(NULL, path, sizeof(path));
	char* cc = strrchr(path, '\\');
	printf(cc);
	printf("\n");
	printf("%d", len);
	return 0;
}


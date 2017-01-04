#pragma once

#include <Windows.h>
#include <stdio.h>
#include <fltuser.h>
#include <FltUserStructures.h>
#include <Winuser.h>
#include <winbase.h>
#include <stdlib.h>
#include <shellapi.h>
#include <Shlobj.h>
#include "message_to_driver.h"
#pragma comment(lib, "FltLib.lib")

#define MINISPY_NAME      L"MiniSpy"

#define MINISPY_PORT_NAME    L"\\MiniSpyPort"

#define _CMD_PATH 296
#define PRO_LEN	100

typedef enum _MY_COMMAND
{
	ADD_PROTECTED_PATH,
	DEL_PROTECTED_PATH,
	START_SERVICE,
	STOP_SERVICE,
	ADD_PROTECTED_PROCESS,
	DEL_PROTECTED_PROCESS
} MY_COMMAND;

/*
* if protect file normal
* if protect dir ,fileType will fill with "dir"
* if protect ext file ,fileType will fill with "ext"
*/
typedef struct _INPUT_BUFFER
{
	MY_COMMAND command;
	WCHAR protectedPath[_CMD_PATH];
	WCHAR fileType[_CMD_PATH];
	CHAR process[PRO_LEN];
}INPUT_BUFFER, *PINPUT_BUFFER;

void flush_desktop()
{
	wchar_t user[100];
	wchar_t desk_path[200] = { 0 };
	int len;

	HWND desktop = GetDesktopWindow();

	len = GetEnvironmentVariable(L"USERNAME", user, 100);

	user[len] = 0;

	wcscat(desk_path, L"C:\\Users\\");

	wcscat(desk_path, user);

	wcscat(desk_path, L"\\Desktop");

	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
}

void start_service()
{
	HANDLE port = INVALID_HANDLE_VALUE;
	DWORD result;
	INPUT_BUFFER input;
	HRESULT hResult = S_OK;

	hResult = FilterConnectCommunicationPort(MINISPY_PORT_NAME,
		0,
		NULL,
		0,
		NULL,
		&port);

	if (IS_ERROR(hResult))
	{
		goto Exit;
	}

	ZeroMemory(&input, sizeof(INPUT_BUFFER));

	input.command = START_SERVICE;
	wcscpy(input.protectedPath, L"");
	wcscpy(input.fileType, L"");

	FilterSendMessage(port,
		&input,
		sizeof(INPUT_BUFFER),
		NULL,
		0,
		&result);

Exit:
	if (INVALID_HANDLE_VALUE != port)
	{
		CloseHandle(port);
	}
	flush_desktop();
}

void stop_service()
{
	HANDLE port = INVALID_HANDLE_VALUE;
	DWORD result;
	INPUT_BUFFER input;
	HRESULT hResult = S_OK;

	hResult = FilterConnectCommunicationPort(MINISPY_PORT_NAME,
		0,
		NULL,
		0,
		NULL,
		&port);

	if (IS_ERROR(hResult))
	{
		goto Exit;
	}

	ZeroMemory(&input, sizeof(INPUT_BUFFER));

	input.command = STOP_SERVICE;
	wcscpy(input.protectedPath, L"");
	wcscpy(input.fileType, L"");

	FilterSendMessage(port,
		&input,
		sizeof(INPUT_BUFFER),
		NULL,
		0,
		&result);

Exit:
	if (INVALID_HANDLE_VALUE != port)
	{
		CloseHandle(port);
	}
	flush_desktop();
}

VOID MyAddProtectPath(WCHAR *path, WCHAR *fileType)
{
	HANDLE port = INVALID_HANDLE_VALUE;
	DWORD result;
	INPUT_BUFFER input;
	HRESULT hResult = S_OK;

	hResult = FilterConnectCommunicationPort(MINISPY_PORT_NAME,
		0,
		NULL,
		0,
		NULL,
		&port);

	if (IS_ERROR(hResult))
	{
		goto Exit;
	}

	ZeroMemory(&input, sizeof(INPUT_BUFFER));

	input.command = ADD_PROTECTED_PATH;
	wcscpy(input.protectedPath, path);
	wcscpy(input.fileType, fileType);


	//发送 ADD_PROTECTED_PATH命令，准备和驱动层进行通信
	FilterSendMessage(port,
		&input,
		sizeof(INPUT_BUFFER),
		NULL,
		0,
		&result);

Exit:
	if (INVALID_HANDLE_VALUE != port)
	{
		CloseHandle(port);
	}
	flush_desktop();
}


VOID MyDelProtectPath(WCHAR *path, WCHAR *fileType)
{
	HANDLE port = INVALID_HANDLE_VALUE;
	DWORD result;
	INPUT_BUFFER input;
	HRESULT hResult = S_OK;

	hResult = FilterConnectCommunicationPort(MINISPY_PORT_NAME,
		0,
		NULL,
		0,
		NULL,
		&port);

	if (IS_ERROR(hResult))
	{
		goto Exit;
	}

	ZeroMemory(&input, sizeof(INPUT_BUFFER));

	input.command = DEL_PROTECTED_PATH;
	wcscpy(input.protectedPath, path);
	wcscpy(input.fileType, fileType);


	//发送 DEL_PROTECTED_PATH命令，准备和驱动层进行通信
	FilterSendMessage(port,
		&input,
		sizeof(INPUT_BUFFER),
		NULL,
		0,
		&result);

Exit:
	if (INVALID_HANDLE_VALUE != port)
	{
		CloseHandle(port);
	}
	flush_desktop();
}

VOID MyAddProtectProcess(CHAR* process)
{
	HANDLE port = INVALID_HANDLE_VALUE;
	DWORD result;
	INPUT_BUFFER input;
	HRESULT hResult = S_OK;

	hResult = FilterConnectCommunicationPort(MINISPY_PORT_NAME,
		0,
		NULL,
		0,
		NULL,
		&port);

	if (IS_ERROR(hResult))
	{
		goto Exit;
	}

	ZeroMemory(&input, sizeof(INPUT_BUFFER));

	input.command = ADD_PROTECTED_PROCESS;
	strcpy(input.process, process);


	//发送 DEL_PROTECTED_PATH命令，准备和驱动层进行通信
	FilterSendMessage(port,
		&input,
		sizeof(INPUT_BUFFER),
		NULL,
		0,
		&result);

Exit:
	if (INVALID_HANDLE_VALUE != port)
	{
		CloseHandle(port);
	}
}

VOID MyDelProtectProcess(CHAR* process)
{
	HANDLE port = INVALID_HANDLE_VALUE;
	DWORD result;
	INPUT_BUFFER input;
	HRESULT hResult = S_OK;

	hResult = FilterConnectCommunicationPort(MINISPY_PORT_NAME,
		0,
		NULL,
		0,
		NULL,
		&port);

	if (IS_ERROR(hResult))
	{
		goto Exit;
	}

	ZeroMemory(&input, sizeof(INPUT_BUFFER));

	input.command = DEL_PROTECTED_PROCESS;
	strcpy(input.process, process);


	//发送 DEL_PROTECTED_PATH命令，准备和驱动层进行通信
	FilterSendMessage(port,
		&input,
		sizeof(INPUT_BUFFER),
		NULL,
		0,
		&result);

Exit:
	if (INVALID_HANDLE_VALUE != port)
	{
		CloseHandle(port);
	}
}
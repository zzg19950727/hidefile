#ifndef MessDriver_H
#define MessDriver_H

#include <Windows.h>
#include <stdio.h>

VOID flush_desktop();

VOID start_service();

VOID stop_service();

VOID MyAddProtectPath(WCHAR *path, WCHAR *fileType);

VOID MyDelProtectPath(WCHAR *path, WCHAR *fileType);

VOID MyAddProtectProcess(CHAR* process);

VOID MyDelProtectProcess(CHAR* process);

#endif//MessDriver_H

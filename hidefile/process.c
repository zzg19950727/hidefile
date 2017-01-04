#include "process.h"

ULONG g_nProcessNameOffset = 0 ;
LIST_ENTRY g_ProcessListHead ;
KSPIN_LOCK g_ProcessListLock ;

BOOLEAN 
Ps_IsCurrentProcessMonitored(
     WCHAR* pwszFilePathName,
     ULONG uLength)
{
	BOOLEAN bRet = FALSE ;
	UCHAR szProcessName[16] = {0} ;
	PLIST_ENTRY TmpListEntryPtr = NULL ;
	PiPROCESS_INFO psProcessInfo = NULL ;
	WCHAR wszFilePathName[MAX_PATH] = {0} ;
	WCHAR* pwszExt = NULL ;
	//DbgPrint("Enter Ps_IsCurrentProcessMonitored()\n");
	__try{
		Ps_GetProcessName(szProcessName, NULL) ;
		/*
		// save file path name in local buffer
		RtlCopyMemory(wszFilePathName, pwszFilePathName, uLength*sizeof(WCHAR)) ;

		_wcslwr(wszFilePathName) ;
		if (wcsstr(wszFilePathName, L"\\local settings\\temp\\~wrd"))
		{
			bRet = TRUE ;
			///__leave ;
		}

		// go to end of file path name, save pointer in pwszExt
		pwszExt = wszFilePathName + uLength - 1 ;

		// verify file attribute, if directory, return false
		if (pwszFilePathName[uLength-1] == L'\\')
		{//if directory, filter it
			bRet = FALSE ;
			__leave ;
		}

		// redirect to file extension name(including point)
		while (((pwszExt != wszFilePathName) && (*pwszExt != L'\\')) && ((*pwszExt) != L'.')) //定向至扩展名
		{//direct into file extension
			pwszExt -- ;
		}

		// verify this is a file without extension name
		if ((pwszExt == wszFilePathName) || (*pwszExt == L'\\'))
		{//no file extension exists in input filepath name, filter it.
			///bRet = FALSE ;
			///__leave ;
			pwszExt[0] = L'.' ;
			pwszExt[1] = L'\0' ;
		}
		*/
		// compare current process name with process info in monitored list
		// if existing, match file extension name
		TmpListEntryPtr = g_ProcessListHead.Flink ;
		while(&g_ProcessListHead != TmpListEntryPtr)
		{
			psProcessInfo = CONTAINING_RECORD(TmpListEntryPtr, iPROCESS_INFO, ProcessList) ;

			if (!_strnicmp(psProcessInfo->szProcessName, szProcessName, strlen(szProcessName)))
			{
				bRet = psProcessInfo->bMonitor;
				__leave;
				INT nIndex = 0 ;

				if (psProcessInfo->wsszRelatedExt[0][0] == L'\0')
				{//no filter file extension, return monitor flag
					bRet = psProcessInfo->bMonitor;
					__leave ;
				}

				while (FALSE)
				{// judge wether current file extension name is matched with monitored file type in list
					if (psProcessInfo->wsszRelatedExt[nIndex][0] == L'\0')
					{
						bRet = FALSE ;
						break ;
					}
					else if ((wcslen(pwszExt) == wcslen(psProcessInfo->wsszRelatedExt[nIndex])) && !_wcsnicmp(pwszExt, psProcessInfo->wsszRelatedExt[nIndex], wcslen(pwszExt)))
					{// matched, return monitor flag
						bRet = psProcessInfo->bMonitor ;
						break ;
					}
					nIndex ++ ;
				}
				__leave ;
			}

			// move to next process info in list
			TmpListEntryPtr = TmpListEntryPtr->Flink ;
		}

		bRet = FALSE ;
	}
	__finally{
		/**/
		//Todo some post work here
	}
	//DbgPrint("Leave Ps_IsCurrentProcessMonitored()\n");
	return bRet ;
}

ULONG Psi_AddProcessInfo(PUCHAR pszProcessName, BOOLEAN bMonitor)
{
	ULONG uRes = 1 ;
	PiPROCESS_INFO psProcInfo = NULL ;
	BOOLEAN bRet ;

	DbgPrint("Enter Psi_AddProcessInfo()\n");

	__try{
		if (NULL == pszProcessName)
		{
			uRes = 0 ;
			_leave ;
		}

		/**
		* search for process name, if exists, donnot insert again
		*/
		bRet = Psi_SearchForSpecifiedProcessInList(pszProcessName, FALSE) ;
		if (bRet)
		{
			uRes = 0 ;
			_leave ;
		}

		/**
		* allocate process info structure
		*/
		psProcInfo = ExAllocatePoolWithTag(NonPagedPool, sizeof(iPROCESS_INFO), 'ipws') ;
		if (NULL == psProcInfo)
		{
			uRes = 0 ;
			_leave ;
		}

		RtlZeroMemory(psProcInfo, sizeof(iPROCESS_INFO)) ;
		psProcInfo->bMonitor = bMonitor ;
		RtlCopyMemory(psProcInfo->szProcessName, pszProcessName, strlen(pszProcessName)) ;
		ExInterlockedInsertTailList(&g_ProcessListHead, &psProcInfo->ProcessList, &g_ProcessListLock) ;
	}
	__finally{
	}
	DbgPrint("Leave Psi_AddProcessInfo()\n");
	return uRes ;
}

ULONG Psi_DelProcessInfo(PUCHAR pszProcessName, BOOLEAN bMonitor) 
{
	ULONG uRes = 0 ;
	BOOLEAN bRet ;
	DbgPrint("Enter Psi_DelProcessInfo()\n");
	__try{
		if (NULL == pszProcessName)
		{
			uRes = 0 ;
			_leave ;
		}

		/**
		* search for process name, if exists, donnot insert again
		*/
		bRet = Psi_SearchForSpecifiedProcessInList(pszProcessName, TRUE) ;
		if (!bRet)
		{
			uRes = 0 ;
			_leave ;
		}
	}
	__finally{
	}
	DbgPrint("Leave Psi_DelProcessInfo()\n");
	return uRes ;
}

static BOOLEAN Psi_SearchForSpecifiedProcessInList(PUCHAR pszProcessName, BOOLEAN bRemove)
{
	BOOLEAN bRet = TRUE ;
	KIRQL oldIrql ;
	PLIST_ENTRY TmpListEntryPtr = NULL ;
	PiPROCESS_INFO psProcessInfo = NULL ;

	__try{

		TmpListEntryPtr = g_ProcessListHead.Flink ;
		while(&g_ProcessListHead != TmpListEntryPtr)
		{
			psProcessInfo = CONTAINING_RECORD(TmpListEntryPtr, iPROCESS_INFO, ProcessList) ;

			if (!_strnicmp(psProcessInfo->szProcessName, pszProcessName, strlen(pszProcessName)))
			{
				bRet = TRUE;
				if (bRemove)
				{
					KeAcquireSpinLock(&g_ProcessListLock, &oldIrql) ;
					RemoveEntryList(&psProcessInfo->ProcessList) ;
					KeReleaseSpinLock(&g_ProcessListLock, oldIrql) ;
					ExFreePool(psProcessInfo) ;
					psProcessInfo = NULL ;
				}
				__leave ;
			}

			TmpListEntryPtr = TmpListEntryPtr->Flink ;
		}

		bRet = FALSE ;
	}
	__finally{
		/**/
		//Todo some post work here
	}

	return bRet ;
}


PCHAR Ps_GetProcessName(PCHAR pszProcessName, PEPROCESS pEProcess)
{
	PEPROCESS curproc = pEProcess;
	char *nameptr ;

	if (g_nProcessNameOffset)
	{
		if (!curproc)
		{
			curproc = PsGetCurrentProcess() ;
		}
		nameptr = (PCHAR)curproc + g_nProcessNameOffset ;
		strncpy(pszProcessName, nameptr, 15) ;
	}
	else
	{
		strcpy(pszProcessName, "???") ;
	}

	return pszProcessName ;
}

ULONG
Ps_GetProcessNameOffset(
	VOID
	)
{
	PEPROCESS curproc = NULL ;
	INT i = 0 ;

	curproc = PsGetCurrentProcess() ;

	for (i=0; i<3*PAGE_SIZE; i++)
	{
		if (!strncmp("System", (PCHAR)curproc+i, strlen("System")))
		{
			return i ;
		}
	}

	return 0 ;
}

#include "precomp.h"
 
LIST_ENTRY WhiteNameListHeader;
KSPIN_LOCK WhiteNameListLock;


VOID InitWhiteProcNameListAndLock()
{
	DbgPrint("Enter InitWhiteProcNameListAndLock()\n");

	InitializeListHead(&WhiteNameListHeader);
	KeInitializeSpinLock(&WhiteNameListLock);

	DbgPrint("Leave InitWhiteProcNameListAndLock()\n");

}

VOID AddNameToWhiteNameList(PWSTR xxName, PWSTR xxFileType)
{
	PWHITE_NAME_LIST whiteListNode;
	BOOL bRet;
	KIRQL oldIrql;

	bRet = IsNameInWhiteNameList(xxName, xxFileType);
	if (bRet)
	{
		return ;
	}

	whiteListNode = (PWHITE_NAME_LIST)ExAllocatePoolWithTag(NonPagedPool,sizeof(WHITE_NAME_LIST),'dsf ');
	if (whiteListNode == NULL)
	{
		return ;
	}

	wcscpy(whiteListNode->xxName,xxName);
	wcscpy(whiteListNode->xxFileType, xxFileType);
	KeAcquireSpinLock(&WhiteNameListLock,&oldIrql);
	InsertTailList(&WhiteNameListHeader,&whiteListNode->listNode);
	KeReleaseSpinLock(&WhiteNameListLock,oldIrql);
}


VOID DelNameFromWhiteNameList(PWSTR xxName, PWSTR xxFileType)
{
	PWHITE_NAME_LIST whiteList;
	PLIST_ENTRY pListNode;
	KIRQL oldIrql;
	
	if (!IsListEmpty(&WhiteNameListHeader))
	{
		for (pListNode = WhiteNameListHeader.Flink; pListNode!=&WhiteNameListHeader; pListNode = pListNode->Flink)
		{
			whiteList = CONTAINING_RECORD(pListNode,WHITE_NAME_LIST,listNode);
			if(wcscmp(whiteList->xxName,xxName)==0 && wcscmp(whiteList->xxFileType,xxFileType)==0)
			{
				KeAcquireSpinLock(&WhiteNameListLock,&oldIrql);
				RemoveEntryList(&whiteList->listNode);
				ExFreePoolWithTag(whiteList,'dsf ');
				KeReleaseSpinLock(&WhiteNameListLock,oldIrql);
				return ;

			}
		}
	}
}


BOOL IsNameInWhiteNameList(PWSTR xxName, PWSTR xxFileType)
{
	PWHITE_NAME_LIST whiteList;
	PLIST_ENTRY pListNode;

	if (!IsListEmpty(&WhiteNameListHeader))
	{
		for (pListNode = WhiteNameListHeader.Flink; pListNode != &WhiteNameListHeader; pListNode = pListNode->Flink)
		{
			whiteList = CONTAINING_RECORD(pListNode,WHITE_NAME_LIST,listNode);
			if (wcscmp(whiteList->xxName,xxName)==0 && wcscmp(whiteList->xxFileType,xxFileType)==0 )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

void wcslower(PWSTR str, int len)
{
	int i=0;
	for(; i<len; i++)
	{
		if ((str[i] >= L'A') && (str[i] <= L'Z'))
		{
			str[i] += (L'a' - L'A');
		}
	}
}

BOOL SearchIsProtect(PWSTR DirPath,PWSTR FileName)
{
	PWHITE_NAME_LIST whiteList;
	PLIST_ENTRY pListNode;
	PWSTR wTempPath, wTempName;
	size_t file_len = wcslen(FileName);
	size_t file_size = (wcslen(FileName) + 1) * sizeof(WCHAR);
	size_t dir_len = wcslen(DirPath);
	size_t dir_size = (wcslen(DirPath) + 1) * sizeof(WCHAR);

	//DbgPrint("***Enter SearchIsProtect()***\n");
	//对于.、..两个特殊的目录不过滤
	if (wcscmp(L".", FileName) == 0 || wcscmp(L"..", FileName) == 0)
		return FALSE;
	
	wTempPath=(PWSTR)ExAllocatePoolWithTag(NonPagedPool, dir_size,'dsf ');
	wTempName=(PWSTR)ExAllocatePoolWithTag(NonPagedPool, file_size,'dsf ');
	
	if( !wTempName || !wTempPath )
		return FALSE;

	RtlZeroMemory(wTempPath, dir_size);
	RtlZeroMemory(wTempName, file_size);

	RtlCopyMemory(wTempName,FileName, file_size);
	RtlCopyMemory(wTempPath,DirPath, dir_size);
	
	wcslower(wTempPath, dir_len);
	
	wcslower(wTempName, file_len);
	
	if (!IsListEmpty(&WhiteNameListHeader))
	{
		for (pListNode = WhiteNameListHeader.Flink; pListNode != &WhiteNameListHeader; pListNode =  pListNode->Flink)
		{
			//CONTAINING_RECORD这个宏的作用是：根据一个结构体实例中的某一个成员的地址，取到整个结构体实例的地址
			whiteList = CONTAINING_RECORD(pListNode,WHITE_NAME_LIST,listNode);
			//匹配文件后缀策略
			if( wcsstr(wTempName, whiteList->xxName) && wcscmp(whiteList->xxFileType,L"ext")==0 )
			{
				goto quit;
			}
			//匹配文件夹策略
			else if (wcsstr(wTempPath,whiteList->xxName) && wcscmp(whiteList->xxFileType,L"dir")==0 )
			{
				goto quit;
			}
			//匹配文件策略
			else if (wcscmp(whiteList->xxName,wTempPath)==0 && wcsstr(wTempName, whiteList->xxFileType) )
			{
				goto quit;
			}
		}
	}

	//DbgPrint("***Leave SearchIsProtect()***\n");
	return FALSE;
	
quit:
	if (wTempName)
	{
		ExFreePoolWithTag(wTempName,'dsf ');
	}
	if (wTempPath)
	{
		ExFreePoolWithTag(wTempPath,'dsf ');
	}
	return TRUE;
}

BOOL SearchFile(PWSTR FileName)
{
	int i = 0, j =0;
	size_t len = wcslen(FileName);
	size_t size = (len+1) * sizeof(WCHAR);
	PWHITE_NAME_LIST whiteList;
	PLIST_ENTRY pListNode;
	WCHAR* wTempPath, *wTempName;

	//DbgPrint("***Enter SearchFile()***\n");
	//对于.、..两个特殊的目录不过滤
	if (wcscmp(L".", FileName) == 0 || wcscmp(L"..", FileName) == 0 || len<1)
		return FALSE;

	if (FileName[len-1] == L'\\' || FileName[len - 1] == L':')
		return FALSE;
	
	wTempPath = (PWSTR)ExAllocatePoolWithTag(NonPagedPool, size, 'dsf ');
	wTempName = (PWSTR)ExAllocatePoolWithTag(NonPagedPool, size, 'dsf ');

	if (!wTempName || !wTempPath)
		return FALSE;

	RtlZeroMemory(wTempPath, size);
	RtlZeroMemory(wTempName, size);

	RtlCopyMemory(wTempName, FileName, size);
	RtlCopyMemory(wTempPath, FileName, size);

	wcslower(wTempPath, len);

	wcslower(wTempName, len);
	
	for (i = len - 1; i >= 0; i--)
	{
		if (wTempPath[i] == L'/' || wTempPath[i] == L'\\')
			break;
	}

	wTempPath[i] = L'\0';
	for (j = 0; j < len - i - 1; j++)
		wTempName[j] = wTempName[j + 1 + i];
	wTempName[j] = L'\0';
	
	if (!IsListEmpty(&WhiteNameListHeader))
	{
		for (pListNode = WhiteNameListHeader.Flink; pListNode != &WhiteNameListHeader; pListNode = pListNode->Flink)
		{
			//CONTAINING_RECORD这个宏的作用是：根据一个结构体实例中的某一个成员的地址，取到整个结构体实例的地址
			whiteList = CONTAINING_RECORD(pListNode, WHITE_NAME_LIST, listNode);
			//匹配文件后缀策略
			if (wcsstr(wTempName, whiteList->xxName) && wcscmp(whiteList->xxFileType, L"ext") == 0)
			{
				goto quit;
			}
			//匹配文件夹策略
			else if (wcsstr(wTempPath, whiteList->xxName) && wcscmp(whiteList->xxFileType, L"dir") == 0)
			{
				goto quit;
			}
			//匹配文件策略
			else if (wcscmp(whiteList->xxName, wTempPath) == 0 && wcsstr(wTempName, whiteList->xxFileType))
			{
				goto quit;
			}
		}
	}

	//DbgPrint("***Leave SearchFile()***\n");
	return FALSE;

quit:
	if (wTempName)
	{
		ExFreePoolWithTag(wTempName, 'dsf ');
	}
	if (wTempPath)
	{
		ExFreePoolWithTag(wTempPath, 'dsf ');
	}
	return TRUE;
}

VOID ClearWhiteNameList()
{
	PWHITE_NAME_LIST whiteList;
	PLIST_ENTRY pListNode;
	KIRQL oldIrql;
	
	KeAcquireSpinLock(&WhiteNameListLock,&oldIrql);
	while(!IsListEmpty(&WhiteNameListHeader))
	{
		pListNode = RemoveTailList(&WhiteNameListHeader);
		whiteList = CONTAINING_RECORD(pListNode,WHITE_NAME_LIST,listNode);
		if (whiteList)
		{
			ExFreePoolWithTag(whiteList,'dsf ');
		}
	}
	KeReleaseSpinLock(&WhiteNameListLock,oldIrql);
}


static int __inline Lower(int c)
{
	if ((c >= L'A') && (c <= L'Z'))
	{
		return(c + (L'a' - L'A'));
	}
	else
	{
		return(c);
	}
}


BOOL RtlPatternMatch(WCHAR * pat, WCHAR * str)
{
	register WCHAR * s;
	register WCHAR * p;
	BOOL star = FALSE;

loopStart:
	for (s = str, p = pat; *s; ++s, ++p) {
		switch (*p) {
		 case L'?':
			 if (*s == L'.') goto starCheck;
			 break;
		 case L'*':
			 star = TRUE;
			 str = s, pat = p;
			 if (!*++pat) return TRUE;
			 goto loopStart;
		 default:
			 if (Lower(*s) != Lower(*p))
				 goto starCheck;
			 break;
		} 
	} 
	if (*p == L'*') ++p;
	return (!*p);

starCheck:
	if (!star) return FALSE;
	str++;
	goto loopStart;
}
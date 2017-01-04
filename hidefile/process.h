#ifndef _PROCESS_H_
#define _PROCESS_H_
#include "precomp.h"
BOOLEAN Psi_SearchForSpecifiedProcessInList(PUCHAR pszProcessName, BOOLEAN bRemove) ;
ULONG   Psi_AddProcessInfo(PUCHAR pszProcessName, BOOLEAN bMonitor) ;
ULONG   Psi_DelProcessInfo(PUCHAR pszProcessName, BOOLEAN bMonitor) ;

/**
 * Get current process name
//*/
PCHAR 
Ps_GetProcessName(
	PCHAR pszProcessName,
	PEPROCESS pEProcess
	) ;

/**
 * Get process name offset in KPEB
 */
ULONG
Ps_GetProcessNameOffset(
	VOID
	) ;

/**
 * Is current process monitored
 */
BOOLEAN
Ps_IsCurrentProcessMonitored(
	WCHAR* pwszFilePathName,
	ULONG  uLength
	) ;

#pragma pack(1)

/**
 * process list used to monitor user processes
 */
typedef struct _iPROCESS_INFO{
	CHAR    szProcessName[16] ;
	BOOLEAN bMonitor ;
	WCHAR   wsszRelatedExt[64][6] ; /*< related file extension, containing maximum 10 extensions and each length is 6 characters */
	LIST_ENTRY ProcessList ;
}iPROCESS_INFO,*PiPROCESS_INFO ;

#pragma pack()

extern ULONG g_nProcessNameOffset ; /*< process name offset in PEB*/
extern LIST_ENTRY g_ProcessListHead ; /*< process info list */
extern KSPIN_LOCK g_ProcessListLock ; /*< process list operation lock */

#endif
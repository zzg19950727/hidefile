
#ifndef __PASSTHROUGH_H__
#define __PASSTHROUGH_H__

#define PT_DBG_PRINT( _dbgLevel, _string )          \
    (FlagOn(gTraceFlags,(_dbgLevel)) ?              \
        DbgPrint _string :                          \
        ((int)0))

DRIVER_INITIALIZE DriverEntry;
NTSTATUS
DriverEntry (
    __in PDRIVER_OBJECT DriverObject,
    __in PUNICODE_STRING RegistryPath
    );

NTSTATUS
PtUnload (
    __in FLT_FILTER_UNLOAD_FLAGS Flags
    );

NTSTATUS
InstanceSetup(
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ FLT_INSTANCE_SETUP_FLAGS Flags,
	_In_ DEVICE_TYPE VolumeDeviceType,
	_In_ FLT_FILESYSTEM_TYPE VolumeFilesystemType
);

FLT_PREOP_CALLBACK_STATUS
PtPreOperationPassThrough (
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext
    );


FLT_POSTOP_CALLBACK_STATUS
PtPostOperationPassThrough (
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in_opt PVOID CompletionContext,
    __in FLT_POST_OPERATION_FLAGS Flags
    );

FLT_PREOP_CALLBACK_STATUS
PreCreate(
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__deref_out_opt PVOID *CompletionContext
);

FLT_POSTOP_CALLBACK_STATUS
PtPostDirCtrlPassThrough (
						  __inout PFLT_CALLBACK_DATA Data,
						  __in PCFLT_RELATED_OBJECTS FltObjects,
						  __in_opt PVOID CompletionContext,
						  __in FLT_POST_OPERATION_FLAGS Flags 
						  );

FLT_PREOP_CALLBACK_STATUS
PtPreRead(
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__deref_out_opt PVOID *CompletionContext
);


FLT_PREOP_CALLBACK_STATUS
PtPreWrite(
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__deref_out_opt PVOID *CompletionContext
);

FLT_PREOP_CALLBACK_STATUS
PreQueryInformation(
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__deref_out_opt PVOID CompletionContext
);

FLT_POSTOP_CALLBACK_STATUS
PostQueryInformation(
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__in PVOID CompletionContext,
	__in FLT_POST_OPERATION_FLAGS Flags
);

NTSTATUS
SpyConnect(
		   __in PFLT_PORT ClientPort,
		   __in PVOID ServerPortCookie,
		   __in_bcount(SizeOfContext) PVOID ConnectionContext,
		   __in ULONG SizeOfContext,
		   __deref_out_opt PVOID *ConnectionCookie
		   );

VOID
SpyDisconnect(
			  __in_opt PVOID ConnectionCookie
			  );

NTSTATUS
SpyMessage (
			__in PVOID ConnectionCookie,
			__in_bcount_opt(InputBufferSize) PVOID InputBuffer,
			__in ULONG InputBufferSize,
			__out_bcount_part_opt(OutputBufferSize,*ReturnOutputBufferLength) PVOID OutputBuffer,
			__in ULONG OutputBufferSize,
			__out PULONG ReturnOutputBufferLength
			);

NTSTATUS
FileMonGetFullPathName(
					   PFLT_FILE_NAME_INFORMATION nameInfo,
					   WCHAR * fullpathname
					   );

void RemoveBacklash(wchar_t *szFileName);

#endif

#ifndef __COMMON_H__
#define __COMMON_H__

#define MINISPY_PORT_NAME                   L"\\MiniSpyPort"

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

#endif
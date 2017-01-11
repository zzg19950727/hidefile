#ifndef __BASE32_H__
#define __BASE32_H__

#include <fltKernel.h>
/*#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS
#include <winternl.h>
#include <ntstatus.h>
*/
#ifdef __cplusplus
extern "C" {
#endif

int  UStringBase32EncodeFileName(PUNICODE_STRING instr, PUNICODE_STRING outstr);
int  UStringBase32DecodeFileName(PUNICODE_STRING instr, PUNICODE_STRING outstr);

#ifdef __cplusplus
}       //  Balance extern "C" above
#endif

#endif
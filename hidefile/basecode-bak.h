#ifndef __BASE64_H__
#define __BASE64_H__
#include <fltKernel.h>
#ifdef __cplusplus
extern "C" {
#endif


/*************************************************
Function:       base64_to_ascii
Description:    decodes string from base64 to ascii
Input:          const char *in: the input string (NIL-terminated)
int inlen: length of input string
int maxlen: the output buffer size limit, 0 to ignore
Output:         unsigned char *out: decoded output string
Return:         length of output string on successful
less than 0 on error occur, then the output is invalid
Others:
*************************************************/
int  base64_decode(unsigned char *out, unsigned char *in, int inlen, int maxlen);

/*************************************************
Function:       ascii_to_base64
Description:    encodes string from ascii to base64
Input:          const char *in: the input string (NIL-terminated)
int inlen: length of input string
int maxlen: the output buffer size limit, 0 to ignore
Output:         unsigned char *out: decoded output string
Return:         length of output string on successful
less than 0 on error occur, then the output is invalid
Others:
*************************************************/
int  base64_encode(char *out, unsigned char *in, int inlen, int maxlen);
int  UStringBase64EncodeFileName(PUNICODE_STRING instr, PUNICODE_STRING outstr);
int  UStringBase64DecodeFileName(PUNICODE_STRING instr, PUNICODE_STRING outstr);

#ifdef __cplusplus
}       //  Balance extern "C" above
#endif

#endif
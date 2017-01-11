#include "basecode.h"
static const size_t BASE32_INPUT = 5;
static const size_t BASE32_OUTPUT = 8;
static const char* const BASE32_TABLE = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567=";


size_t Base32Encode(char* dest, const void* src, size_t size)
{
	if (dest && src)
	{
		unsigned char* pSrc = (unsigned char*)src;
		size_t dwSrcSize = size;
		size_t dwDestSize = 0;
		size_t dwBlockSize;
		unsigned char n1, n2, n3, n4, n5, n6, n7, n8;

		while (dwSrcSize >= 1)
		{
			/* Encode inputs */
			dwBlockSize = (dwSrcSize < BASE32_INPUT ? dwSrcSize : BASE32_INPUT);
			n1 = n2 = n3 = n4 = n5 = n6 = n7 = n8 = 0;
			switch (dwBlockSize)
			{
			case 5:
				n8 = (pSrc[4] & 0x1f);
				n7 = ((pSrc[4] & 0xe0) >> 5);
			case 4:
				n7 |= ((pSrc[3] & 0x03) << 3);
				n6 = ((pSrc[3] & 0x7c) >> 2);
				n5 = ((pSrc[3] & 0x80) >> 7);
			case 3:
				n5 |= ((pSrc[2] & 0x0f) << 1);
				n4 = ((pSrc[2] & 0xf0) >> 4);
			case 2:
				n4 |= ((pSrc[1] & 0x01) << 4);
				n3 = ((pSrc[1] & 0x3e) >> 1);
				n2 = ((pSrc[1] & 0xc0) >> 6);
			case 1:
				n2 |= ((pSrc[0] & 0x07) << 2);
				n1 = ((pSrc[0] & 0xf8) >> 3);
				break;

			default:
				break;
			}
			pSrc += dwBlockSize;
			dwSrcSize -= dwBlockSize;

			
			/* Padding */
			switch (dwBlockSize)
			{
			case 1: n3 = n4 = 32;
			case 2: n5 = 32;
			case 3: n6 = n7 = 32;
			case 4: n8 = 32;
			case 5:
				break;

			default:
				break;
			}

			/* 8 outputs */
			*dest++ = BASE32_TABLE[n1];
			*dest++ = BASE32_TABLE[n2];
			*dest++ = BASE32_TABLE[n3];
			*dest++ = BASE32_TABLE[n4];
			*dest++ = BASE32_TABLE[n5];
			*dest++ = BASE32_TABLE[n6];
			*dest++ = BASE32_TABLE[n7];
			*dest++ = BASE32_TABLE[n8];
			dwDestSize += BASE32_OUTPUT;
		}
		*dest++ = '\x0'; /*append terminator*/

		return dwDestSize;
	}
	else
		return 0; /*ERROR - null pointer*/
}

static const size_t BASE32_MAX_PADDING = 6;
static const unsigned char BASE32_MAX_VALUE = 31;
static const unsigned char BASE32_DTABLE[0x80] = {
	/*00-07*/ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	/*08-0f*/ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	/*10-17*/ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	/*18-1f*/ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	/*20-27*/ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	/*28-2f*/ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	/*30-37*/ 0xFF, 0xFF, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, /*6 = '2'-'7'*/
	/*38-3f*/ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0xFF, 0xFF, /*1 = '='*/
	/*40-47*/ 0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, /*7 = 'A'-'G'*/
	/*48-4f*/ 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, /*8 = 'H'-'O'*/
	/*50-57*/ 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, /*8 = 'P'-'W'*/
	/*58-5f*/ 0x17, 0x18, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, /*3 = 'X'-'Z'*/
	/*60-67*/ 0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, /*7 = 'a'-'g' (same as 'A'-'G')*/
	/*68-6f*/ 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, /*8 = 'h'-'o' (same as 'H'-'O')*/
	/*70-77*/ 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, /*8 = 'p'-'w' (same as 'P'-'W')*/
	/*78-7f*/ 0x17, 0x18, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF  /*3 = 'x'-'z' (same as 'X'-'Z')*/
};

size_t Base32Decode(void* dest, const char* src, size_t size)
{
	if (dest && src && (size % BASE32_OUTPUT == 0))
	{
		unsigned char* pDest = (unsigned char*)dest;
		size_t dwSrcSize = size;
		size_t dwDestSize = 0;
		unsigned char in1, in2, in3, in4, in5, in6, in7, in8;

		while (dwSrcSize >= 1)
		{
			/* 8 inputs */
			in1 = *src++;
			in2 = *src++;
			in3 = *src++;
			in4 = *src++;
			in5 = *src++;
			in6 = *src++;
			in7 = *src++;
			in8 = *src++;
			dwSrcSize -= BASE32_OUTPUT;

			/* Validate ASCII */
			if (in1 >= 0x80 || in2 >= 0x80 || in3 >= 0x80 || in4 >= 0x80
				|| in5 >= 0x80 || in6 >= 0x80 || in7 >= 0x80 || in8 >= 0x80)
				return 0; /*ERROR - invalid base32 character*/

						  /* Convert ASCII to base32 */
			in1 = BASE32_DTABLE[in1];
			in2 = BASE32_DTABLE[in2];
			in3 = BASE32_DTABLE[in3];
			in4 = BASE32_DTABLE[in4];
			in5 = BASE32_DTABLE[in5];
			in6 = BASE32_DTABLE[in6];
			in7 = BASE32_DTABLE[in7];
			in8 = BASE32_DTABLE[in8];

			/* Validate base32 */
			if (in1 > BASE32_MAX_VALUE || in2 > BASE32_MAX_VALUE)
				return 0; /*ERROR - invalid base32 character*/
						  /*the following can be padding*/
			if (in3 > BASE32_MAX_VALUE + 1 || in4 > BASE32_MAX_VALUE + 1 || in5 > BASE32_MAX_VALUE + 1
				|| in6 > BASE32_MAX_VALUE + 1 || in7 > BASE32_MAX_VALUE + 1 || in8 > BASE32_MAX_VALUE + 1)
				return 0; /*ERROR - invalid base32 character*/

						  /* 5 outputs */
			*pDest++ = ((in1 & 0x1f) << 3) | ((in2 & 0x1c) >> 2);
			*pDest++ = ((in2 & 0x03) << 6) | ((in3 & 0x1f) << 1) | ((in4 & 0x10) >> 4);
			*pDest++ = ((in4 & 0x0f) << 4) | ((in5 & 0x1e) >> 1);
			*pDest++ = ((in5 & 0x01) << 7) | ((in6 & 0x1f) << 2) | ((in7 & 0x18) >> 3);
			*pDest++ = ((in7 & 0x07) << 5) | (in8 & 0x1f);
			dwDestSize += BASE32_INPUT;

			/* Padding */
			if (in8 == BASE32_MAX_VALUE + 1)
			{
				--dwDestSize;
				//assert((in7 == BASE32_MAX_VALUE + 1 && in6 == BASE32_MAX_VALUE + 1) || (in7 != BASE32_MAX_VALUE + 1));
				if (in6 == BASE32_MAX_VALUE + 1)
				{
					--dwDestSize;
					if (in5 == BASE32_MAX_VALUE + 1)
					{
						--dwDestSize;
						//assert((in4 == BASE32_MAX_VALUE + 1 && in3 == BASE32_MAX_VALUE + 1) || (in4 != BASE32_MAX_VALUE + 1));
						if (in3 == BASE32_MAX_VALUE + 1)
						{
							--dwDestSize;
						}
					}
				}
			}
		}

		return dwDestSize;
	}
	else
		return 0; /*ERROR - null pointer, or size isn't a multiple of 8*/
}


int  UStringBase32EncodeFileName(PUNICODE_STRING instr, PUNICODE_STRING outstr)
{
	char temp[1024];//is enough only for filename
	ANSI_STRING astr;

	
	RtlInitAnsiString(&astr,NULL);
	  
	RtlUnicodeStringToAnsiString(&astr,instr, TRUE);
	Base32Encode(temp,astr.Buffer, astr.Length);
	RtlFreeAnsiString(&astr);

	RtlInitAnsiString(&astr, temp);
	RtlAnsiStringToUnicodeString(outstr, &astr, TRUE);
	return 0;
}
int  UStringBase32DecodeFileName(PUNICODE_STRING instr, PUNICODE_STRING outstr)
{
	char temp[1024] = { 0 };//is enough only for filename
	ANSI_STRING astr;


	RtlInitAnsiString(&astr, NULL);

	RtlUnicodeStringToAnsiString(&astr, instr, TRUE);
	Base32Decode(temp, astr.Buffer, astr.Length);
	RtlFreeAnsiString(&astr);

	RtlInitAnsiString(&astr, temp);
	RtlAnsiStringToUnicodeString(outstr, &astr, TRUE);
	return 0;
}
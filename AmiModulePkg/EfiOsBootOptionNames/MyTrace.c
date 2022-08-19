//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2011, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//#include "..\..\..\Include\efi.h"
#include <token.h>
#include <Library/PcdLib.h>

#define MY_TRACE(arg) my_trace arg
#define INIT_MY_TRACE init_my_trace();

VOID init_my_trace(VOID);

VOID my_trace(UINTN Level, CHAR8 *sFormat,...);
UINT8 IoRead8(UINT16 Port);
void IoWrite8(UINT16 Port,UINT8 Value);
#if MyTrace_PEI_SUPPORT
UINT64 Div64 (
	IN UINT64	Dividend,
	IN UINTN	Divisor,	//Can only be 31 bits.
  	OUT UINTN	*Remainder OPTIONAL
  	);
#endif

#define LSR_OFFSET			0x05
#define LCR_OFFSET			0x03
#define DIV_MSB				0x01
#define DIV_LSB				0
#define TRANSMIT_READY_BIT	        0x020

#define MT_INTSIZEOF(n)( (sizeof(n) + sizeof(UINTN) - 1) & ~(sizeof(UINTN) - 1) )
#define MT_va_arg(ap,t)    ( *(t *)((ap += MT_INTSIZEOF(t)) - MT_INTSIZEOF(t)) )
#define MT_Strtol(nptr,endptr,base) MT_StrtolEx(nptr,endptr,base,1)
#define MT_Itoa(value,string,radix) MT_ItoaEx((INT32)value,string,radix,FALSE)
#define MT_I64toa(value,string,radix) MT_ItoaEx(value,string,radix,TRUE)
#define MT_VA_LIST MT_va_list
#ifndef MT_va_start
#define MT_va_start(ap,v)  ( ap = (MT_va_list)&(v) + MT_INTSIZEOF(v) )
#endif
#define MT_va_arg(ap,t)    ( *(t *)((ap += MT_INTSIZEOF(t)) - MT_INTSIZEOF(t)) )
#define MT_va_end(ap)      ( ap = (MT_va_list)0 )

#ifndef EFI_ERROR_BIT
#define EFI_ERROR_BIT 	((INTN)1<<(sizeof(UINTN)*8-1))
#endif
#ifndef NUM_OF_WARNINGS
#define NUM_OF_WARNINGS	4
#endif

#ifndef NUM_OF_AMI_ERRORS
#define NUM_OF_AMI_ERRORS 2
#endif
#ifndef NUM_OF_ERRORS
#define NUM_OF_ERRORS	29
#endif

#ifndef true
#define true  TRUE
#define false FALSE
#endif

#define MT_COM_BASE_ADDR 0x3f8

typedef CHAR8 *MT_VA_LIST;
char * MT_ItoaEx(INT64 value, char *string,int radix, BOOLEAN is_int64);
char * MT_ItoaHelper(INT64 value, char *string,int radix, BOOLEAN is_int64);
UINTN MT_Strlen(char *string);
char * MT_Strupr(char *str);

char MT_EfiUnknown[] = "Status Code";	//If unknown value.
char MT_EfiWarningStatus[][26] =
{
	"EFI_WARN_UNKNOWN_GLYPH",
	"EFI_WARN_DELETE_FAILURE",
	"EFI_WARN_WRITE_FAILURE",
	"EFI_WARN_BUFFER_TOO_SMALL"
};
char MT_AmiErrorStatus[][25] = 
{
	"EFI_DBE_EOF",
	"EFI_DBE_BOF"
};
char MT_EfiErrorStatus[][25] = 
{
	"EFI_LOAD_ERROR",
	"EFI_INVALID_PARAMETER",
	"EFI_UNSUPPORTED",
	"EFI_BAD_BUFFER_SIZE",
	"EFI_BUFFER_TOO_SMALL",
	"EFI_NOT_READY",
	"EFI_DEVICE_ERROR",
	"EFI_WRITE_PROTECTED",
	"EFI_OUT_OF_RESOURCES",
	"EFI_VOLUME_CORRUPTED",
	"EFI_VOLUME_FULL",
	"EFI_NO_MEDIA",
	"EFI_MEDIA_CHANGED",
	"EFI_NOT_FOUND",
	"EFI_ACCESS_DENIED",
	"EFI_NO_RESPONSE",
	"EFI_NO_MAPPING",
	"EFI_TIMEOUT",
	"EFI_NOT_STARTED",
	"EFI_ALREADY_STARTED",
	"EFI_ABORTED",
	"EFI_ICMP_ERROR",
	"EFI_TFTP_ERROR",
	"EFI_PROTOCOL_ERROR",
	"EFI_INCOMPATIBLE_VERSION",
	"EFI_SECURITY_VIOLATION",
	"EFI_CRC_ERROR",
	"EFI_NOT_AVAILABLE_YET",
	"EFI_UNLOAD_IMAGE",
	"EFI_DBE_EOF",
	"EFI_DBE_BOF"
};


UINTN MT_Sprintf_va_list( char *buffer, char *format, MT_va_list arg );
UINTN MT_Sprintf_s_va_list(char *Buffer, UINTN BufferSize, char *Format, MT_va_list Marker);
long MT_StrtolEx(char *nptr,char **endptr,int base, int increment);
UINTN MT_Sprintf_s( char *Buffer, UINTN BufferSize, char *Format, ... );


//<AMI_PHDR_START>
//============================================================================
// Procedure:	init_my_trace
//
// Description:	
//
// Input:
//
// Output: EFI_SUCCESS
//
//============================================================================
//<AMI_PHDR_END>
VOID init_my_trace(VOID)
{
#if MT_COM_BASE_ADDR
	IoWrite8(MT_COM_BASE_ADDR+3,0x87);
	IoWrite8(MT_COM_BASE_ADDR  ,0x01);
	IoWrite8(MT_COM_BASE_ADDR+1,0x00);
	IoWrite8(MT_COM_BASE_ADDR+3,0x07);
#else
	/*IoWrite8((UINT16)PcdGet64 (PcdSerialRegisterBase)+3,0x87);
	IoWrite8((UINT16)PcdGet64 (PcdSerialRegisterBase)  ,0x01);
	IoWrite8((UINT16)PcdGet64 (PcdSerialRegisterBase)+1,0x00);
	IoWrite8((UINT16)PcdGet64 (PcdSerialRegisterBase)+3,0x07);*/
#endif
}

//<AMI_PHDR_START>
//============================================================================
// Procedure:	my_SendByte
//
// Description:	
//
// Input:
//      
// Output: EFI_SUCCESS
//
//============================================================================
//<AMI_PHDR_END>
VOID my_SendByte(UINT8 Byte)
{
#if MT_COM_BASE_ADDR
	UINT8 Buffer8;
	do {
		// read the ready signal to see if the serial port is
		//	ready for the next byte
		Buffer8 = IoRead8(MT_COM_BASE_ADDR + LSR_OFFSET);
	// Loop until the serial port is ready for the next byte
	} while ( (Buffer8 & TRANSMIT_READY_BIT) == FALSE);
	IoWrite8(MT_COM_BASE_ADDR, Byte);

#else
	/*UINT8 Buffer8;
	do {
		// read the ready signal to see if the serial port is
		//	ready for the next byte
		Buffer8 = IoRead8((UINT16)PcdGet64 (PcdSerialRegisterBase) + LSR_OFFSET);
	// Loop until the serial port is ready for the next byte
	} while ( (Buffer8 & TRANSMIT_READY_BIT) == FALSE);
	IoWrite8((UINT16)PcdGet64 (PcdSerialRegisterBase), Byte);*/
#endif
}

//<AMI_PHDR_START>
//============================================================================
// Procedure:	SerialOutput
//
// Description:	Writes the given string to the serial port byte by byte 
//              using the function SendByte.
//
// Input:
//      IN EFI_PEI_SERVICES **PeiServices
//		*String - String to be sent over the serial port
//
// Output: EFI_SUCCESS
//
//============================================================================
//<AMI_PHDR_END>
void my_SerialOutput(
	IN	UINT8	*String
	)
{
	UINT8		*Buffer;

	// first make data useable
	Buffer = String;

	// now send data one byte at a time until the string hits the end of string
	// or we hit the max number of characters
	while (*Buffer)
	{
		// replace "\n" with "\r\n"
		if (*Buffer=='\n') my_SendByte('\r');
		my_SendByte(*Buffer);

		// move Data pointer to the next byte
		Buffer++;
	}

}

VOID my_PrepareStatusCodeString(VOID* pBuffer, CHAR8 *sFormat, MT_va_list ArgList)
{
	MT_Sprintf_va_list((CHAR8*)(pBuffer),sFormat,ArgList);
}
VOID my_PrintDebugMessageVaList(UINTN Level, CHAR8 *sFormat, MT_va_list ArgList)
{
	CHAR8  Buffer[1024];
	my_PrepareStatusCodeString(Buffer,sFormat,ArgList);
	my_SerialOutput(Buffer);

}
VOID my_trace(UINTN Level, CHAR8 *sFormat,...)
{
	MT_va_list	ArgList = MT_va_start(ArgList,sFormat);
	my_PrintDebugMessageVaList(Level, sFormat, ArgList);
	MT_va_end(ArgList);
}

//*************************************************************************
//
// Name: MT_Sprintf_va_list
//
// Description:
//  UINTN MT_Sprintf_va_list(OUT CHAR8 *buffer, IN CHAR8 *format,
// IN MT_va_list arg) produces a null-terminated ASCII string in the output
// buffer.  The ASCII string is produced by parsing the format string
// specified by format.  Arguments are pulled from the variable argument
// list, specified by arg, based on the contents of the format string.  The
// number of ASCII characters in the produced output buffer is returned, not
// including the null-terminator.
//
// Input:
//  buffer - Pointer to a null-terminated output ASCII string buffer.  
//  User is responsible for allocating the necessary memory resources!
//
//  BufferSize - Size of the buffer in bytes.
//
//  format - Pointer to a null-terminated format ASCII string.
//
//  arg - Marker for the variable argument list.
//
// Output:
//  UINTN - number of ASCII characters in the produced output buffer, not
//  including the null-terminator.
//
// Referrals:
//  MT_va_arg
//  MT_Strtol
//  Sprintf
//  MT_Strupr
//  MT_I64toa
//  MT_Itoa
//  MT_Strlen
// 
// Notes: Refer to Sprintf function for format string syntax.
//
//*************************************************************************
UINTN MT_Sprintf_va_list( char *buffer, char *format, MT_va_list arg )
{
    return MT_Sprintf_s_va_list( buffer, 0, format, arg );
}

//*************************************************************************
//
// Name: MT_Sprintf_s_va_list
//
// Description:
//  UINTN MT_Sprintf_s_va_list(OUT CHAR8 *Buffer, IN UINTN BufferSize,
// IN CHAR8 *Format, IN MT_va_list Marker) produces a null-terminated ASCII
// string in the output Buffer of size BufferSize.  The ASCII string is
// produced by parsing the format string specified by Format.  Arguments are
// pulled from the variable argument list, Marker, based on the contents of
// the format string.  The number of ASCII characters in the produced output
// buffer is returned, not including the null-terminator.
//
// Input:
//  OUT CHAR8 *Buffer
// Pointer to a null-terminated output ASCII string buffer.  User is
// responsible for allocating the necessary memory resources!
//
//  IN UINTN BufferSize
// The size, in bytes, of the output Buffer.
//
//  IN CHAR8 *Format
// Pointer to a null-terminated format ASCII string.
//
//  IN MT_va_list Marker
// Marker for the variable argument list.
//
// Output:
//  UINTN number of ASCII characters in the produced output buffer, not
// including the null-terminator.
//
// Modified:
//
// Referrals:
//  MT_Sprintf_va_list
//  MemCpy
// 
// Notes:	
//  This is a helper function for MT_Sprintf_s.  Refer to Sprintf function for
// format string syntax.
// 
//*************************************************************************
UINTN MT_Sprintf_s_va_list(char *Buffer, UINTN BufferSize, char *Format, MT_va_list Marker)
{
	char filler;
	int width;
	char numbuffer[32];
	UINTN strlength;
	char *strp = Format;
	char *buffp = Buffer;
	char *_strp, *str2p;
	int	radix;
    BOOLEAN its_int64;
    UINTN n;

    if (Buffer==NULL || Format==NULL) return -1;
    //If BuuferSize is 0, no size check required
    while(BufferSize!=1 && *strp) {
        if (*strp != '%'){
            *buffp++ = *strp++;
            BufferSize--;
            continue;
        }
        strp++; //skip %
        if (*strp=='%') {
            strp++;
            *buffp++ = '%';
            BufferSize--;
            continue;
        }
        
        filler = ' ';
    
        //Get flags
        if (*strp == '0') { 
            filler = '0'; 
            *strp++;
        }

        //Get Width
        if (*strp == '*') { //width is supplied by argument.
            strp++;
            width = MT_va_arg(Marker,int);
        } else {
            width = MT_Strtol(strp,&_strp,10);	//If no width, then 0 returned.
            strp=_strp;
        }
    
        //Get type.
        if (*strp == 's' || *strp == 'a'/*to be compatible with Intel Print library*/) { //string
            char *str = MT_va_arg(Marker,char *);
            while (*str) {
                if ( --BufferSize == 0 ) {
                    *buffp = 0; // null terminator
                    return buffp - Buffer;
                }
                *buffp++ = *str++;
            }
            ++strp;
            continue;
        }
        
        if (*strp == 'S') { // unicode string
            CHAR16 *str = MT_va_arg(Marker,CHAR16 *);
            while (*str) {
                if ( --BufferSize == 0 ) {
                    *buffp = 0; // null terminator
                    return buffp - Buffer;
                }
                *buffp++ = (char)*str++;
            }
            ++strp;
            continue;
        }
        
        if (*strp == 'c') { //character
            *buffp++ = MT_va_arg(Marker,char);
            ++strp;
            continue;
        }
        
        if ((*strp & 0xdf) == 'G') {     //'G' or 'g'
            EFI_GUID *guid = MT_va_arg(Marker,EFI_GUID*);
            CHAR8 *origp = buffp;

            n = MT_Sprintf_s( 
                buffp,
                BufferSize,
                "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                guid->Data1, guid->Data2, guid->Data3, guid->Data4[0],
                guid->Data4[1], guid->Data4[2], guid->Data4[3], guid->Data4[4],
                guid->Data4[5], guid->Data4[6], guid->Data4[7]
            );
            buffp += n;
            if (*strp == 'G') 
                MT_Strupr(origp);
            BufferSize -= n;
            ++strp;
            continue;
        }
        
        if (*strp == 'r') {
            EFI_STATUS Status = MT_va_arg(Marker,EFI_STATUS);

            if (Status == 0)
                n = MT_Sprintf_s(buffp, BufferSize, "EFI_SUCCESS");
            else if ((Status & EFI_ERROR_BIT) == 0 ) {
                if (Status > NUM_OF_WARNINGS) 
                    n = MT_Sprintf_s( buffp, BufferSize, "%s(%08x)", MT_EfiUnknown, Status );
                else 
                    n = MT_Sprintf_s( buffp, BufferSize, "%s", MT_EfiWarningStatus[Status-1] );
            } else {
                Status &= ~EFI_ERROR_BIT;
                if (Status > 0xc0000000) {
                    Status -= 0xc0000000;
                    if (Status > NUM_OF_AMI_ERRORS) 
                        n = MT_Sprintf_s( buffp, BufferSize, "%s(%08x)" ,MT_EfiUnknown, Status );
                    else 
                        n = MT_Sprintf_s( buffp, BufferSize, "%s", MT_AmiErrorStatus[Status-1] );
                } else {
                    if (Status > NUM_OF_ERRORS) 
                        n = MT_Sprintf_s( buffp, BufferSize, "%s(%08x)", MT_EfiUnknown, Status );
                    else 
                        n = MT_Sprintf_s( buffp, BufferSize, "%s", MT_EfiErrorStatus[Status-1] );
                }
            }
            buffp += n;
            BufferSize -= n;
            ++strp;
            continue;
        }
        
        if (*strp == 'l') { 
            strp++; 
            its_int64 = TRUE;
        } else 
            its_int64 = FALSE;
    
        if (*strp == 'd' || *strp == 'i') 
            radix = 10;
        else if ( (*strp & 0xdf) == 'X' ) 
            radix = 16; //'X' or 'x'
        else 
            continue;	//invalid *strp
        
        if (its_int64) 
            MT_I64toa( MT_va_arg(Marker,INT64), numbuffer, radix );
        else 
            MT_Itoa( MT_va_arg(Marker,int), numbuffer, radix );
        if (*strp == 'X') 
            MT_Strupr(numbuffer);
        
        strlength = MT_Strlen(numbuffer);
        while ( strlength++ < (unsigned)width ) {
            if ( --BufferSize == 0 ) {
                *buffp = 0; // null terminator
                return buffp - Buffer;
            }
            *buffp++ = filler;
        }
        
        str2p = numbuffer;
        while (*str2p) {
            if ( --BufferSize == 0 ) {
                *buffp = 0; // null terminator
                return buffp - Buffer;
            }
            *buffp++ = *str2p++;
        }
        
        strp++;
    }
    
    *buffp = 0;
    return buffp - Buffer;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: MT_Strlen
//
// Description:
//  UINTN MT_Strlen(IN CHAR8 *string) takes a null-terminated CHAR8 string and
// returns its UINTN length (not including the null-terminator).
//
// Input:
//  IN CHAR8 *string
// Pointer to a null-terminated CHAR8 string.
//
// Output:
//  UINTN length of the string (not including the null-terminator).
//
// Modified:
//
// Referrals:
// 
// Notes:	
// 
//<AMI_PHDR_END>
//*************************************************************************
UINTN MT_Strlen(char *string) {
	UINTN length=0;
	while(*string++) length++;
	return length;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: MT_Strupr
//
// Description:
//  CHAR8* MT_Strupr(IN CHAR8 *str) converts a CHAR8 string to all uppercase
// characters.
//
// Input:
//  IN CHAR8 *str
// CHAR8 string to be converted to all uppercase.
//
// Output:
//  CHAR8* string pointer to the start of the modified string.
//
// Modified:
//
// Referrals:
// 
// Notes:	
//  Original string will be modified!
// 
//<AMI_PHDR_END>
//*************************************************************************
char * MT_Strupr(char *str) {
	char *strptr=str;
	while (*strptr) {	//End of string?
		if (*strptr >=0x61 && *strptr<=0x7a) *strptr-=0x20;	//If char is lower case, convert to upper.
		strptr++;		//Next char
	}
	return str;
}


//*************************************************************************
//<AMI_PHDR_START>
//
// Name: MT_StrtolEx
//
// Description:
//  INT64 MT_StrtolEx(IN CHAR8 *nptr, OUT CHAR8 **endptr, IN INT32 base,
// IN INT32 increment) converts a CHAR8 string representation of a number
// into an INT64 representation.
//
// Input:
//  IN CHAR8 *nptr
// CHAR8 string to be converted into an INT64.
//
//  OUT CHAR8 **endptr
// CHAR8 string pointer to the final character read from nptr.
//
//  IN INT32 base
// The base of the string being passed in.  If 0, string will be parsed by
// defaults; preceding "0x" treated as hexadecimal, preceding "0" treated as
// octal, everything else treated as decimal.
//
//  IN INT32 increment
// The number of characters in between each number digit.  For example,
// "14131" would be 1; "1 4 1 3 1" would be 2.
//
// Output:
//  INT64 representation of the number in the ASCII nptr string using the
// provided base and increment amount.
//
// Modified:
//
// Referrals:
// 
// Notes:	
// 
//<AMI_PHDR_END>
//*************************************************************************
long MT_StrtolEx(char *nptr,char **endptr,int base, int increment) {
	unsigned char overflow=false;
	char sign=1;
	char digit;
	long value=0;

	while(*nptr == ' ' || *nptr == '\t') nptr+=increment;
	if (*nptr == '\0') {*endptr=(char*)nptr;return 0;}

	if (*nptr == '-') {sign = -1;nptr+=increment;}
	if (*nptr == '+') nptr+=increment;

	if (!base) {
		base=10;
		if (*nptr == '0') {
			base=8;
			nptr+=increment;
			if ((*nptr&0xdf)=='X') {	//Check for 'x' or 'X'
				base=16;
				nptr+=increment;
			}
		}
	}
	while(true) {
		if (*nptr >= '0' && *nptr <='9') digit=*nptr-'0';
		else if ((*nptr&0xdf)>='A'&& (*nptr&0xdf)<='Z') digit = (*nptr&0xdf) - 'A' + 0xa;
		else break;
		if (digit >= base) break;
		value = value * base + digit;
		if (sign==1) {
			if ((unsigned) value >= (unsigned) 0x80000000) overflow=true;
		} else if ((unsigned) value > (unsigned) 0x80000000) overflow=true;
		nptr+=increment;
	}
	*endptr=(char*)nptr;
	if (overflow) {
		value=0x7fffffff;
		if (sign==-1) value++;
	}
	return value*sign;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: MT_ItoaEx
//
// Description:
//  CHAR8* MT_ItoaEx(IN INT64 value, IN OUT CHAR8 *string, IN INT32 radix, 
// IN BOOLEAN is_int64) converts an INT64 value into a CHAR8 ASCII 
// representation and returns a pointer to the ASCII string.
//
// Input:
//  IN INT64 value
// Value that needs to be converted into an ASCII representation.
//
//  IN OUT CHAR8 *string
// Pointer to a CHAR8 string buffer which will contain the result of the
// conversion.  User is responsible for allocating the necessary memory
// resources.
//
//  IN INT32 radix
// Radix of the conversion.  For example, 10 for decimal, 16 for hexadecimal,
// etc.
//
//  IN BOOLEAN is_int64
// TRUE if value is a 64-bit integer; FALSE if it's 32-bit.
//
// Output:
//  CHAR8* string pointer, string, that contains the ASCII representation of
// value in the user requested radix.
//
// Modified:
//
// Referrals:
//  MT_ItoaHelper
// 
// Notes:	
// 
//<AMI_PHDR_END>
//*************************************************************************
char * MT_ItoaEx(INT64 value, char *string,int radix, BOOLEAN is_int64) {
	char *strp=string;
	char *str2p=MT_ItoaHelper(value,strp,radix,is_int64);
	//reverse string
	while(strp<str2p) {
		char temp=*strp;
		*strp=*str2p;
		*str2p=temp;
		strp++;str2p--;
	}
	return string;
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: MT_Sprintf_s
//
// Description:
//  UINTN MT_Sprintf_s(OUT CHAR8 *Buffer, IN UINTN BufferSize, IN CHAR8 *Format,
// IN ...) produces a null-terminated ASCII string in the output Buffer of
// size BufferSize.  The ASCII string is produced by parsing the format
// string specified by Format.  Arguments are pulled from the variable
// argument list based on the contents of the format string.  The number of
// ASCII characters in the produced output buffer is returned, not including
// the null-terminator.
//
// Input:
//  OUT CHAR8 *Buffer
// Pointer to a null-terminated output ASCII string buffer.  User is
// responsible for allocating the necessary memory resources!
//
//  IN UINTN BufferSize
// The size, in bytes, of the output Buffer.
//
//  IN CHAR8 *Format
// Pointer to a null-terminated format ASCII string.
//
//  IN ...
// Variable argument list of data/variables used within the format string.
//
// Output:
//  UINTN number of ASCII characters in the produced output buffer, not
// including the null-terminator.
//
// Modified:
//
// Referrals:
//  MT_va_start
//  MT_Sprintf_s_va_list
//  MT_va_end
// 
// Notes:	
//  Refer to Sprintf function for format string syntax.
// 
//<AMI_PHDR_END>
//*************************************************************************
UINTN MT_Sprintf_s( char *Buffer, UINTN BufferSize, char *Format, ... )
{
	MT_va_list	ArgList = MT_va_start(ArgList, Format);
	UINTN Ret = MT_Sprintf_s_va_list( Buffer, BufferSize, Format, ArgList );
	MT_va_end(ArgList);
	return Ret;
}
#if (MyTrace_PEI_SUPPORT == 0)
//*************************************************************************
//<AMI_PHDR_START>
//
// Name: MT_Div64
//
// Description:
//  UINT64 MT_Div64(IN UINT64 Dividend, IN UINTN Divisor, 
// OUT UINTN *Remainder OPTIONAL) divides a 64-bit number, Dividend, by the
// Divisor, which can be up to 31-bits.
//
// Input:
//  IN UINT64 Dividend
// The 64-bit number to be divided.
//
//  IN UINT Divisor
// The number to divide Dividend by; may not exceed 31-bits in size.
//
//  OUT UINTN *Remainder OPTIONAL
// The remainder of the division.  Provide NULL if undesired; otherwise user
// is responsible for handling the necessary memory resources.
//
// Output:
//  UINT64 result of dividing Dividend by Divisor.
//
// Modified:
//
// Referrals:
// 
// Notes:	
// 
//<AMI_PHDR_END>
//*************************************************************************
UINT64 MT_Div64 (
	IN UINT64	Dividend,
	IN UINTN	Divisor,	//Can only be 31 bits.
  	OUT UINTN	*Remainder OPTIONAL
  	)
{
	UINT64 	Result = Dividend/Divisor;
	if (Remainder) *Remainder=(UINTN)Dividend%Divisor;
  	return Result;
}
#endif
//*************************************************************************
//
// Name: MT_ItoaHelper
//
// Description:
//  CHAR8* MT_ItoaHelper(IN INT64 value, IN OUT CHAR8 *string, IN INT32 radix,
// IN BOOLEAN is_int64) is a helper function for MT_ItoaEx and ItowEx which 
// converts an INT64 value into a CHAR8 ASCII representation in reverse
// order.
//
// Input:
//  IN INT64 value
// Value to be converted.
//
//  IN OUT CHAR8 *string
// Pointer to a CHAR8 string buffer that will hold the result of the reverse
// conversion.  User is responsible for allocating the necessary memory
// resources.
//
//  IN INT32 radix
// Radix of the conversion.
//
//  IN BOOLEAN is_int64
// TRUE if value is a 64-bit integer; FALSE if it's 32-bit.
//
// Output:
//  CHAR8* string pointer to the last character added to the input string, 
// which now contains an ASCII string that is the reverse of what value 
// represents.
//
// Modified:
//
// Referrals:
//  MT_Div64
// 
// Notes:	
// 
//*************************************************************************
char * MT_ItoaHelper(INT64 value, char *string,int radix, BOOLEAN is_int64) {
	UINTN digit;
	UINT64 v = (value>=0) 
			   ? (UINT64) value
			   : (radix==10) 
				 ? (UINT64)-value 
				 : (is_int64) 
				   ? (UINT64)value 
				   : (UINT32)value;
	if (v)
		while (v) {
			#if MyTrace_PEI_SUPPORT
			v = Div64(v,radix,&digit);
			#else
			v = MT_Div64(v,radix,&digit);
			#endif
			if (digit<0xa) *string=(char)(digit+'0');
			else *string=(char)(digit-0xa+'a');
			string++;
		}
	else *string++='0';
	if (radix==10 && value<0) *string++='-';
	*string--=0;
	return string;
}
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2011, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//

#ifndef _GALILEOFM_MISC
#define _GALILEOFM_MISC

#include <galileofm/64bit.h>

/*****************************************************************************

 Miscellaneous functions

 *****************************************************************************/

// Hex ascii to long
long Atoh(char *,long);

// Long to ascii (various methods)
void Itoa(long,char *,char);
void ItoaU(unsigned long,char *,char);

// 64bit unsigned to ascii
QtoaU(UQUAD *,STRPTR,char);

// Convert long value to byte string
void BytesToString(unsigned long,char *,short,char);

// Convert unsigned 64bit value to byte string
void BytesToString64(UQUAD *,char *,short,char);

// Disk geometry (blocks*blocksize) to byte string
void GeometryToString(ULONG,ULONG,char,short,char);

// Division to a string
void DivideToString(char *,unsigned long,unsigned long,short,char);

// 32-bit divide with remainder
ULONG DivideU(unsigned long,unsigned long,unsigned long *,struct Library *);

// Convert BSTR to char *
void BtoCStr(BSTR,char *,long);

// Random numbers
void Seed(long);
long Random(long);

// Build ascii string from rawkey values
void BuildKeyString(unsigned short,unsigned short,unsigned short,unsigned short,char *);

// Valid key qualifiers
UWORD QualValid(unsigned short);

// Convert rawkey values to a character
BOOL ConvertRawKey(unsigned short,unsigned short,char *);

// String manipulation
BOOL StrCombine(char *,char *,char *,long);
BOOL StrConcat(char *,char *,long);

// Calculate a percentage
long CalcPercent(ULONG,ULONG,struct Library *);

#endif

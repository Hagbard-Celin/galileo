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
STRPTR JoinString(APTR memory, STRPTR first, STRPTR second, STRPTR third, ULONG flags);
STRPTR CopyString(APTR memory, STRPTR original);

#define JSF_FS_SLASH	(1<<0)
#define JSF_ST_SLASH	(1<<1)
#define JSF_E_SLASH	(1<<2)
#define JSF_FS_ADDPART	(1<<3)
#define JSF_ST_ADDPART	(1<<4)
#define JSF_E_ADDPART	(1<<5)

// Calculate a percentage
long CalcPercent(ULONG,ULONG,struct Library *);

#endif

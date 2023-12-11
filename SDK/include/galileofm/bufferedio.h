#ifndef _GALILEOFM_BUFIO
#define _GALILEOFM_BUFIO

/*****************************************************************************

 Buffered IO

 *****************************************************************************/

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef DOS_DOS_H
#include <dos/dos.h>
#endif

long CloseBuf(APTR file);
long ExamineBuf(APTR file,struct FileInfoBlock *fib);
BPTR FHFromBuf(APTR file);
long FlushBuf(APTR file);
APTR OpenBuf(char *name,long mode,long buffer_size);
long ReadBuf(APTR file,char *data,long size);
long ReadBufLine(APTR file,char *data,long size);
long SeekBuf(APTR file,long offset,long mode);
long WriteBuf(APTR file,char *data,long size);

#endif

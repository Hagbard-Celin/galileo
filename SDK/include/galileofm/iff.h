#ifndef _GALILEOFM_IFF
#define _GALILEOFM_IFF

/*****************************************************************************

 IFF routines

 *****************************************************************************/

// File modes
#define IFF_READ	MODE_OLDFILE		// Reading
#define IFF_WRITE	MODE_NEWFILE		// Writing
#define IFF_CLIP	0x8000			// Clipboard flag
#define IFF_CLIP_READ	(IFF_CLIP|IFF_READ)	// Read clipboard
#define IFF_CLIP_WRITE	(IFF_CLIP|IFF_WRITE)	// Write clipboard
#define IFF_SAFE	0x4000			// Safe write


ULONG IFFChunkID(APTR);
long IFFChunkRemain(APTR);
long IFFChunkSize(APTR);
void IFFClose(APTR);
void IFFFailure(APTR);
ULONG IFFGetFORM(APTR);
ULONG IFFNextChunk(APTR,ULONG);
APTR IFFOpen(char *,unsigned short,ULONG);
long IFFPopChunk(APTR);
long IFFPushChunk(APTR,ULONG);
long IFFReadChunkBytes(APTR,APTR,long);
long IFFWriteChunkBytes(APTR,APTR,long);
long IFFWriteChunk(APTR,APTR,ULONG,ULONG);

#endif

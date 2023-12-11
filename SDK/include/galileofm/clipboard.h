#ifndef _GALILEOFM_CLIP
#define _GALILEOFM_CLIP

/*****************************************************************************

 Clipboard

 *****************************************************************************/

void CloseClipBoard(APTR);
APTR OpenClipBoard(ULONG);
long ReadClipString(APTR,char *,long);
BOOL WriteClipString(APTR,char *,long);

#endif

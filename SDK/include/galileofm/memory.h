#ifndef _GALILEOFM_MEMORY
#define _GALILEOFM_MEMORY

/*****************************************************************************

 Memory functions

 *****************************************************************************/

void *AllocMemH(void *handle,ULONG size);
void ClearMemHandle(void *handle);
void FreeMemH(void *memory);
void FreeMemHandle(void *handle);
void *NewMemHandle(ULONG puddle_size,ULONG thresh_size,ULONG type);

#endif

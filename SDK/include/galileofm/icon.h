#ifndef _GALILEOFM_ICON
#define _GALILEOFM_ICON

/*****************************************************************************

 Icon support

 *****************************************************************************/


// Copy icon from one file to another
void CopyFileIcon(char *,char *);


// Special icon flags
#define ICONF_POSITION_OK	(1<<31)		// Galileo position stored in icon
#define ICONF_NO_BORDER		(1<<28)		// No icon border
#define ICONF_NO_LABEL		(1<<27)		// No label

ULONG GetIconFlags(struct DiskObject *);
void SetIconFlags(struct DiskObject *,ULONG);
void GetIconPosition(struct DiskObject *,short *,short *);
void SetIconPosition(struct DiskObject *,short,short);


// Icon caching
struct DiskObject *GetCachedDefDiskObject(long);
void FreeCachedDiskObject(struct DiskObject *);
struct DiskObject *GetCachedDiskObject(char *,long);
struct DiskObject *GetCachedDiskObjectNew(char *,long);

#endif

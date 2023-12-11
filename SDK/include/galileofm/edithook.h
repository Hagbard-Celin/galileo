#ifndef _GALILEOFM_EDITHOOK
#define _GALILEOFM_EDITHOOK

/*****************************************************************************

 String edit-hook

 *****************************************************************************/

#define EDITF_NO_SELECT_NEXT	(1<<0)		// Don't select next field
#define EDITF_PATH_FILTER	(1<<1)		// Filter path characters
#define EDITF_SECURE		(1<<2)		// Hidden password field

#define HOOKTYPE_STANDARD	0

#define EH_History		TAG_USER + 33	// History list pointer
#define EH_ChangeSigTask	TAG_USER + 46	// Task to signal on change
#define EH_ChangeSigBit		TAG_USER + 47	// Signal bit to use

void FreeEditHook(struct Hook *);
struct Hook *GetEditHook(ULONG,ULONG,struct TagItem *tags);
struct Hook *__stdargs GetEditHookTags(ULONG,ULONG,Tag,...);
char *GetSecureString(struct Gadget *);

#endif

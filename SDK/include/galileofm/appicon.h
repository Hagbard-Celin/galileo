#ifndef _GALILEOFM_APPICON
#define _GALILEOFM_APPICON

/*****************************************************************************

 AppIcon, etc

 *****************************************************************************/

#ifndef WORKBENCH_WORKBENCH_H
#include <workbench/workbench.h>
#endif

#define GAE_Local		TAG_USER + 0	// Add to Galileo only, not WB
#define GAE_SnapShot		TAG_USER + 1	// Supports snapshot
#define GAE_Menu		TAG_USER + 2	// Menu item
#define GAE_Close		TAG_USER + 3	// Close item
#define GAE_Background		TAG_USER + 4	// Background colour
#define GAE_ToggleMenu		TAG_USER + 5	// Toggle item
#define GAE_ToggleMenuSel	TAG_USER + 6	// Toggle item (selected)
#define GAE_Info		TAG_USER + 7	// Supports Information
#define GAE_Locked		TAG_USER + 8	// Position locked
#define GAE_MenuBase		TAG_USER + 9	// Menu ID base


APTR FindAppWindow(struct Window *);
struct MsgPort *AppWindowData(APTR,ULONG *,ULONG *);
struct Window *AppWindowWindow(APTR);

// Messages sent from AppIcons

#define MTYPE_APPSNAPSHOT	0x3812		// Snapshot message

struct AppSnapshotMsg
{
	struct AppMessage	ap_msg;		// Message
	long			position_x;	// Icon x-position
	long			position_y;	// Icon y-position
	struct IBox		window_pos;	// Window position
	unsigned long		flags;		// Flags
	long			id;		// ID
};

#define APPSNAPF_UNSNAPSHOT	(1<<0)		// Set "no position"
#define APPSNAPF_WINDOW_POS	(1<<1)		// Window position supplied
#define APPSNAPF_MENU		(1<<2)		// Menu operation
#define APPSNAPF_CLOSE		(1<<3)		// Close command
#define APPSNAPF_HELP		(1<<4)		// Help on a command
#define APPSNAPF_INFO		(1<<5)		// Information command


// Change AppIcons

void ChangeAppIcon(APTR,struct Image *,struct Image *,char *,ULONG);
long SetAppIconMenuState(APTR,long,long);

#define CAIF_RENDER	(1<<0)
#define CAIF_SELECT	(1<<1)
#define CAIF_TITLE	(1<<2)
#define CAIF_LOCKED	(1<<3)
#define CAIF_SET	(1<<4)
#define CAIF_BUSY	(1<<5)
#define CAIF_UNBUSY	(1<<6)


// AppWindow messages

typedef struct _GalileoAppMessage
{
	struct AppMessage	da_Msg;		// Message
	Point			*da_DropPos;	// Drop array
	Point			da_DragOffset;	// Mouse pointer offset
	ULONG			da_Flags;	// Flags
	ULONG			da_Pad[2];
} GalileoAppMessage;

#define GAPPF_ICON_DROP		(1<<16)		// Dropped with icon

GalileoAppMessage *AllocAppMessage(APTR,struct MsgPort *,short);
BOOL CheckAppMessage(GalileoAppMessage *);
void FreeAppMessage(GalileoAppMessage *);
BOOL GetWBArgPath(struct WBArg *,char *,long);
void ReplyAppMessage(GalileoAppMessage *);
BOOL SetWBArg(GalileoAppMessage *,short,BPTR,char *,APTR);

#endif

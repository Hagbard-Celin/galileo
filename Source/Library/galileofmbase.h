/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 1993-2012 Jonathan Potter & GP Software
Copyright 2012 Roman Kargin <kas1e@yandex.ru>
Copyright 2024,2025 Hagbard Celine

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

This program is based on the source code of Directory Opus Magellan II, 
released by GPSoftware under the APL license in 2012. Re-licensed under GPL by 
permission of Dr Greg Perry, Managing Director of GPSoftware.

Opus® and Directory Opus® and associated images are registered trademarks of GPSoftware.
DOpus(TM), DirOpus(TM) and Opus File Manager(TM) are trademarked 1991 by GPSoftware.
Opus Magellan (TM) is trademarked 1996 GPSoftware.
All other trademarks are the property of their respective owners.

The release of Directory Opus 5 under the GPL in NO WAY affects
the existing commercial status of Directory Opus for Windows.

For more information on Directory Opus for Windows please see:

		 http://www.gpsoft.com.au

*/

#ifndef _GALILEOFMBASE
#define _GALILEOFMBASE


#include <sys/commargs.h>

#include <exec/types.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <graphics/rastport.h>
#include <graphics/gels.h>
#include <intuition/intuition.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <libraries/commodities.h>

#include "/Program/galileo_config.h"
#ifdef _DEBUG_STACK
#include <gfm/stack_check_ipc.h>
#endif
#ifndef _GALILEOFMBASE_LOCALE_H
#include <gfm/galileolocale.h>
#endif
#ifndef _GALILEOFMBASE_IPC_H
#include <gfm/galileo_ipc.h>
#endif
#ifndef _GALILEOFM_LISTLOCK_H
#include <gfm/listlock.h>
#endif
#ifndef _GALILEOFM_LIST_MANAGEMENT_H
#include <gfm/list_management.h>
#endif
#ifndef _GALILEOFM_CONFIGWIN_H
#include <gfm/configwin.h>
#endif
#ifndef _GALILEOFM_FUNCARGS_H
#include "funcargs.h"
#endif
#ifndef _GALILEOFM_GADTOOLS_H
#include <gfm/gadtools.h>
#endif
#ifndef _GALILEOFM_REQUESTER_H
#include <gfm/requester.h>
#endif
#ifndef _GALILEOFM_GALILEONOTIFY_H
#include "galileonotify.h"
#endif
#ifndef _GALILEOFM_PATHS_PROTO_H
#include "paths_proto.h"
#endif
#ifndef _GALILEOFM_TIMER_H
#include <gfm/timer.h>
#endif
#ifndef GALILEOFM_POPUPMENU_H
#include <gfm/popupmenu.h>
#endif
#ifndef _GALILEOFM_IMAGEREMAP_H
#include <gfm/imageremap.h>
#endif
#ifndef GALILEOFM_BUFFERED_IO_H
#include <gfm/buffered_io.h>
#endif
#ifndef GALILEOFM_IFF_H
#include <gfm/iff.h>
#endif
#ifndef GALILEOFM_CONFIGURATION_H
#include <gfm/configuration.h>
#endif
#ifndef GALILEOFM_LAYOUT_H
#include <gfm/layout.h>
#endif
#ifndef _GALILEOFM_DRAG_ROUTINES_H
#include <gfm/drag_routines.h>
#endif
#ifndef GALILEOFM_WINDOWDATA_H
#include <gfm/windowdata.h>
#endif
#ifndef GALILEOFM_LIST_H
#include "list.h"
#endif
#ifndef GALILEOFM_COLORSPEC_H
#include <gfm/colorspec.h>
#endif
#ifndef GALILEOFM_POPUPITEM_H
#include <gfm/popupitem.h>
#endif
#ifndef GALILEOFM_POPUPMENU_H
#include <gfm/popupmenu.h>
#endif
#ifndef GALILEOFM_MEMORY_H
#include <gfm/memory.h>
#endif

// Layout routines
#define FPOS_TEXT_OFFSET	16384

// Set by the system
#define OPEN_USED_DEFAULT	(1<<16)	// To open had to use default font
#define OPEN_USED_TOPAZ		(1<<17)	// To open had to use topaz
#define OPEN_SHRUNK_VERT	(1<<18)	// Window is not full vertical size requested
#define OPEN_SHRUNK_HORIZ	(1<<19)	// Window is not full horizontal size requested
#define OPEN_SHRUNK		(OPEN_SHRUNK_VERT|OPEN_SHRUNK_HORIZ)


#define GAD_ID_ICONIFY		0xffa0

#define GADFLAG_GLASS			(1<<16)		// Magnifying-glass gadget

#define OBJECTFLAG_DRAWN		(1<<31)		// Object has been drawn

#define OBJECTF_NO_SELECT_NEXT	(1<<0)	    // Don't select next field
#define OBJECTF_PATH_FILTER	(1<<1)	    // Filter path characters
#define OBJECTF_SECURE		(1<<2)	    // Hide string
#define OBJECTF_INTEGER		(1<<3)	    // Integer gadget
#define OBJECTF_READ_ONLY	(1<<4)	    // Read-only
#define OBJECTF_HOTKEY		(1<<5)	    // Hotkey string
#define OBJECTF_UPPERCASE	(1<<6)	    // Uppercase string

#define IS_GADTOOLS(obj)		(BOOL)(obj->gl_info.gl_gadget.context)

#define WINDOW_MAGIC		0x83224947
#define WINDOW_UNKNOWN		(ULONG)-1
#define WINDOW_UNDEFINED	0

#define FILE_GLASS_KIND	1000
#define DIR_GLASS_KIND	1001

#define GM_RESIZE	(20)

struct gpResize
{
    ULONG		    MethodID;
    struct GadgetInfo	    *gpr_GInfo;
    struct RastPort	    *gpr_RPort;
    struct IBox		    gpr_Size;
    long		    gpr_Redraw;
    struct Window	    *gpr_Window;
    struct Requester	    *gpr_Requester;
};

#define LAYOUTF_SAME_HEIGHT	(1<<0)
#define LAYOUTF_SAME_WIDTH	(1<<1)
#define LAYOUTF_TOP_ALIGN	(1<<2)
#define LAYOUTF_BOTTOM_ALIGN	(1<<3)
#define LAYOUTF_LEFT_ALIGN	(1<<4)
#define LAYOUTF_RIGHT_ALIGN	(1<<5)

#define GIA_Type		TAG_USER + 5	// Image type
#define GIA_FrontPen		TAG_USER + 7	// Image front pen
#define GIA_ThinBorders		TAG_USER + 27	// Thin borders

#define IM_ARROW_UP	0
#define IM_ARROW_DOWN	1
#define IM_CHECK	2
#define IM_DRAWER	3
#define IM_BORDER_BOX	4
#define IM_BBOX		5
#define IM_ICONIFY	6
#define IM_CROSS	7
#define IM_LOCK		8
#define IM_GAUGE	9	// Not really an image

typedef struct _CompoundObjectList
{
    struct MinNode	    node;
    struct MinList          objects;
    UWORD	            id;
} CompoundObjectList;

typedef struct _CompoundObject
{
    struct MinNode	node;
    struct Rectangle	coords;
    struct Gadget	*object;
} CompoundObject;

// Pathgadget editbuffer flags
#define PEBPF_LAST_PART 	(1<<7)

// Pathgadget editbuffer length
#define PWB_PART_LEN 119
//#define PWB_PART_LEN 4
//#define PWB_PART_LEN 24

//Pathgadget editbuffer
struct PathEditBufferPart
{
    struct MinNode  pebp_Node;
    UBYTE       pebp_Length;
    TEXT        pebp_Buf[PWB_PART_LEN];
};

struct PathEditBuffer
{
    struct MinList	    peb_List;
    ULONG		    peb_Length;
    struct PathEditBufferPart	    *peb_DisplayPart;
    ULONG		    peb_DPOffset;
    struct PathEditBufferPart	    *peb_CursorPart;
    ULONG		    peb_CursorPos;
    ULONG		    peb_CPOffset;
    struct PathEditBufferPart	    peb_FirstPart;
};

// Some useful macros
#define WINFLAG(win) (DATA(win)->flags)
#define WINMEMORY(win) (DATA(win)->memory)
#define VISINFO(win) (DATA(win)->visinfo)
#define GADSPECIAL(list,id) (GADGET(L_GetObject(list,id))->SpecialInfo)
#define GADSTRING(list,id) ((struct StringInfo *)GADSPECIAL(list,id))->Buffer
#define GADNUMBER(list,id) ((struct StringInfo *)GADSPECIAL(list,id))->LongInt
#define GADSEL(list,id) (GADGET(L_GetObject(list,id))->Flags&GFLG_SELECTED)
#define GADGET_SPECIAL(list,id) (GADGET(GetObject(list,id))->SpecialInfo)
#define GADGET_STRING(list,id) ((struct StringInfo *)GADGET_SPECIAL(list,id))->Buffer
#define GADGET_NUMBER(list,id) ((struct StringInfo *)GADGET_SPECIAL(list,id))->LongInt
#define GADGET_SEL(list,id) (GADGET(GetObject(list,id))->Flags&GFLG_SELECTED)


// Window dimensions
typedef struct
{
    struct IBox	    wd_Normal;
    struct IBox	    wd_Zoomed;
    unsigned short  wd_Flags;
} WindowDimensions;

#define WDF_VALID	(1<<0)
#define WDF_ZOOMED	(1<<1)

void InitWindowDims(struct Window *,WindowDimensions *);
void StoreWindowDims(struct Window *,WindowDimensions *);
BOOL CheckWindowDims(struct Window *,WindowDimensions *);


#define ATTNODE_DISKCHANGENODE	175
#define ATTNODE_LOCKNODE	176
#define ATTNODE_TYPE_MASK	240

#define ISATTDISKCHANGENODE(type) ((type&ATTNODE_TYPE_MASK) == ATTNODE_DISKCHANGENODE)
#define ISATTLOCKNODE(type)     ((type&ATTNODE_TYPE_MASK) == ATTNODE_LOCKNODE)

char *Att_NodeName(Att_List *list,long number);
void LockAttList(Att_List *list,short exclusive);
void UnlockAttList(Att_List *list);

// Timer
BOOL TimerActive(TimerHandle *);

typedef struct
{
    char		    devname[40];
    char		    volname[32];
    struct DateStamp	    date;
    UBYTE		    volnamelen;
} diskchange_data;

// Notification message
void SendNotifyMsg(ULONG,ULONG,ULONG,short,char *,struct FileInfoBlock *);
void SetNotifyRequest(APTR,ULONG,ULONG);


// Requesters

long __stdargs AsyncRequestTags(IPCData *,long,struct Window *,REF_CALLBACK(),APTR,Tag,...);
struct IntuiMessage *CheckRefreshMsg(struct Window *,ULONG);



// Pop-up menu stuff

#define POPUP_BARLABEL		(char *)-1

#define POPUPMF_HELP		(1<<0)		// Supports help
#define POPUPMF_REFRESH		(1<<1)		// Use refresh callback
#define POPUPMF_ABOVE		(1<<2)		// Open above parent window
#define POPUPMF_STICKY		(1<<3)		// Go sticky immediately
#define POPUPMF_FONT		(1<<4)		// Use font
#define POPUPMF_NO_SCALE	(1<<5)		// No image scaling
#define POPUPMF_BACKFILL	(1<<6)		// Backfill hook is valid
#define POPUPMF_USE_PEN		(1<<7)		// Use pen in flags
#define POPUPMF_PEN(x)		((x)<<24)

#define POPUP_HELPFLAG		(1<<15)

PopUpItem *GetPopUpItem(PopUpMenu *,UWORD);
void GetPopUpImageSize(struct Window *,PopUpMenu *,short *,short *);
void SetPopUpDelay(short);

typedef struct
{
	struct MinNode	ss_Node;
	struct List	*ss_List;
} SubStack;


// Disk IO stuff
typedef struct
{
    struct MsgPort		    *dh_port;	    // Message port
    struct IOExtTD		    *dh_io;	    // IO request
    struct FileSysStartupMsg	    *dh_startup;    // Startup message
    struct DosEnvec		    *dh_geo;	    // Disk geometry
    char			    dh_name[32];    // Disk name
    char			    dh_device[32];  // Device name
    struct InfoData		    dh_info;	    // Disk information
    unsigned long		    dh_result;
    unsigned long		    dh_root;	    // Root block
    unsigned long		    dh_blocksize;   // Block size
    struct DateStamp		    dh_stamp;	    // DateStamp
} DiskHandle;

DiskHandle *OpenDisk(char *,struct MsgPort *);
void CloseDisk(DiskHandle *);


// String edit hook

struct Hook *GetEditHook(ULONG,ULONG,struct TagItem *tags);
struct Hook *__stdargs GetEditHookTags(ULONG,ULONG,Tag,...);
void FreeEditHook(struct Hook *);
char *GetSecureString(struct Gadget *);

#define EDITF_NO_SELECT_NEXT	(1<<0)		// Don't select next field
#define EDITF_PATH_FILTER	(1<<1)		// Filter path characters
#define EDITF_PASSWORD		(1<<2)		// Hidden password field


// Screen Info

#define SCRI_LORES	(1<<0)

ULONG ScreenInfo(struct Screen *);


// Images

#define IRF_REMAP_COL0		(1<<0)
#define IRF_PRECISION_EXACT	(1<<1)
#define IRF_PRECISION_ICON	(1<<2)
#define IRF_PRECISION_GUI	(1<<3)

void FlushImages(void);
short RenderImageTags(struct RastPort *,APTR,UWORD,UWORD,Tag,...);
ULONG *GetImagePalette(APTR image);

// Unsigned 64bit
typedef struct {
	ULONG hi;
	ULONG lo;
} UQUAD;

struct DosPacket64
{
    struct Message  *dp_Link;   /* EXEC message                    */
    struct MsgPort  *dp_Port;   /* Reply port for the packet,      */
				/* must be filled in on each send. */
    LONG	    dp_Type;    /* See ACTION_... below            */
    LONG	    dp_Res0;    /* Special compatibility field. [See below] */

    LONG	    dp_Res2;    /* This is returned for IoErr()    */
    ULONG	    Pad1;
    UQUAD	    dp_Res1;    /* This is the 64 bit primary result */
    LONG	    dp_Arg1;    /* 32 bit argument */
    ULONG	    Pad2;
    UQUAD	    dp_Arg2;    /* 64 bit argument */
    LONG	    dp_Arg3;    /* 32 bit argument */
    LONG	    dp_Arg4;    /* 32 bit argument */
    UQUAD	    dp_Arg5;    /* 64 bit argument */
};

#define ACTION_GET_FILE_SIZE64	8004

/* The DosPacket64 dp_Res0 member initialisation value. */
#define DP64_INIT	-3L

struct StandardPacket64
{
    struct Message      sp_Msg;
    struct DosPacket64  sp_Pkt;
};

/* Function prototypes */

ULONG Atoh(char *,short);
Itoa(long,char *,char);
ItoaU(unsigned long,char *,char);
QtoaU(UQUAD * number, STRPTR string, char sep);
void GeometryToString(ULONG blocks, ULONG blocksize, char *string, short places, char sep);
BytesToString(unsigned long,char *,short,char);
void BytesToString64( UQUAD *bytes, char *string, short places, char sep);
ULONG LsrQtoUL(UQUAD number, BYTE steps);
DivideToString(char *,unsigned long,unsigned long,short,char);

ULONG GetFileSize64(BPTR lock, UQUAD * size);

void SetCxSelectUpDown(CxObj * cxo);
CxObj *GetCxSelectUpDown(void);


void Seed(int);
Random(int);
ConvertRawKey(unsigned short,unsigned short,char *);
void SetBusyPointer(struct Window *);

// strings
StrCombine(char *,char *,char *,int);
BOOL GetLastPathComponent(STRPTR dest, STRPTR path);

BPTR GetDosPathList(BPTR);
void CopyLocalEnv(void);
void UpdatePathList(void);
void UpdateMyPaths(void);

// Layout functions
void FreeObject(ObjectList *,GL_Object *);
void UpdateGadgetList(ObjectList *);
void StripIntuiMessages(struct Window *);
void StripWindowMessages(struct MsgPort *,struct IntuiMessage *);
void CloseWindowSafely(struct Window *);
void FreeWindowMenus(struct Window *);
struct Gadget *FindKeyEquivalent(ObjectList *,struct IntuiMessage *,int);
void ShowProgressBar(struct Window *,GL_Object *,ULONG,ULONG);
void SetObjectKind(ObjectList *,ULONG,UWORD);
void RefreshObjectList(struct Window *,ObjectList *);
struct Menu *BuildMenuStrip(MenuData *,struct GalileoLocale *);

void LayoutResize(struct Window *);

void StartRefreshConfigWindow(struct Window *,long);
void EndRefreshConfigWindow(struct Window *);

// configuration
short ReadSettings(CFG_SETS *,char *);
Cfg_Lister *ReadListerDef(APTR,ULONG);

void DefaultSettings(CFG_SETS *);
Cfg_ButtonBank *DefaultButtonBank(void);

void FreeButtonImages(struct List *list);
void FreeFiletypeList(Cfg_FiletypeList *list);


// IPC
BOOL IPC_Startup(IPCData *,APTR,struct MsgPort *);
ULONG IPC_SafeCommand(IPCData *,ULONG,ULONG,APTR,APTR,struct MsgPort *,struct ListLock *);
IPCData *IPC_FindProc(struct ListLock *,char *,BOOL,ULONG);
void IPC_Hello(IPCData *,IPCData *);
void IPC_QuitName(struct ListLock *,char *,ULONG);



// gui
void DrawFieldBox(struct RastPort *,struct Rectangle *,struct DrawInfo *);
char *GetIconFullname(char *name);
BOOL WriteIcon(char *,struct DiskObject *);
BOOL DeleteIcon(char *);

// status
void SetStatusText(struct Window *,char *);
void UpdateStatusGraph(struct Window *,char *,ULONG,ULONG);

// clipboard
struct ClipHandle *OpenClipBoard(ULONG);
void CloseClipBoard(struct ClipHandle *);
BOOL WriteClipString(struct ClipHandle *,char *,long);
long ReadClipString(struct ClipHandle *,char *,long);

// palette
void LoadPalette32(struct ViewPort *,unsigned long *);
void GetPalette32(struct ViewPort *,unsigned long *,unsigned short,short);

// boopsi
struct Gadget *AddScrollBars(struct Window *,struct List *,struct DrawInfo *,short);
struct Gadget *FindBOOPSIGadget(struct List *,UWORD);
void BOOPSIFree(struct List *);
struct Gadget *CreateTitleGadget(struct Screen *,struct List *,BOOL,short,short,unsigned short);
struct Gadget *FindGadgetType(struct Gadget *,UWORD);
void FixTitleGadgets(struct Window *);

#define SCROLL_NOIDCMP	(1<<0)
#define SCROLL_VERT	(1<<1)
#define SCROLL_HORIZ	(1<<2)



ULONG DivideU(unsigned long,unsigned long,unsigned long *,struct Library *);

void ReplyFreeMsg(APTR);

void WriteFileIcon(char *,char *);

struct PubScreenNode *FindPubScreen(struct Screen *,BOOL);


#define LAUNCH_REPLY			-1
#define LAUNCH_NOWAIT			0
#define LAUNCH_WAIT			1
#define LAUNCH_WAIT_TIMEOUT		2

struct LaunchWBArg
{
    struct WBArg    *lwba_args;
    LONG            lwba_numargs;
    long            lwba_stack;
    char            *lwba_default_tool;
    char            *lwba_toolwindow;
};

void FreeLaunchWBArgs(struct WBArg **args, LONG numargs);
BOOL WB_LaunchArg(struct LaunchWBArg *, struct Screen *, short, struct Process **, IPCData *, ULONG);

BOOL WB_Launch(char *,struct Screen *,short,long,char *,struct Process **,IPCData *,ULONG);
void MUFSLogin(struct Window *,char *,char *);

#define LAUNCHF_USE_STACK		(1<<14)

#define LAUNCHF_OPEN_UNDER_MOUSE	(1<<30)

// anim
void AnimDecodeRIFFXor(unsigned char *,char *,unsigned short,unsigned short);
void AnimDecodeRIFFSet(unsigned char *,char *,unsigned short,unsigned short);

// paths
UBYTE VolIdFromLock(BPTR lock, struct DateStamp *date, char *name);
BPTR LockFromVolIdPath(STRPTR volumename, CONST_STRPTR path, struct DateStamp *volumedate, UBYTE namelen, ULONG flags);
BPTR LockFromPathQuick(CONST_STRPTR,ULONG);

#define LFPF_TRY_ICON		(1<<0)
#define LFPF_TRY_ICON_FIRST	(1<<1)




enum
{
    GAD_xxx=1,

    GAD_VERT_SCROLLER,
    GAD_VERT_ARROW_UP,
    GAD_VERT_ARROW_DOWN,

    GAD_HORIZ_SCROLLER,
    GAD_HORIZ_ARROW_LEFT,
    GAD_HORIZ_ARROW_RIGHT,
};


// Workbench patch stuff
enum
{
    APP_ICON,
    APP_WINDOW,
    APP_MENU
};

typedef struct
{
    struct MinNode	ae_node;
    ULONG		ae_type;	   // Type of entry

    ULONG		ae_id;		   // ID
    ULONG		ae_userdata;	   // User data

    APTR		ae_object;	   // Type-specific object
    char		*ae_text;	   // If text is needed
    struct MsgPort	*ae_port;	   // Message port

    APTR		ae_os_object;	   // Object from OS routine

    unsigned short	ae_flags;	   // Some flags

    struct MinList	ae_menu;	   // Menu items

    unsigned short	ae_data;
    long		ae_menu_id_base;   // Base ID for menu
}  AppEntry;

#define GAE_Local		TAG_USER + 0		// Local entry
#define GAE_SnapShot		TAG_USER + 1		// Supports snapshot
#define GAE_Menu		TAG_USER + 2		// Menu item
#define GAE_Close		TAG_USER + 3		// Close item
#define GAE_Background		TAG_USER + 4		// Background colour
#define GAE_ToggleMenu		TAG_USER + 5		// Toggle item
#define GAE_ToggleMenuSel	TAG_USER + 6		// Toggle item (selected)
#define GAE_Info		TAG_USER + 7		// Supports Information
#define GAE_Locked		TAG_USER + 8		// Position locked
#define GAE_MenuBase		TAG_USER + 9		// Menu ID base
#define GAE_Special		TAG_USER + 10		// Special

#define ln_MenuFlags		ln_Type

#define MNF_TOGGLE		(1<<0)
#define MNF_SEL			(1<<1)

#define APPENTF_LOCAL		(1<<0)
#define APPENTF_SNAPSHOT	(1<<1)
#define APPENTF_ICON_COPY	(1<<2)
#define APPENTF_CLOSE		(1<<3)
#define APPENTF_NO_OPEN		(1<<4)
#define APPENTF_BACKGROUND	(1<<5)
#define APPENTF_INFO		(1<<6)
#define APPENTF_LOCKED		(1<<7)
#define APPENTF_BUSY		(1<<8)
#define APPENTF_SPECIAL		(1<<9)
#define APPENTF_GHOSTED		(1<<10)

void WB_Install_Patch(void);
BOOL WB_Remove_Patch(void);
struct AppWindow *WB_AddAppWindow(ULONG,ULONG,struct Window *,struct MsgPort *,struct TagItem *);
struct AppWindow *WB_AddAppWindowTr(ULONG,ULONG,struct Window *,struct MsgPort *,struct TagItem *);
BOOL WB_RemoveAppWindow(struct AppWindow *);
BOOL WB_RemoveAppWindowTr(struct AppWindow *);
struct AppWindow *WB_FindAppWindow(struct Window *);
struct MsgPort *WB_AppWindowData(struct AppWindow *,ULONG *,ULONG *);
BOOL WB_AppWindowLocal(struct AppWindow *);
struct Window *WB_AppWindowWindow(struct AppWindow *);
ULONG WB_AppIconFlags(struct AppIcon *);
APTR LockAppList(void);
APTR NextAppEntry(APTR,ULONG);
void UnlockAppList(void);

#define MTYPE_APPSNAPSHOT	0x3812
#define MTYPE_APPICONMENU	0x3813

struct AppSnapshotMsg
{
    struct AppMessage	    ap_msg;	    // Message
    long		    position_x;	    // Icon x-position
    long		    position_y;	    // Icon y-position
    struct IBox		    window_pos;	    // Window position
    unsigned long	    flags;	    // Flags
    long		    id;		    // ID
};

#define APPSNAPF_UNSNAPSHOT	(1<<0)		// Set "no position"
#define APPSNAPF_WINDOW_POS	(1<<1)		// Window position supplied
#define APPSNAPF_MENU		(1<<2)		// Menu operation
#define APPSNAPF_CLOSE		(1<<3)		// Close command
#define APPSNAPF_HELP		(1<<4)		// Help on a command
#define APPSNAPF_INFO		(1<<5)		// Information command

#define ICONF_POSITION_OK	(1<<31)		// Galileo position stored in icon
#define ICONF_BORDER_ON		(1<<30)		// Icon border on
#define ICONF_ICON_VIEW		(1<<29)		// View as icons
#define ICONF_BORDER_OFF	(1<<28)		// Icon border off
#define ICONF_NO_LABEL		(1<<27)		// No label

ULONG GetIconFlags(struct DiskObject *);
void SetIconFlags(struct DiskObject *,ULONG);
void GetIconPosition(struct DiskObject *,short *,short *);
void SetIconPosition(struct DiskObject *,short,short);

struct DiskObject *CopyDiskObject(struct DiskObject *,ULONG);
void FreeDiskObjectCopy(struct DiskObject *);

#define DOCF_COPYALL		(1<<0)		// Copy tooltypes, etc
#define DOCF_NOIMAGE		(1<<1)		// Don't copy image data

unsigned long IconCheckSum(struct DiskObject *,short);
BOOL RemapIcon(struct DiskObject *,struct Screen *,short);
struct DiskObject *GetOriginalIcon(struct DiskObject *);

void ChangeAppIcon(APTR,struct Image *,struct Image *,char *,ULONG);
long SetAppIconMenuState(APTR,long,long);

#define GCDOF_NOCACHE		(1<<28)
#define GCDOFN_REAL_ICON	(1<<0)

#define CAIF_RENDER	(1<<0)
#define CAIF_SELECT	(1<<1)
#define CAIF_TITLE	(1<<2)
#define CAIF_LOCKED	(1<<3)
#define CAIF_SET	(1<<4)
#define CAIF_BUSY	(1<<5)
#define CAIF_UNBUSY	(1<<6)
#define CAIF_GHOST	(1<<7)

#define MTYPE_GALILEOFM		(UWORD)-1

#define GNT_LISTER_APPMSG_PORT  161
#define GNT_APPMSG_PORT		173
#define GNT_MAIN_APPMSG_PORT    196
#define	MTYPE_LISTER_APPWINDOW	167
#define GLAM_VERSION		1
#define GLAMCLASS_LISTER	162
#define GLAMCLASS_LISTER_ICON	163
#define GLAMCLASS_DESKTOP_ICON	164
#define GLAMCLASS_GROUP_ICON	165


struct GLArg {
    BYTE *		gla_Name;	 /* a string relative to that lock */
    Point		gla_DropPos;
    ULONG		gla_Flags;
};

struct GLAData {
    Point		glad_DropPos;
    ULONG		glad_Flags;
};

typedef struct _GalileoAppMessage
{
    struct AppMessage	ga_Msg;
    Point		*ga_DropPos;
    Point		ga_DragOffset;
    ULONG		ga_Flags;
    ULONG		ga_Pad[2];
} GalileoAppMessage;

#define GAPPF_ICON_DROP		(1<<16)		// Dropped with icon

struct AppMessage *AllocAppMessage(APTR,struct MsgPort *,WORD);
void FreeAppMessage(struct AppMessage *);
BOOL CheckAppMessage(struct AppMessage *);
struct AppMessage *CopyAppMessage(struct AppMessage *,APTR);
BOOL SetWBArg(struct AppMessage *,WORD,BPTR,char *,APTR);

#define WBAF_NOT_A_LOCK		(1<<31)

typedef struct
{
    struct Message	gham_Message;
    STRPTR		gham_Command;
    APTR		gham_SourceLister;
    APTR		gham_DestLister;
    BPTR		gham_DestLock;
    ULONG		gham_NumArgs;
    UWORD		gham_Qual;
    UWORD		gham_Flags;
    struct GLAData	*gham_ArgData;
    struct WBArg	gham_ArgList[0];
} GalileoHandlerAppMessage;


typedef struct
{
    struct Message	ghm_Message;
    STRPTR		ghm_Command;
    ULONG		ghm_Data;
    APTR		ghm_SourceLister;
    APTR		ghm_DestLister;
    BPTR		ghm_DestLock;
    APTR		ghm_Entry;
    STRPTR		ghm_Args;
    APTR		ghm_FunctionHandle;
    UWORD		ghm_Qual;
    UWORD		ghm_Flags;
} GalileoHandlerMessage;


void FreeHandlerMessage(GalileoHandlerMessage *msg);
void ReplyHandlerMessage(GalileoHandlerMessage *msg);

// Progress window
APTR OpenProgressWindow(struct TagItem *);
void CloseProgressWindow(APTR);
void HideProgressWindow(APTR);
void ShowProgressWindow(APTR,struct Screen *,struct Window *);
void SetProgressWindow(APTR,struct TagItem *);
void GetProgressWindow(APTR,struct TagItem *);
BOOL CheckProgressAbort(APTR);
long CalcPercent(ULONG,ULONG);

APTR __stdargs OpenProgressWindowTags(Tag,...);
void __stdargs SetProgressWindowTags(APTR,Tag,...);
void __stdargs GetProgressWindowTags(APTR,Tag,...);

#define PW_Screen	TAG_USER + 0		// Screen to open on
#define PW_Window	TAG_USER + 1		// Owner window
#define PW_Title	TAG_USER + 2		// Window title
#define PW_SigTask	TAG_USER + 3		// Task to signal
#define PW_SigBit	TAG_USER + 4		// Signal bit
#define PW_Flags	TAG_USER + 5		// Flags
#define PW_FileName	TAG_USER + 6		// File name
#define PW_FileSize	TAG_USER + 7		// File size
#define PW_FileDone	TAG_USER + 8		// File done
#define PW_FileCount	TAG_USER + 9		// Number of files
#define PW_FileNum	TAG_USER + 10		// Current number
#define PW_Info		TAG_USER + 11		// Information line
#define PW_Info2	TAG_USER + 12		// Information line
#define PW_Info3	TAG_USER + 13		// Information line
#define PW_FileInc	TAG_USER + 14		// Increment file number

#define PWF_FILENAME		(1<<0)		// Filename display
#define PWF_FILESIZE		(1<<1)		// Filesize display
#define PWF_INFO		(1<<2)		// Information line
#define PWF_GRAPH		(1<<3)		// Bar graph display
#define PWF_NOABORT		(1<<4)		// No abort gadget
#define PWF_INVISIBLE		(1<<5)		// Open invisibly
#define PWF_ABORT		(1<<6)		// Want abort gadget
#define PWF_SWAP		(1<<7)		// Swap bar and size displays
#define PWF_DEBUG		(1<<8)		// Debug stuff
#define PWF_NOIPC		(1<<9)		// No IPC from sender
#define PWF_INFO2		(1<<10)		// Information line
#define PWF_INFO3		(1<<11)		// Information line

// Search
long SearchFile(APTR,UBYTE *,ULONG,UBYTE *,ULONG);

// Search Flags
#define SEARCH_NOCASE		(1<<0)	// Not case sensitive
#define SEARCH_WILDCARD		(1<<1)	// Wildcards
#define SEARCH_ONLYWORDS	(1<<2)	// Only match whole words


// Date routines
char *ParseDateStrings(char *,char *,char *,long *);
BOOL DateFromStrings(char *,char *,struct DateStamp *,ULONG);
void SetLocaleFlags(UWORD,UWORD);
UWORD GetLocaleSettings(UBYTE);
void DateToStrings(struct DateStamp *date,char *date_buf,char *time_buf,int flags);

#define RANGE_BETWEEN		1
#define RANGE_AFTER		2
#define RANGE_WEIRD		-1


// Filetype matching
APTR GetMatchHandle(char *,BPTR);
void FreeMatchHandle(APTR);
BOOL MatchFiletype(APTR,APTR);
void ClearFiletypeCache(void);


// version
BOOL GetFileVersion(char *,short *,short *,struct DateStamp *,APTR);


// Set flags in library
ULONG SetLibraryFlags(ULONG,ULONG);
ULONG GetLibraryFlags(void);

#define LIBDF_NO_CACHING	(1<<1)
#define LIBDF_NOSTIPPLE		(1<<2)
#define LIBDF_DOS_PATCH		(1<<3)
#define LIBDF_3DLOOK		(1<<4)
#define LIBDF_FT_CACHE		(1<<5)
#define LIBDF_REDIRECT_TOOLS	(1<<6)
#define LIBDF_BORDERS_OFF	(1<<7)
#define LIBDF_NO_CUSTOM_DRAG	(1<<8)
#define LIBDF_THIN_BORDERS	(1<<9)
#define LIBDF_USING_OS35	(1<<10) // gjp

// Bitmap
struct BitMap *NewBitMap(ULONG,ULONG,ULONG,ULONG,struct BitMap *);
void DisposeBitMap(struct BitMap *);


void SetEnv(char *,char *,BOOL);


// Original DOS functions
BPTR OriginalCreateDir(char *);
long OriginalDeleteFile(char *);
BOOL OriginalSetFileDate(char *,struct DateStamp *);
BOOL OriginalSetComment(char *,char *);
BOOL OriginalSetProtection(char *,ULONG);
BOOL OriginalRename(char *,char *);
BOOL OriginalRelabel(char *,char *);
BPTR OriginalOpen(char *,LONG);
BOOL OriginalClose(BPTR);
LONG OriginalWrite(BPTR,void *,LONG);

// REXX stuff

#define RexxTag_Arg0		( TAG_USER + 0x1 )
#define RexxTag_Arg1		( TAG_USER + 0x2 )
#define RexxTag_Arg2		( TAG_USER + 0x3 )
#define RexxTag_Arg3		( TAG_USER + 0x4 )
#define RexxTag_Arg4		( TAG_USER + 0x5 )
#define RexxTag_Arg5		( TAG_USER + 0x6 )
#define RexxTag_Arg6		( TAG_USER + 0x7 )
#define RexxTag_Arg7		( TAG_USER + 0x8 )
#define RexxTag_Arg8		( TAG_USER + 0x9 )
#define RexxTag_Arg9		( TAG_USER + 0xa )
#define RexxTag_Arg10		( TAG_USER + 0xb )
#define RexxTag_Arg11		( TAG_USER + 0xc )
#define RexxTag_Arg12		( TAG_USER + 0xd )
#define RexxTag_Arg13		( TAG_USER + 0xe )
#define RexxTag_Arg14		( TAG_USER + 0xf )
#define RexxTag_Arg15		( TAG_USER + 0x10 )
#define RexxTag_VarName		( TAG_USER + 0x11 )
#define RexxTag_VarValue	( TAG_USER + 0x12 )

struct RexxMsg *CreateRexxMsgEx(struct MsgPort *,UBYTE *,UBYTE *);
void FreeRexxMsgEx(struct RexxMsg *);
long SetRexxVarEx(struct RexxMsg *,char *,char *,long);
long GetRexxVarEx(struct RexxMsg *,char *,char **);
struct RexxMsg *BuildRexxMsgEx(struct MsgPort *,UBYTE *,UBYTE *,struct TagItem *);
struct RexxMsg *BuildRexxMsgExTags(struct MsgPort *,UBYTE *,UBYTE *,Tag,...);


// doslist

typedef struct
{
    struct Node	    dle_Node;
    struct DosList  dle_DosList;
    char	    dle_DeviceName[40];
    short	    dle_DeviceUnit;
} DosListEntry;

void NotifyDiskChange(void);
void GetDosListCopy(struct List *,APTR);
void FreeDosListCopy(struct List *);


#ifndef MTYPE_APPWINDOW
#define MTYPE_APPWINDOW		7
#endif
#ifndef MTYPE_APPICON
#define MTYPE_APPICON		8
#endif
#ifndef MTYPE_APPMENUITEM
#define MTYPE_APPMENUITEM	9
#endif


#define PAGEUP		0x3f
#define PAGEDOWN	0x1f
#define HOME		0x3d
#define END		0x1d

struct NewIconDiskObject
{
    struct DiskObject	    nido_DiskObject;
    struct NewDiskObject    *nido_NewDiskObject;
    short		    nido_Flags;
};

#define NIDOF_REMAPPED		(1<<0)

enum
{
    ICON_NORMAL,
    ICON_CACHED,
    ICON_NEWICON
};

short GetIconType(struct DiskObject *);


void SetNewIconsFlags(ULONG,short);
void SetReqBackFill(struct Hook *hook,struct Screen **);

struct BackFillInfo {
    struct Layer *layer;
    struct Rectangle bounds;
    LONG offsetx;
    LONG offsety;
};

struct GalileoSemaphore {
    struct SignalSemaphore  sem;
    APTR		    main_ipc;
    struct ListLock	    modules;
};


long GetStatistics(long id);

#define STATID_TASKCOUNT	0
#define STATID_CPU_USAGE	1


typedef struct
{
    struct Node	    node;
    struct IBox	    pos;
    ULONG	    flags;
    ULONG	    icon_pos_x;
    ULONG	    icon_pos_y;
} ButtonBankNode;

typedef struct
{
    struct Node	    node;
    APTR	    *lister;
} OpenListerNode;


struct AllocBitmapPatchNode
{
    struct MinNode  abp_Node;
    struct Screen   *abp_Screen;
    struct Task	    *abp_Task;
};

#ifndef GALILEOFM_PRAG
#include "galileofmpragmas.h"
#endif

#endif

/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 1993-2012 Jonathan Potter & GP Software
Copyright 2025 Hagbard Celine

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

#ifndef _GALILEOFM_BACKDROP
#define _GALILEOFM_BACKDROP

#include "app_msg.h"
#include "filetypes.h"

typedef struct _BackdropObject
{
    struct Node			    bdo_node;

    short			    bdo_type;		// Type of object
    short			    bdo_state;		// Object state

    struct DiskObject		    *bdo_icon;		// Object icon
    char			    *bdo_name;		// Object name
    char			    *bdo_path;		// Path name
    struct DateStamp		    bdo_date;		// Object date
    char			    *bdo_device_name;   // Device name (for disks)

    unsigned long 		    bdo_flags;	        // Flags

    struct IBox			    bdo_pos;	        // Object position
    struct Rectangle		    bdo_show_rect;      // Rectangle we display in
    struct Rectangle		    bdo_full_size;      // Full size of object
    struct Rectangle		    bdo_image_rect;     // Rectangle for image
    ULONG			    bdo_custom_pos;	// Custom position

    UWORD			    *bdo_image_mask[2];	// Image mask

    DragInfo			    *bdo_drag_info;	// Drag info for this object
    short			    bdo_drag_x_offset;
    short			    bdo_drag_y_offset;

    ULONG			    bdo_misc_data;	// Miscellaneous data
    unsigned long		    bdo_value;		// Value for position-sorting
    Cfg_Filetype		    *bdo_filetype;	// Filetype

    unsigned long		    bdo_size;		// File size

    BPTR			    bdo_parent_lock;	// Lock on parent dir for left-outs

    ULONG			    bdo_vol_name_len;

} BackdropObject;


typedef struct _BackdropInfo
{
    struct Window		    *window;		    // Window pointer
    struct SignalSemaphore	    window_lock;	    // Lock on window
    struct List			    boopsi_list;	    // BOOPSI list
    struct Gadget		    *vert_scroller;	    // Scrollers
    struct Gadget		    *horiz_scroller;

    struct ListLock		    objects;		    // Buffer pointer
    unsigned short		    flags;		    // Flags

    long			    virt_width;		    // Virtual window size
    long			    virt_height;
    long			    offset_x;		    // Offset in display
    long			    offset_y;

    struct Rectangle		    area;		    // Virtual area needed
    struct Rectangle		    size;		    // Physical area available
    struct Rectangle		    select;		    // Select area

    struct Region		    *clip_region;	    // Region for clipping

    BackdropObject		    *last_sel_object;	    // Last selected object
    BackdropObject		    *first_sel_tool;	    // First tool selected
    unsigned long		    seconds;			    // Double-click info
    unsigned long		    micros;
    struct Gadget		    *down_gadget;	    // Gadget held down

    char			    buffer[512];	    // Some memory to play with
    APTR			    memory;			    // memory handle to use

    short			    last_x,last_y;	    // Last drag positions

    BackdropObject		    *current_sel_object;    // Currently selected object

    struct MsgPort		    *notify_port;	    // Notification port
    APTR			    notify_req;

    short			    top_border;
    short			    bottom_border;
    short			    left_border;
    short			    right_border;

    struct RastPort		    rp;			    // Rastport

    short			    last_x_pos;
    short			    last_y_pos;

    unsigned long		    tick_count;		    // Intuiticks count
    unsigned long		    last_tick;

    Lister			    *lister;		    // Lister pointer
    IPCData			    *ipc;		    // Owner IPC

    unsigned short		    last_button;

    struct SignalSemaphore	    idcmp_lock;		    // Get IDCMP lock

    struct TextFont		    *font;		    // Font for icons

    struct Region		    *icon_space;	    // Free icon space

    REF_CALLBACK		    (callback);		    // Refresh callback

    ULONG			    drag_sec;		    // Drag start seconds
    ULONG			    drag_mic;		    // Drag start micros

    struct Region		    *temp_region;
    struct SignalSemaphore	    icon_lock;		    // lister_get_icons lock
} BackdropInfo;


enum
{
    BDO_NO_TYPE=-1,
    BDO_DISK,
    BDO_LEFT_OUT,
    BDO_APP_ICON,
    BDO_BAD_DISK,
    BDO_GROUP,
};

#define BDOF_SELECTED			(1<<0)		// This object is selected
#define BDOF_TEMP_LEFTOUT		(1<<1)		// Temporarily left-out
#define BDOF_AUTO_POSITION		(1<<2)		// Position icon automatically
#define BDOF_ICON_VIEW			(1<<3)		// Object is in an iconview lister
#define BDOF_FAKE_ICON			(1<<4)		// Not a real icon
#define BDOF_ASSIGN_VOL			(1<<5)		// Assign points to root of a volume
#define BDOF_STATE_CHANGE		(1<<6)		// State changed
#define BDOF_CUSTOM_POS			(1<<7)		// Custom position
#define BDOF_NO_LABEL			(1<<8)		// No label
#define BDOF_NO_POSITION		(1<<9)		// No position set
#define BDOF_CUSTOM_LABEL		(1<<10)		// Custom label
#define BDOF_LEFTOUT_POS		(1<<11)		// Left-out position
#define BDOF_LINK_ICON			(1<<12)		// Icon for a link
#define BDOF_ASSIGN			(1<<13)		// Not really a disk
#define BDOF_CACHE			(1<<14)		// Cache directory
#define BDOF_BACKGROUND			(1<<15)		// Background colour
#define BDOF_LOCKED			(1<<16)		// Position locked
#define BDOF_BUSY			(1<<17)		// Busy
#define BDOF_BORDER_ADJUST		(1<<18)		// Adjust for border
#define BDOF_SPECIAL			(1<<19)		// Special flag
#define BDOF_DESKTOP_FOLDER		(1<<20)		// In desktop folder
#define BDOF_GHOSTED			(1<<21)		// Icon to be rendered ghosted
#define BDOF_REMAPPED			(1<<22)		// Icon has been remapped at least once

#define BDOF_TEMP_FLAG			(1<<26)		// A temporary flag
#define BDOF_REFRESH			(1<<27)		// Icon needs to be refreshed
#define BDOF_AUTO_OPEN			(1<<28)		// Automatically open this group
#define BDOF_SAVE_LEFTOUT		(1<<29)		// Need to save left outs
#define BDOF_DO_LEFT_OUT		(1<<30)		// Need to do left outs
#define BDOF_OK				(1<<31)		// Object is ok

#define BDIF_DRAGGING			(1<<0)		// Currently dragging
#define BDIF_IGNORE_GADGET		(1<<1)		// Ignore the next gadget
#define BDIF_NEW_SIZE			(1<<2)		// Just re-sized
#define BDIF_RUBBERBAND			(1<<3)		// Rubber-banding select area
#define BDIF_ANT_ON			(1<<4)		// Which ants?
#define BDIF_TICK_SKIP			(1<<5)		// Skip this tick?
#define BDIF_KEY_SELECTION		(1<<6)		// Key selection mode on
#define BDIF_LAST_POS_OK		(1<<7)		// Last position is valid
#define BDIF_REFRESHED			(1<<8)		// Already refreshed
#define BDIF_ORDERING			(1<<9)		// Ordering icons
#define BDIF_LAST_SORT_HORIZ		(1<<10)		// Last sort was horizontal
#define BDIF_MAIN_DESKTOP		(1<<11)		// Main desktop
#define BDIF_GROUP			(1<<12)		// Program group
#define BDIF_CUSTOM_DRAG		(1<<13)		// Custom drag routines
#define BDIF_SHOW_ALL			(1<<14)		// Show All (Listers only)

#define BACKDROP_POS_OK			(1<<31)
#define BACKDROP_GROUP_OBJECT		(1<<30)
#define BACKDROP_ICON_VIEW		(1<<29)

typedef struct
{
    IPCData		    *ipc;
    struct Screen	    *screen;
    struct IBox		    dimensions;
    char		    name[32];
    BackdropObject	    *object;

    struct Window	    *window;
    BackdropInfo	    *info;

    struct AppWindow	    *appwindow;
    struct MsgPort	    *appport;

    TimerHandle		    *timer;
    BOOL		    got_quit;

    struct TextFont	    *font;
    short		    error_count;

    WindowID		    id;			    // Window ID pointer

    PatternInstance	    pattern;		    // Backdrop pattern

    PopUpHandle		    *popup;
} GroupData;

#define GETICON_CD		(1<<0)
#define GETICON_KEEP		(1<<1)
#define GETICON_SAVE_POS	(1<<2)
#define GETICON_NO_POS		(1<<3)
#define GETICON_POS_ONLY	(1<<4)
#define GETICON_FAIL		(1<<5)
#define GETICON_DEFDIR		(1<<6)
#define GETICON_DEFAULT		(1<<7)
#define GETICON_REMAP		(1<<8)

#define BIDCMPF_DOUBLECLICK	(1<<0)
#define BIDCMPF_LISTER		(1<<1)

#define BDSF_CLEAR		(1<<0)
#define BDSF_CLEAR_ONLY		(BDSF_CLEAR|(1<<1))
#define BDSF_REFRESH		(1<<2)
#define BDSF_REFRESH_DONE	(BDSF_REFRESH|(1<<3))
#define BDSF_RECALC		(1<<4)
#define BDSF_IN_REFRESH		(1<<5)
#define BDSF_RESET		(1<<6)
#define BDSF_NO_CLIP		(1<<7)

#define BDNF_CD			(1<<0)
#define BDNF_KEEP		(1<<1)
#define BDNF_RECALC		(1<<2)
#define BDNF_NO_DISKS		(1<<3)
#define BDNF_NO_ARRANGE		(1<<4)


#define BOOF_ACTIVATE	    	(1<<0)
#define BOOF_NOARGS	    	(1<<1)


#define BAPPF_MENU		(1<<15)
#define BAPPF_HELP		(1<<14)
#define BAPPF_CLOSE		(1<<0)
#define BAPPF_UNSNAPSHOT	(1<<1)
#define BAPPF_INFO		(1<<2)

#define BAPPF_ID(id)		(id&~(BAPPF_MENU|BAPPF_HELP))

typedef struct
{
	ULONG   bpflags;
	UWORD   bpcode;
} BackdropPopupData;

#define BPOPF_ICONS		(1<<0)
#define BPOPF_DIRENTRY		(1<<1)

#define BLNF_CUSTOM_LABEL	(1<<0)

#define BLOF_PERMANENT		(1<<0)
#define BLOF_REFRESH		(1<<1)

#define BDEVF_DISK_REMOVED	(1<<0)
#define BDEVF_DISK_ADDED	(1<<1)

#define BDEVF_FORCE_LOCK	(1<<0)
#define BDEVF_MARK_OK		(1<<1)
#define BDEVF_SHOW		(1<<2)

#define ID_BUSY		0x42555359


#define BGOF_DISKS		(1<<0)
#define BGOF_APPICONS		(1<<1)
#define BGOF_LEFTOUT		(1<<2)
#define BGOF_GROUPS		(1<<3)

#define BGOF_DESKTOP		(1<<7)
#define BGOF_SHOW		(1<<8)
#define BGOF_FIRST		(1<<9)
#define BGOF_ALL		(BGOF_DISKS|BGOF_APPICONS|BGOF_LEFTOUT|BGOF_GROUPS)

#define BDAF_SHOW		(1<<0)
#define BDAF_LOCK		(1<<1)
#define BDAF_CHANGE		(1<<2)
#define BDAF_NEW_IMAGE		(1<<3)

enum
{
	BSORT_NORM,
	BSORT_HORIZ,
	BSORT_NAME,
	BSORT_TYPE,
	BSORT_SIZE,
	BSORT_DATE
};


#define BRENDERF_CLEAR	(1<<0)
#define BRENDERF_CLIP	(1<<1)
#define BRENDERF_LABEL	(1<<2)
#define BRENDERF_REAL	(1<<3)


#define WBICONMAGIC_X	4
#define WBICONMAGIC_Y	3


// Flags for backdrop_get_object()
#define BDGOF_CHECK_MASK	(1<<0)
#define BDGOF_CHECK_LABEL	(1<<1)


enum
{
	WBVIEWMODE_DEFAULT,
	WBVIEWMODE_ICON,
	WBVIEWMODE_NAME,
	WBVIEWMODE_DATE,
	WBVIEWMODE_SIZE,
};

#define WBSHOWFLAG_SHOWALL	(1<<1)
#define WBSHOWFLAG_ACTION	(1<<28)


#define CLEANUPF_ALIGN_OK	(1<<0)
#define CLEANUPF_CHECK_POS	(1<<1)


#define ICON_BORDER_X		4
#define ICON_BORDER_Y_TOP	3
#define ICON_BORDER_Y_BOTTOM	2
#define CLEANUP_SPACE_X		7
#define CLEANUP_SPACE_Y		5
#define CLEANUP_START_X		3
#define CLEANUP_START_Y		7
#define ICON_LABEL_SPACE	3

#endif

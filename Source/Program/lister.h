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

#ifndef _GALILEOFM_LISTER
#define _GALILEOFM_LISTER

#include "popup.h"
#include "reselect.h"
#include "lister_ipc.h"
#ifndef _GALILEOFM_LISTERWINDOW_H
#include "listerwindow.h"
#endif

#define SLIDER_VERT			(1<<0)
#define SLIDER_HORZ			(1<<1)
#define SLIDER_VERT_DISPLAY		(1<<2)
#define SLIDER_HORZ_DISPLAY		(1<<3)
#define SLIDER_SCROLL			(1<<6)
#define UPDATE_POS_ONLY			(1<<7)

#define LISTER_BFPF_CHECK_LOCKS		(1<<0)
#define LISTER_BFPF_DONT_MOVE		(1<<1)
#define LISTER_BFPF_DONT_LOCK		(1<<2)
#define LISTER_BFPF_DONT_TEST		(1<<3)
#define LISTER_BFPF_DONT_UNLOCK		(1<<5)
#define LISTER_BFPF_ONLY_CACHE		(1<<6)
#define LISTER_BFPF_COMPARE_LOCKADDRESS	(1<<7)

#define LISTERF_MAKE_SOURCE		(1<<0)
#define LISTERF_ACTIVATE		(1<<1)
#define LISTERF_DEVICES			0x400000

#define KEY_SEL_OFFSET	12

typedef struct
{
	struct MinNode	node;
	IPCMessage	*msg;
	struct Message	*msg2;
	short		type;
} ListerWaiter;

#define LISTERWAIT_ALL			0
#define LISTERWAIT_UNBUSY		1
#define LISTERWAIT_BUSY			2
#define LISTERWAIT_AUTO_CMD		3

typedef struct
{
	ULONG	total;
	char	*operation;
	ULONG	flags;
} ProgressPacket;

enum
{
	GAD_PATH,
	GAD_PARENT,

	GAD_ABORT=8,
	GAD_ROOT,
	GAD_ICONIFY,
	GAD_LOCK,
	GAD_GAUGE,
	GAD_ZOOM,
};


#define LISTERF_BUSY		(1<<1)	// Lister is busy
#define LISTERF_SOURCE		(1<<2)	// Lister is a source
#define LISTERF_DEST		(1<<3)	// Lister is a destination
#define LISTERF_STORED_SOURCE	(1<<4)	// Lister was a source
#define LISTERF_STORED_DEST	(1<<5)	// Lister was a destination
#define LISTERF_LOCK		(1<<6)	// Lister is locked
#define LISTERF_LOCKED		(1<<7)	// Lister is locked (but not actually busy)
#define LISTERF_PROGRESS	(1<<8)	// Progress indicator is displayed
#define LISTERF_ABORTED		(1<<9)	// As operation has been aborted
#define LISTERF_SOURCEDEST_LOCK	(1<<10)	// Source/Destination locked
#define LISTERF_SNIFFING	(1<<11)	// Lister is sniffing things
#define LISTERF_RESCAN		(1<<12)	// Rescan when we become unbusy
#define LISTERF_BUSY_VISUAL	(1<<13)	// Visually busy
#define LISTERF_KEY_SELECTION	(1<<14)	// Keyboard selection active
#define LISTERF_SAVE_STATUS	(1<<15)	// Save status message
#define LISTERF_ICONIFIED	(1<<16)	// Lister is iconified
#define LISTERF_CLOSING		(1<<17)	// Lister is closing
#define LISTERF_CHANGED_MODE	(1<<18)	// Just changed mode
#define LISTERF_RESIZED		(1<<19)	// Just resized
#define LISTERF_NO_REFRESH	(1<<20)	// Don't refresh files
#define LISTERF_TITLE_DEFERRED	(1<<21)	// Title set deferred
#define LISTERF_ICON_ACTION	(1<<22)	// Icon Action mode
#define LISTERF_SHOW_TITLE	(1<<23)	// Update title
#define LISTERF_SHOW_ALL	(1<<24)	// Show All Files
#define LISTERF_PATH_FIELD	(1<<25)	// Path field is added
#define LISTERF_VIEW_ICONS	(1<<26)	// Icons being viewed
#define LISTERF_LOCK_POS	(1<<27)	// Position is locked
#define LISTERF_TEMP_FLAG	(1<<28)	// Temporary magic flag
#define LISTERF_TOOLBAR		(1<<29)	// Valid toolbar
#define LISTERF_DEVICE_LIST	(1<<30)	// Showing device list
#define LISTERF_FIRST_TIME	(1<<31)	// First initialisation

#define LISTERF2_UNAVAILABLE	(1<<0)	// Lister is unavailable for functions

#define LISTERF_PROP_FONT	(1<<0)	// Proportional font
#define LISTERF_TITLE		(1<<1)	// Title for fields
#define LISTERF_HIDE_TITLE	(1<<2)	// Title is hidden
#define LISTERF_CONFIGURE	(1<<3)	// Configuration window up
#define LISTERF_LOCK_FORMAT	(1<<4)	// Sort format is locked
#define LISTERF_MODE_CHANGED	(1<<5)	// Mode was just changed
#define LISTERF_NO_ACTIVE	(1<<6)	// Don't activate lister
#define LISTERF_TITLEBARRED	(1<<7)	// Shrunk to title bar
#define LISTERF_NEED_RECALC	(1<<8)	// Contents changed, needs recalc
#define LISTERF_FAKEDIR     (1<<9)  // Buffer contains fake directory

#define EDITF_REFRESH		(1<<0)	// Refresh needed
#define EDITF_CANCEL		(1<<1)	// Cancel
#define EDITF_LONGEST		(1<<2)	// This becomes the longest entry
#define EDITF_RECALC		(1<<3)	// Recalculate horizontal width
#define EDITF_NOREFRESH		(1<<4)	// Don't refresh

#define EDITF_KEEP		(1<<0)	// Keep changes
#define EDITF_OFFSET		(1<<1)	// Calculate offset
#define EDITF_NO_TITLE		(1<<2)	// Don't refresh title

extern UWORD lister_popup_data[];

#define REFRESHF_SLIDERS	(1<<0)     // 95, 185, 85, A5
#define REFRESHF_NAME		(1<<1)
#define REFRESHF_STATUS		(1<<2)     // 95, 185, 85, A5
#define REFRESHF_RESORT		(1<<3)
#define REFRESHF_ICONS		(1<<4)     // 95, 85
#define REFRESHF_CLEAR_ICONS	(1<<5)     // A5
#define REFRESHF_NO_ICONS	(1<<6)
#define REFRESHF_UPDATE_NAME	(1<<7)     // 95, 185, 85, A5
#define REFRESHF_CD		(1<<8)     // 185
#define REFRESHF_FULL		(1<<9)
#define REFRESHF_DATESTAMP	(1<<10)
#define REFRESHF_REFRESH	(1<<11)
#define REFRESHF_PATH		(1<<12)
#define REFRESHF_SORTSEL	(1<<13)
#define REFRESHF_COLOUR		(1<<14)
#define REFRESHF_NO_DIRLIST	(1<<15)

enum
{
	PP_PARENT=1,
	PP_ROOT,
	PP_DEVICE_LIST,
	PP_BUFFER_LIST,
	PP_REREAD_DIR,
	PP_BASE
};


#define FIELD_FONT lister->window->WScreen->RastPort.Font

#define LREFRESH_FULL		(1<<0)
#define LREFRESH_CLEAR		(1<<1)
#define LREFRESH_NOICON		(1<<2)
#define LREFRESH_REFRESH	(1<<3)
#define LREFRESH_FULL_ICON	(1<<4)
#define LREFRESH_SIMPLEREFRESH	(1<<5)


#define DROPF_DEVICE	    (1<<0)  // Dropped item is a device
#define DROPF_ICON_MODE	    (1<<1)  // Source lister in icon mode
#define DROPF_PARENT	    (1<<2)  // Dropped on parent area
#define DROPF_BORROWED_LOCK (1<<3)  // Lock belongs to lister buffer, do not unlock

typedef struct
{
	char			old_name[32];
	char			new_name[32];
	struct DateStamp	date;
} devname_change;


#define LISTERPOPUP_CLOSE	0


#define GETICONSF_CLEAN		(1<<0)
#define GETICONSF_NO_REFRESH	(1<<1)
#define GETICONSF_SHOW		(1<<2)


#define LISTER_TABSIZE 3

#endif

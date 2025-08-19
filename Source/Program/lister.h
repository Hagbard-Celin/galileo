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

// Maximum length we can display in a window
#define MAXDISPLAYLENGTH  256

typedef struct
{
	short	tb_Left;
	short	tb_Right;
	short	tb_Item;
	short	tb_Position;
	short	tb_Width;
	short	tb_ItemNum;
} TitleBox;

typedef struct
{
	short		cr_Pen[2];
	short		cr_Alloc;
} ColourRecord;


// Defines a directory lister
typedef struct ListerWindow
{
	IPCData				*ipc;			// IPC data

	struct Window			*window;		// Lister window
	struct IBox			pos;			// Window position
	struct AppWindow		*appwindow;		// AppWindow pointer
	struct AppIcon			*appicon;		// AppIcon pointer
	struct List			_boopsi_list;		// List of boopsi objects

	GUI_Element			name_area;		// Disk name area
	GUI_Element			status_area;		// Lister status area
	GUI_Element			list_area;		// Full lister area
	GUI_Element			text_area;		// Area for the text
	UWORD				text_width;		// Displayed columns of text
	UWORD				text_height;		// Displayed lines of text
	struct Rectangle		scroll_border;		// Scroll border

	struct Gadget			*_vert_scroll;		// Vertical scroller
	struct Gadget			*_horiz_scroll;		// Horizontal scroller
	struct Gadget			*path_field;		// Path field
	STRPTR				path_buffer;		// Path field string
	struct Gadget			parent_button;		// Parent button

	Cfg_Lister			*lister;		// Lister pointer

	struct DirectoryBuffer		*cur_buffer;		// Current directory buffer
	struct DirectoryBuffer		*special_buffer;	// Special buffer
	struct DirectoryBuffer		*old_buffer;		// Last buffer displayed

	ListFormat			format;			// List format

	struct Gadget			*down_gadget;		// Gadget held down

	struct _BackdropInfo		*backdrop_info;		// Backdrop information

	long				selector_pos;		// Selector position

	struct DiskObject		*appicon_icon;		// AppIcon icon
	char				icon_name[36];		// Icon name

	struct Hook			*path_edit_hook;	// Edit hook for path field
	Att_List			*path_history;		// Path history

	WindowDimensions		dimensions;		// Stored dimensions

	short				abort_signal;		// Signal for abort

	ULONG				flags;			// Flags

	char				*title;			// Title buffer

	struct BitMap			*render_bitmap;		// Bitmap for file display
	long				render_bitmap_width;	// Width
	struct Layer_Info		*render_layerinfo;
	struct Layer			*render_layer;

	short				title_drag_x;

	ULONG				sep_old_seconds;	// Old event seconds
	ULONG				sep_old_micros;		// Old event micros

	DragInfo			*drag_info;		// File drag information
	struct MsgPort			*app_port;		// Application message port

	struct Requester		busy_req;		// Busy requester
	struct ListerWindow		*lock_list;		// Lister we are locked by
	struct IBox			win_limits;		// Normal window size limits

	IPCData				*locker_ipc;		// Locker's IPC data

	ULONG				seconds;		// Event seconds
	ULONG				micros;			// Event micros
	ULONG				old_seconds;		// Old event seconds
	ULONG				old_micros;		// Old event micros

	struct DrawInfo			*drawinfo;		// DrawInfo
	APTR				_visualinfo;		// VisualInfo

	short				normal_pri;		// Priority
	short				busy_pri;

	APTR				progress_window;	// Progress window
	APTR				progress_memory;	// Memory for progress names

	struct TextAttr			lister_font;		// Font used by lister
	struct TextAttr			screen_font;		// Font used by screen

	struct MsgPort			*timer_port;		// Timer port
	TimerHandle			*busy_timer;		// Timer request for busy

	PopUpMenu			lister_menu;		// Lister menus
	PopUpMenu			lister_tool_menu_old;
	APTR				lister_memory;		// Lister memory

	ULONG				rmb_old_seconds;	// Old event seconds
	ULONG				rmb_old_micros;		// Old event micros

	WindowID			id;			// Window ID pointer

	struct DirectoryEntry		*selected_entry;	// Stores selected entry
	struct DirectoryEntry		*last_selected_entry;	// Last selected entry

	struct TextFont			*font;			// Font we use
	char				font_name[33];		// Font name
	UBYTE				font_size;		// Font size

	char				work_buffer[1024];	// Work space
	struct FileInfoBlock		*fib;			// FIB to play with

	char				display_buf[MAXDISPLAYLENGTH];	// Display buffer

	TimerHandle			*scroll_timer;		// Scroll timer

	ToolBarInfo			*toolbar;		// Toolbar
	GUI_Element			toolbar_area;		// Area for the toolbar

	short				tool_type;		// Function type
	short				tool_sel;		// Selected button
	short				tool_button;		// Mouse button down
	short				tool_selected;		// Button is selected
	Cfg_Button			*tool_sel_button;	// Selected button

	struct _BackdropObject		*backdrop;		// Backdrop object we came from

	TimerHandle			*foo_timer;		// Another timer

	GUI_Element			parent_area;		// Parent/history popup
	GUI_Element			command_area;		// Command popup

	struct Gadget			size_cover_gadget;	// Covers sizing gadget
	struct Gadget			zoom_cover_gadget;	// Covers zoom gadget

	struct ListLock			user_data_list;		// User data list

	short				history_count;

	short				toolbar_offset;		// Toolbar offset
	short				toolbar_show;		// Number of displayed buttons

	short				drag_x;			// Drag shit
	short				drag_y;

	TimerHandle			*icon_drag_timer;	// Timer for dragging icons

	struct Rectangle		path_border;		// Path field border size

	struct position_record		*pos_rec;		// Original position record

	PatternInstance			pattern;		// Backfill pattern

	ToolBarInfo			*toolbar_alloc;		// Allocated ToolBar

	ULONG				old_flags;

	char				toolbar_path[256];	// Toolbar path

	short				tool_row;		// Selected button row

	APTR				toolbar_image[2];	// Images of current button

	struct MinList			wait_list;

	ReselectionData			reselect;		// Reselection information

	struct DirectoryEntry		*last_highlight;	// Highlighted entry
	short				last_highpos;		// position

	APTR				notify;			// Lister notify
	struct MsgPort			*notify_port;

	char				hot_name[32];		// Hot name
	short				hot_name_pos;
	struct Requester		*hot_name_req;
	short				hot_name_bit;
	struct Hook			*hot_name_hook;

	ListFormat			user_format;		// User-defined format

	ULONG				more_flags;		// Some more flags

	GUI_Element			title_area;		// List title
	struct Region			*title_region;		// Region for title clipping

	TitleBox			title_boxes[16];	// Position of title boxes

	short				title_click;		// Title clicked on
	short				title_sel;		// Selected or not

	struct IBox			other_dims;		// Dimensions in other mode

	short				title_click_x;
	short				title_click_y;

	long				icon_pos_x;
	long				icon_pos_y;

	short				cursor_pos;		// Position of cursor in display
	short				cursor_line;		// Line in the display being edited
	short				edit_type;		// Item being edited
	short				edit_pos;		// Position of cursor in item
	TimerHandle			*edit_timer;		// Timer for editing
	struct DirectoryEntry		*edit_entry;		// Entry being edited
	short				edit_length;		// Length of entry
	short				edit_max_length;	// Maximum length of entry
	char				edit_buffer[256];	// Edit buffer
	short				edit_field_pos;		// Field position

	UWORD				flags2;			// More flags

	char				*edit_ptr;		// Edit pointer
	char				*edit_old_ptr;		// Old edit pointer
	ULONG				edit_flags;		// Edit flags
	short				edit_old_width;		// Last max width
	short				edit_v_scroll;		// Amount to scroll by

	short				edit_old_offset;	// Offset for new edit
	short				edit_new_offset;

	short				toolbar_arrow_left;	// Arrow position
	short				toolbar_arrow_right;

	struct Gadget			*gauge_gadget;		// Gauge
	short				old_border_left;
	short				pad;

	struct Region			*refresh_extra;		// Refresh region

	struct IBox			restore_dims;		// Dimensions when restored from zoom

	PopUpHandle			*lister_tool_menu;

	ColourRecord			lst_Colours[ENVCOL_MAX];

	struct RastPort			render_rast;

	BPTR				lister_orgdir;

	struct Border			size_gadget_border;
	WORD				size_gadget_border_coords[6];
} Lister;	


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

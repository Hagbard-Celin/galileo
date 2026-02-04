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

#ifndef _GALILEOFM_LISTERWINDOW_H
#define _GALILEOFM_LISTERWINDOW_H

#ifndef _GALILEOFMBASE_IPC_H
#include <gfm/galileo_ipc.h>
#endif
#ifndef _GALILEOFM_LIST_MANAGEMENT_H
#include <gfm/list_management.h>
#endif
#ifndef _GALILEOFM_DRAG_ROUTINES_H
#include <gfm/drag_routines.h>
#endif
#ifndef _GALILEOFM_TIMER_H
#include <gfm/timer.h>
#endif
#ifndef GALILEOFM_POPUPMENU_H
#include <gfm/popupmenu.h>
#endif
#ifndef GALILEOFM_WINDOWID_H
#include <gfm/windowid.h>
#endif
#ifndef _GALILEOFM_CFG_BUTTONBANK_H
#include <gfm/cfg_buttonbank.h>
#endif
#ifndef _GALILEOFM_LISTLOCK_H
#include <gfm/listlock.h>
#endif
#ifndef _GALILEO_PATTERN_H
#include "pattern.h"
#endif
#ifndef _GALILEOFM_RESELECT
#include "reselect.h"
#endif
#ifndef _GALILEOFM_DIRECTORYBUFFER_H
#include "directorybuffer.h"
#endif
#ifndef _GALILEOFM_TOOLBAR_H
#include "toolbar.h"
#endif
#ifndef _GALILEOFM_CONFIG_FLAGS_H
#include <gfm/config_flags.h>
#endif

typedef struct
{
	short		cr_Pen[2];
	short		cr_Alloc;
} ColourRecord;

typedef struct
{
	short	tb_Left;
	short	tb_Right;
	short	tb_Item;
	short	tb_Position;
	short	tb_Width;
	short	tb_ItemNum;
} TitleBox;

// Maximum length we can display in a window
#define MAXDISPLAYLENGTH  256

// Defines a directory lister
struct ListerWindow
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
};

#endif


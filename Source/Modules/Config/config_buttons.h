/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 1993-2012 Jonathan Potter & GP Software

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

#ifndef _GALILEOCONFIG_CONFIG_BUTTONS_H
#define _GALILEOCONFIG_CONFIG_BUTTONS_H


#ifndef LIBRARIES_IFFPARSE_H
#include <libraries/iffparse.h>
#endif
#ifndef _GALILEOSCREENDATA_H
#include <gfm/galileoscreendata.h>
#endif
#ifndef _GALILEOFMBASE_IPC_H
#include <gfm/galileo_ipc.h>
#endif
#ifndef _GALILEOFM_CFG_BUTTONBANK_H
#include <gfm/cfg_buttonbank.h>
#endif
#ifndef GALILEOFM_LAYOUT_H
#include <gfm/layout.h>
#endif
#ifndef GALILEOFM_OBJECTLIST_H
#include <gfm/objectlist.h>
#endif
#ifndef _GALILEOFM_LISTLOCK_H
#include <gfm/listlock.h>
#endif
#ifndef GALILEOFM_CFGDRAGINFO_H
#include <gfm/cfgdraginfo.h>
#endif
#ifndef _GALILEOFM_IMAGEREMAP_H
#include <gfm/imageremap.h>
#endif
#ifndef _GALILEOFM_CFG_INSTRUCTION_H
#include <gfm/cfg_instruction.h>
#endif


typedef struct _bank_node
{
	struct Node		node;
	Cfg_ButtonBank		*bank;		// Bank pointer
	Cfg_ButtonBank		*bank_backup;	// Backup of bank
	IPCData			*button_ipc;	// IPC of button bank process
	struct ListLock		proc_list;	// List of launched processes
	short			pending_quit;	// Quit pending on this bank
	IPCMessage		*quit_msg;	// Quit message
	short			toolbar;	// Node is a toolbar
} bank_node;

typedef struct
{
	struct Node		node;
	Cfg_Button		*button;
	IPCData			*editor;
} edit_node;

typedef struct
{
	NewConfigWindow		newwin;		// New window definition
	struct Window		*window;	// Our window
	ObjectList		*objlist;	// Object list

	struct List		bank_list;	// List of banks we know about
	bank_node		*bank_node;	// Current bank we're editing

	struct FontRequester	*font_req;	// Font requester
	IPCData			*ipc;		// Our IPC port
	IPCData			*owner_ipc;	// Owner's IPC port

	struct MsgPort		*app_port;	// Application port

	struct MsgPort		*clip_port;	// Clipboard port
	struct ClipboardHandle	clip_handle;	// Clipboard IO handle
	struct List		clip_list;	// List of clipped buttons
	long			clip_count;	// Count of entries in list
	struct Rectangle	clip_rect;	// Button clipboard position
	BOOL			clip_valid;	// Current clipboard is valid
	struct Gadget		*clip_scroller;	// Clipboard scroller

	GalileoScreenData		screen_data;	// Screen data

	Cfg_Button		*last_clip_draw;// Last button shown in clipboard

	short			select_col;	// Selected column
	short			select_row;	// Selected row

	struct List		edit_list;	// List of button editors

	struct IBox		window_pos;	// Window position
	BOOL			pos_valid;	// Position valid
	ConfigWindow		win_dims;	// Window dimensions

	ULONG			command_list;

	struct AppWindow	*appwindow;

	BOOL			change;

	IPCData			*paint_box;
	PaletteBoxData		palette_data;

	struct Window		*clip_window;	// Clipboard window
	ObjectList		*clip_objlist;
	struct IBox		clip_window_pos;
	BOOL			clip_pos_valid;

	CfgDragInfo		drag;

	ImageRemap		remap;
} config_buttons_data;


#define BUTREFRESH_SELECTOR		(1<<0)
#define BUTREFRESH_FONT			(1<<1)
#define BUTREFRESH_RESIZE		(1<<2)
#define BUTREFRESH_REFRESH		(1<<3)

#define WINDOW_BUTTONS			0x4000004


#define DRAW_MASK	1
#define DRAW_FIRST	2


extern MenuData button_toolbar_menu[],button_menu[];

#endif

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

#ifndef _GALILEOFM_FUNCEDDATA_H
#define _GALILEOFM_FUNCEDDATA_H

#ifndef _GALILEOFMBASE_IPC_H
#include <gfm/galileo_ipc.h>
#endif
#ifndef _GALILEOFM_CONFIGWIN_H
#include <gfm/configwin.h>
#endif
#ifndef GALILEOFM_OBJECTLIST_H
#include <gfm/objectlist.h>
#endif
#ifndef _GALILEOFM_CONFIG_DRAG_H
#include "config_drag.h"
#endif

typedef struct
{
	struct Window		*window;
	Cfg_Function		*function;
	APTR			memory;
	IPCData			*owner_ipc;
	struct _FuncEdData	*data;
	ConfigWindow		*win_def;
	ObjectDef		*obj_def;

	ConfigWindow		*req_win_def;
	ObjectDef		*req_obj_def;

	UWORD			*func_labels;
	long			*flag_list;
	ULONG			func_list;
	UWORD			*arg_list;

	APTR			object;
	ULONG			object_flags;

	char			title[80];

	ULONG			flags;

	IPCData			*main_owner;
} FunctionStartup;

typedef struct _FuncEdData
{
	struct Window		*window;		// Window
	ObjectList		*objlist;		// List of objects

	struct MsgPort		*appport;		// WB App port
	struct AppWindow	*appwindow;		// WB App window
	struct AppIcon		*appicon;		// WB App icon

	Cfg_Function		*function;		// Function to be edited
	Att_Node		*edit_node;		// Line being edited
	Att_List		*function_list;		// Function list
	Att_List		*func_display_list;	// Function display list
	Att_List		*flag_list;		// List of flags
	FunctionStartup		*startup;

	short			last_type;

	CfgDragInfo		drag;
	short			drag_shift;

	char			buffer[256];

	ObjectList		*p_objlist;

	char			label[80];
} FuncEdData;

#endif

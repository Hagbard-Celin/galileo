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

#ifndef _GALILEOCONFIG_BUTTON_EDITOR_H
#define _GALILEOCONFIG_BUTTON_EDITOR_H

#ifndef _GALILEOFMBASE_IPC_H
#include <gfm/galileo_ipc.h>
#endif
#ifndef _GALILEOFM_CFG_FUNCTION_H
#include <gfm/cfg_function.h>
#endif
#ifndef _GALILEOFM_CFG_BUTTONBANK_H
#include <gfm/cfg_buttonbank.h>
#endif
#ifndef _GALILEOFM_LISTLOCK_H
#include <gfm/listlock.h>
#endif
#ifndef _GALILEOFM_IMAGEREMAP_H
#include <gfm/imageremap.h>
#endif
#ifndef _GALILEOFM_FUNCEDDATA_H
#include "funceddata.h"
#endif
#ifndef _GFM_FUNCTION_EDITOR_H
#include "function_editor.h"
#endif

typedef struct
{
	struct MinNode		node;
	Cfg_ButtonFunction	*func;
	IPCData			*editor;
} ButtonEditorNode;

typedef struct
{
	struct _bank_node	*bank;
	Cfg_Button		*button;
	Cfg_Button		*orig_button;
} ButtonReturn;

typedef struct
{
	struct Window		*window;
	Cfg_Button		*button;
	IPCData			*owner_ipc;
	struct _ButtonEdData	*data;
	FunctionStartup		func_startup;
	ButtonReturn		retdata;

	ConfigWindow		*win_text_def;
	ConfigWindow		*win_image_def;
	ObjectDef		*obj_def;

	PaletteBoxData		palette_data;

	ObjectDef		*obj_name;
	ObjectDef		*obj_image;

	ULONG			command_list;

	IPCData			*main_owner;

	ULONG			flags;
} ButtonEdStartup;

#define BUTTONEDF_NO_ADD	(1<<0)

typedef struct _ButtonEdData
{
	NewConfigWindow		newwin;

	IPCData			*ipc;
	struct Window		*window;	// Window
	ObjectList		*objlist;	// List of objects

	Cfg_Button		*button;	// Button being edited
	short			which;		// Function being editor (left/right/etc)
	GL_Object		*display;	// Button display area
	struct Rectangle	button_rect;	// Button display rectangle
	ButtonEdStartup		*startup;

	Att_List		*func_list;

	struct ListLock		proc_list;

	struct AppWindow	*appwindow;
	struct MsgPort		*app_port;

	CfgDragInfo		drag;

	GL_Object		*lister;
	ULONG			last_sec,last_mic;
	short			change;
	struct MinList		editors;

	short			change_bit;

	struct IBox		display_box;

	ImageRemap		remap;
} ButtonEdData;

extern ConfigWindow
	_button_editor_window;
extern ObjectDef
	_button_editor_objects[],
	_button_editor_image_objects[],
	_button_editor_label_objects[];

enum
{
	FUNCTION_LEFT,
	FUNCTION_RIGHT,
	FUNCTION_MIDDLE,
};

#endif

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

#include "config_lib.h"
#include "config_filetypes.h"
#include "//Program/main_commands.h"

// Edit a filetype (library function)

Cfg_Filetype *__asm __saveds L_EditFiletype(
	register __a0 Cfg_Filetype *type,
	register __a1 struct Window *window,
	register __a2 IPCData *ipc,
	register __a3 IPCData *galileo_ipc,
	register __d0 ULONG flags)
{
	filetype_ed_data *data;
	IPCData *editor;
	IPCMessage *imsg;
	Cfg_Filetype *ret_type=0;

	// Valid filetype?
	if (!type) return 0;

	// Allocate data for the editor, copy filetype
	if (!(data=AllocVec(sizeof(filetype_ed_data),MEMF_CLEAR)) ||
		!(data->type=CopyFiletype(type,0)))
	{
		FreeVec(data);
		return 0;
	}

	// Fill out data
	data->owner_ipc=ipc;
	data->action_lookup=filetype_action_lookup;

	// Supply libraries
	data->func_startup.galileofm_base=GalileoFMBase;
	data->func_startup.dos_base=(struct Library *)DOSBase;
	data->func_startup.int_base=(struct Library *)IntuitionBase;
	data->func_startup.util_base=UtilityBase;
	data->func_startup.cx_base=CxBase;
	data->func_startup.wb_base=WorkbenchBase;
	data->func_startup.gfx_base=(struct Library *)GfxBase;
	data->func_startup.asl_base=AslBase;
	data->func_startup.layers_base=LayersBase;

#if RESOURCE_TRACKING
    data->func_startup.restrack_base=ResTrackBase;
#endif

	// Supply a4
	data->func_startup.a4=getreg(REG_A4);

	// Supply locale
	data->func_startup.locale=locale;

	// Supply gui data pointers
	data->new_win.parent=window;
	data->new_win.dims=&_filetype_editor_window;
	data->new_win.locale=locale;
	data->obj_def=_filetype_editor_objects;

	// Supply data pointers for function editor
	data->func_startup.win_def=&_function_editor_window;
	data->func_startup.obj_def=_function_editor_objects;
	data->func_startup.func_labels=_function_type_labels;
	data->func_startup.flag_list=_funced_flaglist;
	data->func_startup.func_list=IPC_Command(galileo_ipc,MAINCMD_GET_LIST,0,0,0,REPLY_NO_PORT);
	data->func_startup.flags=FUNCEDF_NO_KEY;
	data->func_startup.main_owner=ipc;

	// Supply data pointers for the fileclass editor
	data->class_win=&_fileclass_editor_window;
	data->class_obj=_fileclass_editor_objects;
	data->class_lookup=fileclass_match_lookup;
	data->class_strings=matchtype_labels;

	// Open class editor automatically?
	if (flags&EFTF_EDIT_CLASS) data->edit_flag=1;

	// Launch editor
	if (!(IPC_Launch(
		0,
		&editor,
		"galileo_filetype_editor",
		(ULONG)FiletypeEditor,
		STACK_DEFAULT,
		(ULONG)data,
		(struct Library *)DOSBase)) || !editor)
	{
		// Failed; free data
		FreeFiletype(data->type);
		FreeVec(data);
		return 0;
	}

	// Event loop
	FOREVER
	{
		BOOL quit=0;

		// IPC messages
		while (imsg=(IPCMessage *)GetMsg(ipc->command_port))
		{
			// Filetype returned?
			if (imsg->command==FILETYPEEDIT_RETURN)
			{
				// Copy the filetype
				ret_type=CopyFiletype((Cfg_Filetype *)imsg->flags,0);
			}

			// Said goodbye?
			else
			if (imsg->command==IPC_GOODBYE && imsg->data==editor)
				quit=1;

			// Abort?
			else
			if (imsg->command==IPC_QUIT)
			{
				// Send abort message
				IPC_Quit(editor,imsg->flags,TRUE);
			}

			// Activate?
			else
			if (imsg->command==IPC_ACTIVATE)
			{
				// Send message
				IPC_Command(editor,IPC_ACTIVATE,imsg->flags,imsg->data,0,0);
			}

			// Identify?
			else
			if (imsg->command==IPC_IDENTIFY)
			{
				// Pass to Galileo
				IPC_Command(galileo_ipc,IPC_IDENTIFY,imsg->flags,imsg->data,0,REPLY_NO_PORT);
			}

			// Reply to message
			IPC_Reply(imsg);
		}

		// Check quit flag
		if (quit) break;

		// Wait for message
		WaitPort(ipc->command_port);
	}

	// Return new filetype (if any)
	return ret_type;
}

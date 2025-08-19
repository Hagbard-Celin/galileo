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

Cfg_Function *__asm __saveds L_Config_EditFunction(
	register __a0 IPCData *ipc,
	register __a1 IPCData *galileo_ipc,
	register __a2 struct Window *window,
	register __a3 Cfg_Function *function,
	register __a4 APTR memory,
	register __d0 ULONG command_list)
{
	FunctionStartup *startup;
	Cfg_Function *edit_func;
	IPCData *editor=0;
	IPCMessage *msg;

	// Allocate startup data
	if (!(startup=AllocVec(sizeof(FunctionStartup),MEMF_CLEAR)))
		return 0;

	// Copy function (if it is supplied)
	if (function)
		edit_func=CopyFunction(function,0,0);

	// Otherwise, allocate a new function
	else
		edit_func=NewFunction(0,0);

	// Invalid function to edit?
	if (!edit_func)
	{
		FreeVec(startup);
		return 0;
	}

	// Initialise startup data
	function_editor_init(startup,command_list);
	startup->window=window;
	startup->function=edit_func;
	startup->owner_ipc=ipc;
	startup->main_owner=galileo_ipc;
	startup->flags|=FUNCEDF_CENTER|FUNCEDF_SAVE;

	// Launch editor
	if (!(IPC_Launch(
		0,
		&editor,
		"galileo_function_editor",
		(ULONG)FunctionEditor,
		STACK_DEFAULT,
		(ULONG)startup)) || !editor)
	{
		// Failed
		FreeVec(startup);
		FreeFunction(edit_func);
		return 0;
	}

	// Clear function pointer
	function=0;

	// Wait on message port
	FOREVER
	{
		BOOL quit_flag=0;
	
		// Get messages
		while (msg=(IPCMessage *)GetMsg(ipc->command_port))
		{
			// Command comes back
			if (msg->command==FUNCTIONEDIT_RETURN)
			{
				FunctionReturn *ret;

				// Get return packet
				ret=(FunctionReturn *)msg->data;

				// Copy returned function
				function=CopyFunction(ret->function,memory,0);
			}

			// Function editor returned
			else
			if (msg->command==IPC_GOODBYE)
			{
				// Editor?
				if (msg->data==(IPCData *)editor) quit_flag=1;
			}

			// Quit/hide/activate/show - pass on
			else
			if (msg->command==IPC_QUIT ||
				msg->command==IPC_HIDE ||
				msg->command==IPC_SHOW ||
				msg->command==IPC_ACTIVATE)
			{
				// Pass through
				IPC_Command(editor,msg->command,msg->flags,msg->data,0,0);
			}

			// Reply to message
			IPC_Reply(msg);
		}

		// Check quit flag
		if (quit_flag) break;

		// Wait for message
		Wait(1<<ipc->command_port->mp_SigBit);
	}

	// Return the new function
	return function;
}

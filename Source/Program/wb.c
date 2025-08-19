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

#include "galileofm.h"
#include "misc_protos.h"
#include "icons.h"
#include "menu_data.h"
#include "clock_task.h"
#include "wb.h"
#include "lsprintf_protos.h"
#include "/Modules/modules.h"

// Run Workbench startup programs
void wb_do_startup(void)
{
	BPTR olddir,lock,dirlock;
	struct FileInfoBlock __aligned fib,fib2;
	char *name,*name_ptr;
	Att_List *launch_list;
	Att_Node *node;

	// Create list
	if (!(launch_list=Att_NewList(0)))
		return;

	// Change to wbstartup directory
	if (!(lock=Lock("sys:wbstartup",ACCESS_READ)))
	{
		Att_RemList(launch_list,0);
		return;
	}
	dirlock=DupLock(lock);
	olddir=CurrentDir(lock);

	// Allocate buffer
	if (!(name=AllocVec(256,0)))
	{
		UnLock(lock);
		Att_RemList(launch_list,0);
		return;
	}

	// Set busy pointer in main window
	if (GUI->window) SetBusyPointer(GUI->window);
	name[0]='\"';
	name_ptr=name+1;

	// Search dir
	Examine(dirlock,&fib);
	while (ExNext(dirlock,&fib))
	{
		BOOL ok=1;

		// Is file an icon?
		if (isicon(fib.fib_FileName))
		{
			struct DiskObject *icon;

			// Copy name, strip .info
			strcpy(name_ptr,fib.fib_FileName);
			name_ptr[strlen(name_ptr)-5]=0;

			// Check it's not Galileo
			if (stricmp(name_ptr,"galileo")==0)
				continue;

			// Lock file
			if (lock=Lock(name_ptr,ACCESS_READ))
			{
				// Examine file
				Examine(lock,&fib2);
				UnLock(lock);

				// Is this a directory?
				if (fib2.fib_DirEntryType>0) ok=0;
			}

			// Ok to run?
			if (ok)
			{
				// Get icon
				if (icon=GetCachedDiskObject(name_ptr,0))
				{
					short pri=0;
					char *ptr;

					// Check it doesn't launch Galileo
					if (icon->do_Type!=WBPROJECT ||
						!icon->do_DefaultTool ||
						stricmp(FilePart(icon->do_DefaultTool),"galileo")!=0)
					{
						// Get priority
						if (ptr=FindToolType(icon->do_ToolTypes,"STARTPRI"))
							pri=atoi(ptr);
						if (pri<-128) pri=-128;
						else if (pri>127) pri=127;

						// Add quote to end
						strcat(name_ptr,"\"");

						// Add to launch list
						if (node=Att_NewNode(launch_list,name,pri,ADDNODE_PRI))
						{
							// Wait delay?
							if (ptr=FindToolType(icon->do_ToolTypes,"WAIT"))
							{
								node->node.ln_Type=atoi(ptr);
								node->node.ln_Type&=~LAUNCH_DONOTWAIT;
							}

							// Do not wait?
							if (ptr=FindToolType(icon->do_ToolTypes,"DONOTWAIT"))
								node->node.ln_Type|=LAUNCH_DONOTWAIT;
						}
					}

					// Free icon
					FreeCachedDiskObject(icon);
				}
			}
		}
	}

	// Launch workbench launcher
	if (!(misc_startup(
		"galileo_wbstartup",
		WORKBENCH_STARTUP,
		0,
		launch_list,1)))
	{
		// Failed
		Att_RemList(launch_list,0);
	}

	// Cleanup
	UnLock(dirlock);
	UnLock(CurrentDir(olddir));
	FreeVec(name);

	// Clear busy pointer in main window
	if (GUI->window) ClearPointer(GUI->window);
}


// Launch programs from list
void wb_launch_list(IPCData *ipc,Att_List *launch_list)
{
	Att_Node *launch;

	// Go through list
	for (launch=(Att_Node *)launch_list->list.lh_Head;
		launch->node.ln_Succ;
		launch=(Att_Node *)launch->node.ln_Succ)
	{
		IPCMessage *msg;
		BOOL quit=0;

		// IPC messages?
		while (msg=(IPCMessage *)GetMsg(ipc->command_port))
		{
			// Quit?
			if (msg->command==IPC_QUIT) quit=1;
			IPC_Reply(msg);
		}

		// Quit?
		if (quit) break;

		// Set screen title (if screen is open)
		if (GUI->window)
		{
			char *buf;

			if (buf=AllocVec(384,0))
			{
				strcpy(buf,launch->node.ln_Name+1);
				buf[strlen(buf)-1]=0;
				lsprintf(buf+256,GetString(&locale,MSG_LAUNCHING_PROGRAM),buf);
				title_error(buf+256,0);
				FreeVec(buf);
			}
		}

		// Launch program
		if (WB_Launch(launch->node.ln_Name,
			      (struct Screen *)-1,
			      (launch->node.ln_Type&LAUNCH_DONOTWAIT)?LAUNCH_REPLY:LAUNCH_WAIT_TIMEOUT,
			      environment->env->default_stack,
			      0,0,0,0))
		{
			// Delay?
			if (LAUNCH_GETWAIT(launch->node.ln_Type))
			{
				// Do a delay
				Delay(LAUNCH_GETWAIT(launch->node.ln_Type)*50);
			}
		}
	}

	// Free list
	Att_RemList(launch_list,0);
}

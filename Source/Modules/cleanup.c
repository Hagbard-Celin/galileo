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

#include "cleanup.h"
#include "/Program/position.h"
#include "/Program/main_commands.h"

char *version="$VER: cleanup.gfmmodule 0.1 "__AMIGADATE__" ";

int __asm __saveds L_Module_Entry(
	register __a0 struct List *files,
	register __a1 struct Screen *screen,
	register __a2 IPCData *ipc,
	register __a3 IPCData *main_ipc,
    register __a4 APTR *memhandlep,
	register __d0 ULONG mod_id,
	register __d1 EXT_FUNC(func_callback))
{
	struct Window *window;
	APTR memory;
	BOOL change=0;

	// Open status window
	window=OpenStatusWindow(
		GetString(locale,MSG_CLEANUP_TITLE),
		GetString(locale,MSG_PLEASE_WAIT),
		screen,
		0,
		WINDOW_NO_CLOSE|WINDOW_NO_ACTIVATE);

	// Tell Galileo to save positions out
	IPC_Command(main_ipc,MAINCMD_SAVE_POSITIONS,0,0,0,REPLY_NO_PORT);

	// Allocate memory handle
	if (memory=NewMemHandle(4096,1024,MEMF_CLEAR))
	{
		APTR iff;

		// Open position info file
		if (iff=IFFOpen("PROGDIR:system/position-info",IFF_READ,ID_GILO))
		{
			ULONG id;
			BOOL ok=1;
			char name[40],*ptr;
			struct DosList *dos;
			struct List list;

			// Initialise list
			NewList(&list);

			// Lock dos list
			dos=LockDosList(LDF_VOLUMES|LDF_DEVICES|LDF_READ);

			// Read file
			while (id=IFFNextChunk(iff,0))
			{
				struct Node *entry;

				// Valid chunk?
				if (id!=ID_POSI && id!=ID_LOUT && id!=ID_ICON && id!=ID_STRT)
					continue;

				// Allocate record
				if (!(entry=AllocMemH(memory,IFFChunkSize(iff)+sizeof(struct Node))))
				{
					ok=0;
					break;
				}

				// Read chunk data
				IFFReadChunkBytes(iff,(APTR)(entry+1),-1);

				// Set type and name pointer
				if (id==ID_POSI)
				{
					entry->ln_Type=PTYPE_POSITION;
					entry->ln_Name=((position_rec *)entry)->name;
				}
				else
				if (id==ID_LOUT)
				{
					entry->ln_Type=PTYPE_LEFTOUT;
					entry->ln_Name=((leftout_record *)entry)->name;
				}
				else
				if (id==ID_ICON)
				{
					entry->ln_Type=PTYPE_APPICON;
					entry->ln_Name=((leftout_record *)entry)->name;
				}

				// Only check if a position or leftout
				if (id==ID_POSI || id==ID_LOUT)
				{
					// Get device name
					stccpy(name,entry->ln_Name,39);
					if (ptr=strchr(name,':')) *ptr=0;

					// Find in dos list
					if (FindDosEntry(dos,name,(id==ID_POSI)?LDF_VOLUMES:LDF_DEVICES))
					{
						BPTR lock;

						// Try to lock thing
						if (!(lock=Lock(entry->ln_Name,ACCESS_READ)))
						{
							// Failed to find, free node
							FreeMemH(entry);
							entry=0;
							change=1;
						}
						else UnLock(lock);
					}
				}
	
				// Add to list
				if (entry) AddTail(&list,entry);
			}

			// Unlock DOS list
			UnLockDosList(LDF_VOLUMES|LDF_DEVICES|LDF_READ);

			// Close file
			IFFClose(iff);

			// No changes?
			if (!change) ok=0;

			// Re-open file
			if (ok && (iff=IFFOpen("PROGDIR:system/position-info",IFF_WRITE|IFF_SAFE,ID_GILO)))
			{
				struct Node *node;

				// Go through entries
				for (node=list.lh_Head;node->ln_Succ;node=node->ln_Succ)
				{
					// Valid path, or AppIcon?
					if ((node->ln_Name && *node->ln_Name) ||
						node->ln_Type==PTYPE_APPICON)
					{
						ULONG id=0;
						short size=0;

						// Position?
						if (node->ln_Type==PTYPE_POSITION)
						{
							// Get id and chunk size
							id=ID_POSI;
							size=sizeof(position_rec)-sizeof(struct Node);
						}

						// Left-out?
						else
						if (node->ln_Type==PTYPE_LEFTOUT)
						{
							// Get id and chunk size
							id=ID_LOUT;
							size=sizeof(leftout_record)-sizeof(struct Node);
						}

						// AppIcon?
						else
						if (node->ln_Type==PTYPE_APPICON)
						{
							// Get id and chunk size
							id=ID_ICON;
							size=sizeof(leftout_record)-sizeof(struct Node);
						}

						// Write chunk
						if (!(IFFWriteChunk(iff,(APTR)(node+1),id,size+((node->ln_Name)?strlen(node->ln_Name):0))))
							break;
					}
				}

				// Close file
				IFFClose(iff);
			}
		}

		// Free memory
		FreeMemHandle(memory);
	}

	// Tell Galileo to get new positions out
	if (change) IPC_Command(main_ipc,MAINCMD_LOAD_POSITIONS,0,0,0,0);

	// Close status window
	if (window) CloseConfigWindow(window);
	return 1;
}

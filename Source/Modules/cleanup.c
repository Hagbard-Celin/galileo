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

#include "cleanup.h"
#include "/Program/position.h"
#include "/Program/main_commands.h"

char __far _LibID[] = "cleanup.gfmmodule "__VERSTR__" "__AMIGADATE__" "__SUBTITLE__;

int __asm __saveds L_Module_Entry(
	register __a0 char *args,
	register __a1 struct Screen *screen,
	register __a2 IPCData *ipc,
	register __a3 IPCData *main_ipc,
	register __d0 ULONG mod_id,
	register __d1 CONST GalileoCallbackInfo *gci)
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
				ULONG size;

				// Valid chunk?
				if (id!=ID_POSI && id!=ID_LOUT && id!=ID_LFTO  && id!=ID_ICON && id!=ID_STRT)
					continue;

				size = IFFChunkSize(iff) + sizeof(struct Node);

				// Allocate record
				if (!(entry=AllocMemH(memory,size)))
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
					entry->ln_Name=((leftout_record_old *)entry)->name;
				}
				else
				if (id==ID_LFTO)
				{
					entry->ln_Type=PTYPE_LEFTOUT;
				        entry->ln_Pri = 1;
					entry->ln_Name=((leftout_record *)entry)->name;
				}
				else
				if (id==ID_ICON)
				{
					entry->ln_Type=PTYPE_APPICON;
					entry->ln_Name=((appicon_record *)entry)->name;
				}

				// Only check if a leftout
				if (id==ID_LFTO)
				{
				    BPTR volume_lock;

				    // Try to lock volume root
				    if (volume_lock = LockFromVolIdPath(entry->ln_Name, NULL,
									&((leftout_record *)entry)->vol_date,
									((leftout_record *)entry)->vol_name_len, NULL))
				    {
					BPTR lock, org_dir = CurrentDir(volume_lock);

					// Get path relative to volume root
					ptr = entry->ln_Name + ((leftout_record *)entry)->vol_name_len + 1;

					// Try to lock thing
					if (!(lock = LockFromPath(ptr, NULL, NULL)))
					{
						// Failed to find, free node
						FreeMemH(entry);
						entry=0;
						change=1;
					}
					else UnLock(lock);


					UnLock(CurrentDir(org_dir));
				    }
				}
				// ..or position
				else
				if (id==ID_POSI)
				{
					// Get device name
					stccpy(name,entry->ln_Name,39);
					if (ptr=strchr(name,':')) *ptr=0;

					// Find in dos list
					if (FindDosEntry(dos,name,LDF_VOLUMES))
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
						    if (node->ln_Pri == 1)
						    {
							// Get id and chunk size
							id=ID_LFTO;
							size=sizeof(leftout_record)-sizeof(struct Node);
						    }
						    else
						    {
							// Get id and chunk size
							id=ID_LOUT;
							size=sizeof(leftout_record_old)-sizeof(struct Node);
						    }
						}

						// AppIcon?
						else
						if (node->ln_Type==PTYPE_APPICON)
						{
							// Get id and chunk size
							id=ID_ICON;
							size=sizeof(appicon_record)-sizeof(struct Node);
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

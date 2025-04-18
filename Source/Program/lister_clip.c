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

Opus� and Directory Opus� and associated images are registered trademarks of GPSoftware.
DOpus(TM), DirOpus(TM) and Opus File Manager(TM) are trademarked 1991 by GPSoftware.
Opus Magellan (TM) is trademarked 1996 GPSoftware.
All other trademarks are the property of their respective owners.

The release of Directory Opus 5 under the GPL in NO WAY affects
the existing commercial status of Directory Opus for Windows.

For more information on Directory Opus for Windows please see:

		 http://www.gpsoft.com.au

*/

#include "galileofm.h"

// Copy lister entries to clipboard
void lister_clip_entries(Lister *lister,unsigned short qual)
{
	short path_len=0;
	long len=0,num=0;
	char *buffer;

	// Make lister busy
	lister_busy(lister,0);

	// Get path length (unless shift is down, in which case we just do filenames)
	if (!(qual&IEQUAL_ANYSHIFT))
		path_len=strlen(lister->cur_buffer->buf_ExpandedPath)+1;

	// Icon mode?
	if (lister->flags&LISTERF_VIEW_ICONS)
	{
		BackdropObject *icon;

		// Lock icon list
		lock_listlock(&lister->backdrop_info->objects,FALSE);

		// Go through icons
		for (icon=(BackdropObject *)lister->backdrop_info->objects.list.lh_Head;
			icon->bdo_node.ln_Succ;
			icon=(BackdropObject *)icon->bdo_node.ln_Succ)
		{
			// Selected?
			if (icon->bdo_state)
			{
				// Add length to total
				len+=path_len;
				len+=strlen(icon->bdo_name);

				// Add space for linefeed
				if (num++>0)
				{
					++len;
					if (num==1) ++len;
				}
			}
		}
	}

	// Name mode
	else
	{
		DirEntry *entry;

		// Lock buffer
		buffer_lock(lister->cur_buffer,FALSE);

		// Go through buffer
		for (entry=(DirEntry *)lister->cur_buffer->entry_list.mlh_Head;
			entry->de_Node.dn_Succ;
			entry=(DirEntry *)entry->de_Node.dn_Succ)
		{
			// Selected?
			if (entry->de_Flags&ENTF_SELECTED)
			{
				// Add length to total
				len+=path_len;
				len+=strlen(entry->de_Node.dn_Name);

				// Add space for linefeed
				if (num++>0)
				{
					++len;
					if (num==1) ++len;
				}
			}
		}
	}

	// Allocate buffer
	if (len>0 &&
		(buffer=AllocVec(len+2,MEMF_CLEAR)))
	{
		char *ptr;
		struct ClipHandle *clip;

		// Initialise pointer
		ptr=buffer;

		// Icon mode?
		if (lister->flags&LISTERF_VIEW_ICONS)
		{
			BackdropObject *icon;

			// Go through icons
			for (icon=(BackdropObject *)lister->backdrop_info->objects.list.lh_Head;
				icon->bdo_node.ln_Succ;
				icon=(BackdropObject *)icon->bdo_node.ln_Succ)
			{
				// Selected?
				if (icon->bdo_state)
				{
					// Add pathname to buffer (unless shift is down)
					if (!(qual&IEQUAL_ANYSHIFT))
					{
						strcat(ptr,lister->cur_buffer->buf_ExpandedPath);
						AddPart(ptr,icon->bdo_name,256);
					}

					// If shift is down, just add name
					else strcat(ptr,icon->bdo_name);

					// Add linefeed if needed
					if (num>1) strcat(ptr,"\n");

					// Bump pointer
					ptr+=strlen(ptr);
				}
			}
		}

		// Name mode
		else
		{
			DirEntry *entry;

			// Go through buffer
			for (entry=(DirEntry *)lister->cur_buffer->entry_list.mlh_Head;
				entry->de_Node.dn_Succ;
				entry=(DirEntry *)entry->de_Node.dn_Succ)
			{
				// Selected?
				if (entry->de_Flags&ENTF_SELECTED)
				{
					// Add pathname to buffer (unless shift is down)
					if (!(qual&IEQUAL_ANYSHIFT))
					{
						strcat(ptr,lister->cur_buffer->buf_ExpandedPath);
						AddPart(ptr,entry->de_Node.dn_Name,256);
					}

					// If shift is down, just add name
					else strcat(ptr,entry->de_Node.dn_Name);

					// Add linefeed if needed
					if (num>1) strcat(ptr,"\n");

					// Bump pointer
					ptr+=strlen(ptr);
				}
			}
		}

		// Open clipboard
		if (clip=OpenClipBoard(0))
		{
			// Write data to clipboard
			WriteClipString(clip,buffer,strlen(buffer));

			// Close clipboard
			CloseClipBoard(clip);
		}

		// Free buffer
		FreeVec(buffer);
	}

	// Icon mode?
	if (lister->flags&LISTERF_VIEW_ICONS)
	{
		// Unlock icon list
		unlock_listlock(&lister->backdrop_info->objects);
	}

	// Name mode
	else
	{
		// Unlock buffer
		buffer_unlock(lister->cur_buffer);
	}

	// Make lister unbusy
	lister_unbusy(lister,0);
}

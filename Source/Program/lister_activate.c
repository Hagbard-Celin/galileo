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
#include "lister_protos.h"
#include "misc_protos.h"

// Get the current source lister
Lister *lister_source(void)
{
	return lister_default(LISTERF_SOURCE,0);
}


// Get the current destination lister
Lister *lister_dest(void)
{
	return lister_default(LISTERF_DEST,0);
}


// Get the default lister
Lister *lister_default(ULONG flags,BOOL valid)
{
	Lister *lister,*source=0,*first=0;
	IPCData *ipc;

	// Lock lister list
	lock_listlock(&GUI->lister_list,FALSE);

	// Go through list
	for (ipc=(IPCData *)GUI->lister_list.list.lh_Head;
		ipc->node.mln_Succ;
		ipc=(IPCData *)ipc->node.mln_Succ)
	{
		// Get lister
		lister=IPCDATA(ipc);

		// Is this lister the right type and not busy
		if (lister->flags&flags && !(lister->flags&LISTERF_LOCK))
		{
			// Does lister need to be valid?
			if (!valid ||
				(lister->cur_buffer->buf_Path && lister->cur_buffer->buf_Path[0]) ||
				lister->cur_buffer->buf_CustomHandler[0])
			{
				// Is this the first lister we find?
				if (!first) first=lister;

				// Is lister not locked?
				if (!(lister->flags&LISTERF_SOURCEDEST_LOCK))
				{
					source=lister;
					break;
				}
			}
		}
	}

/* ACTION_CHANGE */
	// Haven't got a source?
	if (!source)
	{
		// Did we want a source?
		if (flags&LISTERF_SOURCE)
		{
			Lister *current=GUI->current_lister;

			// Is the current lister in icon mode?
			if (current &&
				current->flags&LISTERF_VIEW_ICONS &&
				!(current->flags&LISTERF_ICON_ACTION))
			{
				// Use this one
				source=current;
			}
		}
	}

	// Unlock lister list
	unlock_listlock(&GUI->lister_list);

	// Did we get one?
	if (source) return source;

	// Did we find anything at all?
	if (first) return first;

	return 0;
}


// Make sure there are no other sources except this one
void lister_check_source(Lister *source)
{
	Lister *lister;
	IPCData *ipc;

	// If lister is locked as source, it's ok
	if (source->flags&LISTERF_SOURCEDEST_LOCK)
		return;

	// Lock lister list
	lock_listlock(&GUI->lister_list,FALSE);

	// Go through list
	for (ipc=(IPCData *)GUI->lister_list.list.lh_Head;
		ipc->node.mln_Succ;
		ipc=(IPCData *)ipc->node.mln_Succ)
	{
		// Get lister
		lister=IPCDATA(ipc);

		// Not our lister?
		if (lister!=source)
		{
			// Is this lister an unlocked source?
			if (lister->flags&LISTERF_SOURCE && !(lister->flags&LISTERF_SOURCEDEST_LOCK))
			{
				// Make it a destination
				IPC_Command(lister->ipc,LISTER_MAKE_DEST,0,0,0,0);
			}
		}
	}

	// Unlock lister list
	unlock_listlock(&GUI->lister_list);
}


// Make sure there are no other destinations except this one
void lister_check_dest(Lister *dest)
{
	Lister *lister;
	IPCData *ipc;

	// If lister is locked as dest, it's ok
	if (dest->flags&LISTERF_SOURCEDEST_LOCK)
		return;

	// Lock lister list
	lock_listlock(&GUI->lister_list,FALSE);

	// Go through list
	for (ipc=(IPCData *)GUI->lister_list.list.lh_Head;
		ipc->node.mln_Succ;
		ipc=(IPCData *)ipc->node.mln_Succ)
	{
		// Get lister
		lister=IPCDATA(ipc);

		// Not our lister?
		if (lister!=dest)
		{
			// Is this lister an unlocked destination?
			if (lister->flags&LISTERF_DEST && !(lister->flags&LISTERF_SOURCEDEST_LOCK))
			{
				// Make it a destination
				IPC_Command(lister->ipc,LISTER_OFF,0,0,0,0);
			}
		}
	}

	// Unlock lister list
	unlock_listlock(&GUI->lister_list);
}


// Split the display with another lister
void lister_split_display(Lister *lister,Lister *other_lister)
{
	struct IBox dims,other_dims;

	// Get list lock
	lock_listlock(&GUI->lister_list,FALSE);

	// Get other lister's dimensions
	if (lister_valid_window(other_lister))
	{
		other_dims=*((struct IBox *)&other_lister->window->LeftEdge);
	}
	else other_dims=other_lister->pos;

	// Horizontal split?
	if (other_dims.Width>other_dims.Height)
	{
		dims.Width=other_dims.Width/2;
		other_dims.Width-=dims.Width;
		dims.Left=other_dims.Left+other_dims.Width;
		dims.Top=other_dims.Top;
		dims.Height=other_dims.Height;
	}

	// Vertical split
	else
	{
		dims.Height=other_dims.Height/2;
		other_dims.Height-=dims.Height;
		dims.Left=other_dims.Left;
		dims.Top=other_dims.Top+other_dims.Height;
		dims.Width=other_dims.Width;
	}

	// Set other lister's dimensions
	if (lister_valid_window(other_lister))
	{
		ChangeWindowBox(
			other_lister->window,
			other_dims.Left,
			other_dims.Top,
			other_dims.Width,
			other_dims.Height);
	}
	else other_lister->pos=other_dims;

	// Set this lister's dimensions
	if (lister_valid_window(lister))
	{
		ChangeWindowBox(
			lister->window,
			dims.Left,
			dims.Top,
			dims.Width,
			dims.Height);
	}
	else
	{
		lister->lister->lister.pos[0]=dims;
		lister->dimensions.wd_Normal=dims;
	}

	// Unlock list
	unlock_listlock(&GUI->lister_list);
}

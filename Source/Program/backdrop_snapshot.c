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

#include "galileofm.h"
#include <gfm/config_display.h>
#include "lister_protos.h"
#include "misc_protos.h"
#include "backdrop_protos.h"
#include "groups.h"
#include "rexx_protos.h"
#include "position_protos.h"

// Snapshot objects
void backdrop_snapshot(BackdropInfo *info,BOOL unsnapshot,BOOL all,BackdropObject *icon)
{
	BackdropObject *object;
	Lister *lister;
	IPCData *ipc;
	BPTR old,lock;
	short count=0,beep=0;
#ifdef DISTINCT_OK
	BOOL save_env=0;
#endif
	BOOL savepos=0;

	// Lock backdrop list
	lock_listlock(&info->objects,0);

	// Lock lister list
	lock_listlock(&GUI->lister_list,0);

	// Lock group list
	lock_listlock(&GUI->group_list,0);

	// Go through backdrop list
	for (object=(BackdropObject *)info->objects.list.lh_Head;
		object->bdo_node.ln_Succ;
		object=(BackdropObject *)object->bdo_node.ln_Succ)
	{
		BOOL ok=0;

		// Icon supplied?
		if (icon)
		{
			// Does it match?
			if (object!=icon || !object->bdo_icon) continue;
		}

		// Is object selected?
		else
		if ((!object->bdo_state && !all) || !object->bdo_icon) continue;
		beep=1;

		// Appicon?
		if (object->bdo_type==BDO_APP_ICON)
		{
			// Does icon support snapshot itself?
			if (WB_AppIconFlags((struct AppIcon *)object->bdo_misc_data)&APPENTF_SNAPSHOT)
			{
				backdrop_appicon_message(object,(unsnapshot)?BAPPF_UNSNAPSHOT:0);
				++count;
			}

			// Otherwise, fake snapshot
			else
			if (*object->bdo_name) ok=1;
		}

		// Otherwise, is it ok to snapshot?
		else
		if (object->bdo_type!=BDO_BAD_DISK) ok=1;

		// Ok to snapshot?
		if (ok)
		{
			short x,y;

			// UnShapshot?
			if (unsnapshot)
			{
				// Set "no position" position
				x=-1;
				y=-1;
			}

			// Snapshot
			else
			{
				// Drawer?
				if (object->bdo_icon->do_DrawerData)
				{
					// Group icon?
					if (object->bdo_type==BDO_GROUP)
					{
						GroupData *group;

						// See if group is open
						if (group=backdrop_find_group(object))
						{
							// Update window position
							*((struct IBox *)&object->bdo_icon->do_DrawerData->dd_NewWindow.LeftEdge)=group->dimensions;
						}
					}

					// Otherwise go through listers
					else
					for (ipc=(IPCData *)GUI->lister_list.list.lh_Head;
						ipc->node.mln_Succ;
						ipc=(IPCData *)ipc->node.mln_Succ)
					{
						// Get lister
						lister=IPCDATA(ipc);

						// Is lister from this icon?
						if (lister->backdrop==object)
						{
							// Update lister
							PositionUpdate(lister,POSUPF_SAVE);
							break;
						}
					}
				}

				// Get position
				x=object->bdo_pos.Left;
				y=object->bdo_pos.Top;
			}

			// AppIcon?
			if (object->bdo_type==BDO_APP_ICON)
			{
				appicon_record *record;

				// Lock position list
				lock_listlock(&GUI->positions,TRUE);

				// Look for entry for icon
				for (record=(appicon_record *)&GUI->positions.list.lh_Head;
					record->node.ln_Succ;
					record=(appicon_record *)record->node.ln_Succ)
				{
					// Leftout?
					if (record->node.ln_Type==PTYPE_APPICON)
					{
						// Match this icon?
						if (strcmp(record->icon_label,object->bdo_name)==0)
							break;
					}
				}

				// Didn't find one?
				if (!(record->node.ln_Succ) && !unsnapshot)
				{
					// Create a new entry
					if (record=AllocMemH(GUI->position_memory,sizeof(appicon_record)))
					{
						// Fill out entry
						stccpy(record->icon_label,object->bdo_name,sizeof(record->icon_label));
						record->node.ln_Name=record->name;
						record->node.ln_Type=PTYPE_APPICON;

						// Add to list
						AddTail((struct List *)&GUI->positions,(struct Node *)record);
					}
				}

				// Got one?
				if (record->node.ln_Succ)
				{
					// Unsnapshot?
					if (unsnapshot)
					{
						// Remove entry and free it
						Remove((struct Node *)record);
						FreeMemH(record);
					}

					// Update entry
					else
					{
						ULONG iflags;

						// Adjust for borders?
						if (!((iflags=GetIconFlags(object->bdo_icon))&ICONF_BORDER_OFF) &&
							(!(environment->env->desktop_flags&DESKTOPF_NO_BORDERS) || (iflags&ICONF_BORDER_ON)))
						{
							// Shift back by border size
							x-=ICON_BORDER_X;
							y-=ICON_BORDER_Y_TOP;
						}

						// Save position
						record->icon_x=x;
						record->icon_y=y;
						record->flags&=~LEFTOUTF_NO_POSITION;
					}

					// Set flag to save positions
					savepos=1;
				}

				// Can't snapshot for some reason
				else
				if (!unsnapshot) beep=1;

				// Unlock position list
				unlock_listlock(&GUI->positions);
			}

			// Left-out on desktop?
			else
			if (object->bdo_type==BDO_LEFT_OUT && !(object->bdo_flags&BDOF_DESKTOP_FOLDER) && info->flags&BDIF_MAIN_DESKTOP)
			{
				leftout_record *left;

				// Lock position list
				lock_listlock(&GUI->positions,0);

				// Look for entry for icon
				for (left=(leftout_record *)&GUI->positions.list.lh_Head;
					left->node.ln_Succ;
					left=(leftout_record *)left->node.ln_Succ)
				{
					// Leftout?
					if (left->node.ln_Type==PTYPE_LEFTOUT)
					{
						// Match this icon?
						if (object->bdo_misc_data==(ULONG)left) break;
					}
				}

				// Got one?
				if (left->node.ln_Succ)
				{
					// Save position
					left->icon_x=x;
					left->icon_y=y;
					left->flags&=~LEFTOUTF_NO_POSITION;

					// Set flag to save positions
					savepos=1;
				}

				// Must be temporary
				else beep=1;

				// Unlock position list
				unlock_listlock(&GUI->positions);
			}

#ifdef DISTINCT_OK
			// Distinct icon positions?
			else
			if (environment->env->desktop_flags&DESKTOPF_DISTINCT &&
				info->flags&BDIF_MAIN_DESKTOP)
			{
				// Snapshot icon for this environment
				desktop_snapshot_icon(object,x,y);
				save_env=1;
			}
#endif

			// Group icon?
			else
			if (info->flags&BDIF_GROUP)
			{
				// Snapshot in group
				group_snapshot_icon(info,object,x,y);
			}

			// Save to icon
			else
			{
				// Using Workbench positions?
				if (environment->env->display_options&DISPOPTF_ICON_POS)
				{
					// Set position
					object->bdo_icon->do_CurrentX=(unsnapshot)?NO_ICON_POSITION:x-WBICONMAGIC_X;
					object->bdo_icon->do_CurrentY=(unsnapshot)?NO_ICON_POSITION:y-WBICONMAGIC_Y;

					// Removing Galileo positions?
					if (environment->env->display_options&DISPOPTF_REMGALILEOPOS)
					{
						// Remove Galileo position from icon
						SetIconFlags(object->bdo_icon,GetIconFlags(object->bdo_icon)&~ICONF_POSITION_OK);
					}
				}

				// Using Galileo positions
				else
				{
					// Set "position ok" flag
					SetIconFlags(object->bdo_icon,GetIconFlags(object->bdo_icon)|ICONF_POSITION_OK);

					// Update icon position
					SetIconPosition(object->bdo_icon,x,y);
				}

				// Get icon lock
				if (lock=backdrop_icon_lock(object))
				{
					// Change directory
					old=CurrentDir(lock);

					// Save this object out
					if (!(WriteIcon((object->bdo_icon->do_Type==WBDISK)?"Disk":object->bdo_name,object->bdo_icon)))
						beep=1;

					// Success; is this in a lister?
					else
					if (info->lister)
					{
						char name[256];

						// Get icon name
						strcpy(name,(object->bdo_icon->do_Type==WBDISK)?"Disk":object->bdo_name);
						strcat(name,".info");

						// Reload the file
						rexx_lister_reload_file(info->lister,name);
					}

					// Restore directory
					CurrentDir(old);
					UnLock(lock);
				}
			}

			++count;
		}

		// Icon supplied?
		if (icon) break;
	}

	// Failed to do anything?
	if (!count && beep) DisplayBeep(info->window->WScreen);

	// Unlock group list
	unlock_listlock(&GUI->group_list);

	// Unlock backdrop list
	unlock_listlock(&info->objects);

	// Unlock lister list
	unlock_listlock(&GUI->lister_list);

#ifdef DISTINCT_OK
	// Save environment file?
	if (save_env && environment->env->flags&ENVF_CHANGED)
	{
		// Save environment
		if (environment_save(environment,environment->path,0,0))
			environment->env->flags&=~ENVF_CHANGED;
	}
#endif

	// Save positions?
	if (savepos)
	{
		// Lock list
		lock_listlock(&GUI->positions,0);

		// Save list
		SavePositions(&GUI->positions.list,GUI->position_name);

		// Unlock list
		unlock_listlock(&GUI->positions);
	}

	// If this is a lister, update its datestamp
	if (info->lister) update_buffer_stamp(info->lister);
}

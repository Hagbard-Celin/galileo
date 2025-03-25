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

// Select all objects
void backdrop_select_all(BackdropInfo *info,short state)
{
	BackdropObject *object;
	BOOL draw=0;

	// Lock backdrop list
	lock_listlock(&info->objects,0);

	// Go through backdrop list
	for (object=(BackdropObject *)info->objects.list.lh_Head;
		object->bdo_node.ln_Succ;
		object=(BackdropObject *)object->bdo_node.ln_Succ)
	{
		// Is object not selected?
		if (((state && !object->bdo_state) ||
			(!state && object->bdo_state)) && object->bdo_icon)
		{
			// Select object
			object->bdo_state=state;
			object->bdo_flags|=BDOF_STATE_CHANGE;

/*
			// Add to selection list
			backdrop_selection_list(info,object);
*/
			draw=1;
		}
	}

	// Unlock backdrop list
	unlock_listlock(&info->objects);

	// Fix selection count
	backdrop_fix_count(info,1);

	// Fix menus for a lister
	if (info->lister) lister_fix_menus(info->lister,1);

	// Show objects
	if (draw) backdrop_show_objects(info,0);
}


// Select all objects within an area
void backdrop_select_area(BackdropInfo *info,short state)
{
	BackdropObject *object;

	// Dragging?
	if (GUI->flags2&GUIF2_ICONPOS && info->flags&BDIF_MAIN_DESKTOP) return;

	// Lock backdrop list
	lock_listlock(&info->objects,0);

	// Install clip
	if (info->clip_region)
		InstallClipRegion(info->window->WLayer,info->clip_region);

	// Go through backdrop list
	for (object=(BackdropObject *)info->objects.list.lh_Head;
		object->bdo_node.ln_Succ;
		object=(BackdropObject *)object->bdo_node.ln_Succ)
	{
		// Valid icon?	
		if (object->bdo_icon)
		{
			// Icon temporarily selected?
			if (object->bdo_state==2)
			{
				// Turn it on?
				if (state==1)
				{
					// Select object
					object->bdo_state=1;

					// Is this a tool?
					if (object->bdo_type!=BDO_APP_ICON &&
						object->bdo_icon->do_Type==WBTOOL)
					{
						// If no tools selected, remember this one
						if (!info->first_sel_tool)
							info->first_sel_tool=object;
					}
				}

				// See if it needs to be turned off
				else
				if (state==0 || !(geo_box_intersect(&object->bdo_image_rect,&info->select)))
				{
					// Deselect this object
					object->bdo_state=0;
					object->bdo_flags|=BDOF_STATE_CHANGE;
					backdrop_render_object(info,object,0);
				}
			}

			// Currently off?
			else
			if (!object->bdo_state && state==2)
			{
				// See if icon is in select area
				if (geo_box_intersect(&object->bdo_image_rect,&info->select))
				{
					// Select this object
					object->bdo_state=2;
					object->bdo_flags|=BDOF_STATE_CHANGE;
					backdrop_render_object(info,object,0);
				}
			}
		}
	}

	// Fix selection count
	backdrop_fix_count(info,1);

	// Remove clip 
	if (info->clip_region)
		InstallClipRegion(info->window->WLayer,0);

	// Unlock backdrop list
	unlock_listlock(&info->objects);
}

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
#include "config_environment.h"
#include "//Modules/modules.h"

typedef struct
{
	BPTR	next;
	BPTR	lock;
} PathListEntry;

// Build path list
void config_env_build_pathlist(config_env_data *data,BOOL reset)
{
	BPTR list;

	// No list?
	if (!data->path_list)
		return;
	// Remove from gadget
	SetGadgetChoices(data->option_list,GAD_SETTINGS_PATHLIST,(APTR)~0);

	// Clear the list
	Att_RemList(data->path_list,REMLIST_SAVELIST);

	// Get pathlist
	if (!reset && (list=GetGalileoPathList()))
	{
		PathListEntry *path;

    	// Go through path list
		for (path=(PathListEntry *)BADDR(list);path;path=BADDR(path->next))
		{
			char buf[256];

			// Get path entry
			if (NameFromLock(path->lock,buf,256))
				Att_NewNode(data->path_list,buf,0,0);
		}

		// Free list
		FreeDosPathList(list);
	}
		
	// Flag indicates change
	data->path_list->list.l_pad=1;

	// Add list back to gadget
	SetGadgetChoices(data->option_list,GAD_SETTINGS_PATHLIST,(APTR)data->path_list);
}


// Add a path entry
void config_env_path_add(config_env_data *data)
{
	if (data->path_list)
	{
		char *ptr;

		// Get path
		if ((ptr=(char *)GetGadgetValue(data->option_list,GAD_SETTINGS_PATHLIST_PATH)) && *ptr)
		{
			BPTR lock;
			char buf[256];
			Att_Node *node;
#if 0
			// Get full path
			if (lock=Lock(ptr,ACCESS_READ))
			{
				NameFromLock(lock,buf,256);
				UnLock(lock);
				ptr=buf;
			}
#endif
			// Add to list
			SetGadgetChoices(data->option_list,GAD_SETTINGS_PATHLIST,(APTR)~0);
			node=Att_NewNode(data->path_list,ptr,0,0);
			SetGadgetChoices(data->option_list,GAD_SETTINGS_PATHLIST,data->path_list);

			// Flag indicates a change to the pathlist
			data->path_list->list.l_pad=1;

			// Make last entry visible
			my_SetGadgetAttrs(
				GADGET(GetObject(data->option_list,GAD_SETTINGS_PATHLIST)),
				data->window,
				DLV_MakeVisible,Att_FindNodeNumber(data->path_list,node),
				TAG_END);
		}
		else DisplayBeep(data->window->WScreen);
	}
}


// Path selected
void config_env_path_sel(config_env_data *data)
{
	Att_Node *node;

	// Get node
	node=Att_FindNode(data->path_list,GetGadgetValue(data->option_list,GAD_SETTINGS_PATHLIST));

	// Enable delete gadget
	DisableObject(data->option_list,GAD_SETTINGS_PATHLIST_DELETE,(node)?FALSE:TRUE);

	// Copy string to path gadget
	SetGadgetValue(data->option_list,GAD_SETTINGS_PATHLIST_PATH,(ULONG)((node)?node->node.ln_Name:0));
}


// Delete path
void config_env_path_del(config_env_data *data)
{
	Att_Node *node;

	// Get node
	node=Att_FindNode(data->path_list,GetGadgetValue(data->option_list,GAD_SETTINGS_PATHLIST));

	// Disable delete gadget
	DisableObject(data->option_list,GAD_SETTINGS_PATHLIST_DELETE,TRUE);

	// Clear path gadget and list selection
	SetGadgetValue(data->option_list,GAD_SETTINGS_PATHLIST_PATH,0);
	SetGadgetValue(data->option_list,GAD_SETTINGS_PATHLIST,(ULONG)~0);

	// Remove node from list
	if (node)
	{
		SetGadgetChoices(data->option_list,GAD_SETTINGS_PATHLIST,(APTR)~0);
		Att_RemNode(node);
		SetGadgetChoices(data->option_list,GAD_SETTINGS_PATHLIST,data->path_list);

		// Flag indicates a change to the pathlist
		data->path_list->list.l_pad=1;
	}
}

// Edit path
void config_env_path_edit(config_env_data *data)
{
	Att_Node *node;
	char *ptr;

	// Get node
	if (!(node=Att_FindNode(data->path_list,GetGadgetValue(data->option_list,GAD_SETTINGS_PATHLIST))))
		return;

	// Get path
	if ((ptr=(char *)GetGadgetValue(data->option_list,GAD_SETTINGS_PATHLIST_PATH)) && *ptr)
	{
		BPTR lock;
		char buf[256];
#if 0
		// Get full path
		if (lock=Lock(ptr,ACCESS_READ))
		{
			NameFromLock(lock,buf,256);
			UnLock(lock);
			ptr=buf;
		}
#endif
		// Change node name
		SetGadgetChoices(data->option_list,GAD_SETTINGS_PATHLIST,(APTR)~0);
		Att_ChangeNodeName(node,ptr);
		SetGadgetChoices(data->option_list,GAD_SETTINGS_PATHLIST,data->path_list);

		// Flag indicates a change to the pathlist
		data->path_list->list.l_pad=1;
	}
}


// End drag
void config_env_paths_end_drag(config_env_data *data,BOOL ok)
{
	struct Window *window;
	short item,sel;

	// Not dragging something?
	if (!data->drag.drag) return;

	// End drag
	if (!(window=config_drag_end(&data->drag,ok))) return;
	ok=0;

	// Unlock layer
	UnlockLayerInfo(&data->window->WScreen->LayerInfo);

	// Get drag item
	item=Att_FindNodeNumber(data->path_list,data->drag.drag_node);

	// Dropped on our window?
	if (window==data->window)
	{
		// Get selection we dropped on
		sel=functioned_get_line(
				data->window,
				GetObject(data->objlist,GAD_SETTINGS_PATHLIST),
				data->drag.drag_x,data->drag.drag_y,
				(struct Library *)IntuitionBase);

		// Valid selection?
		if (sel!=item)
		{
			Att_Node *before;

			// Swap the nodes
			SetGadgetChoices(data->option_list,GAD_SETTINGS_PATHLIST,(APTR)~0);
			Remove((struct Node *)data->drag.drag_node);
			if (before=Att_FindNode(data->path_list,sel))
				Insert((struct List *)data->path_list,(struct Node *)data->drag.drag_node,before->node.ln_Pred);
			else
			{
				AddTail((struct List *)data->path_list,(struct Node *)data->drag.drag_node);
				sel=Att_NodeCount(data->path_list)-1;
			}
			SetGadgetChoices(data->option_list,GAD_SETTINGS_PATHLIST,data->path_list);

			// Flag indicates a change to the pathlist
			data->path_list->list.l_pad=1;
			ok=1;

			// Select the one that was dropped
			SetGadgetValue(data->option_list,GAD_SETTINGS_PATHLIST,sel);
			config_env_path_sel(data);
		}
	}

	// Failed?
	if (!ok) DisplayBeep(data->window->WScreen);
}

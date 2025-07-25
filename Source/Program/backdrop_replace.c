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

/*
	Modified GJP 8-11-99 to fix V44 problems.

*/
#include "galileofm.h"
#include <proto/newicon.h>


void backdrop_replace_icon_image(BackdropInfo *info,char *source_name,BackdropObject *dest)
{
    char *buffer;
    BPTR lock;

    // Allocate buffer
    if (!(buffer=AllocVec(1024,MEMF_CLEAR)))
	return;

    // First selected icon?
    if (!source_name)
    {
	BackdropObject *icon;

	// Lock backdrop list
	lock_listlock(&info->objects,0);

	// Go through backdrop list
	for (icon=(BackdropObject *)info->objects.list.lh_Head;
	     icon->bdo_node.ln_Succ;
	     icon=(BackdropObject *)icon->bdo_node.ln_Succ)
	{
	    // Is icon selected?
	    if (icon->bdo_state)
	    {
		// Get icon lock
		if (lock=backdrop_icon_lock(icon))
		{
		    // Build name
		    DevNameFromLock(lock,buffer,512);
		    if (icon->bdo_type==BDO_DISK)
			AddPart(buffer,"Disk",512);
		    else
			AddPart(buffer,icon->bdo_name,512);

		    // Unlock lock
		    UnLock(lock);

		    // Get name pointer
		    source_name=buffer;
		    break;
		}
	    }
	}

	// Unlock backdrop list
	unlock_listlock(&info->objects);
    }

    // Otherwise, check for disk
    else
    if (source_name[strlen(source_name)-1]==':')
    {
	// Build disk icon name
	strcpy(buffer,source_name);
	strcat(buffer,"Disk");
	source_name=buffer;
    }

    // No source, or can't lock destination?
    if (!source_name || !(lock=backdrop_icon_lock(dest)))
    {
	FreeVec(buffer);	
	return;
    }

    // Build destination name
    DevNameFromLock(lock,buffer+512,512);
    if (dest->bdo_type==BDO_DISK)
	AddPart(buffer+512,"Disk",512);
    else
	AddPart(buffer+512,dest->bdo_name,512);

    // Unlock lock
    UnLock(lock);


    if (IconBase->lib_Version>=44)
    {
	struct DiskObject *source;
	struct DiskObject *dest;

	// Get source icon
	source=GetIconTags(source_name,
		           ICONGETA_FailIfUnavailable,TRUE,
		           ICONGETA_RemapIcon,FALSE,
		           TAG_DONE);

	dest=GetIconTags(buffer+512,
		         ICONGETA_FailIfUnavailable,TRUE,
		         ICONGETA_RemapIcon,FALSE,
		         TAG_DONE);

	if (source)
	{
	    // Get destination icon
	    if (dest)
	    {
		struct DiskObject *new;

		// duplicate the source icon to copy images etc
		if (new=DupDiskObject(source,TAG_DONE))
		{
		    // fix new icon with dest details
		    char * do_DefaultTool;
		    char ** do_ToolTypes;
		    struct DrawerData * do_DrawerData;
		    char * do_ToolWindow;

		    //preserve pointer data
		    do_DefaultTool=new->do_DefaultTool;
		    do_ToolTypes=new->do_ToolTypes;
		    do_DrawerData=new->do_DrawerData;
		    do_ToolWindow=new->do_ToolWindow;

		    //update new icon copy
		    new->do_Type=dest->do_Type;
		    new->do_DefaultTool=dest->do_DefaultTool;
		    new->do_ToolTypes=dest->do_ToolTypes;
		    new->do_CurrentX=dest->do_CurrentX;
		    new->do_CurrentY=dest->do_CurrentY;
		    new->do_DrawerData=dest->do_DrawerData;
		    new->do_StackSize=dest->do_StackSize;
		    new->do_ToolWindow=dest->do_ToolWindow;
				    
		    // Save new icon
		    PutDiskObject(buffer+512,new);
				    
		    // recover old data
		    new->do_DefaultTool=do_DefaultTool;
		    new->do_ToolTypes=do_ToolTypes;
		    new->do_DrawerData=do_DrawerData;
		    new->do_ToolWindow=do_ToolWindow;

		    // Free temp icon
		    FreeDiskObject(new);
		}

		// Free destination icon
		FreeDiskObject(dest);
	    }

	    // Couldn't load destination, so just write source as new icon
	    else
		PutDiskObject(buffer+512,source);

	    // Free source icon
	    FreeDiskObject(source);
	}
    }
	
    else
    {
	// Got NewIcons?
	if (NewIconBase)
	{
	    struct NewDiskObject *source;

	    // Get source icon
	    if (source=GetNewDiskObject(source_name))
	    {
		struct NewDiskObject *dest;

		// Get destination icon
		if (dest=GetNewDiskObject(buffer+512))
		{
		    APTR old_GadgetRender,old_SelectRender;
		    UWORD old_Flags;
		    struct ChunkyImage *old_Normal,*old_Selected;

		    // Backup old image settings
		    old_GadgetRender=dest->ndo_StdObject->do_Gadget.GadgetRender;
		    old_SelectRender=dest->ndo_StdObject->do_Gadget.SelectRender;
		    old_Flags=dest->ndo_StdObject->do_Gadget.Flags;
		    old_Normal=dest->ndo_NormalImage;
		    old_Selected=dest->ndo_SelectedImage;

		    // Copy new image settings
		    dest->ndo_StdObject->do_Gadget.GadgetRender=source->ndo_StdObject->do_Gadget.GadgetRender;
		    dest->ndo_StdObject->do_Gadget.SelectRender=source->ndo_StdObject->do_Gadget.SelectRender;
		    dest->ndo_StdObject->do_Gadget.Flags=source->ndo_StdObject->do_Gadget.Flags;
		    dest->ndo_StdObject->do_Gadget.Width=source->ndo_StdObject->do_Gadget.Width;
		    dest->ndo_StdObject->do_Gadget.Height=source->ndo_StdObject->do_Gadget.Height;
		    dest->ndo_NormalImage=source->ndo_NormalImage;
		    dest->ndo_SelectedImage=source->ndo_SelectedImage;

		    // Save new icon
		    PutNewDiskObject(buffer+512,dest);

		    // Restore image settings
		    dest->ndo_StdObject->do_Gadget.GadgetRender=old_GadgetRender;
		    dest->ndo_StdObject->do_Gadget.SelectRender=old_SelectRender;
		    dest->ndo_StdObject->do_Gadget.Flags=old_Flags;
		    dest->ndo_NormalImage=old_Normal;
		    dest->ndo_SelectedImage=old_Selected;

		    // Free destination icon
		    FreeNewDiskObject(dest);
		}

		// Couldn't load destination, so just write source as new icon
		else PutNewDiskObject(buffer+512,source);

		// Free source icon
		FreeNewDiskObject(source);
	    }
	}

	// Standard icon
	else
	{
	    struct DiskObject *source;

	    // Get source icon
	    if (source=GetDiskObject(source_name))
	    {
		struct DiskObject *dest;

		// Get destination icon
		if (dest=GetDiskObject(buffer+512))
		{
		    APTR old_GadgetRender,old_SelectRender;
		    UWORD old_Flags;

		    // Backup old image settings
		    old_GadgetRender=dest->do_Gadget.GadgetRender;
		    old_SelectRender=dest->do_Gadget.SelectRender;
		    old_Flags=dest->do_Gadget.Flags;

		    // Copy new image settings
		    dest->do_Gadget.GadgetRender=source->do_Gadget.GadgetRender;
		    dest->do_Gadget.SelectRender=source->do_Gadget.SelectRender;
		    dest->do_Gadget.Flags=source->do_Gadget.Flags;
		    dest->do_Gadget.Width=source->do_Gadget.Width;
		    dest->do_Gadget.Height=source->do_Gadget.Height;

		    // Save new icon
		    PutDiskObject(buffer+512,dest);

		    // Restore image settings
		    dest->do_Gadget.GadgetRender=old_GadgetRender;
		    dest->do_Gadget.SelectRender=old_SelectRender;
		    dest->do_Gadget.Flags=old_Flags;

		    // Free destination icon
		    FreeDiskObject(dest);
		}

		// Couldn't load destination, so just write source as new icon
		else PutDiskObject(buffer+512,source);

		// Free source icon
		FreeDiskObject(source);
	    }
	}
    }

    // Free buffer
    FreeVec(buffer);
}

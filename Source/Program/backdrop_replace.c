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

/*
	Modified GJP 8-11-99 to fix V44 problems.

*/
#include "galileofm.h"
#include "misc_protos.h"
#include "backdrop_protos.h"
#include <proto/newicon.h>

char disk[] = "Disk";

UBYTE get_deficon_type(char *name);
//static const char *disk = "Disk";

void backdrop_replace_icon_image(BackdropInfo *info,char *source_name,BPTR source_lock,BackdropObject *dest)
{
    char *dest_name;
    BPTR lock = 0, dest_lock, org_dir;

    // First selected icon?
    if (!source_name && !source_lock)
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
		if (lock = backdrop_icon_lock(icon))
		{
		    // Get name
		    if (icon->bdo_type==BDO_DISK)
			source_name = disk;
		    else
			source_name = icon->bdo_name;

		    source_lock = lock;
		}
	    }
	}

	// Unlock backdrop list
	unlock_listlock(&info->objects);
    }

    // No source, or can't lock destination?
    if ((!source_name && !source_lock) || !(dest_lock=backdrop_icon_lock(dest)))
    {
	if (lock)
	    UnLock(lock);

	return;
    }

    if (dest->bdo_type == BDO_DISK)
    {
	dest_name = disk;
    }
    else
	dest_name = dest->bdo_name;

    org_dir = CurrentDir(source_lock);

    if (IconBase->lib_Version>=44)
    {
	struct DiskObject *source;
	struct DiskObject *dest;

	// Get source icon
	source=GetIconTags(source_name,
		           ICONGETA_FailIfUnavailable,TRUE,
		           ICONGETA_RemapIcon,FALSE,
		           TAG_DONE);

	CurrentDir(dest_lock);

	// Get destination icon
	if (!(dest=GetIconTags(dest_name,
		         ICONGETA_FailIfUnavailable,TRUE,
		         ICONGETA_RemapIcon,FALSE,
		         TAG_DONE)))
	{
	    UBYTE type;

	    // Couldn't load destination, so try deficon
	    type = get_deficon_type(dest_name);

	    if (type)
	        dest = GetIconTags(dest_name,
	             ICONGETA_FailIfUnavailable,TRUE,
	             ICONGETA_GetDefaultType,type,
	             ICONGETA_RemapIcon,FALSE,
	             TAG_DONE);
	}

	if (source)
	{
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
		    PutDiskObject(dest_name,new);
				    
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


	        CurrentDir(dest_lock);

		// Get destination icon
		if (!(dest=GetNewDiskObject(dest_name)))
		{
		    UBYTE type;

		    type = get_deficon_type(dest_name);

		    if (type)
			dest = GetDefNewDiskObject(type);
		}

		if (dest)
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
		    PutNewDiskObject(dest_name,dest);

		    // Restore image settings
		    dest->ndo_StdObject->do_Gadget.GadgetRender=old_GadgetRender;
		    dest->ndo_StdObject->do_Gadget.SelectRender=old_SelectRender;
		    dest->ndo_StdObject->do_Gadget.Flags=old_Flags;
		    dest->ndo_NormalImage=old_Normal;
		    dest->ndo_SelectedImage=old_Selected;

		    // Free destination icon
		    FreeNewDiskObject(dest);
		}

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

	        CurrentDir(dest_lock);

		// Get destination icon
		if (!(dest=GetDiskObject(dest_name)))
		{
		    UBYTE type;

		    type = get_deficon_type(dest_name);

		    if (type)
			dest = GetDefDiskObject(type);
		}

		if (dest)
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
		    PutDiskObject(dest_name,dest);

		    // Restore image settings
		    dest->do_Gadget.GadgetRender=old_GadgetRender;
		    dest->do_Gadget.SelectRender=old_SelectRender;
		    dest->do_Gadget.Flags=old_Flags;

		    // Free destination icon
		    FreeDiskObject(dest);
		}

		// Free source icon
		FreeDiskObject(source);
	    }
	}
    }

    if (org_dir)
	CurrentDir(org_dir);

    if (lock)
	UnLock(lock);

    UnLock(dest_lock);
}

// Find deficon type for file
UBYTE get_deficon_type(char *name)
{
    struct FileInfoBlock __aligned fib;
    BPTR lock;
    UBYTE type = 0;

    if (name == disk)
	type = WBDISK;
    else
    if (lock = Lock(name, ACCESS_READ))
    {
	Examine(lock, &fib);

	if (fib.fib_DirEntryType > 0)
	    type = WBDRAWER;
	else
	if (!(fib.fib_Protection&FIBF_EXECUTE) && !(fib.fib_Protection&FIBF_SCRIPT))
	    type = WBTOOL;
	else
	    type = WBPROJECT;

	UnLock(lock);

    }

    return type;
}

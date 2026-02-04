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
#include <gfm/icon_cache_protos.h>
#include <gfm/config_display.h>
#include "misc_protos.h"
#include "icons.h"
#include <proto/newicon.h>



// Write an icon for a file
icon_write(short type,char *filename,BOOL replace_image,ULONG iflags,ULONG imask,short *err)
{
	struct DiskObject *icon=0;
	unsigned long use_flags;
	short suc=ICONWRITE_OK;

	if (err) *err=0;


	// Group icon?
	if (type==ICONTYPE_GROUP)
	{
		// Try to get group icon
		if (!(icon=GetCachedDiskObject("PROGDIR:icons/Group",GCDOF_NOCACHE)))
			icon=GetCachedDefDiskObject(WBDRAWER|GCDOF_NOCACHE);
	}

	// Otherwise
	else
	{
		Cfg_Filetype *ftype;


		// See if this file matches a filetype with an icon
		if (ftype=filetype_identify(filename,NULL,FTTYPE_ICON,0,0))
		{
			char path[256];
			char *ptr;

			// Copy path, strip .info
			strcpy(path,ftype->icon_path);
			if (ptr=strstr(path,".info")) *ptr=0;

			// Get icon
			icon=GetCachedDiskObject(path,GCDOF_NOCACHE);
		}

		// No icon?
		if (!icon)
		{

			// Drawer?
			if (type==ICONTYPE_DRAWER)
			{
				icon=GetCachedDefDiskObject(WBDRAWER|GCDOF_NOCACHE);
			}

			// File
			else
			{
				BPTR file;
				long longword=0;


				// Try to open file
				if (file=Open(filename,MODE_OLDFILE))
				{
					// Read first longword
					Read(file,(char *)&longword,sizeof(longword));
					Close(file);
				}

				// Get default icon (tool/project)
				icon=GetCachedDefDiskObject(((longword==0x3f3)?WBTOOL:WBPROJECT)|GCDOF_NOCACHE);
			}
		}
	}

	// If no icon, fail
	if (!icon) return ICONWRITE_FAILED;

	// Get flags from icon
	use_flags=GetIconFlags(icon);

	// Clear flags in mask
	use_flags&=~imask;

	// Set flags we're given
	use_flags|=iflags|ICONF_POSITION_OK;

	// Remove Galileo position?
	if (environment->env->display_options&DISPOPTF_REMGALILEOPOS)
		use_flags&=~ICONF_POSITION_OK;

	// Set flags in icon
	SetIconFlags(icon,use_flags);

	// Set "no position" position
	SetIconPosition(icon,-1,-1);

	// Using Workbench positions?
	if (environment->env->display_options&DISPOPTF_ICON_POS)
	{
		// Set 'no position'
		icon->do_CurrentX=NO_ICON_POSITION;
		icon->do_CurrentY=NO_ICON_POSITION;
	}

	// Replace image?
	if (replace_image)
	{
		struct NewDiskObject *ndo;
		struct DiskObject *write_icon;
		APTR gadgetrender,selectrender;
		unsigned short flags;


		// NewIcons?
		if (NewIconBase &&
			(ndo=GetNewDiskObject(filename)))
		{
			struct ChunkyImage *old_normal,*old_selected;

			// Save old images
			gadgetrender=ndo->ndo_StdObject->do_Gadget.GadgetRender;
			selectrender=ndo->ndo_StdObject->do_Gadget.SelectRender;
			flags=ndo->ndo_StdObject->do_Gadget.Flags;
			old_normal=ndo->ndo_NormalImage;
			old_selected=ndo->ndo_SelectedImage;

			// Is new image a NewIcon?
			if (GetIconType(icon)==ICON_NEWICON)
			{
				struct NewIconDiskObject *nido;

				// Get NIDO pointer
				nido=(struct NewIconDiskObject *)icon;

				// Set image pointers in new icon
				ndo->ndo_StdObject->do_Gadget.GadgetRender=nido->nido_NewDiskObject->ndo_StdObject->do_Gadget.GadgetRender;
				ndo->ndo_StdObject->do_Gadget.SelectRender=nido->nido_NewDiskObject->ndo_StdObject->do_Gadget.SelectRender;
				ndo->ndo_StdObject->do_Gadget.Flags=nido->nido_NewDiskObject->ndo_StdObject->do_Gadget.Flags;
				ndo->ndo_NormalImage=nido->nido_NewDiskObject->ndo_NormalImage;
				ndo->ndo_SelectedImage=nido->nido_NewDiskObject->ndo_SelectedImage;
			}

			// New image is an OldIcon
			else
			{
				// Set new image pointers
				ndo->ndo_StdObject->do_Gadget.GadgetRender=icon->do_Gadget.GadgetRender;
				ndo->ndo_StdObject->do_Gadget.SelectRender=icon->do_Gadget.SelectRender;
				ndo->ndo_StdObject->do_Gadget.Flags=icon->do_Gadget.Flags;
			}

			// Set new size
			ndo->ndo_StdObject->do_Gadget.Width=icon->do_Gadget.Width;
			ndo->ndo_StdObject->do_Gadget.Height=icon->do_Gadget.Height;

			// New other stuff
			ndo->ndo_StdObject->do_Gadget.UserData=icon->do_Gadget.UserData;
			ndo->ndo_StdObject->do_Gadget.MutualExclude=icon->do_Gadget.MutualExclude;

			// Write icon
			if (!(PutNewDiskObject(filename,ndo)))
			{
				suc=ICONWRITE_FAILED;
				if (err) *err=IoErr();
			}

			// Restore old images
			ndo->ndo_StdObject->do_Gadget.GadgetRender=gadgetrender;
			ndo->ndo_StdObject->do_Gadget.SelectRender=selectrender;
			ndo->ndo_StdObject->do_Gadget.Flags=flags;
			ndo->ndo_NormalImage=old_normal;
			ndo->ndo_SelectedImage=old_selected;

			// Free icon
			FreeNewDiskObject(ndo);
		}

		// Standard icons
		else
		{

		if (write_icon=GetCachedDiskObject(filename,GCDOF_NOCACHE))
		{
			// Save old images
			gadgetrender=write_icon->do_Gadget.GadgetRender;
			selectrender=write_icon->do_Gadget.SelectRender;
			flags=write_icon->do_Gadget.Flags;

			// Get new image pointers
			write_icon->do_Gadget.GadgetRender=icon->do_Gadget.GadgetRender;
			write_icon->do_Gadget.SelectRender=icon->do_Gadget.SelectRender;
			write_icon->do_Gadget.Flags=icon->do_Gadget.Flags;
	
			// New size
			write_icon->do_Gadget.Width=icon->do_Gadget.Width;
			write_icon->do_Gadget.Height=icon->do_Gadget.Height;

			// New other stuff
			write_icon->do_Gadget.UserData=icon->do_Gadget.UserData;
			write_icon->do_Gadget.MutualExclude=icon->do_Gadget.MutualExclude;

			// Write icon

			if	(!(PutDiskObject(filename,write_icon)))
			{
				suc=ICONWRITE_FAILED;
				if (err) *err=IoErr();
			}
				
			// Restore old images
			write_icon->do_Gadget.GadgetRender=gadgetrender;
			write_icon->do_Gadget.SelectRender=selectrender;
			write_icon->do_Gadget.Flags=flags;

			// Free icon
			FreeCachedDiskObject(write_icon);
		}
		}
	}

	// Just write icon
	else
	{
		// Drawer or disk?
		if (icon->do_Type==WBDRAWER ||
			icon->do_Type==WBDISK)
		{
			// Got drawer data?
			if (icon->do_DrawerData)
			{
				// Store default position in new icon
				icon->do_DrawerData->dd_NewWindow.Width=
					(environment->env->lister_width<64)?320:environment->env->lister_width;
				icon->do_DrawerData->dd_NewWindow.Height=
					(environment->env->lister_height<32)?200:environment->env->lister_height;
			}
		}

		// Try to write icon

		if	(!(PutDiskObject(filename,icon)))
		{
			suc=ICONWRITE_FAILED;
			if (err) *err=IoErr();
		}
	}

	// Free icon
	FreeCachedDiskObject(icon);

	// Return success code
	return suc;
}


// See if file is an icon
char *isicon(char *filename)
{
	return sufcmp(filename,".info");
}


// Get an icon, using filetypes
struct DiskObject *GetProperIcon(char *name,short *fake,ULONG flags)
{
	struct DiskObject *icon;
	Cfg_Filetype *ftype;

	// Clear fake icon
	if (fake) *fake=0;

	// Try to get real icon
	if (icon=GetCachedDiskObject(name,flags)) return icon;

	// Set fake icon
	if (fake) *fake=1;

	// See if this file matches a filetype with an icon
	if (ftype=filetype_identify(name,NULL,FTTYPE_ICON,0,0))
	{
		char path[256];
		char *ptr;

		// Copy path, strip .info
		strcpy(path,ftype->icon_path);
		if (ptr=strstr(path,".info")) *ptr=0;

		// Get icon
		if (icon=GetCachedDiskObject(path,flags))
			return icon;
	}

	// Get default icon
	return GetCachedDiskObjectNew(name,flags);
}

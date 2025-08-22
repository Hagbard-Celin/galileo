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

#include "galileofmlib.h"
#include <proto/newicon.h>

/*	
	Modified 3 nov 99 gjp to now call puticontags if V44


*/

// Patched PutDiskObject()
BOOL __asm __saveds L_WB_PutDiskObject(
	register __a0 char *name,
	register __a1 struct DiskObject *diskobj)
{
	BOOL result,magic=0;

	// See if icon type is set to inverse magic
	if (diskobj->do_Magic==(UWORD)~WB_DISKMAGIC)
	{
		// Fix it
		diskobj->do_Magic=WB_DISKMAGIC;

		// Set magic flag, which will stop us sending notification
		magic=1;
	}

	// Write icon
	result=L_WriteIcon(name,diskobj);

	// Succeeded?
	if (result && !magic) icon_notify(name,0,0);

	return result;
}


// Patched DeleteDiskObject()
BOOL __asm __saveds L_WB_DeleteDiskObject(register __a0 char *name)
{
	BOOL result;
	char *full_name;

	// Get full name
	full_name = L_GetIconFullname(name);

	// Write icon
	result=L_DeleteIcon(name);

	// Succeeded?
	if ((result || IoErr()==ERROR_OBJECT_NOT_FOUND) && full_name)
		icon_notify(full_name,INF_FULLNAME,1);

	// Free full name buffer
	L_FreeMemH(full_name);

	return result;
}


// Send icon notification
void icon_notify(char *name,ULONG flags,short delete)
{
	char *full_name;

	// Given full name?
	if (flags&INF_FULLNAME) full_name=name;

	// Get full name
	else
	if (!(full_name = L_GetIconFullname(name)))
		return;

	// Send notify message
	L_SendNotifyMsg(GN_WRITE_ICON,0,delete,FALSE,full_name,0);

	// Free buffer
	if (!(flags&INF_FULLNAME)) L_FreeMemH(full_name);
}


// Get icon full name
char * __asm __saveds L_GetIconFullname(register __a0 char *name)
{
	BPTR lock;
	ULONG len = 0;
	char *full_name,*ptr;

	// Lock icon
	if (!(lock=L_LockFromPath(name,NULL,LFPF_TRY_ICON_FIRST)))
	    return 0;

	// Expand path
	if (full_name = L_PathFromLock(NULL, lock, PFLF_USE_DEVICENAME, NULL))
	{
		len = getreg(REG_D1);

		// Disk?
		if ((ptr=strchr(full_name,':')) && stricmp(ptr+1,"disk.info")==0)
		{
			L_FreeMemH(full_name);

			// Get real full name
			if (full_name = L_PathFromLock(NULL, lock, NULL, NULL))
			{
			    len = getreg(REG_D1);

			    // Strip after colon
			    if (ptr=strchr(full_name,':')) *(ptr+1)=0;
			}
		}
	}

	// Unlock lock
	UnLock(lock);

	// Strip .info from name
	if (full_name && len>5 &&
	    stricmp(full_name+len-5,".info")==0)
	{
		// Strip it
		*(full_name+len-5)=0;
	}

	return full_name;
}


// PutDiskObject without notification
BOOL __asm __saveds L_WriteIcon(
	register __a0 char *name,
	register __a1 struct DiskObject *icon)
{
	BOOL result;

	// No icon?
	if (!icon) return 0;

	// A NewIcon?
	if (L_GetIconType(icon)==ICON_NEWICON && NewIconBase)
	{
		struct NewIconDiskObject *nido;
		struct DiskObject *temp,write_obj;

		// Get NIDO pointer
		nido=(struct NewIconDiskObject *)icon;

		// Save structure DiskObject pointer
		temp=nido->nido_NewDiskObject->ndo_StdObject;

		// Copy DiskObject to write
		CopyMem((char *)icon,(char *)&write_obj,sizeof(struct DiskObject));

		// Get original gadget structure fields
		write_obj.do_Gadget.GadgetRender=temp->do_Gadget.GadgetRender;
		write_obj.do_Gadget.SelectRender=temp->do_Gadget.SelectRender;
		write_obj.do_Gadget.Flags=temp->do_Gadget.Flags;
		write_obj.do_Gadget.Width=temp->do_Gadget.Width;
		write_obj.do_Gadget.Height=temp->do_Gadget.Height;

		// Fix revision
		if ((((ULONG)write_obj.do_Gadget.UserData)&WB_DISKREVISIONMASK)==0)
			*((ULONG *)&write_obj.do_Gadget.UserData)|=WB_DISKREVISION;

		// Clear SpecialInfo field so we won't get stuck in a loop
		write_obj.do_Gadget.SpecialInfo=0;

		// Set new icon pointer
		nido->nido_NewDiskObject->ndo_StdObject=&write_obj;

		// This is heavy magic, which stops us from notifying things twice
		// (PutNewDiskObject calls PutDiskObject..)
		write_obj.do_Magic=(UWORD)~WB_DISKMAGIC;

		// Write the NewIcon
		result=PutNewDiskObject(name,nido->nido_NewDiskObject);

		// Restore the DiskObject pointer in the NewIcon
		nido->nido_NewDiskObject->ndo_StdObject=temp;
	}

	// Write normally
	else
	{
		// Fix revision
		if ((((ULONG)icon->do_Gadget.UserData)&WB_DISKREVISIONMASK)==0)
			*((ULONG *)&icon->do_Gadget.UserData)|=WB_DISKREVISION;
	
	if	(IconBase->lib_Version>=44)
		result=PutIconTags(name,icon,TAG_DONE);

	else
		// Write the icon
		result=((BOOL (* __asm)
					(register __a0 char *,register __a1 struct DiskObject *,register __a6 struct Library *))
						gfmlib_data.wb_data.old_function[WB_PATCH_PUTDISKOBJECT])
					(name,icon,IconBase);
	}

	return result;
}


// DeleteDiskObject without notification
BOOL __asm __saveds L_DeleteIcon(
	register __a0 char *name)
{
	// Delete it
	return 
		((BOOL (* __asm)
			(register __a0 char *,register __a6 struct Library *))
				gfmlib_data.wb_data.old_function[WB_PATCH_DELETEDISKOBJECT])
			(name,IconBase);
}


// Get icon flags
ULONG __asm __saveds L_GetIconFlags(
	register __a0 struct DiskObject *icon)
{
	ULONG flags;

	// Valid icon?
	if (!icon) return 0;

	// Stored in gadget userdata
	flags=(ULONG)icon->do_Gadget.UserData;

	// Mask off flags we're not interested in
	flags&=ICONF_POSITION_OK|ICONF_ICON_VIEW|ICONF_BORDER_OFF|ICONF_NO_LABEL|ICONF_BORDER_ON;
	return flags;
}

// Get Galileo icon position
void __asm __saveds L_GetIconPosition(
	register __a0 struct DiskObject *icon,
	register __a1 short *x,
	register __a2 short *y)
{
	// Clear values
	if (x) *x=0;
	if (y) *y=0;

	// Valid icon?
	if (!icon) return;

	// Get positions
	if (x) *x=((ULONG)icon->do_Gadget.MutualExclude)>>16;
	if (y) *y=((ULONG)icon->do_Gadget.MutualExclude)&0xffff;
}

// Set icon flags
void __asm __saveds L_SetIconFlags(
	register __a0 struct DiskObject *icon,
	register __d0 ULONG flags)
{
	ULONG oldflags;

	// Valid icon?
	if (!icon) return;

	// Get old flags from gadget
	oldflags=(ULONG)icon->do_Gadget.UserData;

	// Clear flags we're interested in
	oldflags&=~(ICONF_POSITION_OK|ICONF_ICON_VIEW|ICONF_BORDER_OFF|ICONF_NO_LABEL|ICONF_BORDER_ON);

	// Or new flags in
	oldflags|=flags;

	// Store back in gadget
	icon->do_Gadget.UserData=(APTR)oldflags;
}

// Set Galileo icon position
void __asm __saveds L_SetIconPosition(
	register __a0 struct DiskObject *icon,
	register __d0 short x,
	register __d1 short y)
{
	// Valid icon?
	if (!icon) return;

	// Set position
	*((ULONG *)&icon->do_Gadget.MutualExclude)=(x<<16)|y;
}


// Get the icon type
short __asm __saveds L_GetIconType(register __a0 struct DiskObject *icon)
{
	if (!icon) return 0;

	if (icon->do_Gadget.SpecialInfo==(APTR)icon)
		return ICON_CACHED;
	else
	if (icon->do_Gadget.SpecialInfo==(APTR)(icon+1))
		return ICON_NEWICON;

	return ICON_NORMAL;
}

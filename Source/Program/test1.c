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

#include "icon.h"

// Get icon flags
ULONG GetIconFlags(struct DiskObject *icon)
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
void GetIconPosition(struct DiskObject *icon,short *x,short *y)
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
void SetIconFlags(struct DiskObject *icon,ULONG flags)
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
void SetIconPosition(struct DiskObject *icon,short x,short y)
{
	// Valid icon?
	if (!icon) return;

	// Set position
	*((ULONG *)&icon->do_Gadget.MutualExclude)=(x<<16)|y;
}


// Get the icon type
short GetIconType(struct DiskObject *icon)
{
	if (!icon) return 0;

	if (icon->do_Gadget.SpecialInfo==(APTR)icon)
		return ICON_CACHED;
	else
	if (icon->do_Gadget.SpecialInfo==(APTR)(icon+1))
		return ICON_NEWICON;

	return ICON_NORMAL;
}


void main(int argc, char ** argv)
{
char *name;

if	(argc ==2)
	{
	name=argv[1];
	printf("icon %s\n",name);
	}



}

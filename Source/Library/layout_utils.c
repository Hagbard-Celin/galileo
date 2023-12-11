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

#include "galileofmlib.h"
#include "layout_routines.h"

// Initialise window dimensions
void __asm __saveds L_InitWindowDims(
	register __a0 struct Window *window,
	register __a1 WindowDimensions *dims)
{
	// Not valid?
	if (!(dims->wd_Flags&WDF_VALID))
	{
		// Store normal dimensions
		dims->wd_Normal=*((struct IBox *)&window->LeftEdge);

		// Use minimum as zoomed dimensions
		dims->wd_Zoomed.Left=window->LeftEdge;
		dims->wd_Zoomed.Top=window->TopEdge;
		dims->wd_Zoomed.Width=window->MinWidth;
		dims->wd_Zoomed.Height=window->MinHeight;

		// Set valid flag
		dims->wd_Flags|=WDF_VALID;
	}
}


// Store window dimensions
void __asm __saveds L_StoreWindowDims(
	register __a0 struct Window *window,
	register __a1 WindowDimensions *dims)
{
	// Is window zoomed?
	if (window->Flags&WFLG_ZOOMED)
	{
		// Store zoom dimensions
		dims->wd_Zoomed=*((struct IBox *)&window->LeftEdge);

		// Set zoomed flag
		dims->wd_Flags|=WDF_ZOOMED;
	}

	// Not zoomed
	else
	{
		// Store normal dimensions
		dims->wd_Normal=*((struct IBox *)&window->LeftEdge);

		// Clear zoomed flag
		dims->wd_Flags&=~WDF_ZOOMED;
	}
}


// See if size has changed
BOOL __asm __saveds L_CheckWindowDims(
	register __a0 struct Window *window,
	register __a1 WindowDimensions *dims)
{
	struct IBox *box;

	// If zoom state has changed, sense changes
	if ((window->Flags&WFLG_ZOOMED && !(dims->wd_Flags&WDF_ZOOMED)) ||
		(!(window->Flags&WFLG_ZOOMED) && dims->wd_Flags&WDF_ZOOMED))
	{
		// Is window zoomed?
		if (window->Flags&WFLG_ZOOMED) box=&dims->wd_Normal;
		else box=&dims->wd_Zoomed;
	}

	// Otherwise, normal sense
	else
	{
		// Is window zoomed?
		if (window->Flags&WFLG_ZOOMED) box=&dims->wd_Zoomed;
		else box=&dims->wd_Normal;
	}

	// See if size is different
	if (window->Width!=box->Width || window->Height!=box->Height)
		return 1;

	return 0;
}

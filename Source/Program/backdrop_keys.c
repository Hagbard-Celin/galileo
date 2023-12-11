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

// Get next icon for a key selection - list must be sorted in position order
BackdropObject *backdrop_key_select(BackdropObject *last,USHORT code)
{
	BackdropObject *icon;
	unsigned short icon_left,icon_top,left,top;

	// Get left and top positions
	icon_left=last->pos.Left+(last->pos.Width>>1);
	icon_top=last->pos.Top+(last->pos.Height>>1);

	// Right?
	if (code==CURSORRIGHT)
	{
		// Go through icons until the end
		for (icon=(BackdropObject *)last->node.ln_Succ;
			icon->node.ln_Succ;
			icon=(BackdropObject *)icon->node.ln_Succ)
		{
			// Get left and top for this icon
			left=icon->pos.Left+(icon->pos.Width>>1);
			top=icon->pos.Top+(icon->pos.Height>>1);

			// Must be roughly the same vertically and greater horizontally
			if (left>=icon_left+(last->pos.Width>>1) &&
				top>=icon_top-(last->pos.Height>>1) &&
				top<=icon_top+last->pos.Height) break;
		}

		// Didn't find anything?
		if (!icon->node.ln_Succ)
		{
			// Go through icons until the end
			for (icon=(BackdropObject *)last->node.ln_Succ;
				icon->node.ln_Succ;
				icon=(BackdropObject *)icon->node.ln_Succ)
			{
				// Get left for this icon
				left=icon->pos.Left+(icon->pos.Width>>1);

				// Must be just be greater horizontally
				if (left>=icon_left+(last->pos.Width>>1)) break;
			}

			// Still didn't find anything?
			if (!icon->node.ln_Succ) code=CURSORDOWN;
		}
	}

	// Down?
	if (code==CURSORDOWN)
	{
		// Moving down, we should just be able to grab the next one
		icon=(BackdropObject *)last->node.ln_Succ;
	}

	// Left?
	if (code==CURSORLEFT)
	{
		// Go through icons until the beginning
		for (icon=(BackdropObject *)last->node.ln_Pred;
			icon->node.ln_Pred;
			icon=(BackdropObject *)icon->node.ln_Pred)
		{
			// Get left and top for this icon
			left=icon->pos.Left+(icon->pos.Width>>1);
			top=icon->pos.Top+(icon->pos.Height>>1);

			// Must be roughly the same vertically and less horizontally
			if (left<=icon_left-(last->pos.Width>>1) &&
				top>=icon_top-(last->pos.Height>>1) &&
				top<=icon_top+last->pos.Height) break;
		}

		// Didn't find anything?
		if (!icon->node.ln_Pred)
		{
			// Go through icons until the beginning
			for (icon=(BackdropObject *)last->node.ln_Pred;
				icon->node.ln_Pred;
				icon=(BackdropObject *)icon->node.ln_Pred)
			{
				// Get left for this icon
				left=icon->pos.Left+(icon->pos.Width>>1);

				// Must be just be less horizontally
				if (left<=icon_left-(last->pos.Width>>1)) break;
			}

			// Still didn't find anything?
			if (!icon->node.ln_Pred) code=CURSORUP;
		}
	}

	// Up?
	if (code==CURSORUP)
	{
		// Moving up, we should just be able to grab the previous one
		icon=(BackdropObject *)last->node.ln_Pred;
	}

	return icon;
}

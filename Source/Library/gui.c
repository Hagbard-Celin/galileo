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

// Draw a 3D box
void __asm __saveds L_DrawBox(
	register __a0 struct RastPort *rp,
	register __a1 struct Rectangle *rect,
	register __a2 struct DrawInfo *info,
	register __d0 BOOL recessed)
{
	UBYTE shine,shadow;

	// Get pens we need
	if (recessed)
	{
		shine=info->dri_Pens[SHADOWPEN];
		shadow=info->dri_Pens[SHINEPEN];
	}
	else
	{
		shine=info->dri_Pens[SHINEPEN];
		shadow=info->dri_Pens[SHADOWPEN];
	}

	// Do shine lines
	SetAPen(rp,shine);
	Move(rp,rect->MinX,rect->MaxY);
	Draw(rp,rect->MinX,rect->MinY);
	Draw(rp,rect->MaxX-1,rect->MinY);

	// Do shadow lines
	SetAPen(rp,shadow);
	Move(rp,rect->MaxX,rect->MinY);
	Draw(rp,rect->MaxX,rect->MaxY);
	Draw(rp,rect->MinX+1,rect->MaxY);
}


// Draw a 3D string box
void __asm __saveds L_DrawFieldBox(
	register __a0 struct RastPort *rp,
	register __a1 struct Rectangle *rect,
	register __a2 struct DrawInfo *info)
{
	draw_field_box(rp,rect,info);
}

void draw_field_box(
	struct RastPort *rp,
	struct Rectangle *rect,
	struct DrawInfo *info)
{
	// Do shine lines
	SetAPen(rp,info->dri_Pens[SHINEPEN]);
	Move(rp,rect->MinX,rect->MaxY);
	Draw(rp,rect->MinX,rect->MinY);
	Draw(rp,rect->MaxX-1,rect->MinY);
	Draw(rp,rect->MaxX-1,rect->MaxY-1);
	Draw(rp,rect->MinX+2,rect->MaxY-1);

	// Do shadow lines
	SetAPen(rp,info->dri_Pens[SHADOWPEN]);
	Move(rp,rect->MaxX,rect->MinY);
	Draw(rp,rect->MaxX,rect->MaxY);
	Draw(rp,rect->MinX+1,rect->MaxY);
	Draw(rp,rect->MinX+1,rect->MinY+1);
	Draw(rp,rect->MaxX-2,rect->MinY+1);
}


// Get information about a screen
ULONG __asm __saveds L_ScreenInfo(register __a0 struct Screen *screen)
{
	ULONG info=0,mode;
	struct DisplayInfo disp;

	// Valid screen?
	if (!screen) return 0;

	// Get display mode
	if ((mode=GetVPModeID(&screen->ViewPort))==INVALID_ID) return 0;

	// Get DisplayInfo
	if (!(GetDisplayInfoData(0,(char *)&disp,sizeof(struct DisplayInfo),DTAG_DISP,mode)))
		return 0;

	// See if screen is lo-res
	if (disp.Resolution.y>=disp.Resolution.x*2)
		info=SCRI_LORES;

	return info;
}

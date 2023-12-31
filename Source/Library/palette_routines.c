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

#include "galileofmlib.h"
#include "config.h"
#include "/Program/galileo_config.h"

// Load a 32 bit palette, even under <39
void __asm __saveds L_LoadPalette32(
	register __a0 struct ViewPort *vp,
	register __a1 unsigned long *palette)
{
	// Under 39 pass to Gfx library
	if (GfxBase->LibNode.lib_Version>=39)
	{
		LoadRGB32(vp,palette);
	}

	// Otherwise, convert to 4 bit
	else
	{
		unsigned short *backup_palette;
		short pen,numcols;

		// Get count
		numcols=*(unsigned short *)palette;
		++palette;

		// Allocate backup palette
		if (backup_palette=AllocVec(numcols*sizeof(unsigned short),0))
		{
			// Convert to 4 bit colour
			for (pen=0;pen<numcols;pen++)
			{
				backup_palette[pen]=((*palette++)&0xf0000000)>>20;
				backup_palette[pen]|=((*palette++)&0xf0000000)>>24;
				backup_palette[pen]|=((*palette++)&0xf0000000)>>28;
			}

			// Load palette
			LoadRGB4(vp,backup_palette,numcols);

			// Free backup
			FreeVec(backup_palette);
		}
	}
}


// Get a 32 bit palette, even under <39
void __asm __saveds L_GetPalette32(
	register __a0 struct ViewPort *vp,
	register __a1 unsigned long *palette,
	register __d0 unsigned short count,
	register __d1 short first)
{
	// If under 39+ we get it directly
	if (GfxBase->LibNode.lib_Version>=39)
		GetRGB32(vp->ColorMap,first,count,palette);

	// Otherwise, we have to convert from 4 bit colour
	else
	{
		unsigned short pen;
		unsigned long colour;

		// Go through each pen
		for (pen=0;pen<count;pen++)
		{
			// Get colour value of this pen
			colour=GetRGB4(vp->ColorMap,pen+first);

			// Convert from 4 to 32 bit colour
			*palette++=(((colour>>8)&0xf)<<28)|0x0fffffff;
			*palette++=(((colour>>4)&0xf)<<28)|0x0fffffff;
			*palette++=((colour&0xf)<<28)|0x0fffffff;
		}
	}
}

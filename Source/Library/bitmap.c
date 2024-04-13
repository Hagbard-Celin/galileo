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

struct BitMap *__asm __saveds L_NewBitMap(
	register __d0 ULONG sizex,
	register __d1 ULONG sizey,
	register __d2 ULONG depth,
	register __d3 ULONG flags,
	register __a0 struct BitMap *friend)
{
	struct BitMap *bm;
	short plane;

	// Under 39, allocate directly
	if (GfxBase->LibNode.lib_Version>=39)
	{
        // Got friend and P96?
        if (friend && P96Base)
        {
            // Friend is Picasso bitmap?
            if (p96GetBitMapAttr(friend,P96BMA_ISP96))
            {
                return p96AllocBitMap(sizex,sizey,depth,flags,friend,NULL);
            }
        }
#if 0
		// Friend bitmap, minplanes not set?
		if (friend && !(flags&BMF_MINPLANES))
		{
			// Non-standard friend bitmap?
			if (!(GetBitMapAttr(friend,BMA_FLAGS)&BMF_STANDARD))
			{
				// Set min-planes in flags
				flags|=BMF_MINPLANES;
			}
		}
#endif
		return AllocBitMap(sizex,sizey,depth,flags,friend);
	}

	// Round up width, check depth
	sizex=(((sizex+15)>>3)&0xfffe)<<3;
	if (depth<1) depth=1;
	else
	if (depth>8) depth=8;

	// Allocate and initialise bitmap
	if (!(bm=AllocVec(sizeof(struct BitMap),MEMF_CLEAR)))
		return 0;
	InitBitMap(bm,depth,sizex,sizey);

	// Allocate planes
	for (plane=0;plane<depth;plane++)
	{
		// Allocate plane
		if (!(bm->Planes[plane]=AllocRaster(sizex,sizey)))
			break;

		// Clear?
		if (flags&BMF_CLEAR) BltClear(bm->Planes[plane],RASSIZE(sizex,sizey),0);
	}

	// Failed?
	if (plane<depth)
	{
		// Free what we got
		while ((--plane)>=0)
			FreeRaster(bm->Planes[plane],sizex,sizey);
		FreeVec(bm);
		return 0;
	}

	return bm;
}

void __asm __saveds L_DisposeBitMap(
	register __a0 struct BitMap *bm)
{
	short plane;

	// Invalid pointer?
	if (!bm) return;

	// Under 39, free directly
	if (GfxBase->LibNode.lib_Version>=39)
	{
		if (P96Base)
        {
            if (p96GetBitMapAttr(bm,P96BMA_ISP96))
            {
                p96FreeBitMap(bm);
                return;
            }
        }
        FreeBitMap(bm);
		return;
	}

	// Free planes
	for (plane=0;plane<bm->Depth;plane++)
		if (bm->Planes[plane])
			FreeRaster(bm->Planes[plane],bm->BytesPerRow<<3,bm->Rows);

	// Free bitmap
	FreeVec(bm);
}

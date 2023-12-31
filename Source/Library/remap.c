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
#include "images.h"

// Remap an image
Object *__saveds __asm L_RemapBitmap(
	register __a0 struct BitMap *source_bm,
	register __a1 ULONG *palette,
	register __a2 struct Screen *screen,
	register __d0 long precision,
	register __a6 struct MyLibrary *libbase)
{
	Object *obj;
	short ncolors;
	struct BitMapHeader *bmhd;

	// Don't have DataTypes?
	if (!DataTypesBase) return 0;

	// Do new method
	if (!(obj=
		NewDTObject(0,
			DTA_SourceType,DTST_RAM,
			DTA_GroupID,GID_PICTURE,
			PDTA_Screen,screen,
			OBP_Precision,precision,
			TAG_END))) return 0;

	// Get BitMapHeader
	GetDTAttrs(obj,PDTA_BitMapHeader,&bmhd,TAG_DONE);

	// Fill out header
	bmhd->bmh_Width=GetBitMapAttr(source_bm,BMA_WIDTH);
	bmhd->bmh_Height=GetBitMapAttr(source_bm,BMA_HEIGHT);
	bmhd->bmh_Depth=GetBitMapAttr(source_bm,BMA_DEPTH);

	// Get number of colours
	ncolors=1<<bmhd->bmh_Depth;

	// Set colours and bitmap pointer
	SetDTAttrs(obj,0,0,
		PDTA_NumColors,ncolors,
		PDTA_BitMap,source_bm,
		TAG_DONE);

	// Got a palette?
	if (palette)
	{
		short i,p;
    	struct ColorRegister *cmap;
	    long *cregs;

		// Get palette fields
		GetDTAttrs(obj,
			PDTA_ColorRegisters,&cmap,
			PDTA_CRegs,&cregs,
			TAG_DONE);

		// Go through colours
		for (i=0,p=0;i<ncolors;i++,p+=3)
		{
			// Set master colour table
			cmap->red=(palette[p+0]>>24)&0xff;
			cmap->green=(palette[p+1]>>24)&0xff;
			cmap->blue=(palette[p+2]>>24)&0xff;

			// Set remapping table
			cregs[p+0]=palette[p+0];
			cregs[p+1]=palette[p+1];
			cregs[p+2]=palette[p+2];
		}
	}

	// Do layout		
	DoMethod(obj,DTM_PROCLAYOUT,0,1);

	// Clear bitmap pointer (need to give it a dummy bitmap since 0 causes hits)
	SetDTAttrs(obj,0,0,
		PDTA_BitMap,AllocBitMap(1,1,1,0,0),
		TAG_DONE);

	return obj;
}

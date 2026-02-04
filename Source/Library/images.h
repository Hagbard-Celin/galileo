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

#include <libraries/newicon.h>
#ifndef GALILEOFM_IMAGE_PROTOS_H
#include <gfm/openimageinfo.h>
#endif

typedef struct
{
	struct Node		node;			// Image path in ln_Name
	long			count;			// Open count

	unsigned short		width;			// Width of image
	unsigned short		height;			// Height of image
	unsigned short		depth;			// Depth of image

	unsigned short		**planes[2];		// Image planes (not necessarily chip memory)
	unsigned short		flags;			// Flags

	ILBMHandle		*ilbm;			// ILBM

	unsigned short		**remap_planes[2];	// Remapped planes
	unsigned long		checksum[2];		// Image checksum (for icons)
	short			remap_depth;		// Depth remapped to

	struct SignalSemaphore	lock;			// Image lock
	short			remap_count;		// Number of times remapped

	struct NewDiskObject	*new_diskobj;		// NewIcons disk object
	struct Image		*remap_image[2];	// Remapped images

	ImageRemap		*remap_info;		// Image remap info

/*
	Object			*dt_object;		// Datatypes object
	struct BitMap		*dt_object_bm;		// Datatypes bitmap
*/
} Image_Data;

#define IMAGEF_SEL_IMAGE	(1<<0)		// Selected imagery
#define IMAGEF_ILBM		(1<<1)		// Image from ILBM
#define IMAGEF_CHIP		(1<<2)		// Image already in chip memory
#define IMAGEF_ALT_CHIP		(1<<3)		// Alternate image in chip memory
#define IMAGEF_ICON		(1<<4)		// Cached icon
#define IMAGEF_DEFICON		(1<<5)		// Cached default icon
#define IMAGEF_ICON_IMAGE	(1<<6)		// Image is from an icon
#define IMAGEF_NEWICON		(1<<7)		// Icon is a NewIcon

Image_Data *read_image(char *,OpenImageInfo *);
Image_Data *new_image(short,short,short);
void free_image(Image_Data *);
BOOL copy_icon_image(struct Image *,Image_Data *,short);
BOOL add_remap_pen(struct Screen *screen,ImageRemap *remap,unsigned short pen);

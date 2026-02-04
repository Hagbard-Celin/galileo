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

#ifndef GALILEOFM_IMAGE_TAGS_H
#define GALILEOFM_IMAGE_TAGS_H

// Image routine tags
#define IM_Width		TAG_USER + 0		// Width of image
#define IM_Height		TAG_USER + 1		// Height of image
#define IM_State		TAG_USER + 2		// 1 = selected, 0 = normal (default)
#define IM_Rectangle		TAG_USER + 3		// Rectangle to center within
#define IM_Mask			TAG_USER + 4		// 1 = mask image
#define IM_Depth		TAG_USER + 5		// Depth of image
#define IM_ClipBoundary		TAG_USER + 6		// Clip boundary size
#define IM_Erase		TAG_USER + 7		// Erase background
#define IM_NoDrawInvalid	TAG_USER + 8		// Don't draw if image is invalid
#define IM_NoIconRemap		TAG_USER + 9		// Don't remap icons
#define IM_Scale		TAG_USER + 10		// Scale image to IM_Rectangle
#define IM_Backfill		TAG_USER + 11

#endif

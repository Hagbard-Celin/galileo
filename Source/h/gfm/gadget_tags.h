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

#ifndef GALILEOFM_GADGET_TAGS_H
#define GALILEOFM_GADGET_TAGS_H

// Propgadget tags
#define GPGA_SpringLoaded   	TAG_USER + 63

// Gauge tags
#define GGG_Total		( TAG_USER + 0x64 )	// Total size
#define GGG_Free		( TAG_USER + 0x65 )	// Free size
#define GGG_Flags		( TAG_USER + 0x66 )	// Flags
#define GGG_FillPen		( TAG_USER + 0x67 )	// Pen to fill gauge with
#define GGG_FillPenAlert	( TAG_USER + 0x68 )	// Pen when space is running out

// File button tags
#define GFB_DefPath		TAG_USER + 19		// Default path

// Palette tags
#define GPG_Redraw		TAG_USER + 20		// Redraw a pen
#define GPG_Pen			TAG_USER + 21		// Ordinal selected pen
#define GPG_SelectPrevious	TAG_USER + 22		// Select previous pen
#define GPG_SelectNext		TAG_USER + 23		// Select next pen

//Pathgadget tags
#define PATHGA_TextVal          TAG_USER + 64
#define PATHGA_StringPointer	TAG_USER + 65
#define PATHGA_EditBuffer       TAG_USER + 66

#endif

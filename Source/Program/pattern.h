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

#ifndef _GALILEO_PATTERN_H
#define _GALILEO_PATTERN_H
#include <intuition/classusr.h>
#include <prefs/wbpattern.h>

typedef struct PatternInstance
{
	struct Hook		hook;			// Backfill hook
	struct PatternData	*pattern;		// Pattern info

	unsigned short		fill_fix[8][16];	// Adjusted pattern
	short			offset_x;		// Pattern offset
	short			offset_y;		// Pattern offset

	BOOL			disabled;		// Pattern is not to be shown
} PatternInstance;

typedef struct PatternData
{
	BOOL			valid;			// Valid pattern?

	struct WBPatternPrefs	prefs;			// Preferences data
	char			*data;			// Pattern data

	short			width;			// Pattern width
	short			height;			// Pattern height

	unsigned short		fill[8][16];		// Fill pattern
	unsigned short		fill_plane_key;		// Indicates planes that are used

	Object			*object;		// Datatypes object
	struct BitMap		*bitmap;		// Bitmap to blit from
	struct BitMap		*pattern_bitmap;	// Might be allocated

	short			old_depth;		// Old bitmap depth
	short			border_pen;		// Allocated border pen
	short			random;

	struct Screen		*screen;		// Screen pattern is displayed on

	ULONG			flags;			// FastIPrefs flags
	short			precision;		// Precision type
} PatternData;

#endif

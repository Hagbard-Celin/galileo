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

#ifndef _GALILEOFM_LISTFORMAT_H
#define _GALILEOFM_LISTFORMAT_H

#ifndef INTUITION_INTUITION_H
#include <intuition/intuition.h>
#endif
#ifndef DOS_DOS_H
#include <dos/dos.h>
#endif

// Sort format
typedef struct {
    BYTE    sort;		// Sort method
    BYTE    sort_flags;		// Sort flags
    BYTE    separation;		// File separation
} SortFormat;

// List format
typedef struct {
    UBYTE	files_unsel[2];	    // Unselected files
    UBYTE	files_sel[2];	    // Selected files
    UBYTE	dirs_unsel[2];	    // Unselected directories
    UBYTE	dirs_sel[2];	    // Selected directories

    SortFormat	sort;		    // Sort method
    BYTE	display_pos[16];    // Item display position
    BYTE	display_len[15];    // Display length

    UBYTE	flags;

    BYTE	show_free;	    // Show free space type
    char	show_pattern[40];   // Show pattern
    char	hide_pattern[40];   // Hide pattern

    char	show_pattern_p[40]; // Show pattern parsed
    char	hide_pattern_p[40]; // Hide pattern parsed
} ListFormat;


// LSTR - defines a lister
typedef struct {
    struct IBox	    pos[2];	// Window position
    ULONG	    icon_x;	// Icon position
    ULONG	    icon_y;
    ListFormat	    format;	// Lister format
    ULONG	    flags;	// Flags
    ULONG	    pad[4];

// Followed by path to load
} CFG_LSTR;


// Lister definition
typedef struct {
    struct Node		node;
    CFG_LSTR		lister;	    // Lister definition
    char		*path;	    // Path to load
    BPTR		lock;	    // Lock on path to load
} Cfg_Lister;

#endif


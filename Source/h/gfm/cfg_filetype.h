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

#ifndef GALILEOFM_CFG_FILETYPE_H
#define GALILEOFM_CFG_FILETYPE_H


// TYPE - Filetype definition
typedef struct {
    char		name[32];	// Filetype name
    char		id[8];		// Filetype ID
    UWORD		flags;		// Flags
    char		priority;	// Priority
    char		pad;		// Padding
    UWORD		count;		// Number of functions defined

// Followed by recognition string
// Followed by icon path
// Followed by action strings
} CFG_TYPE;

#define FILETYPEF_CHECKMOD  (1<<0)	// Check for ST module
#define FILETYPEF_OVERRIDE  (1<<1)	// Over-ride

// List of filetypes (for storage)
typedef struct {
    struct Node		node;
    char		path[256];	// File path
    struct DateStamp	date;		// File datestamp
    struct List		filetype_list;	// List of types
    ULONG		flags;
} Cfg_FiletypeList;

#define FTLISTF_INTERNAL	(1<<0)	// Internal list, not to be modified
#define FTLISTF_CHANGED		(1<<1)	// List has been modified
#define FTLISTF_STORE		(1<<2)	// List is to be stored
#define FTLISTF_NOT_FOUND	(1<<31)	// Special flag; internal use only


// Filetype definition
typedef struct {
    struct Node		node;
    CFG_TYPE		type;		// Filetype information
    unsigned char	*recognition;	// Recognition string
    char		*icon_path;	// Icon path
    char		**actions;	// Action strings
    struct List		function_list;	// Function list
    Cfg_FiletypeList	*list;		// List we are in
} Cfg_Filetype;

#endif

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

#ifndef _GALILEOFM_RESELECT
#define _GALILEOFM_RESELECT

typedef struct
{
	struct DirectoryBuffer	*buffer;	// Buffer these came from
	APTR			memory;		// Memory pool
	struct MinList		files;		// List of files
	char			*top_name;	// Top file displayed
	long			v_offset;	// Vertical offset
	long			h_offset;	// Horizontal offset
	unsigned long		flags;		// Flags
} ReselectionData;

#define RESELF_SAVE_FILETYPES	(1<<0)

typedef struct
{
	struct MinNode		node;
	unsigned short		flags;		// Selection state
	char			*filetype;	// Saved filetype pointer
	struct DateStamp	date;		// File date
	char			name[1];	// Filename
} ReselectionFile;

void InitReselect(ReselectionData *);
void MakeReselect(ReselectionData *,struct DirectoryBuffer *,ULONG);
void DoReselect(ReselectionData *,struct ListerWindow *,BOOL);
void GetReselectFiletypes(ReselectionData *,struct MinList *);
void FreeReselect(ReselectionData *);

#endif

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

#ifndef _LISTER_XFER_H
#define _LISTER_XFER_H

int replace_requester( struct hook_rec_data *, ULONG flags, struct entry_info *src, struct entry_info *dst );

int xfer_update  ( struct update_info *ui, unsigned int total, unsigned int bytes );
int getput_update( struct update_info *ui, unsigned int total, unsigned int bytes );

//
//	Flags for replace_requester()
//
enum
{
REPLACEF_NORESUME	= (1 << 0),	// Resume option not supported on some endpoint
REPLACEF_UPDATE		= (1 << 1),	// Handle the 'Copy UPDATE' option
REPLACEF_NEWER		= (1 << 2),	// Handle the 'Copy NEWER' option
};

//
//	Returned by replace_requester()
//
enum
{
REPLACE_ABORT,
REPLACE_RESUME,
REPLACE_RESUMEALL,
REPLACE_REPLACE,
REPLACE_REPLACEALL,
REPLACE_SKIP,
REPLACE_SKIPALL,
REPLACE_INVALID
};

//
//	Used by handle_samefilename()
//
enum
{
SKIP_RESUME_AND_REPLACE = 0,
SKIP_REPLACE            = 1,
SKIP_RESUME             = 2,
REQUEST_ALL             = 3
};

#endif

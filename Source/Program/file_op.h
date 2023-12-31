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

#ifndef _GALILEOFM_FILE_OP
#define _GALILEOFM_FILE_OP

#define DELETE_NOT_EMPTY	-2	// Directory not empty
#define DELETE_ABORTED		-1	// Aborted
#define DELETE_FAILED		0	// Failed
#define DELETE_OK			1	// Ok
#define DELETE_OK_UNPROTECT	2	// Ok, unprotect all

#define HUNT_GONE_SOMEWHERE	-3	// Found a match and gone to it
#define HUNT_ABORTED		-1	// Aborted
#define HUNT_FAILED			0	// Failed
#define HUNT_MATCHED		1	// Matched

#define SEARCH_ABORTED		-1	// Aborted
#define SEARCH_FAILED		0	// Failed
#define SEARCH_NO_MATCH		1	// Didn't matched
#define SEARCH_READ_FILE	2	// Read a file
#define SEARCH_SKIPPED		3	// Matched but skipped
#define SEARCH_SKIP_ALL		4	// Skip all

#define COPY_NO_PASSWORD	0
#define COPY_NO_ENCRYPT		0
#define COPY_ENCRYPT		1
#define COPY_DECRYPT		0

// Prototypes
file_copy(char *,char *,int *,char *,int,struct ProgressInfo *);
file_delete(char *,char *,int,int);
file_hunt(Lister *,char *,char *,char *);
file_search(char *,int *,int,char *);

#endif

/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 1993-2012 Jonathan Potter & GP Software
Copyright 2025 Hagbard Celine

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

#ifndef _GALILEOFM_DIRECTORYBUFFER_H
#define _GALILEOFM_DIRECTORYBUFFER_H

#include <libraries/multiuser.h>
#ifndef _GALILEOFM_LISTFORMAT_H
#include <gfm/listformat.h>
#endif
#ifndef _GALILEOFM_ENVOY_INFO_H
#include "envoy_info.h"
#endif

typedef struct ListerWindow Lister;

// Special node structure
typedef struct _DirNode {
	struct _DirNode	*dn_Succ;
	struct _DirNode	*dn_Pred;
	short		dn_Type;
	char		*dn_Name;
} DirNode;

// Structure defining a directory entry
typedef struct DirectoryEntry {
	DirNode			de_Node;
	short			de_SubType;		// Entry subtype
	unsigned long		de_Size;		// Entry size
	unsigned short		de_Protection;		// Protection
	unsigned short		de_Flags;		// Entry flags
	struct DateStamp	de_Date;		// Date
	char			de_ProtBuf[9];		// Protection string
	char			de_DateBuf[23];		// Date/time string
	unsigned char		de_NameLen;		// Length of name field
	unsigned char		de_Colour;		// Display pens
	unsigned long		de_UserData;		// User data
	struct TagItem		*de_Tags;		// Extension tags
} DirEntry;

// Holds information about a directory list
typedef struct DirectoryBuffer
{
	struct Node		node;
	struct SignalSemaphore	semaphore;		// Semaphore for locking

	struct ListerWindow	*buf_CurrentLister;	// Lister showing this buffer
	ListFormat		buf_ListFormat;		// Current format of this list

	short			dim_valid;		// Are dimensions valid?
	struct IBox		dimensions;		// Last window dimensions

	unsigned long		flags;			// Flags
	ULONG			cust_flags;
	long			more_flags;		// More flags

	struct MinList		entry_list;		// List of entries
	DirEntry		*first_file;		// First file in list
	DirEntry		*first_dir;		// First directory in list

	char			*buf_Path;		// Path this list is showing

	long			buf_TotalEntries[2];	// Total number of entries
	long			buf_TotalFiles[2];	// Total number of files
	long			buf_TotalDirs[2];	// Total number of directories
	unsigned long		buf_TotalBytes[2];	// Total number of bytes
	long			buf_SelectedFiles[2];	// Selected files
	long			buf_SelectedDirs[2];	// Selected directories
	unsigned long		buf_SelectedBytes[2];	// Selected bytes

	unsigned long		buf_TotalDiskSpace;	// Total disk space
	unsigned long		buf_FreeDiskSpace;	// Free disk space

	long			buf_VertOffset;		// Vertical display offset
	long			buf_HorizOffset;	// Horizontal display offset
	long			buf_OldVertOffset;	// Old vertical offset
	long			buf_OldHorizOffset;	// Old horizontal offset
	long			buf_HorizLength;	// Horizontal display width

	char			buf_CustomHandler[32];	// Port name of custom handler
	char			buf_CustomTitle[32];	// User title

	struct DateStamp	buf_DirectoryDate;	// Datestamp of this directory
	struct DateStamp	buf_VolumeDate;		// Disk datestamp

	char			buf_VolumeLabel[32];	// Volume name
	char			*buf_ObjectName;	// Object filename

	char			*buf_ExpandedPath;	// Expanded pathname

	unsigned short		last_owner;		// Owner ID of last entry we read
	unsigned short		last_group;		// Group ID of last entry we read
	char			owner_name[32];		// Last owner name (optimisation)
	char			group_name[32];		// Last group name

	APTR			memory;			// Memory pool to allocate files from
	struct MinList		reject_list;		// Files rejected by display

	short			size_length;		// Length of longest size
	short			comment_length;		// Length of longest comment
	short			type_length;		// Length of longest filetype
	short			owner_length;		// Length of longest owner
	short			group_length;		// Length of longest group
	short			name_length;		// Length of largest name
	unsigned char		name_field_size;	// Size of name field
	unsigned char		date_length;		// Length of date field

	char			last_status[80];	// Buffer status

	struct UserInfo		*user_info;
	struct GroupInfo	*group_info;

	char			buf_CustomLabel[32];	// User icon label

	short			version_length;		// Length of longest version

	short			comment_field_size;	// Size of comment field

	short			buf_FieldWidth[18];	// Field widths (pixels)

	Lister			*buf_OwnerLister;	// Lister that owns us

	ULONG			buf_DiskType;		// Current disk type

	ULONG			buf_VolNameLength;	// Name length for current disk


	char			buf_CustomHeader[80];	// Custom header

	char			*buf_TitleFields[10];	// User-strings for field titles

	struct muUserInfo	*mu_user_info;
	struct muGroupInfo	*mu_group_info;

	ULONG			buf_CustomWidthFlags;


	BPTR			buf_Lock;		// Lock on the dir this buffer represents
} DirBuffer;

#endif

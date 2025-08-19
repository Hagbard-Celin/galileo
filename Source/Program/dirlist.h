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

Opus� and Directory Opus� and associated images are registered trademarks of GPSoftware.
DOpus(TM), DirOpus(TM) and Opus File Manager(TM) are trademarked 1991 by GPSoftware.
Opus Magellan (TM) is trademarked 1996 GPSoftware.
All other trademarks are the property of their respective owners.

The release of Directory Opus 5 under the GPL in NO WAY affects
the existing commercial status of Directory Opus for Windows.

For more information on Directory Opus for Windows please see:

		 http://www.gpsoft.com.au

*/

#ifndef _GALILEOFM_DIRLIST
#define _GALILEOFM_DIRLIST

#include "lister.h"

#define RESORT_SORT		1
#define RESORT_REJECTS		2
#define RESORT_REVERSE		4
#define RESORT_SEPARATE		8

// Holds Envoy information about an entry
typedef struct NetworkStuff {
	UWORD		owner_id;		// Envoy owner ID
	UWORD		group_id;		// Envoy group ID
	char		owner[32];		// Owner string
	char		group[32];		// Group string
	char		net_protbuf[10];	// Protection bits string
} NetworkInfo;

// Holds version information
typedef struct _VersionInfo {
	UWORD		vi_Version;		// Major version #
	UWORD		vi_Revision;		// Minor revision #
	long		vi_Days;		// Days of date
	ULONG		vi_Flags;		// Flags
	char		vi_Char;		// Version character
	char		vi_String[1];		// Version string
} VersionInfo;

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

#define DE_PopupMenu		( TAG_USER + 0x1 )
#define DE_NetworkInfo		( TAG_USER + 0x2 )
#define DE_UserData		( TAG_USER + 0x3 )
#define DE_Comment		( TAG_USER + 0x4 )
#define DE_Filetype		( TAG_USER + 0x5 )
#define DE_DisplayString	( TAG_USER + 0x6 )
#define DE_VersionInfo		( TAG_USER + 0x7 )
#define	DE_AssignName		( TAG_USER + 0x8 )

#define de_dest_blocksize	de_SubType
#define de_block_total		de_UserData

#define de_foreground(e)	((e->colour>>4)&0xf)
#define de_background(e)	(e->colour&0xf)

// Entry flags
#define ENTF_SELECTED		(1<<0)		// Entry is selected
#define ENTF_NETWORK		(1<<1)		// Entry has network information
#define ENTF_SKIPPED		(1<<2)		// Skipped (temporarily)
#define ENTF_SKIP		(1<<3)		// Skip this entry
#define ENTF_REJECTED		(1<<4)		// Entry has been rejected

#define ENTF_LINK		(1<<6)		// File is a link
#define ENTF_SOFTLINK		(1<<7)		// File is a soft link
#define ENTF_VERSION		(1<<8)		// Has version information
#define ENTF_FAKE		(1<<9)		// Fake entry
#define ENTF_HIGHLIGHT		(1<<10)		// Highlight entry
#define ENTF_USER_MENU		(1<<11)		// User menu
#define ENTF_NO_SIZE		(1<<12)		// Entry has no size

#define ENTF_COLOUR_DEVICE	(1<<14)		// Device colour
#define ENTF_COLOUR_ASSIGN	(1<<15)		// Assign colour

// Entry types
#define ENTRY_FILE		-1		// File
#define ENTRY_DEVICE		0		// Device
#define ENTRY_DIRECTORY		1		// Directory
#define ENTRY_ANYTHING		2		// Used for something different

#define SUBENTRY_BUFFER		(1<<0)
#define SUBENTRY_ASSIGN		(1<<1)
#define SUBENTRY_MULTI_ASSIGN	(1<<2)

// Action codes sent to custom handlers
#define USERENTRY_ERROR		-1		// Error
#define USERENTRY_FUNCTION	0		// Function (not supported)
#define USERENTRY_DOUBLECLICK	1		// Double-click
#define USERENTRY_CLICKMCLICK	2		// Click-m-click

// Define to return a useful entry type
#define ENTRYTYPE(t) ((t==0)?ENTRY_DEVICE:((t<0)?ENTRY_FILE:ENTRY_DIRECTORY))

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

// DirList flags
#define DWF_ABORTED 		(1<<0)	// Read was aborted
#define DWF_READONLY		(1<<1)	// Volume is read only
#define DWF_VALID		(1<<2)	// Buffer is valid
#define DWF_SPECIAL_SORT	(1<<3)	// Special sort for this buffer
#define DWF_ROOT		(1<<4)	// Shows a root directory
#define DWF_ICONS		(1<<5)	// Show as icons
#define DWF_READING		(1<<6)	// Reading directory now
#define DWF_SAVE_STATUS		(1<<7)	// Save status

#define DWF_CASE		(1<<0)	// Case-insensitive
#define DWF_DEVICE_LIST		(1<<1)	// Device list
#define DWF_DEV_FULL		(1<<2)	// Full
#define DWF_DEV_BRIEF		(1<<3)	// Brief
#define DWF_CACHE_LIST		(1<<4)	// Cache list
#define DWF_LOCK_STATE		(1<<5)	// Lock state
#define DWF_LOCK_FORMAT		(1<<6)	// Lock format
#define DWF_HIDE_TITLE		(1<<7)	// Hide titles

#define FINDENTRY_ICON		(1<<14)	// Look for icon

#define CUSTF_WANT_QUOTES	(1<<0)	// Names should be in quotes
#define CUSTF_WANT_STEM		(1<<1)	// Stem variable names
#define CUSTF_WANT_FULLPATH	(1<<2)	// Full paths
#define CUSTF_EDITING		(1<<3)	// Supports editing
#define CUSTF_NOPOPUPS		(1<<4)	// No popups
#define CUSTF_GAUGE		(1<<5)	// Enable gauge
#define CUSTF_SUBDROP		(1<<6)	// Enable sub-drop
#define CUSTF_INHERIT		(1<<7)	// Support inherit
#define CUSTF_LEAVEGAUGE	(1<<8)	// Leave fuel gauge alone
#define CUSTF_SYNCTRAPS		(1<<9)	// Synchronous trap messages
#define CUSTF_NO_EXT_APPMSG	(1<<10)	// Only supports drag'n'drop form lister or desktop-folder

#define CUSTH_TYPE_GFMMODULE	MAKE_ID('g', 'f', 'm', '_')


struct title_entry
{
	DirEntry		entry;
	char			buf[2];
	struct TagItem		tags[5];
	NetworkInfo		network;
	VersionInfo		version;
};

#endif

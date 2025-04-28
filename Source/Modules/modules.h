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

#ifndef _GALILEOFM_MODULES
#define _GALILEOFM_MODULES

#include "/Include/galileo/os.h"

#include "/Library/galileofmbase.h"
#include "modules_pragmas.h"

typedef struct
{
	ULONG	id;		// Function ID code
	char	*name;          // Function name
	ULONG	desc;           // Locale string ID for function description
	ULONG	flags;		// Function flags
	char	*template;	// Command template
} ModuleFunction;

#ifndef _GALILEOFM_FUNCTION_LAUNCH

typedef struct _PathNode
{
	struct MinNode		pn_node;
	char			pn_path_buf[512];
	char			*pn_path;
	APTR			pn_lister;
	ULONG			pn_flags;
} PathNode;

typedef struct _FunctionEntry
{
	struct MinNode		fe_node;
	char			*fe_name;
	struct DirEntry		*fe_entry;
	short			fe_type;
	short			fe_flags;
} FunctionEntry;

#define LISTNF_UPDATE_STAMP	(1<<5)	// Update datestamp
#define LISTNF_RESCAN		(1<<6)	// Rescan this lister

#endif

// Function flags
#define FUNCF_NEED_SOURCE		(1<<0)	// Needs a source directory
#define FUNCF_NEED_DEST			(1<<1)	// Needs a destination directory
#define FUNCF_NEED_FILES		(1<<2)	// Needs some files to work with
#define FUNCF_NEED_DIRS			(1<<3)	// Needs some files to work with
#define FUNCF_NEED_ENTRIES	(FUNCF_NEED_FILES|FUNCF_NEED_DIRS)
#define FUNCF_CAN_DO_ICONS		(1<<6)	// Function can do icons
#define FUNCF_SINGLE_SOURCE		(1<<8)	// Only a single source needed
#define FUNCF_SINGLE_DEST		(1<<9)	// Only a single destination needed
#define FUNCF_NO_LOOP_SOURCE	(1<<10)	// Don't loop through sources
#define FUNCF_WANT_DEST			(1<<11)	// Want destinations, don't need them
#define FUNCF_WANT_SOURCE		(1<<12)	// Want source, don't need it
#define FUNCF_CREATE_SOURCE		(1<<13)	// Can create our own source

#define FUNCF_WANT_ENTRIES		(1<<19)	// Want entries
#define FUNCF_NO_ARGS			(1<<20)	// Don't pass arguments
#define FUNCF_NO_BUSY			(1<<21)	// Don't send lister busy
#define FUNCF_SYNC				(1<<24)	// Function wants to be synchronous

typedef struct
{
	ULONG		ver;		// Module version
	char		*name;		// Module name
	char		*locale_name;	// Catalog name
	ULONG		flags;		// Module flags
	ULONG		function_count;	// Number of functions in module
	ModuleFunction	function[1];	// Definition of first function
} ModuleInfo;

#define MODULEF_CALL_STARTUP		(1<<0)	// Call ModuleEntry() on startup
#define MODULEF_STARTUP_SYNC		(1<<1)	// Run Synchronously on startup
#define MODULEF_CATALOG_VERSION		(1<<2)	// Use version for catalog
#define MODULEF_HELP_AVAILABLE		(1<<3)	// Help is available

#define FUNCID_STARTUP			0xffffffff

int		Module_Entry(struct List *,struct Screen *,IPCData *,IPCData *,ULONG,ULONG);
ModuleInfo	*__asm __saveds Module_Identify(register __d0 int num);

// Callback commands
enum
{
	EXTCMD_GET_SOURCE,		// Get current source path
	EXTCMD_NEXT_SOURCE,		// Get next source path
	EXTCMD_UNLOCK_SOURCE,		// Unlock source paths
	EXTCMD_GET_ENTRY,		// Get entry
	EXTCMD_END_ENTRY,		// End entry
	EXTCMD_RELOAD_ENTRY,		// Reload entry
	EXTCMD_OPEN_PROGRESS,		// Open progress indicator
	EXTCMD_UPDATE_PROGRESS,		// Update progress indicator
	EXTCMD_CLOSE_PROGRESS,		// Close progress indicator
	EXTCMD_CHECK_ABORT,		// Check abort status
	EXTCMD_ENTRY_COUNT,		// Get entry count
	EXTCMD_GET_WINDOW,		// Get window handle
	EXTCMD_GET_DEST,		// Get next destination
	EXTCMD_END_SOURCE,		// Cleanup current source path
	EXTCMD_END_DEST,		// Cleanup current destination path
	EXTCMD_ADD_CHANGE,		// Add a change to a lister
	EXTCMD_ADD_FILE,		// Add a file to a lister
	EXTCMD_GET_HELP,		// Get help on a topic
	EXTCMD_GET_PORT,		// Get ARexx port name
	EXTCMD_GET_SCREEN,		// Get public screen name
	EXTCMD_REPLACE_REQ,		// Show exists/replace? requester
	EXTCMD_REMOVE_ENTRY,		// Mark an entry for removal
	EXTCMD_GET_SCREENDATA,		// Get Galileo screen data
	EXTCMD_FREE_SCREENDATA,		// Free screen data      23
	EXTCMD_CREATE_FILE_ENTRY,	// Create a file entry   24
	EXTCMD_FREE_FILE_ENTRY,		// Free a file entry     25
	EXTCMD_SORT_FILELIST,		// Sort an entire list of file entries    26
	EXTCMD_NEW_LISTER,		// Open a new lister     27
	EXTCMD_GET_POINTER,		// Get a pointer         28
	EXTCMD_FREE_POINTER,		// Free a pointer        29
	EXTCMD_SEND_COMMAND,		// Send a command to Galileo
	EXTCMD_DEL_FILE,		// Delete a file from a lister
	EXTCMD_DO_CHANGES,		// Perform changes
	EXTCMD_LOAD_FILE,		// Load files to listers
};

struct progress_packet
{
	struct _PathNode	*path;
	char			*name;
	ULONG			count;
};

struct endentry_packet
{
	struct _FunctionEntry	*entry;
	BOOL			deselect;
};

struct addchange_packet
{
	struct _PathNode	*path;
	USHORT			change;
	APTR			data;
};

struct addfile_packet
{
	char			*path;
	struct FileInfoBlock	*fib;
	struct ListerWindow	*lister;
};

struct delfile_packet
{
	char			*path;
	char			*name;
	struct ListerWindow	*lister;
};

struct loadfile_packet
{
	char			*path;
	char			*name;
	short			flags;
	short			reload;
};

struct sortlist_packet
{
	struct _PathNode	*path;
	struct List		*list;
	long			file_count;
	long			dir_count;
};

struct replacereq_packet
{
	struct Window		*window;
	struct Screen		*screen;
	IPCData			*ipc;
	struct FileInfoBlock	*file1;
	struct FileInfoBlock	*file2;
	short			default_option;
};

struct GalileoScreenData
{
	struct Screen		*screen;
	struct DrawInfo		*draw_info;
	unsigned short		depth;
	unsigned short		pen_alloc;
	unsigned short		pen_array[16];
	short			pen_count;
};	


ULONG __asm __saveds function_external_hook(
	register __d0 ULONG command,
	register __a0 struct _FunctionHandle *handle,
	register __a1 APTR packet);

#define EXT_FUNC(name)	unsigned long __asm (*name)(register __d0 ULONG,register __a0 APTR,register __a1 APTR)
#define TYPE_EXT(var)	(unsigned long (*)())var

#define IDCMP_FUNC(name)	unsigned long __asm (*name)(register __d0 ULONG,register __a0 struct IntuiMessage *)

#define SHOWF_SELECTED	(1<<0)
#define SHOWF_DELETE	(1<<1)

enum
{
	MODPTR_OPTIONS,
	MODPTR_FILETYPES,
	MODPTR_HANDLE,
	MODPTR_DEFFORMAT,
	MODPTR_COMMANDS,
	MODPTR_SCRIPTS,
};

struct pointer_packet
{
	ULONG		type;
	APTR		pointer;
	ULONG		flags;
};

#define POINTERF_COPY		(1<<0)
#define POINTERF_DELPORT    (1<<1)
#define POINTERF_LOCKED		(1<<16)

struct command_packet
{
	char		*command;
	ULONG		flags;
	char		*result;
	ULONG		rc;
};

#define COMMANDF_RESULT		(1<<0)
#define COMMANDF_RUN_SCRIPT	(1<<1)

#ifndef DEF_GALILEOFMCOMMANDLIST
#define DEF_GALILEOFMCOMMANDLIST

struct GalileoCommandList
{
	struct Node	gcl_Node;
	ULONG		gcl_Flags;
	char		*gcl_Description;
	char		*gcl_Template;
	char		*gcl_Module;
	char		*gcl_Help;
};

#endif

#endif

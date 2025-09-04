/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 1993-2012 Jonathan Potter & GP Software
Copyright 2023,2025 Hagbard Celine

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

#ifndef _GALILEOFM_FUNCTION_LAUNCH
#define _GALILEOFM_FUNCTION_LAUNCH

#include "buttons.h"
#include "dirlist.h"
#include "app_msg.h"

#define COMMAND_LINE_LEN	512

enum
{
	FUNCTION_READ_DIRECTORY,		// Read a directory
	FUNCTION_BUFFERLIST,			// Generate list of buffers
	FUNCTION_DEVICELIST,			// Generate list of devices
	FUNCTION_TREE,				// Generate directory tree
	FUNCTION_RUN_FUNCTION,			// Run a function
	FUNCTION_FILETYPE,			// Do filetype action on a file
	FUNCTION_RUN_FUNCTION_EXTERNAL,		// Run with external args
	FUNCTION_GET_ICONS,			// Get icons
};

typedef struct _FunctionEntry
{
	struct MinNode		fe_node;
	char			*fe_name;
	DirEntry		*fe_entry;
	short			fe_type;
	short			fe_flags;
	BPTR			fe_lock;
	long			fe_size;
} FunctionEntry;

typedef struct
{
	struct MinNode		een_node;
	short			een_type;
	short			een_flags;
	DirEntry		*een_entry;
	char			*een_name;
	BPTR			een_lock;
	char			een_path[1];
} ExternalEntry;

typedef struct _InstructionParsed
{
	struct MinNode		ipa_node;
	struct _CommandList	*ipa_command;
	short			ipa_type;
	unsigned char		*ipa_string;
	ULONG			ipa_flags;
	char			ipa_inst_data[256];
	ULONG			ipa_inst_flags;
	FuncArgs		*ipa_funcargs;
	short			ipa_new_arg;
	unsigned short		ipa_count;
} InstructionParsed;

typedef struct _AsyncData
{
    char 			*args;
    struct Screen 		*screen;
    IPCData 			*main_ipc;
    struct _CommandList		*command;
    struct Library 		*module;
    struct Library  		*library;
    APTR            		*memhandlep;
    struct _FunctionHandle	*handle;
}  AsyncData;

#define FUNCENTF_UNSELECT		(1<<0)	// Deselect this entry
#define FUNCENTF_REMOVE			(1<<1)	// Remove this entry
#define FUNCENTF_RECURSE		(1<<2)	// Entry collected recursively
#define FUNCENTF_FAKE			(1<<3)	// Fake entry
#define FUNCENTF_ENTERED		(1<<4)	// Entered this entry
#define FUNCENTF_EXITED			(1<<5)	// Exited this entry
#define FUNCENTF_ICON			(1<<6)	// File is an icon
#define FUNCENTF_TOP_LEVEL		(1<<7)	// Top level entry
#define FUNCENTF_RELOAD			(1<<8)	// Reload this entry
#define FUNCENTF_ICON_ACTION		(1<<9)
#define FUNCENTF_ICON_ONLY		(1<<10)
#define FUNCENTF_LINK			(1<<11)	// Link
#define FUNCENTF_FAKE_ICON		(1<<12)
#define FUNCENTF_ASSIGN			(1<<13) // Assign to directory
#define FUNCENTF_MULTI_ASSIGN		(1<<14) // Assign to directory
#define FUNCENTF_LINK_FILE		(1<<15)

typedef struct _PathNode
{
	struct MinNode		pn_node;
	char			*pn_path_buf;
	char			*pn_path;
	Lister			*pn_lister;
	ULONG			pn_flags;
	BPTR			pn_lock;
	BPTR			pn_recurse_lock;
	STRPTR			pn_alt_path;
	BPTR			pn_alt_lock;
} PathNode;

#define LISTNF_INVALID		(1<<0)	// List is invalid
#define LISTNF_NO_TITLE		(1<<1)	// Don't update title at finish
#define LISTNF_LOCKED		(1<<2)	// List has been locked
#define LISTNF_SAME		(1<<3)	// Same as the source
#define LISTNF_CHANGED		(1<<4)	// Use internal buffer
#define LISTNF_UPDATE_STAMP	(1<<5)	// Update datestamp
#define LISTNF_RESCAN		(1<<6)	// Rescan this lister
#define LISTNF_NO_REFRESH	(1<<7)	// Don't refresh this lister
#define LISTNF_MADE_LOCK	(1<<8)	// Temp lock made for PathNode

typedef struct
{
	struct MinList		list;
	PathNode		*current;
} PathList;

typedef struct
{
	struct Node		node;
	struct MinList		files;
	Lister			*lister;
	BPTR			lock;
	short			count;
	char			path[1];
} FileChangeList;

typedef struct
{
	struct Node		node;
	short			count;
	ULONG			flags;
	long			fib_Size;
	short			fib_DirEntryType;
	struct DateStamp	fib_Date;
	unsigned long		fib_Protection;
	char			*fib_Comment;
	NetworkInfo		*fib_Network;
	char			fib_FileName[1];
} FileChange;

#define FCTYPE_ADD		1
#define FCTYPE_DEL		2
#define FCTYPE_RELOAD		3
#define FCTYPE_MODIFY		4
#define FCTYPE_LOAD		8
#define FCTYPE_RENAME		16

#define FCF_SHOW	1
#define FCF_SELECT	2

typedef struct _FunctionHandle
{
    IPCData			*ipc;			// IPC data
    APTR			memory;			// Function memory pool

    ULONG			command;		// Function command
    Cfg_Function		*function;		// Function pointer
    ULONG			data;			// Function data
    ULONG			flags;			// Function flags

    char			*func_source_path;	// Source path
    char			*func_dest_path;	// Destination path

    int				entry_count;		// Total number of entries
    struct List			entry_list;		// Entries to work on
    FunctionEntry		*current_entry;		// Current entry
    FunctionEntry		*use_entry;		// Entry to use

    int				arg_count;		// Argument count
    struct ArgArray		*args;			// Arguments
    ULONG			result;			// Result code

    ULONG			func_flags;		// Function flags
    struct MinList		func_instructions;	// Function instructions (parsed)
    CFG_FUNC			func_parameters;	// Function parameters

    struct List			func_arguments;		// Function arguments (user supplied)
    struct Node			*func_current_arg;	// Current function argument

    short			func_iterations;	// Iteration count
    short			func_source_num;	// Source number we are doing

    struct List			recurse_list;		// List of files collected recursively
    FunctionEntry		*current_recurse;	// Current recursively-collected file

    struct AnchorPath		*anchor;	        // Anchor for recursing directories
    char			*anchor_path;		// Full pathname for anchor
    int				recurse_return;		// Result code from last return
    FunctionEntry		*recurse_entry;		// Recursively collected entry
    FunctionEntry		*recurse_entry_data;

    BPTR			recurse_cd;		// Current dir for recursing

    ULONG			temp_key;		// Key for temp files

    ULONG			special_flags;		// Special flags

    InstructionParsed		*cur_instruction;	// Currently executing instruction

    char			recurse_path[512];	// Pathname of recursed entry
    int				recurse_count;		// Count of recursed files
    ULONG			recurse_bytes;		// Size of recursed files
    int				recurse_depth;		// Recurse depth

    char			func_work_buf[1024];	// A buffer to do things in

    PathList			func_source_paths;	// List of source paths
    PathList			func_dest_paths;	// List of destination paths

    struct FileInfoBlock				// Some FileInfoBlocks to play with
				*s_info,
				*d_info;

    ULONG			instruction_flags;	// Flags for this instruction
    char			final_message[80];	// Final message to display
    APTR			entry_memory;		// Entry memory pool

    char			*inst_data;		// Instruction-specific data pointer
    ULONG			inst_flags;		// Instruction-specific flags

    struct FileInfoBlock	recurse_info;		// Info on last recursed entry

    char			*last_filename;		// Last filename used

    APTR			script_file;		// External script file
    char			script_name[80];	// Script file name
    char			temp_buffer[256];	// Temporary buffer
    char			temp_name[80];		// Temporary file name

    struct MinList		external_list;		// List of external entries

    Lister			*source_lister;		// Source lister
    Lister			*dest_lister;		// Dest lister

    unsigned short		dest_block_size;	// Destination block size
    unsigned short		dest_data_block_size;	// Destination data block size
    unsigned long		dest_recurse_blocks;	// Number of blocks for destination

    FuncArgs			*funcargs;		// Parsed function arguments
    struct List			entry_list_backup;
    FunctionEntry		*current_entry_backup;
    FunctionEntry		*use_entry_backup;
    int				entry_count_backup;

    Lister			*progress_lister;	// Lister with progress req

    struct Message		*done_msg;		// Message to send when done

    char			file_filter[31];	// File filter
    char			got_filter;
    char			file_filter_parsed[63];

    char			replace_option;

    unsigned short		file_count;		// File and dir counts
    unsigned short		dir_count;
    unsigned short		file_count_backup;
    unsigned short		dir_count_backup;

    short			instruction_count;	// Number of instructions

    char			output_handle[120];	// Output handle

    ULONG			result_flags;		// Flags for result

    struct MsgPort		*reply_port;

    Buttons			*func_buttons;		     // Button bank launched from

    struct FileRequester	*filereq;		// File requester
    struct IBox			coords;			// Coordinates

    struct AppMessage		*app_msg;		// AppMessage from drag&drop
    GalileoListerAppMessage     *lister_app_msg;	// Internal drag&drop

    struct List			filechange;		// File changes

    ULONG			flags2;			// More flags

    struct ArgArray		*arg_passthru;		// Arguments passed through

    unsigned short		link_dir_count;
    unsigned short		link_file_count;

    Lister			*saved_source_lister;	// Source lister

    short			ret_code;

    ULONG			func_additional_flags;

    BPTR			func_source_lock;	// Source path
    BPTR			func_dest_lock;		// Destination path
    BPTR			func_drop_on_lock;	// Drag'n'drop destination, must unlock
} FunctionHandle;


typedef struct _FunctionReturn {
	int	rc;
	char	*result;
} FunctionReturn;


// Function flags
#define FUNCF_NEED_SOURCE		(1<<0)	// Needs a source directory
#define FUNCF_NEED_DEST			(1<<1)	// Needs a destination directory
#define FUNCF_NEED_FILES		(1<<2)	// Needs some files to work with
#define FUNCF_NEED_DIRS			(1<<3)	// Needs some files to work with
#define FUNCF_NEED_ENTRIES		(FUNCF_NEED_FILES|FUNCF_NEED_DIRS)
/* DO NOT define flag with value (1<<4) here, it will clash with FUNCF_RUN_ASYNC */
#define FUNCF_SCRIPT_OVERRIDE		(1<<5)	// Can be overridden by a script
#define FUNCF_CAN_DO_ICONS		(1<<6)	// Function can do icons
#define FUNCF_SINGLE_SOURCE		(1<<8)	// Only a single source needed
#define FUNCF_SINGLE_DEST		(1<<9)	// Only a single destination needed
#define FUNCF_WBARG_PASSTRUGH           (1<<10)
#define FUNCF_WANT_DEST			(1<<11)	// Want destinations, don't need them
#define FUNCF_WANT_SOURCE		(1<<12)	// Want source, don't need it
#define FUNCF_CREATE_SOURCE		(1<<13)	// Can create our own source
#define FUNCF_EXTERNAL_FUNCTION		(1<<14)	// Function is external
#define FUNCF_ARG_ENTRIES		(1<<15) // Entries in arguments
#define FUNCF_ASK_FILTER		(1<<16)	// Can ask for a file filter
#define FUNCF_ASK_PATH			(1<<17)	// Allowed to ask for paths
#define FUNCF_SPECIAL_OK		(1<<18)	// Ok if special buffer
#define FUNCF_WANT_ENTRIES		(1<<19)	// Want entries
#define FUNCF_NO_SOURCE			(1<<20)	// Can't use lister sources
#define FUNCF_RESCAN_DESKTOP		(1<<21)	// Rescan the desktop

#define FUNCF_ASK_DEST			(1<<22)	// Must ask for destination
#define FUNCF_ICONS			(1<<23)	// Function running on icons
#define FUNCF_SYNC			(1<<24)	// Function wants to be synchronous
#define FUNCF_DRAG_DROP			(1<<25)	// Called by drag'n'drop
#define FUNCF_PRIVATE			(1<<26) // Function is private

#define FUNCF_MSG_QUIT			(1<<27)	// Send message when done

#define FUNCF_RUN_NO_ICONS		(1<<28)	// Don't run using icons (double-click kludge)
#define FUNCF_COPY_NO_MOVE		(1<<28)	// Don't move even if on same volume (drag'n'drop kludge)

#define FUNCF_DROPON_LOCK		(1<<29) // Drag'n'drop on entry, must unlock supplied destination lock
#define FUNCF_LAST_FILE_FLAG		(1<<29)	// Still using last file
#define FUNCF_GOT_SOURCE		(1<<30)	// Got a source
#define FUNCF_GOT_DEST			(1<<31)	// Got a destination

#define FUNCF_ACTIVATE_ME		(1<<0)	// Want to be activated


// Result flags
#define FRESULTF_RESORT			(1<<0)	// Need to resort
#define FRESULTF_MADE_LISTER		(1<<1)	// We made a lister

// Instruction flags
#define INSTF_RECURSE_DIRS		(1<<0)	// Enter directories recursively
#define INSTF_WANT_DIRS			(1<<1)	// Want directories as well as the contents
#define INSTF_WANT_DIRS_END		(1<<2)	// Want directories at the end
#define INSTF_DELETE_UNPROTECT		(1<<3)	// Unprotect all
#define INSTF_DIR_NO_SIZES		(1<<4)	// Don't count directory sizes
#define INSTF_DIR_CLEAR_SIZES		(1<<5)	// Clear directory sizes
#define INSTF_REPEAT			(1<<6)	// Repeating this instruction
#define INSTF_ABORT_DIR			(1<<7)	// Abort this directory

// Other flags
#define INSTF_UNPROTECT_ALL		(1<<0)	// Unprotect all
#define INSTF_RECURSE			(1<<1)	// Recurse directories
#define INSTF_NO_RECURSE		(1<<2)	// Don't recurse directories
#define INSTF_NO_ASK			(1<<3)	// Don't need to ask

// Copy
#define COPYF_DELETE_ALL		(1<<0)
#define COPYF_UNPROTECT_ALL		(1<<1)
#define COPYF_SKIP_ALL			(1<<2)
#define COPYF_RENAME			(1<<3)
#define COPYF_ICON_COPY			(1<<4)

#define PARSE_MORE_FILES		1
#define PARSE_END			0
#define PARSE_OK			0
#define PARSE_ABORT			-1
#define PARSE_INVALID			-2


#define FM_Size		( TAG_USER + 0x1 )
#define FM_Date		( TAG_USER + 0x2 )
#define FM_Protect	( TAG_USER + 0x3 )
#define FM_Comment	( TAG_USER + 0x4 )
#define FM_Name		( TAG_USER + 0x5 )

#define FMF_SIZE	(1<<0)
#define FMF_DATE	(1<<1)
#define FMF_PROTECT	(1<<2)
#define FMF_COMMENT	(1<<3)
#define FMF_NAME	(1<<4)


#define FFLF_ICON	(1<<0)
#define FFLF_RELOAD	(1<<1)
#define FFLF_DEFERRED	(1<<2)

#endif

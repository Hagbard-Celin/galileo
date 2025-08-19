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

#ifndef _GALILEOFM_REXX
#define _GALILEOFM_REXX

#include "function_launch.h"

#define RXERRORIMGONE	100	// Error to indicate shutting down
#define RXERRORNOCMD	30	// Error to indicate invalid command

#define RX_STRING	0
#define RX_LONG		1
#define RX_CHAR		2

#define RXENTRY_FILE			-1
#define RXENTRY_FILE_COLOUR		-2
#define RXENTRY_FILE_LINK		-3
#define RXENTRY_FILE_COLOUR_LINK	-4

#define RXENTRY_DIR			1
#define RXENTRY_DIR_COLOUR		2
#define RXENTRY_DIR_LINK		3
#define RXENTRY_DIR_COLOUR_LINK		4

#define KEY_NONE		0
#define KEY_ADD			1
#define KEY_SUB			-1
#define KEY_TOGGLE		-2

#define RXMF_WARN		(1<<0)
#define RXMF_NO_INC		(1<<1)
#define RXMF_SYNC		(1<<2)

typedef struct
{
	struct Message	msg;
	struct RexxMsg	*rmsg;
} RexxSyncMessage;

typedef struct
{
	char	*command;
	short	id;
} RexxCommandList;

typedef struct
{
	struct RexxMsg	*msg;
	long		command;
	struct Window	*window;
	char		args[1];
} RexxRequester;

extern RexxCommandList rexx_commands[];
extern short rexx_commands_can_do_all[];
extern char
	*sort_method_strings[],
	*separate_method_strings[],
	*sort_flags_strings[],
	*full_string[],
	*quick_string[],
	*on_off_strings[],
	*on_off_strings2[],
	*force_string[],
	*getfiletype_keys[],
	*custhandler_keys[],
	*port_keys[],
	*all_keys[],
	*new_lister_keys[],
	*mode_keys[],
	*quit_keys[],
	*reader_keys[],
	*command_keys[],
	*pos_keys[],
	*seticon_keys[],
	*app_commands[],
	*progress_keys[],
	*secure_keys[],
	*excommand_keys[],
	*lock_keys[],
	*wait_keys[],
	*move_keys[],
	*update_key[],
	*background_key[],
	*custom_key[],
	*precision_key[],
	*pens_key[],
	*font_key[];

extern unsigned short rexx_error_table[];

enum
{
	RXCMD_BASE,

	RXCMD_LISTER,
	RXCMD_NEW,
	RXCMD_CLOSE,
	RXCMD_QUERY,
	RXCMD_SET,
	RXCMD_ADD,
	RXCMD_REMOVE,
	RXCMD_SELECT,
	RXCMD_REFRESH,
	RXCMD_CLEAR,
	RXCMD_EMPTY,
	RXCMD_WAIT,
	RXCMD_PATH,
	RXCMD_POSITION,
	RXCMD_BUSY,
	RXCMD_HANDLER,
	RXCMD_VISIBLE,
	RXCMD_FILES,
	RXCMD_DIRS,
	RXCMD_ENTRIES,
	RXCMD_SELFILES,
	RXCMD_SELDIRS,
	RXCMD_SELENTRIES,
	RXCMD_ENTRY,
	RXCMD_NUMFILES,
	RXCMD_NUMDIRS,
	RXCMD_NUMENTRIES,
	RXCMD_NUMBYTES,
	RXCMD_NUMSELFILES,
	RXCMD_NUMSELDIRS,
	RXCMD_NUMSELENTRIES,
	RXCMD_NUMSELBYTES,
	RXCMD_SORT,
	RXCMD_SEPARATE,
	RXCMD_DISPLAY,
	RXCMD_FLAGS,
	RXCMD_SOURCE,
	RXCMD_DEST,
	RXCMD_ALL,
	RXCMD_SHOW,
	RXCMD_HIDE,
	RXCMD_TITLE,
	RXCMD_READ,
	RXCMD_COPY,
	RXCMD_GALILEO,
	RXCMD_BACK,
	RXCMD_FRONT,
	RXCMD_PROGRESS,
	RXCMD_COUNT,
	RXCMD_NAME,
	RXCMD_ABORT,
	RXCMD_FIRSTSEL,
	RXCMD_GETSTRING,
	RXCMD_REQUEST,
	RXCMD_LOCK,
	RXCMD_OFF,
	RXCMD_GETFILETYPE,
	RXCMD_COMMAND,
	RXCMD_ADDTRAP,
	RXCMD_REMTRAP,
	RXCMD_ICONIFY,
	RXCMD_VERSION,
	RXCMD_TOOLBAR,
	RXCMD_MODE,
	RXCMD_SCREEN,
	RXCMD_CASE,
	RXCMD_NAMELENGTH,
	RXCMD_ERROR,
	RXCMD_ADDAPPICON,
	RXCMD_REMAPPICON,
	RXCMD_HEXREAD,
	RXCMD_ANSIREAD,
	RXCMD_SMARTREAD,
	RXCMD_NEWPROGRESS,
	RXCMD_LABEL,
	RXCMD_SETAPPICON,
	RXCMD_ADD_STEM,
	RXCMD_HEADER,
	RXCMD_FIELD,
	RXCMD_SEND,
	RXCMD_RX,
	RXCMD_MOVEMENT,
	RXCMD_RELOAD,
	RXCMD_WINDOW,
	RXCMD_FINDCACHE,
	RXCMD_FREECACHES,
	RXCMD_CHECKDESKTOP,
	RXCMD_GETDESKTOP,
	RXCMD_DEFAULT,
	RXCMD_PROC,
	RXCMD_ACTIVE,
	RXCMD_SCRIPT,
	RXCMD_DESKTOPPOPUP,
	RXCMD_BACKGROUND,
	RXCMD_SOUND,
	RXCMD_PENS,
	RXCMD_PALETTE,
	RXCMD_FONT,
	RXCMD_ICONS,
	RXCMD_MATCHDESKTOP,
	RXCMD_VALUE,
	RXCMD_COMMENTLENGTH,
	RXCMD_SOURCEDESTLOCK,
	RXCMD_ASSIGN,
	RXCMD_GET,
	RXCMD_FREE
};

enum
{
	RXSORTF_REVERSE,
	RXSORTF_NOICONS,
	RXSORTF_HIDDEN,
};

enum
{
	RXCOM_WAIT,
	RXCOM_SOURCE,
	RXCOM_DEST,
	RXCOM_ORIGINAL,
};

enum
{
	RXPROG_NAME,
	RXPROG_FILE,
	RXPROG_INFO,
	RXPROG_BAR,
	RXPROG_ABORT,
	RXPROG_TITLE,
	RXPROG_OFF,
	RXPROG_INFO2,
	RXPROG_INFO3,
};

// Error codes (RC)
#define RXERR_OK		0
#define RXERR_FILE_REJECTED	1	// File was rejected
#define RXERR_INVALID_QUERY	5	// Query item invalid
#define RXERR_INVALID_SET	5	// Set item invalid
#define RXERR_INVALID_NAME	6	// Name invalid
#define RXERR_INVALID_KEYWORD	6	// Keyword invalid
#define RXERR_INVALID_TRAP	8	// Trap invalid
#define RXERR_INVALID_HANDLE	10	// Lister handle invalid
#define RXERR_NO_TOOLBAR	12	// No toolbar
#define RXERR_INVALID_PATH	13	// Bad path
#define RXERR_INVALID_FILE	14	// Bad file
#define RXERR_NO_MEMORY		15	// No memory to do something
#define RXERR_NO_LISTER		20	// Lister couldn't open


// Function trap
typedef struct _FunctionTrap
{
	struct MinNode		node;
	char			*command;	// Command name
	char			handler[32];	// Custom handler
	ULONG			flags;
	ULONG			pad[4];
} FunctionTrap;

#define FTRAPF_SIGNAL		(1<<0)

typedef struct _TrapSignal
{
	char			port_name[40];
	short			signal;
} TrapSignal;


typedef struct
{
	struct MinNode		node;
	IPCData			*ipc;
} RexxReader;

typedef struct
{
	struct Node		node;
	APTR			app_thing;
	struct DiskObject	*icon;
	ULONG			id;
	char			icon_name[40];
	char			port_name[80];
	long			pos_x;
	long			pos_y;
	unsigned long		flags;
	struct DiskObject	*busy_icon;
} RexxAppThing;

typedef struct
{
	struct Node		node;
	APTR			progress;
} RexxProgress;

enum
{
	REXXAPP_ICON,
	REXXAPP_PROGRESS,

	REXXAPP_ALL=100
};

#define RATF_QUOTES		(1<<0)	// Quoted filenames
#define RATF_INFO		(1<<1)	// Supports Information
#define RATF_SNAP		(1<<2)	// Supports Snapshot
#define RATF_CLOSE		(1<<3)	// Close instead of Open
#define RATF_LOCAL		(1<<4)	// Local only
#define RATF_LOCKED		(1<<5)	// Locked

enum
{
	REXXAPPCMD_QUIT,
	REXXAPPCMD_DOUBLECLICK,
	REXXAPPCMD_DRAGNDROP,
	REXXAPPCMD_SNAPSHOT,
	REXXAPPCMD_UNSNAPSHOT,
	REXXAPPCMD_CLOSE,
	REXXAPPCMD_INFO,
	REXXAPPCMD_MENU,
	REXXAPPCMD_MENUHELP,
};

enum
{
	READCOM_BASE,
	READCOM_READ,
};


#define HA_String	0x01
#define HA_Value	0x02
#define HA_Qualifier	0x03

typedef struct
{
	ULONG	ha_Type;
	ULONG	ha_Arg;
	ULONG	ha_Data;
} HandlerArg;



typedef struct
{
	struct MinNode	node;
	struct RexxMsg	*rx_msg;
	IPCMessage	*ipc_msg;
} RexxMsgTracker;


typedef struct
{
	char		handler[40];
	struct RexxMsg	*rx_msg;
} RexxDespatch;


enum
{
	SETHF_QUOTES,
	SETHF_FULLPATH,
	SETHF_EDITING,
	SETHF_NOPOPUPS,
	SETHF_GAUGE,
	SETHF_SUBDROP,
	SETHF_INHERIT,
	SETHF_LEAVEGAUGE,
	SETHF_SYNCTRAPS,
};

#define IEQUALIFIER_SUBDROP	0x400

#endif

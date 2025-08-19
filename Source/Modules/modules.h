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

#ifndef _GALILEOFM_MODULES
#define _GALILEOFM_MODULES

#include "/Include/galileo/os.h"

#include "/Library/galileofmbase.h"

typedef struct
{
	ULONG	id;		// Function ID code
	char	*name;          // Function name
	ULONG	desc;           // Locale string ID for function description
	ULONG	flags;		// Function flags
	char	*template;	// Command template
} ModuleFunction;


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

#define ENTRYF_RECURSE_DIRS		(1<<0)

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


struct GalileoScreenData
{
	struct Screen		*screen;
	struct DrawInfo		*draw_info;
	unsigned short		depth;
	unsigned short		pen_alloc;
	unsigned short		pen_array[16];
	short			pen_count;
};	

#define EXT_FUNC(name)	unsigned long (* __asm name)(register __d0 ULONG,register __a0 APTR,register __a1 APTR)
#define TYPE_EXT(var)	(unsigned long (*)())var

#define IDCMP_FUNC(name)	unsigned long (* __asm name)(register __d0 ULONG,register __a0 struct IntuiMessage *)

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

#define GCNLF_ICONIFIED		  (1<<0)	  // Lister opens iconified
#define GCNLF_DEVICE_LIST	  (1<<1)	  // Lister shows device list
#define GCNLF_NO_POSITION	  (1<<2)	  // No position set
#define GCNLF_LOCK_POS		  (1<<3)	  // Position is locked
#define GCNLF_SOURCE		  (1<<4)	  // Lister starts as a source
#define GCNLF_DEST			  (1<<5)	  // Starts as destination
#define GCNLF_LOCKED		  (1<<6)	  // Source/dest locked
#define GCNLF_ICON			  (1<<7)	  // View as icons
#define GCNLF_SHOW_ALL		  (1<<8)	  // Show all
#define GCNLF_CACHE_LIST	  (1<<9)	  // Cache list
#define GCNLF_ICON_ACTION	  (1<<10)	  // Icon action
#define GCNLF_DEV_FULL		  (1<<11)	  // Full device list
#define GCNLF_DEV_BRIEF		  (1<<12)	  // Brief device list
#define GCNLF_INVISIBLE		  (1<<13)	  // Opens invisible
#define GCNLF_NOACTIVE		  (1<<14)	  // No activate
#define GCNLF_FROMICON        (1<<31)

#endif

#endif

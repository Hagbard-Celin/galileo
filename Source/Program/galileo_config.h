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

#ifndef _GALILEOFM_CONFIGURATION
#define _GALILEOFM_CONFIGURATION

#ifndef _GALILEOFM_LISTFORMAT_H
#include <gfm/listformat.h>
#endif
#ifndef _GALILEOFM_CONFIG_COMMAND_H
#include <gfm/config_command.h>
#endif
#ifndef _GALILEOFM_CONFIG_FLAGS_H
#include <gfm/config_flags.h>
#endif
#ifndef _GALILEOFM_CFG_BUTTONBANK_H
#include <gfm/cfg_buttonbank.h>
#endif
#ifndef _GALILEOFM_CFG_ENVIRONMENT_H
#include <gfm/cfg_environment.h>
#endif

#define CONFIG_VERSION_3 	2
#define CONFIG_VERSION_2	3
#define CONFIG_VERSION_4	4
#define CONFIG_VERSION_5	5
#define CONFIG_VERSION_6	6
#define CONFIG_VERSION_7	7
#define CONFIG_VERSION_8	8
#define CONFIG_VERSION_9	9
#define CONFIG_VERSION_10	10
#define CONFIG_VERSION_11	11
#define CONFIG_VERSION_12	12
#define CONFIG_VERSION_13	13

#define SORT_REVERSE	1

#define LFORMATF_REJECT_ICONS	(1<<0)	// Reject icons
#define LFORMATF_HIDDEN_BIT	(1<<1)	// Respect the H bit
#define LFORMATF_ICON_VIEW	(1<<2)	// Default to icon view
#define LFORMATF_SHOW_ALL	(1<<3)	// Show all
#define LFORMATF_TEMP_ICONS	(1<<4)
#define LFORMATF_GAUGE		(1<<5)	// Free space gauge
#define LFORMATF_INHERIT	(1<<6)	// Inherit format

enum
{
    DISPLAY_NAME,
    DISPLAY_SIZE,
    DISPLAY_PROTECT,
    DISPLAY_DATE,
    DISPLAY_COMMENT,
    DISPLAY_FILETYPE,
    DISPLAY_OWNER,
    DISPLAY_GROUP,
    DISPLAY_NETPROT,
    DISPLAY_VERSION,

    DISPLAY_LAST
};

enum
{
    SEPARATE_MIX,
    SEPARATE_DIRSFIRST,
    SEPARATE_FILESFIRST,
};

#define	SHOWFREE_BYTES		(1<<0)
#define	SHOWFREE_KILO		(1<<1)
#define	SHOWFREE_BLOCKS		(1<<2)
#define	SHOWFREE_PERCENT	(1<<3)


// List format storage structure
typedef struct {
    UBYTE	files_unsel[2];	    // Unselected files
    UBYTE	files_sel[2];	    // Selected files
    UBYTE	dirs_unsel[2];	    // Unselected directories
    UBYTE	dirs_sel[2];	    // Selected directories

    SortFormat	sort;		    // Sort method
    BYTE	display_pos[16];    // Item display position
    BYTE	display_len[15];    // Display length

    UBYTE	flags;

    BYTE	show_free;	    // Show free space type
    char	show_pattern[40];   // Show pattern
    char	hide_pattern[40];   // Hide pattern
} ListFormatStorage;


enum
{
    ITEM_FILENAME,
    ITEM_FILESIZE,
    ITEM_PROTECTION,
    ITEM_DATE,
    ITEM_COMMENT,
    ITEM_FILETYPE,
    ITEM_OWNER,
    ITEM_GROUP,
    ITEM_NETPROT,
    ITEM_VERSION,
};


#define STRTF_VERSION2	    (1<<8)

// HKEY - Hotkey definition
typedef struct {
    ULONG		flags;		// Flags

// Followed by name
} CFG_HKEY;


// LBUT - Lister button definition
typedef struct {
    ULONG	    flags;		// Flags
// Followed by buttons
} CFG_LBUT;





enum
{
    DESKTOP_POPUP_NONE,
    DESKTOP_POPUP_LEFTOUT,
    DESKTOP_POPUP_MOVE,
    DESKTOP_POPUP_COPY,
};

// main_window_type
enum
{
    MAINWINDOW_ICONS,
    MAINWINDOW_DEVICES,
    MAINWINDOW_EMPTY
};

typedef struct {
    struct IBox	    pos;	// Window position
    ListFormat	    format;	// Lister format
    ULONG	    flags;	// Flags

// Followed by path to load
} CFG_GLST;

#define GLSTF_ICONIFIED	    (1<<0)	// Lister opens iconified
#define GLSTF_DEVICE_LIST   (1<<1)	// Lister shows device list
#define GLSTF_NO_POSITION   (1<<2)	// No position set
#define GLSTF_LOCK_POS	    (1<<3)	// Position is locked
#define GLSTF_SOURCE	    (1<<4)	// Lister starts as a source
#define GLSTF_DEST	    (1<<5)	// Starts as destination
#define GLSTF_LOCKED	    (1<<6)	// Source/dest locked
#define GLSTF_ICON	    (1<<7)	// View as icons
#define GLSTF_SHOW_ALL	    (1<<8)	// Show all
#define GLSTF_CACHE_LIST    (1<<9)	// Cache list
#define GLSTF_ICON_ACTION   (1<<10)	// Icon action
#define GLSTF_DEV_FULL	    (1<<11)	// Full device list
#define GLSTF_DEV_BRIEF	    (1<<12)	// Brief device list
#define GLSTF_INVISIBLE	    (1<<13)	// Opens invisible
#define GLSTF_NOACTIVE	    (1<<14)	// No activate

// Error codes
#define ERROR_OLD_CONFIG    (-1)

#define ICONPOSF_APPICONS	(1<<0)
#define ICONPOSF_DISKS		(1<<1)
#define ICONPOSF_LISTERS	(1<<2)
#define ICONPOSF_GROUPS		(1<<3)
#define ICONPOSF_LEFTOUTS	(1<<4)
#define ICONPOSF_PRIORITY(x)	(((x)>>24)&0xff)
#define ICONPOSF_CLEARPRI(x)	((x)&0x00ffffff)
#define ICONPOSF_MAKEPRI(x)	((x)<<24)

/****************************************************************************
	     Application Configuration Structures
 ****************************************************************************/


// Hotkey definition
typedef struct {
    struct Node	    node;
    CFG_HKEY	    hotkey;	    // Hotkey definition
    char	    *name;	    // Hotkey name
    struct List	    function_list;  // Hotkey function
} Cfg_Hotkey;


// Lister buttons
typedef struct {
    CFG_LBUT	    header;	    // Lister buttons header
    struct List	    button_list;    // List of buttons
} Cfg_ListerButtons;



#endif

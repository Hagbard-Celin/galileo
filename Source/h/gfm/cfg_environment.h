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

#ifndef _GALILEOFM_CFG_ENVIRONMENT_H
#define _GALILEOFM_CFG_ENVIRONMENT_H

#ifndef INTUITION_INTUITION_H
#include <intuition/intuition.h>
#endif
#ifndef _GALILEOFM_LISTFORMAT_H
#include <gfm/listformat.h>
#endif

#define CUST_PENS   16

// SETS - Settings
typedef struct {
    unsigned long	copy_flags;	    // Copy flags
    unsigned long	delete_flags;	    // Delete flags
    unsigned long	error_flags;	    // Error flags
    unsigned long	general_flags;	    // General flags
    unsigned long	icon_flags;	    // Icon flags
    unsigned short	replace_method;	    // Replace method
    unsigned short	replace_flags;	    // Replace flags
    unsigned long	update_flags;	    // Update flags
    unsigned long	dir_flags;	    // Directory flags
    unsigned long	view_flags;	    // View flags
    unsigned char	hide_method;	    // Hide method
    unsigned char	pad1;
    unsigned short	pad2;
    unsigned short	max_buffer_count;   // Maximum buffer count
    unsigned short	date_format;	    // Date format
    unsigned short	date_flags;	    // Date flags
    char		pri_main[2];	    // Main priority
    char		pri_lister[2];	    // Lister priority
    unsigned long	flags;
    UWORD		pop_code;
    UWORD		pop_qual;
    UWORD		pop_qual_mask;
    UWORD		pop_qual_same;
    short		popup_delay;
    short		max_openwith;
    short		command_line_length;
    short		max_filename;
    unsigned long	pad[16];
} CFG_SETS;

// ENVR - defines the screen and other display info
typedef struct
{
    ULONG	screen_mode;			// Screen mode
    UWORD	screen_flags;			// Screen flags
    UWORD	screen_depth;			// Screen depth
    UWORD	screen_width;			// Screen width
    UWORD	screen_height;			// Screen height
    ULONG	palette[50];			// Screen palette
    char	pubscreen_name[80];		// Public screen name to open on
    struct IBox	window_pos;			// Window dimensions
    ULONG	general_screen_flags;		// General screen flags

    UBYTE	source_col[2];			// Source window colours
    UBYTE	dest_col[2];			// Destination window colours

    UWORD	palette_count;			// Number of user colours
    UWORD	pad1;

    char	font_name[4][40];		// Fonts to use

    ULONG	env_Colours[CUST_PENS][2][3];	// Custom colours
    ULONG	env_ColourFlag;			// Which custom colours are in use

    ULONG	env_NewIconsFlags;		// NewIcons flags

    ULONG	pad3[2];

    char	status_text[80];		// Status bar text

    UBYTE	font_size[4];			// Font sizes

    BYTE	lister_vert_space;		// Vertical space between lister items

    char	pad4[11];

    UWORD	display_options;		// Display options
    UWORD	main_window_type;		// Main window type

    ULONG	hotkey_flags;			// Hotkey flags
    UWORD	hotkey_code;			// Hotkey code
    UWORD	hotkey_qual;			// Hotkey qualifier

    ListFormat	list_format;			// Default list format

    char	backdrop_prefs[80];		// Backdrop pattern prefs file

    char	desktop_location[240];		// Desktop folder location

    char	output_window[80];		// Output window
    char	output_device[80];		// Output device


    unsigned long   default_stack;		// Default stack size

/*
    char	scr_title_text[120];		// Screen title text

    ULONG	pad6[17];
*/

    char	scr_title_text[188];		// Screen title text


    UWORD	lister_options;			// Lister options

    ULONG	flags;				// Misc. flags

    UWORD	lister_popup_code;		// Lister popup code
    UWORD	lister_popup_qual;		// Lister popup qualifier

    ULONG	env_flags;			// Some other flags

    unsigned short  clock_left;			// Clock left position
    unsigned short  clock_top;			// Clock top position

    UBYTE	devices_col[2];			// Source window colours
    UBYTE	volumes_col[2];			// Destination window colours

    UWORD	lister_width;			// Default lister sizes
    UWORD	lister_height;

    short	version;

    UBYTE	gauge_col[2];			// Free space gauge

    unsigned char   icon_fpen;
    unsigned char   icon_bpen;
    unsigned char   icon_style;

    char	env_NewIconsPrecision;

    unsigned long   desktop_flags;

    unsigned char   iconw_fpen;
    unsigned char   iconw_bpen;
    unsigned char   iconw_style;

    unsigned char   desktop_popup_default;

    ULONG	pad[17];			// Save recompilation

    char	env_BackgroundPic[4][256];	// Background pictures
    UWORD	env_BackgroundFlags[4];		// Background flags

    CFG_SETS	settings;
    UWORD	pad2;

    ULONG	env_BackgroundBorderColour[4];

    char	themes_location[256];		// Themes folder location

    ULONG	pad_big[296];
} CFG_ENVR;


// Environment configuration
typedef struct {
    APTR		    memory;		    // Memory handle
    struct SignalSemaphore  lock;		    // Lock

    CFG_ENVR		    *env;		    // Environment data
    char		    path[256];		    // Current environment path

    ULONG		    pad[65];

    struct List		    buttons;		    // Button banks to open
    struct List		    listers;		    // Listers to open

    struct MinList	    path_formats;	    // Specific path formats

    APTR		    volatile_memory;	    // Volatile memory handle
    char		    toolbar_path[256];	    // Lister toolbar path
    APTR		    settings_memory;	    // Settings memory handle
    char		    menu_path[256];	    // Lister menu path
    char		    user_menu_path[256];    // User menu path
    char		    hotkeys_path[256];	    // Hotkeys path
    char		    scripts_path[256];	    // Scripts path

    struct MinList	    desktop;		    // Desktop information
    APTR		    desktop_memory;
    struct SignalSemaphore  desktop_lock;

    struct MinList	    iconpos;		    // Icon areas
    struct List		    startmenus;		    // StartMenus to open

    struct MinList	    path_list;		    // Path list

    struct MinList	    sound_list;		    // Sounds
    struct SignalSemaphore  sound_lock;
} Cfg_Environment;


#endif


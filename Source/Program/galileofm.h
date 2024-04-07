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

#ifndef GALILEOFM_INCLUDE
#define GALILEOFM_INCLUDE


#define STACK_DEFAULT	8192
#define STACK_LARGE	   16384

// INLINE copy_mem!!!!!!!!!!

//#define DEBUG

#include "/Include/galileo/os.h"

#include "/Library/galileofmbase.h"
#include "/Library/galileofmpragmas.h"
#include "/Modules/Config/galileoconfig.h"
#include "main_commands.h"

// Locale
#define CATCOMP_NUMBERS
#include "galileo.strings"


#if 0  // Not used anywhere
// Version number
#define GALILEOFM_VERSION  5
#define GALILEOFM_REV      "0"
#else

#define APPWINID     1

#define VALID_QUALIFIERS (IEQUALIFIER_LCOMMAND|IEQUALIFIER_RCOMMAND|\
                         IEQUALIFIER_CONTROL|IEQUALIFIER_LSHIFT|\
                         IEQUALIFIER_RSHIFT|IEQUALIFIER_LALT|IEQUALIFIER_RALT)

#define IEQUAL_ANYSHIFT	(IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT)
#define IEQUAL_ANYALT	(IEQUALIFIER_LALT|IEQUALIFIER_RALT)
#define IEQUAL_ANYAMIGA	(IEQUALIFIER_LCOMMAND|IEQUALIFIER_RCOMMAND)

#define KEY_CURSORUP	0x3e
#define KEY_CURSORDOWN	0x1e
#define PAGEUP		0x3f
#define PAGEDOWN	0x1f
#define HOME		0x3d
#define END		0x1d

#define isonlyword(c) (!c || c==10 || c==13 || isspace(c) || ispunct(c))

#define NUMFONTS			16					// Number of fonts

enum {
	MAIN_MENU_EVENT,			// Menu event from a sub-process
};

// Include files
#include "function_select.h"
#include "pattern.h"
#include "toolbar.h"
#include "display.h"
#include "reselect.h"
#include "lister.h"
#include "buttons.h"
#include "dirlist.h"
#include "app_msg.h"
#include "function_launch.h"
#include "function_protos.h"
#include "function_data.h"
#include "popup.h"
#include "config.h"
#include "environment.h"

#include "misc.h"
#include "rexx.h"
#include "requesters.h"
#include "file_op.h"
#include "search.h"
#include "filetypes.h"
#include "buffers.h"
#include "path_routines.h"
#include "dates.h"
#include "links.h"
#include "cx.h"
#include "wb.h"
#include "backdrop.h"
#include "desktop.h"
#include "groups.h"
#include "help.h"
#include "scripts.h"
#include "start_menu.h"

#include "position.h"
#include "galileodata.h"
#include "palette_routines.h"
#include "envoy.h"
#include "notify.h"
#include "graphics.h"
#include "file_select.h"
#include "window_activate.h"
#include "cleanup.h"
#include "icons.h"
#include "menu_data.h"
#include "tile.h"
#include "protos.h"

#include "callback.h"

#if RESOURCE_TRACKING
#include <restrack_protos.h>
#include <restrack_pragmas.h>

extern struct Library *ResTrackBase;

extern char *callerid;
#endif

#define REQ_OFF(save) { save=main_proc->pr_WindowPtr; main_proc->pr_WindowPtr=(APTR)-1; }
#define REQ_ON(save) { main_proc->pr_WindowPtr=save; }


typedef struct
{
	ULONG id;
	struct MenuItem *menu;
	struct Window *window;
} MenuEvent;

typedef struct
{
	long	offset;
	APTR	function;
} PatchList;

extern char *about_1,*about_2,*about_3,*about_4;

typedef struct
{
	short	type;
	char	name[2];
} env_packet;

#define WBArgDir(a) (!(a)->wa_Name || !*(a)->wa_Name)

#define ABS(x) (((x)>0)?(x):(-x))

#define SNIFF_REXX	0
#define SNIFF_BOTH	1
#define SNIFF_USER	2


#define FILENAME_LEN	31

#endif

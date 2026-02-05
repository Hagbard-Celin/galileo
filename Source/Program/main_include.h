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

#ifndef GALILEOFM_MAIN_H
#define GALILEOFM_MAIN_H


#include "galileofm.h"
#include <gfm/simplerequest_protos.h>
#include <gfm/popup_protos.h>
#include <gfm/image_protos.h>
#include <gfm/icon_cache_protos.h>
#include <gfm/iff_form.h>
#ifdef _DEBUG_STACK
#include "stack_check.h"
#endif
#include "lister.h"
#include "menu_data.h"
#include "misc_protos.h"
#include "callback.h"
#include "file_openwith.h"
#include "environment.h"
#include "cleanup_protos.h"
#include "lsprintf_protos.h"
#include "backdrop_protos.h"
#include "position_protos.h"
#include "requesters.h"
#include "rexx_protos.h"
#include "cx.h"
#include "function_data.h"
#include "commands.h"
#include "notify.h"
#include "wb.h"
#include "event_loop_protos.h"
#include "pattern_protos.h"


#include "/Modules/modules.h"
#include "/Modules/modules_protos.h"
#include "eliza.h"
#include "scripts.h"
#include "main_protos.h"

#define LIB_VER 0	// Minimum library version we need

#define INIT_STEPS	14


struct Library *main_open_library(char *lib,short ver);

#pragma libcall ModuleBase LibExpunge 12 0
void LibExpunge(void);

#define PROG(x) main_bump_progress(main_status,(x),TRUE)

extern struct Library		   *ConsoleDevice;

#ifdef RESOURCE_TRACKING

ULONG callerid;

#endif

#endif



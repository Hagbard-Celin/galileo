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

#ifndef _GALILEOFM_EVENT_LOOP_INCLUDE
#define _GALILEOFM_EVENT_LOOP_INCLUDE

#include "galileofm.h"
#include "lister_protos.h"
#include "buttons_protos.h"
#include "app_msg_protos.h"
#include "misc_protos.h"
#include "function_launch_protos.h"
#include "main_protos.h"
#include "cleanup_protos.h"
#include "environment.h"
#include "desktop.h"
#include "cx.h"
#include "backdrop_protos.h"
#include "buffers_protos.h"
#include "requesters.h"
#include "callback.h"
#include "lsprintf_protos.h"
#include "event_loop_protos.h"
#include "help.h"
#include "position_protos.h"
#include "icons.h"
#include "notify.h"
#include "palette_routines.h"
#include "iconpos_protos.h"
#include "menu_data.h"
#include "clock_task.h"


extern CONST GalileoCallbackInfo CallBackInfo;

#define DELETE_TIMER	300
#define MENU_TIMER	5
#define FILETYPE_TIMER	5
#define DESKTOP_TIMER	5
#define SEED_TIMER	60

void handle_icon_notify(GalileoNotify *);

TimerHandle *filetype_timer=0;

BOOL menu_process_event(ULONG,struct MenuItem *,struct Window *);

#endif

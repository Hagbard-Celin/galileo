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

#ifndef _GALILEOFM_DESKTOP_H
#define _GALILEOFM_DESKTOP_H

#include "backdrop.h"
#include "app_msg.h"
#include "lister.h"

#ifndef _GALILEOFM_CFG_DESKTOP_H
#include <gfm/cfg_desktop.h>
#endif

BOOL desktop_find_hidden(char *,BOOL);
#ifdef DISTINCT_OK
Cfg_Desktop *desktop_snapshot_icon(BackdropObject *icon,short x,short y);
#endif
Cfg_Desktop *desktop_find_icon(char *name,ULONG *pos);
STRPTR desktop_icon_path(BackdropObject *icon,BPTR our_lock);
void desktop_refresh_icons(BackdropInfo *info,BOOL);

void desktop_delete(IPCData *,BackdropInfo *,BackdropObject *);

ULONG desktop_drop(BackdropInfo *,struct AppMessage *,UWORD);

void notify_disk_name_change(BackdropInfo *,char *,char *);
void backdrop_update_disk(BackdropInfo *info,devname_change *change,BOOL show);

BOOL desktop_add_hidden(char *name);

BOOL desktop_drop_on_object(BackdropInfo *,GalileoListerAppMessage *,struct AppMessage *,BackdropObject *,UWORD);
short read_desktop_folder(BackdropInfo *,BOOL);

#endif

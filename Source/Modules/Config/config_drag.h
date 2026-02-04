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
#ifndef _GALILEOFM_CONFIG_DRAG_H
#define _GALILEOFM_CONFIG_DRAG_H

#ifndef _GALILEOFM_DRAG_ROUTINES_H
#include <gfm/drag_routines.h>
#endif
#ifndef _GALILEOFM_TIMER_H
#include <gfm/timer.h>
#endif
#ifndef _GALILEOFM_CFG_BUTTONBANK_H
#include <gfm/cfg_buttonbank.h>
#endif
#ifndef _GALILEOFM_LIST_MANAGEMENT_H
#include <gfm/list_management.h>
#endif
#ifndef _GALILEOFMBASE_IPC_H
#include <gfm/galileo_ipc.h>
#endif
#ifndef _GALILEOFM_CFG_FUNCTION_H
#include <gfm/cfg_function.h>
#endif
#ifndef GALILEOFM_CFGDRAGINFO_H
#include <gfm/cfgdraginfo.h>
#endif

BOOL config_drag_check(CfgDragInfo *drag);
void config_drag_move(CfgDragInfo *drag);
void config_drag_start(CfgDragInfo *,Att_List *,short,struct TagItem *,BOOL);
struct Window *config_drag_end(CfgDragInfo *,short);
BOOL config_drag_send_button(CfgDragInfo *,IPCData *,Cfg_Button *,Cfg_ButtonFunction *);
Cfg_Button *config_drag_get_button(Cfg_Button *button,Cfg_ButtonFunction *func);
void config_drag_start_window(CfgDragInfo *,struct Window *,struct Rectangle *,short,short);

#endif

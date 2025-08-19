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

#ifndef _GALILEOFM_STARTMENU_PROTOS
#define _GALILEOFM_STARTMENU_PROTOS

#include "start_menu.h"

IPCData *start_new(char *,BPTR,char *,char *,short,short);
void __saveds start_proc(void);
ULONG __asm __saveds start_init(
	register __a0 IPCData *,
	register __a1 StartMenu *);
void start_cleanup(StartMenu *);
void start_show(StartMenu *,struct Screen *);
void start_hide(StartMenu *);
BOOL start_popup(StartMenu *);
void start_draw_button(StartMenu *,short,short);
void start_toggle_drag(StartMenu *,short);
void start_toggle_border(StartMenu *);
void start_fix_button(StartMenu *);
BOOL start_edit(StartMenu *);
BOOL start_save(StartMenu *);
void start_change_image(StartMenu *);
void start_change_label(StartMenu *);
void start_create_new(BOOL);
void start_add_item(StartMenu *,char *);
void start_remap_bank(StartMenu *,struct List *,BOOL);
void start_change_font(StartMenu *,short);
void start_change_picture(StartMenu *);

#endif

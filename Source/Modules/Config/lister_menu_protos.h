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

#ifndef _GALILEOCONFIG_LISTER_MENU_H
#include "lister_menu.h"
#endif

BOOL lister_menu_open(lister_menu_data *data,struct Screen *screen);
void lister_menu_close(lister_menu_data *data,BOOL free);
Cfg_ButtonBank *lister_menu_load_menu(lister_menu_data *data,Cfg_ButtonBank *,char *);
void lister_menu_fix_gadgets(lister_menu_data *data);
void lister_menu_end_edit(lister_menu_data *data,BOOL disable,Att_Node *);
void lister_menu_edit_item(lister_menu_data *data);
BOOL lister_menu_receive_edit(lister_menu_data *data,FunctionReturn *ret);
BOOL lister_menu_save(lister_menu_data *data,unsigned short gadgetid);
short lister_menu_check_change(lister_menu_data *data,BOOL can_cancel);
BOOL lister_menu_load(lister_menu_data *data,short new);
void lister_menu_update_bank(lister_menu_data *data);
void lister_menu_end_drag(lister_menu_data *data,BOOL ok);
Att_Node *lister_menu_add_item(lister_menu_data *data,Att_Node *insert);
void lister_menu_build_buttons(lister_menu_data *data);
Att_Node *lister_menu_new_item(lister_menu_data *,Cfg_ButtonFunction *,Cfg_Button *);
void lister_menu_set_name(lister_menu_data *data,char *name);
void scripts_check_bank(lister_menu_data *data);
void lister_menu_fix_functionlist(lister_menu_data *data);
void lister_menu_del_item(lister_menu_data *data,Att_Node *item);

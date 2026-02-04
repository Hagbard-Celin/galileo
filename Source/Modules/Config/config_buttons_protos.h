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

#ifndef LIBRARIES_IFFPARSE_H
#include <libraries/iffparse.h>
#endif
#ifndef _GALILEOSCREENDATA_H
#include <gfm/galileoscreendata.h>
#endif
#ifndef _GALILEOFMBASE_IPC_H
#include <gfm/galileo_ipc.h>
#endif
#ifndef _GALILEOFM_CFG_BUTTONBANK_H
#include <gfm/cfg_buttonbank.h>
#endif
#ifndef GALILEOFM_LAYOUT_H
#include <gfm/layout.h>
#endif
#ifndef GALILEOFM_OBJECTLIST_H
#include <gfm/objectlist.h>
#endif
#ifndef _GALILEOFM_LISTLOCK_H
#include <gfm/listlock.h>
#endif
#ifndef GALILEOFM_CFGDRAGINFO_H
#include <gfm/cfgdraginfo.h>
#endif
#ifndef _GALILEOFM_IMAGEREMAP_H
#include <gfm/imageremap.h>
#endif
#ifndef _GALILEOFM_CFG_INSTRUCTION_H
#include <gfm/cfg_instruction.h>
#endif
#ifndef _GALILEOCONFIG_CONFIG_BUTTONS_H
#include "config_buttons.h"
#endif


void _config_buttons_update(config_buttons_data *);
BOOL _config_buttons_open(config_buttons_data *data,struct Screen *screen);
void _config_buttons_cleanup(config_buttons_data *data);
void _config_buttons_new_bank(config_buttons_data *,Cfg_ButtonBank *,IPCData *);
void _config_buttons_remove_bank(config_buttons_data *,bank_node *,BOOL,BOOL);
/*APTR __stdargs AllocAslRequestTags(unsigned long type,Tag tag1,...);*/
void _config_buttons_refresh(config_buttons_data *data,bank_node *,ULONG type);
void _config_buttons_reset(config_buttons_data *data);

BOOL _config_buttons_rows_add(config_buttons_data *data,short);
BOOL _config_buttons_columns_add(config_buttons_data *data,short);
BOOL _config_buttons_rows_remove(config_buttons_data *data);
BOOL _config_buttons_columns_remove(config_buttons_data *data);
BOOL _config_buttons_remove_fix(config_buttons_data *data,short old_count);
short _config_buttons_remove_empty_rows(config_buttons_data *data,short max);
short _config_buttons_remove_empty_columns(config_buttons_data *data,short max);
void _config_buttons_remove_rows(config_buttons_data *data,short pos,short count);
void _config_buttons_remove_columns(config_buttons_data *data,short pos,short count);
BOOL _config_buttons_handle_key(config_buttons_data *data,UWORD code,UWORD qual);
BOOL _config_buttons_delete_row(config_buttons_data *data,short);
BOOL _config_buttons_delete_column(config_buttons_data *data,short);

short _config_buttons_get_colrow(config_buttons_data *data,short *col,short *row);
Cfg_Button *_config_buttons_get_button(config_buttons_data *data,short col,short row);
void _config_buttons_flash(config_buttons_data *data,long);
void _config_buttons_redraw_button(config_buttons_data *data,bank_node *,Cfg_Button *button);
BOOL _config_buttons_create_new(config_buttons_data *data,struct List *list,short count);
void _config_buttons_edit_button(config_buttons_data *data,short col,short row);
BOOL _config_buttons_receive_edit(config_buttons_data *data,ButtonReturn *ret,BOOL);

void button_to_clipboard(config_buttons_data *data,Cfg_Button *button);
void button_save_clipboard(config_buttons_data *data,BOOL save);
void button_load_clipboard(config_buttons_data *data);
void show_button_clipboard(config_buttons_data *data,BOOL move);
void button_draw(Cfg_Button *,struct IBox *,struct Rectangle *,struct Window *,short,Cfg_ButtonFunction *,short *,short,short,short,short);
void _config_buttons_fix_controls(config_buttons_data *data);
void _config_buttons_drop_button(config_buttons_data *data,short x,short y);
void _config_buttons_fake_return(config_buttons_data *data,Cfg_Button *orig_button,Cfg_Button *new_button);
void _button_to_bank(config_buttons_data *data,Cfg_Button *button,short x,short y);
void config_buttons_show_clipboard(config_buttons_data *data);
void config_buttons_hide_clipboard(config_buttons_data *data);

void button_receive_appmsg(config_buttons_data *data,struct AppMessage *msg);

Cfg_Button *button_create_drop(
	config_buttons_data *data,
	APTR memory,
	struct WBArg *arg,
	Cfg_Instruction *ins,
	unsigned long flags);

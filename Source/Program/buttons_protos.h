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

#ifndef _GALILEOFM_BUTTONS_PROTOS
#define _GALILEOFM_BUTTONS_PROTOS

#include "buttons.h"


// protos
Buttons *buttons_new(char *,char *,BPTR,Cfg_ButtonBank *,struct IBox *,short,ULONG);
ULONG buttons_command(Buttons *,ULONG,ULONG,APTR,APTR,struct MsgPort *);
void buttons_update(Buttons *buttons);
void buttons_get_font(Buttons *buttons);
BOOL buttons_iconify(Buttons *buttons);

void __saveds buttons_code(void);
ULONG __asm __saveds buttons_init(register __a0 IPCData *,register __a1 Buttons *);
void buttons_cleanup(Buttons *,BOOL);
BOOL buttons_open(Buttons *,struct Screen *,short);
void buttons_close(Buttons *,short);
void buttons_refresh(Buttons *,ULONG);
buttons_process_msg(Buttons *,struct IntuiMessage *);
void buttons_busy(Buttons *);
void buttons_unbusy(Buttons *);

short buttons_draw_border(Buttons *,Cfg_Button *,short,short,short,short,struct RastPort *);
void buttons_get_rect(Buttons *buttons,int x,int y,struct Rectangle *rect);
void buttons_redraw_button(Buttons *buttons,Cfg_Button *button);
short buttons_draw_button(Buttons *,Cfg_Button *,short,short,short,short,struct RastPort *);
Cfg_Button *button_from_point(Buttons *,WORD *,WORD *);
Cfg_Button *button_from_pos(Buttons *buttons,short col,short row);
BOOL buttons_pos(Buttons *buttons,Cfg_Button *button,short *col,short *row);
Cfg_Function *button_valid(Cfg_Button *button,short which);
void buttons_show_highlight(Buttons *buttons);
void buttons_stop_highlight(Buttons *buttons);

Cfg_ButtonFunction *button_find_function(Cfg_Button *button,short which,APTR *);
void buttons_run_button(Buttons *,Cfg_Button *,short);
BOOL buttons_app_message(Buttons *,struct AppMessage *msg);

buttons_save(Buttons *buttons,char *name);
void buttons_saveas(Buttons *buttons);
buttons_load(Buttons *buttons,struct Screen *screen,char *);
int buttons_check_change(Buttons *buttons,BOOL);

void buttons_edit_key(Buttons *buttons,UWORD code,UWORD qual);
BOOL buttons_visible_select(Buttons *buttons);
void buttons_start_drag(Buttons *,short,short,short,short);
void buttons_show_drag(Buttons *,short,short);
void buttons_stop_drag(Buttons *,short,short);
void buttons_edit(IPCData *my_ipc,buttons_edit_packet *);
BOOL buttons_edit_bank(Buttons *buttons,short col,short row,Cfg_Button *,struct AppMessage *,short);


short buttons_get_max_size(Buttons *,unsigned short *,unsigned short *);
void buttons_highlight_button(Buttons *buttons,short state,short);
void buttons_show_button(Buttons *,Cfg_Button *,short,short,short,struct RastPort *);

void buttons_do_popup(Buttons *,UWORD);
BOOL buttons_button_popup(Buttons *,UWORD);

int buttons_do_function(Buttons *buttons,ULONG func);
void buttons_new_bank(Buttons *buttons,short func,Cfg_ButtonBank *);

buttons_request_file(Buttons *,char *,char *,char *,ULONG);

void __asm buttons_refresh_callback(
	register __d0 ULONG type,
	register __a0 struct Window *window,
	register __a1 Buttons *buttons);

void buttons_edit_save(ULONG id);
void buttons_edit_defaults(ULONG id);

BOOL buttons_fix_drag(Buttons *buttons);
void buttons_fix_internal(Buttons *buttons);

void buttons_remap(Buttons *buttons,short remap);


void buttons_update_icon(Buttons *buttons);

UWORD button_border_popup(Buttons *buttons);

#endif

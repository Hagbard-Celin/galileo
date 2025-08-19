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

#ifndef _GALILEOFM_LISTER_PROTOS
#define _GALILEOFM_LISTER_PROTOS

#include "lister.h"

// protos
Lister *lister_new(Cfg_Lister *cfg_lister);
void lister_free(Lister *lister);
ULONG lister_command(Lister *,ULONG,ULONG,APTR,APTR,struct MsgPort *);
Lister *find_lister_lock(BPTR lock);
Lister *find_lister_path(char *);
void lister_update(Lister *);
void listers_update(BOOL,BOOL);
void lister_init_new(Cfg_Lister *cfg,Lister *lister);
void lister_tile(long id);
BOOL set_list_selector(Lister *lister,long pos);
BOOL lister_iconify(Lister *lister);
BOOL lister_new_waiter(Lister *lister,IPCMessage *msg,struct Message *msg2,short type);
void lister_relieve_waiters(Lister *,short);
void lister_smart_source(Lister *);

void __saveds lister_code(void);
ULONG __saveds __asm lister_init(register __a0 IPCData *,register __a1 Lister *);
void lister_cleanup(Lister *,BOOL);
struct DirectoryBuffer *lister_new_buffer(Lister *);
void lister_free_buffer(struct DirectoryBuffer *);
void __asm lister_process_msg(register __d0 Lister *,register __a0 struct IntuiMessage *);
void check_call(char *,Lister *);
void lister_send_abort(Lister *);
int lister_do_function(Lister *,ULONG);

void lister_busy(Lister *,int);
void lister_unbusy(Lister *,int);
void lister_progress_on(Lister *,ProgressPacket *);
void lister_progress_calc_size(Lister *,struct Screen *,long);
void lister_progress_off(Lister *);
void lister_progress_update(Lister *,char *,ULONG);
void check_lister_stored(Lister *lister);
void lister_progress_filetotal(Lister *lister,long total);
void lister_progress_fileprogress(Lister *lister,long count);
void lister_progress_info(Lister *lister,char *info);
void lister_progress_total(Lister *lister,long total,long count);
void lister_progress_file(Lister *lister,long total,long count);
void lister_progress_title(Lister *lister,char *info);

void lister_show_name(Lister *);
void lister_clear_title(Lister *);
void lister_show_status(Lister *);
void lister_title_pens(struct RastPort *,Lister *,int);
void lister_status(Lister *,char *);
void lister_clear_title_area(GUI_Element *area);

Lister *lister_source(void);
Lister *lister_dest(void);
Lister *lister_default(ULONG,BOOL);
void lister_check_source(Lister *);
void lister_check_dest(Lister *);
void lister_split_display(Lister *lister,Lister *other_lister);

void lister_configure(Lister *);
void lister_change_format(Lister *,ListFormat *);
void lister_set_sort(Lister *lister,short item,UWORD);
BOOL lister_resort(Lister *,short);
int resort_test(ListFormat *,ListFormat *);
void lister_parent_popup(Lister *lister,unsigned short code);
void lister_add_history(Lister *lister);

struct DirectoryBuffer *lister_find_buffer(Lister *,struct DirectoryBuffer *,char *,BPTR,struct DateStamp *,char *,ULONG);
struct DirectoryBuffer *lister_buffer_find_empty(Lister *,char *,BPTR,struct DateStamp *);
void lister_check_old_buffer(Lister *,BOOL);
void lister_show_buffer(Lister *,struct DirectoryBuffer *,int,BOOL);
void update_buffer_stamp(Lister *);
struct DirectoryBuffer *lister_get_empty_buffer(void);
void remove_file_global(char *,char *,BOOL);
void update_lister_global(char *path, BPTR lock);

// lister_show.c
void lister_display_dir(Lister *);
void display_entry(struct DirectoryEntry *,Lister *,int);
void lister_draw_entry(Lister *,struct DirectoryEntry *,short,short,short,short,short);
void setdispcol(struct DirectoryEntry *,Lister *);
void getdispcol(struct DirectoryEntry *,Lister *,short *,short *);
void builddisplaystring(struct DirectoryEntry *,char *,Lister *);
void entry_highlight(Lister *,short);
void lister_update_name(Lister *);
void lister_refresh_display(Lister *,ULONG);
void lister_refresh_name(Lister *);
void select_show_info(Lister *,BOOL);

// lister_toolbar.c
void lister_show_toolbar(Lister *);
void lister_toolbar_click(Lister *,short,short,unsigned short,unsigned short);
void lister_toolbar_function(Lister *);
void lister_build_menu(Lister *lister);
void lister_get_toolbar(BOOL,Cfg_ButtonBank *);
void lister_toolbar_free_cache(void);
Cfg_Button *lister_get_toolbar_button(Lister *,short,short,short *);

// lister_icons.c
void lister_get_icons(struct _FunctionHandle *,Lister *lister,char *,BOOL);
void lister_icon_copy(Lister *dest,Lister *source,struct AppMessage *msg);
void lister_icon_copy_cleanup(struct icon_copy_data *data);
void lister_icon_copy_add(struct icon_copy_data *data);
void lister_icon_copy_rem(Lister *lister,char *name);

// lister_window.c
struct Window *lister_open(Lister *lister,struct Screen *screen);
void lister_close(Lister *lister,BOOL);

// lister_display.c
void lister_init_display(Lister *lister);
void lister_refresh(Lister *lister,unsigned short mode);
void lister_init_filelist(Lister *lister);
void lister_init_lister_area(Lister *lister);
void lister_add_pathfield(Lister *lister);
void lister_remove_pathfield(Lister *lister,BOOL);
void lister_disable_pathfield(Lister *lister,short disable);
void lister_mode_change(Lister *,struct IBox *);

void lister_toolbar_update_cache(void);
void center_erase_text(struct RastPort *rp,char *text,short minx,short maxx,short y);
void lister_check_refresh(Lister *lister);
BOOL lister_valid_toolbar(Lister *lister);
void lister_new_toolbar(Lister *lister,char *name,ToolBarInfo *);
void lister_toolbar_edit(short);

void lister_show_title(Lister *lister,BOOL);
void lister_title_highlight(Lister *lister,short item,short);

void lister_calc_limits(Lister *lister,struct Screen *screen);
void lister_set_limits(Lister *lister);
void lister_update_pathfield(Lister *lister);

void lister_fix_priority(Lister *);


BOOL lister_check_valid(Lister *lister);

struct DirectoryEntry *lister_test_drag(Lister *,DragInfo *,short,short,Lister **);
struct DirectoryEntry *lister_highlight(Lister *,short x,short y,DragInfo *drag);

void lister_show_icon(Lister *lister,struct _BackdropObject *object);

void lister_clip_entries(Lister *lister,unsigned short qual);

void lister_set_title(Lister *lister);

void lister_set_busy_icon(Lister *lister);
void lister_clear_busy_icon(Lister *lister);

void lister_build_icon_name(Lister *lister);

void lister_fix_menus(Lister *lister,BOOL);

void lister_fix_cd(Lister *lister);

void lister_add_hotname(Lister *lister,char);
void lister_rem_hotname(Lister *lister);
void lister_handle_hotname(Lister *lister);

void lister_clear_title(Lister *lister);

Lister *lister_open_new(char *,struct DateStamp *voldate,BPTR,struct _BackdropObject *,struct Window *,Lister *);
void lister_title_drop(Lister *lister,short item,short x,short y);

void lister_update_icon(Lister *lister);

void lister_end_edit(Lister *,short);
BOOL lister_edit_key(Lister *,struct IntuiMessage *);
BOOL lister_start_edit(Lister *,short,short,short);
BOOL lister_edit_init(Lister *,BOOL);
short lister_edit_next(Lister *,short);
BOOL lister_edit_check_item(Lister *);
BOOL lister_edit_check_cursor(Lister *,short);
BOOL lister_edit_key_press(Lister *,unsigned char,UWORD);
BOOL lister_edit_key_string(Lister *,unsigned char,UWORD);
BOOL lister_edit_key_protect(Lister *,unsigned char);
BOOL lister_edit_key_netprotect(Lister *,unsigned char);

void __asm __saveds lister_refresh_callback(
	register __d0 ULONG,
	register __a0 struct Window *,
	register __a1 Lister *);

void lister_edit_error(Lister *,short);

char *lister_title_string(struct DirectoryBuffer *buffer,short item);

BOOL lister_find_cached_buffer(Lister *lister,char *path,char *handler);
void lister_free_caches(Lister *lister,char *handler);
BOOL lister_select_cache(Lister *,struct DirectoryBuffer *);
void lister_set_gauge(Lister *,BOOL);

void lister_init_colour(Lister *,short,short);

BOOL lister_check_ftp(Lister *,char *);
BOOL lister_want_gauge(Lister *);

struct Window *lister_valid_window(Lister *);
void lister_zoom_window(Lister *);

struct BitMap *builddisplaystring_prop(struct DirectoryEntry *,char *,Lister *,short);

UWORD lister_listerpopup(Lister *,UWORD);

void lister_edit_calc_position(Lister *lister,short type);

void lister_title_highlight_sep(Lister *lister);

#endif

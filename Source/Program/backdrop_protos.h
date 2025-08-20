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

#ifndef _GALILEOFM_BACKDROP_PROTOS
#define _GALILEOFM_BACKDROP_PROTOS

#include "app_msg.h"
#include "filetypes.h"
#include "backdrop.h"


BackdropInfo *backdrop_new(IPCData *,ULONG);
void backdrop_free(BackdropInfo *info);

BackdropObject *backdrop_new_object(BackdropInfo *,char *,char *,short);

void backdrop_init_info(BackdropInfo *,struct Window *,short);
void backdrop_free_remap(BackdropInfo *,struct Window *);
void backdrop_free_info(BackdropInfo *);
void backdrop_free_list(BackdropInfo *);
void backdrop_remove_object(BackdropInfo *,BackdropObject *);
void backdrop_scroll_objects(BackdropInfo *,short,short);
void backdrop_show_objects(BackdropInfo *,UWORD);
void backdrop_render_object(BackdropInfo *,BackdropObject *,UWORD);
void backdrop_draw_object(BackdropInfo *,BackdropObject *,UWORD,struct RastPort *,short,short);
BOOL backdrop_cleanup_objects(BackdropInfo *,struct Rectangle *,UWORD);
void backdrop_lineup_objects(BackdropInfo *);
long backdrop_arrange_objects(BackdropInfo *);
BackdropObject *__asm backdrop_get_object(
	register __a0 BackdropInfo *info,
	register __d0 short x,
	register __d1 short y,
	register __d2 UWORD flags);
BackdropObject *backdrop_icon_in_rect(BackdropInfo *,struct Rectangle *);
BackdropObject *backdrop_icon_in_rect_full(BackdropInfo *,struct Rectangle *);


void backdrop_object_open(BackdropInfo *,BackdropObject *,UWORD,ULONG,GalileoListerAppMessage *,struct AppMessage *);
//void backdrop_object_open(BackdropInfo *,BackdropObject *,UWORD,BOOL,long,struct WBArg *);
void backdrop_update_window(BackdropInfo *,BackdropObject *,struct ListerWindow *);
BOOL backdrop_start_drag(BackdropInfo *,short,short);
BOOL backdrop_drag_object(BackdropInfo *,BackdropObject *);
BOOL backdrop_stop_drag(BackdropInfo *);
BOOL backdrop_check_deadlocks(BackdropInfo *);
void backdrop_show_drag(BackdropInfo *,BackdropObject *,short,short);
void backdrop_check_pos(BackdropInfo *);
void backdrop_info(BackdropInfo *,BackdropObject *);
void backdrop_snapshot(BackdropInfo *,BOOL,BOOL,BackdropObject *);
void backdrop_select_all(BackdropInfo *,short);
BOOL backdrop_cleanup(BackdropInfo *,short,UWORD);
void backdrop_check_change(BackdropInfo *,BackdropObject *);
void backdrop_get_leftouts(BackdropInfo *,BackdropObject *);
BackdropObject *backdrop_leftout_new(BackdropInfo *,char *,char *,char *);
void backdrop_get_icon(BackdropInfo *,BackdropObject *,short);
void backdrop_leave_icons_out(BackdropInfo *info,BackdropObject *,BOOL);
void backdrop_save_leftouts(BackdropInfo *);
BackdropObject *find_backdrop_object(BackdropInfo *,BackdropObject *);
void backdrop_putaway(BackdropInfo *,BackdropObject *);
void backdrop_calc_virtual(BackdropInfo *);
void backdrop_calc_virtual_size(BackdropInfo *,struct Rectangle *);
void backdrop_scroll(BackdropInfo *,short,short);
void backdrop_pos_slider(BackdropInfo *,short);
#if 0
BackdropObject *backdrop_find_disk(BackdropInfo *,char *);
#endif
void backdrop_update_leftouts(BackdropInfo *);
void backdrop_format(BackdropInfo *,BackdropObject *);
void backdrop_drop_object(BackdropInfo *,BackdropObject *);

BOOL backdrop_idcmp(BackdropInfo *,struct IntuiMessage *,unsigned short flags);
void backdrop_install_clip(BackdropInfo *);
void backdrop_rubber_band(BackdropInfo *,BOOL);
void backdrop_select_area(BackdropInfo *,short);

short geo_box_intersect(struct Rectangle *,struct Rectangle *);

BackdropObject *backdrop_add_appicon(AppEntry *appicon,BackdropInfo *info,short);
void backdrop_rem_appicon(BackdropInfo *info,AppEntry *icon);
struct AppMessage *backdrop_appmessage(BackdropInfo *info,BOOL);
GalileoListerAppMessage *backdrop_lister_appmessage(BackdropInfo *info, ULONG window_id,BOOL need_obj);
void backdrop_drop_appwindow(BackdropInfo *info,struct AppWindow *appwindow,short,short,UWORD);
void backdrop_drop_lister_appwindow(BackdropInfo *info,struct AppWindow *appwindow,short x,short y);
void backdrop_sort_objects(BackdropInfo *info,short,BOOL);
void backdrop_image_bitmap(BackdropInfo *,struct Image *,UWORD *,struct BitMap *);

BPTR backdrop_icon_lock(BackdropObject *object);

void backdrop_read_groups(BackdropInfo *info);
void backdrop_open_group(BackdropInfo *info,BackdropObject *object,BOOL);
GroupData *backdrop_find_group(BackdropObject *object);
void __saveds backdrop_group_handler(void);
ULONG __asm backdrop_group_init(
	register __a0 IPCData *ipc,
	register __a1 GroupData *startup);
BOOL backdrop_show_group(GroupData *startup);
void backdrop_group_get_font(GroupData *group);
void backdrop_hide_group(GroupData *startup);
void backdrop_free_group(GroupData *startup);
void backdrop_read_group_objects(GroupData *startup);
void backdrop_check_groups(BackdropInfo *info);
void backdrop_group_add_object(char *,BackdropInfo *,char *,BPTR,short,short);
void backdrop_delete_group(BackdropInfo *info,BackdropObject *object);
void backdrop_remove_group_objects(GroupData *data,BackdropObject *);
BOOL backdrop_group_do_function(GroupData *,ULONG,struct MenuItem *);

BOOL backdrop_new_group_object(BackdropInfo *info,BackdropObject *object,unsigned short);
BOOL backdrop_check_notify(BackdropInfo *info,GalileoNotify *notify,Lister *lister);

void backdrop_snapshot_group(BackdropInfo *info,char *name);
void backdrop_read_appicons(BackdropInfo *info,short);
BackdropObject *backdrop_find_appicon(BackdropInfo *info,AppEntry *appicon);

void backdrop_update_size(BackdropInfo *);

void backdrop_make_visible(BackdropInfo *info,BackdropObject *object);


void backdrop_appicon_message(BackdropObject *object,unsigned short flags);

ULONG backdrop_image_checksum(BackdropObject *object,short which);
void backdrop_get_masks(BackdropObject *object);

short backdrop_get_devices(BackdropInfo *info);
void backdrop_do_leftouts(BackdropInfo *info,BackdropObject *onlyone);
short backdrop_cleanup_list(BackdropInfo *info,long type);
void backdrop_refresh_drives(BackdropInfo *info, GalileoNotify *notify, BOOL);
void backdrop_remove_leftouts(BackdropInfo *info,BackdropObject *disk);

BOOL backdrop_test_rmb(BackdropInfo *,struct IntuiMessage *,struct IntuiMessage *,ULONG);
BOOL backdrop_popup(BackdropInfo *,short,short,UWORD,long,char *,DirEntry *);


void backdrop_clear_region(BackdropInfo *info);
void backdrop_add_region(BackdropInfo *info,struct Rectangle *rect);
void backdrop_region_bounds(BackdropInfo *info);


void backdrop_fix_count(BackdropInfo *info,BOOL);

void backdrop_place_object(BackdropInfo *info,BackdropObject *object);
void backdrop_icon_size(BackdropInfo *,BackdropObject *,short *,short *);

BackdropObject *backdrop_next_object(
	BackdropInfo *info,
	BackdropObject *object,
	BackdropObject *only_one);

BOOL backdrop_remove_leftout(BackdropInfo *info, BackdropObject *object);

void backdrop_add_volume_leftouts(BackdropInfo *info,
				  char *device,
				  char *volume,
				  struct DateStamp *date,
				  UBYTE len);
void backdrop_add_leftouts(BackdropInfo *info);
BackdropObject *backdrop_leave_out(
	BackdropInfo *info,
	char *name,
	BPTR parent_lock,
	ULONG flags,short,short);


void backdrop_get_objects(BackdropInfo *,short);


void backdrop_command(BackdropInfo *info,ULONG cmd,ULONG flags);
Cfg_Filetype *backdrop_get_filetype(BackdropInfo *info,BackdropObject *object);

short backdrop_check_icons_ok(BackdropInfo *info);

BOOL backdrop_handle_button(BackdropInfo *info,struct IntuiMessage *msg,unsigned short flags);

void backdrop_erase_icon(BackdropInfo *info,BackdropObject *object,UWORD flags);

void backdrop_run_build_args(Att_List *list,BackdropInfo *info,BackdropObject *exclude);

void backdrop_selection_list(BackdropObject *object);

void backdrop_get_font(BackdropInfo *info);


unsigned short *backdrop_space_array(BackdropInfo *info,Point *size);
void backdrop_space_add_icon(BackdropInfo *,unsigned short *array,BackdropObject *icon,Point *size);

void backdrop_icon_position(BackdropInfo *info,BackdropObject *icon,short x,short y);

short backdrop_get_copy_path(PopUpItem *item,char *path);

void popup_get_appicon(PopUpHandle *,AppEntry *);
BOOL popup_get_filetype(PopUpHandle *,BackdropInfo *,MatchHandle *,BackdropObject *,char *,short,ULONG *);

void backdrop_show_rect(BackdropInfo *,BackdropObject *,short,short);
void backdrop_position_object(BackdropInfo *,BackdropObject *,long *,long *,short,short,struct MinList *);

struct MinList *backdrop_build_poslist(BackdropInfo *,ULONG);
void backdrop_free_poslist(struct MinList *);

void popup_build_copyto(PopUpHandle *,PopUpItem *);
void popup_build_openwith(PopUpHandle *);

BackdropObject *backdrop_create_shortcut(BackdropInfo *,char *,short,short);
void backdrop_unique_name(char *);

void backdrop_check_size(BackdropInfo *);


ULONG get_list_type(BackdropObject *);


void backdrop_draw_icon_ghost(struct RastPort *,struct Rectangle *,PLANEPTR);
BOOL backdrop_icon_border(BackdropObject *);


void popup_default_menu(BackdropInfo *,PopUpHandle *,short *);
MatchHandle *popup_build_icon_menu(BackdropInfo *,char *,BackdropObject *,ULONG,short *,PopUpHandle *);

void backdrop_bad_disk_name(BackdropObject *,char *);

void backdrop_check_grid(long *,long *);

void backdrop_replace_icon_image(BackdropInfo *,char *,BPTR,BackdropObject *);
BOOL backdrop_goodbad_check(char *device,struct List *dos_list);

#endif

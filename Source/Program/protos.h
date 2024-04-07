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

#ifndef _GALILEOFM_PROTOS
#define _GALILEOFM_PROTOS

void __stdargs loc_printf(char *,char *,long,...);

// main.c
void main(int,char **);
Cfg_Function *new_default_function(char *func,APTR);
void process_args(int argc,char **argv);
void env_update_settings(BOOL save);
IPCData *show_startup_picture(void);
BOOL main_bump_progress(APTR,short,BOOL);
void startup_misc_init(void);
void startup_check_assign(void);
void startup_open_galileofmlib(void);
void startup_check_duplicate(void);
void startup_run_update(void);
void startup_open_libraries(void);
void startup_init_gui(void);
void startup_process_args(int,char **);
void startup_show_startup_picture(IPCData **);
void startup_init_desktop(void);
void startup_read_positions(void);
void startup_init_ipc(void);
void startup_init_ports(void);
void startup_get_env(void);
void startup_init_arexx_cx(void);
void startup_init_environment(void);
void startup_init_commands(void);
void startup_init_filetypes(void);
void startup_init_notification(void);
void startup_init_icons(void);
void startup_prestartup_script(void);
void startup_init_display(IPCData *);
void startup_misc_final(void);

// commands.c
#define SCAN_MODULES	(1<<0)
#define SCAN_REXX		(1<<2)
#define SCAN_BOTH		(SCAN_MODULES|SCAN_REXX)
#define SCAN_USER		(1<<3)

void startup_init_commands(void);
void update_commands(ULONG flag);
void init_commands_scan(short);
void command_expunge(char *name);
CommandList *add_command(char *,char *,char *,char *,ULONG,char *,Att_List *,char *,ULONG);
void command_new(BackdropInfo *,IPCData *,char *);
void command_remove(char *);

// event_loop.c
void event_loop(void);
BOOL menu_process_event(ULONG,struct MenuItem *,struct Window *);

// string_data.c
void init_locale_data(struct GalileoLocale *);
void free_locale_data(struct GalileoLocale *);

#if 0
// Miscellaneous prototypes
void KPrintF __ARGS((char *,...));

void __stdargs ArgArrayDone(void);
UBYTE **__stdargs ArgArrayInit(long,UBYTE **);
STRPTR __stdargs ArgString(UBYTE **,STRPTR,STRPTR);
#endif
void lsprintf __ARGS((char *,char *,...));

// clock_task.c
void clock_proc(void);
void clock_show_memory(struct RastPort *,long,long,char *);
void title_error(char *txt,short);
void clock_show_custom_title(struct RastPort *,long,long,struct DateStamp *,struct SysInfo *,struct Library *);

// tile.c
void tile_windows(TileInfo *);

// about.c
void show_about(struct Screen *screen,IPCData *ipc);

// status_text.c
status_display_error(Lister *lister,int errcode);
void status_text(Lister *lister,char *text);
void status_okay(Lister *lister);
void status_abort(Lister *lister);

int function_copy_file(
	struct FileInfoBlock *s_info,
	struct FileInfoBlock *d_info,
	FunctionHandle *handle,
	char *,char *,long *,char *,ULONG);

// scripts.c
BOOL RunScript(short,char *);


// iconpos.c
void iconpos_configure(IPCData *,struct MsgPort *);
void iconpos_new_area(struct Rectangle *);
BOOL iconpos_idcmp(struct IntuiMessage *);


// main_reset.c
void main_handle_reset(ULONG *,APTR);


// text.c
#include "text.h"


// file_openwith.c
void file_open_with(struct Window *,char *,ULONG);
BOOL file_launch(char *,short,char *);
void startup_read_openwith(void);
void add_open_with(char *);


// function_runcommand.c
Cfg_Function *function_load_function(char *name);

#endif

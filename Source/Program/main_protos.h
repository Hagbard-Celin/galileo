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

#ifndef GALILEOFM_MAIN_PROTOS_H
#define GALILEOFM_MAIN_PROTOS_H


void main(int,char **);
void env_update_settings(BOOL save);
IPCData *show_startup_picture(void);
BOOL main_bump_progress(APTR,short,BOOL);
void startup_init_commands(void);
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

// string_data.c
void init_locale_data(struct GalileoLocale *);
void free_locale_data(struct GalileoLocale *);

// main_reset.c
void main_handle_reset(ULONG *,APTR);

#endif



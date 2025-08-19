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

#include "function_data.h"

void __stdargs loc_printf(char *,char *,long,...);

// main.c
Cfg_Function *new_default_function(char *func,APTR);
void process_args(int argc,char **argv);

// commands.c
#define SCAN_MODULES	(1<<0)
#define SCAN_REXX		(1<<2)
#define SCAN_BOTH		(SCAN_MODULES|SCAN_REXX)
#define SCAN_USER		(1<<3)

void update_commands(ULONG flag);
void init_commands_scan(short);
void command_expunge(char *name);
CommandList *add_command(char *,char *,char *,char *,ULONG,char *,Att_List *,char *,ULONG);
void command_new(BackdropInfo *,IPCData *,char *);
void command_remove(char *);

// event_loop.c
void event_loop(void);
BOOL menu_process_event(ULONG,struct MenuItem *,struct Window *);

#if 0
// Miscellaneous prototypes
void KPrintF __ARGS((char *,...));

void __stdargs ArgArrayDone(void);
UBYTE **__stdargs ArgArrayInit(long,UBYTE **);
STRPTR __stdargs ArgString(UBYTE **,STRPTR,STRPTR);
#endif

// tile.c
void tile_windows(TileInfo *);

// about.c
void show_about(struct Screen *screen,IPCData *ipc);

// status_text.c
status_display_error(Lister *lister,int errcode);
void status_text(Lister *lister,char *text);
void status_okay(Lister *lister);
void status_abort(Lister *lister);


// scripts.c
BOOL RunScript(short,char *);


// iconpos.c
void iconpos_configure(IPCData *,struct MsgPort *);
void iconpos_new_area(struct Rectangle *);
BOOL iconpos_idcmp(struct IntuiMessage *);

// text.c
#include "text.h"


// function_runcommand.c
Cfg_Function *function_load_function(char *name);

#endif

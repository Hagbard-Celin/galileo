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

#ifndef _FUNCTION_EDITOR_PROTOS_H
#define _FUNCTION_EDITOR_PROTOS_H

#ifndef _GALILEOFM_LIST_MANAGEMENT_H
#include <gfm/list_management.h>
#endif
#ifndef _GALILEOFM_CFG_FUNCTION_H
#include <gfm/cfg_function.h>
#endif
#ifndef _GALILEOFMBASE_IPC_H
#include <gfm/galileo_ipc.h>
#endif
#ifndef _GFM_FUNCTION_EDITOR_H
#include "function_editor.h"
#endif
#ifndef _GALILEOFM_FUNCEDDATA_H
#include "funceddata.h"
#endif

void __asm FunctionEditorTr(void);
void __asm FunctionEditor(void);
ULONG __asm funced_initTr(register __a0 IPCData *,register __a1 FunctionStartup *);
ULONG __asm funced_init(register __a0 IPCData *,register __a1 FunctionStartup *);
void funced_cleanup(FuncEdData *);
void funced_build_flaglist(FuncEdData *);
void funced_update_flaglist(FuncEdData *);
void funced_decompile(FuncEdData *);
void funced_compile(FuncEdData *);
void funced_build_display(FuncEdData *);
void funced_build_entrydisplay(FuncEdData *,Att_Node *,FunctionEditorEntry *);
void funced_start_edit(FuncEdData *);
BOOL funced_end_edit(FuncEdData *,Att_Node *,int,UWORD);
Att_Node *funced_new_entry(FuncEdData *,Att_Node *,FunctionEditorEntry *);
void funced_check_flag(ULONG *,ULONG);
void funced_edit_insertstring(ObjectList *,ULONG,char *);
void funced_store_edits(FuncEdData *);
void funced_init_gads(FuncEdData *);
BOOL funced_command_req(FuncEdData *,char *,short);
void funced_appmsg(FuncEdData *,struct AppMessage *);
short funced_appmsg_arg(struct WBArg *,char *);
void functioned_end_drag(FuncEdData *,short);
void functioned_copy_line(FuncEdData *,FunctionEditorEntry *,unsigned short,Point *);
short functioned_get_line(struct Window *,GL_Object *,unsigned short,unsigned short);
void funced_show_key(FuncEdData *data);
BOOL funced_rexx_args(char *name,char *buffer);

#endif

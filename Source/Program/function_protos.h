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

#ifndef _GALILEOFM_FUNCTION_PROTOS
#define _GALILEOFM_FUNCTION_PROTOS

#include "function_launch.h"
#include "function_data.h"

#define GALILEOFM_FUNC(name) \
	int name(struct _CommandList *command,\
				FunctionHandle *handle,\
				char *args,\
				struct _InstructionParsed *instruction)

GALILEOFM_FUNC(function_all);
GALILEOFM_FUNC(function_none);
GALILEOFM_FUNC(function_toggle);
GALILEOFM_FUNC(function_copy);
GALILEOFM_FUNC(function_copy);
GALILEOFM_FUNC(function_delete);
GALILEOFM_FUNC(function_makedir);
GALILEOFM_FUNC(function_select);
GALILEOFM_FUNC(function_devicelist);
GALILEOFM_FUNC(function_change);
GALILEOFM_FUNC(function_scandir);
GALILEOFM_FUNC(function_rename);
GALILEOFM_FUNC(function_dirtree);
GALILEOFM_FUNC(function_clearbuffers);
GALILEOFM_FUNC(function_show);
GALILEOFM_FUNC(function_iconinfo);
GALILEOFM_FUNC(function_disk);
GALILEOFM_FUNC(function_loadbuttons);
GALILEOFM_FUNC(function_leaveout);
GALILEOFM_FUNC(function_runprog);
GALILEOFM_FUNC(function_addicon);
GALILEOFM_FUNC(function_search);
GALILEOFM_FUNC(function_bufferlist);
GALILEOFM_FUNC(function_getsizes);
GALILEOFM_FUNC(function_hunt);
GALILEOFM_FUNC(function_clearsizes);
GALILEOFM_FUNC(function_loadenvironment);
GALILEOFM_FUNC(function_parent);
GALILEOFM_FUNC(function_user);
GALILEOFM_FUNC(function_verify);
GALILEOFM_FUNC(function_cli);
GALILEOFM_FUNC(function_printdir);
GALILEOFM_FUNC(function_set);
GALILEOFM_FUNC(function_closebuttons);
GALILEOFM_FUNC(function_special);
GALILEOFM_FUNC(function_reselect);
GALILEOFM_FUNC(function_test);
GALILEOFM_FUNC(function_assign);
GALILEOFM_FUNC(function_configure);
GALILEOFM_FUNC(function_deletefile);
GALILEOFM_FUNC(function_setbackground);
GALILEOFM_FUNC(function_runcommand);
GALILEOFM_FUNC(function_editcommand);
GALILEOFM_FUNC(function_logout);
GALILEOFM_FUNC(function_reset);
GALILEOFM_FUNC(function_loadfonts);
GALILEOFM_FUNC(function_stopsniffer);

BOOL rename_get_wild(char *,char *,char *,char *);
ExternalEntry *new_external_entry(FunctionHandle *handle,char *path, BPTR lock);

void network_get_info(NetworkInfo *,DirBuffer *,BPTR,UWORD,UWORD,ULONG);

void function_user_run(FunctionHandle *handle,Cfg_Function *function);

long GalileoDeleteFile(char *name,FunctionHandle *,struct _CommandList *,BOOL);

#endif

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

#ifndef _GALILEOFM_FUNCTION_LAUNCH_PROTOS
#define _GALILEOFM_FUNCTION_LAUNCH_PROTOS

#include "function_launch.h"

// protos
BOOL function_launch_quick(ULONG,Cfg_Function *,Lister *);
BOOL function_launch(ULONG,Cfg_Function *,ULONG,ULONG,Lister *,Lister *,char *,char *,BPTR,BPTR,struct ArgArray *,struct Message *,Buttons *);
FunctionHandle *function_new_handle(struct MsgPort *,BOOL);
void function_handle_init(FunctionHandle *,BOOL);
ULONG __asm function_init(register __a0 IPCData *ipc,register __a1 FunctionHandle *handle);
void function_free(FunctionHandle *);
void __saveds function_launch_code(void);
BOOL function_check_abort(FunctionHandle *);
Lister *function_valid_lister(FunctionHandle *handle,Lister *lister);
BOOL function_lock_paths(FunctionHandle *handle,PathList *,int);
void function_unlock_paths(FunctionHandle *handle,PathList *,int);

int function_build_list(FunctionHandle *function,PathNode **,InstructionParsed *);
FunctionEntry *function_new_entry(FunctionHandle *,char *,BOOL);
FunctionEntry *function_current_entry(FunctionHandle *handle);
FunctionEntry *function_next_entry(FunctionHandle *handle);
void function_files_from_args(FunctionHandle *handle);
void function_files_from_glarg(FunctionHandle *handle);
void function_files_from_wbarg(FunctionHandle *handle);
char *function_build_file_string(FunctionHandle *handle,short);

void function_progress_on(FunctionHandle *handle,char *operation,ULONG total,ULONG flags);
BOOL function_progress_update(FunctionHandle *handle,FunctionEntry *entry,ULONG count);
Lister *function_get_paths(FunctionHandle *,PathList *,ULONG,short);
BOOL function_valid_path(PathNode *path);
PathNode *function_add_path(FunctionHandle *,PathList *,Lister *,char *,BPTR);

void function_read_directory(FunctionHandle *handle,Lister *lister,char *,BPTR);
void buffer_list_buffers(Lister *dest_lister);
void build_device_list(Lister *lister);

void function_run_function(FunctionHandle *);
function_parse_function(FunctionHandle *);
void function_parse_arguments(FunctionHandle *,InstructionParsed *);
void function_parse_instruction(FunctionHandle *,char *,unsigned char *,ULONG *);
short function_run(FunctionHandle *);
function_run_instruction(FunctionHandle *,InstructionParsed *);
function_build_instruction(FunctionHandle *,InstructionParsed *,unsigned char *,char *);
function_add_filename(char *,char *,char *,int,short);
void function_build_default(FunctionHandle *handle,char *def_string,char *buffer);
BOOL function_check_paths(FunctionHandle *handle);
short function_check_single(FunctionHandle *,long,long,short);

function_internal_command(struct _CommandList *,STRPTR,FunctionHandle *,InstructionParsed *);
function_internal_async(AsyncData *);
ULONG __asm __saveds async_command_startup(register __a0 IPCData *ipc, register __a1 AsyncData *adata);
void __saveds async_command(void);
struct _CommandList *function_find_internal(char **,short);
ULONG parse_internal_function(char *str,char *template,ULONG orig_flags);
char *parse_find_path(char *str);

function_external_command(FunctionHandle *,InstructionParsed *);
function_open_script(FunctionHandle *);
function_write_script(FunctionHandle *,char *,short);
void function_close_script(FunctionHandle *,int);

short function_check_same_path(char *,char *);
check_file_destination(FunctionHandle *handle,FunctionEntry *entry,char *destination,BPTR destination_lock,short *confirm_each);
function_error(FunctionHandle *handle,char *name,int action_msg,int error_code);
BOOL function_check_dirs(FunctionHandle *);
FunctionEntry *function_get_entry(FunctionHandle *);
function_end_entry(FunctionHandle *,FunctionEntry *,int);
function_check_filter(FunctionHandle *handle);

void function_abort(FunctionHandle *handle);
void function_error_text(FunctionHandle *handle,int);
void function_text(FunctionHandle *handle,char *text);
function_request(FunctionHandle *,char *,ULONG,...);
PathNode *function_path_current(PathList *);
Lister *function_lister_current(PathList *);
PathNode *function_path_next(PathList *);
void function_path_end(FunctionHandle *,PathList *,int);
void function_cleanup(FunctionHandle *,PathNode *,BOOL);
void function_do_lister_changes(FunctionHandle *,PathList *);
void function_perform_changes(FunctionHandle *,PathNode *);

void function_filetype(FunctionHandle *handle);

void function_check_ins_path(
	FunctionHandle *handle,
	InstructionParsed *instruction,
	PathList *path_list,
	char path_type,
	short locker);

void function_replace_paths(
	FunctionHandle *handle,
	PathList *path_list,
	char *new_path,
	short locker);

void function_progress_file(
	FunctionHandle *handle,
	long size,
	long count);

void function_progress_info(FunctionHandle *handle,char *info);

short func_requester(
	FunctionHandle *handle,
	unsigned char *instruction,
	char *buf,
	short max_len,
	short func_len,
	short *position);

FileChangeList *function_find_filechanges(FunctionHandle *,FileChangeList *,char *,BPTR,Lister *,short *);
FileChangeList *function_add_filechanges(FunctionHandle *,char *,BPTR,Lister *);
FileChange *function_filechange_addfile(FunctionHandle *,char *,BPTR,struct FileInfoBlock *,NetworkInfo *,Lister *);
FileChange *function_filechange_loadfile(FunctionHandle *,char *,BPTR,char *,short);
void function_filechange_reloadfile(FunctionHandle *,char *,BPTR,char *,short);
FileChange *function_filechange_delfile(FunctionHandle *,char *,BPTR,char *,Lister *,BOOL);
void function_filechange_do(FunctionHandle *,BOOL);

FileChange *function_filechange_modify(FunctionHandle *,char *,BPTR,char *,Tag tag,...);
FileChange *function_filechange_rename(FunctionHandle *,char *,BPTR,char *);


void function_build_info(FunctionHandle *handle,char *,char *,short);


void function_iconinfo_update(FunctionHandle *handle,Att_List *list);

void get_trunc_filename(char *source,char *dest);
void get_trunc_path(char *source,char *dest);

// function_runcommand.c
Cfg_Function *function_load_function(char *name);

#endif

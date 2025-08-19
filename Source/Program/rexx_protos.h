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

#ifndef _GALILEOFM_REXX_PROTOS
#define _GALILEOFM_REXX_PROTOS

#include "function_launch.h"
#include "rexx.h"

void __saveds rexx_proc(void);
ULONG __asm __saveds rexx_init(register __a0 IPCData *ipc,register __a1 ULONG *foo);
BOOL rexx_process_msg(struct RexxMsg *,struct MsgPort *,long *);
void rexx_set_return(struct RexxMsg *msg,long rc,char *result);
RexxReader *rexx_read_file(short,char *,struct RexxMsg *);
void rexx_goodbye_reader(IPCData *ipc);
void rexx_reply_msg(struct RexxMsg *rmsg);

void rexx_lister_new(struct RexxMsg *msg,char *args);
BOOL rexx_lister_cmd(struct RexxMsg *msg,short command,char *args);
void rexx_lister_file_return(struct RexxMsg *msg,DirBuffer *,short id,char,short,char *);
void rexx_lister_entry_info(struct RexxMsg *msg,DirBuffer *buffer,char *args,short,char *);
void rexx_lister_get_current(struct RexxMsg *,short,short,char *);
void rexx_query_format(short id,ListFormat *format,char *result);
long rexx_set_format(short command,short id,ListFormat *format,char *args);

BOOL rexx_galileo_cmd(struct RexxMsg *,short,char *);

void rexx_app_msg_files(DirEntry *entry,
			DirBuffer *buffer,
			BOOL multifiles,
			char *action,
			Lister *dest,
			char *dest_path,
			unsigned short qual);

void rexx_custom_app_msg(struct AppMessage *msg,DirBuffer *buffer,char *action,Lister *,char *,unsigned short);
char *rexx_build_filestring(struct AppMessage *,ULONG *,ULONG);
short rexx_lister_add_file(Lister *lister,char *args,struct RexxMsg *);
DirEntry *rexx_lister_get_file(DirBuffer *buffer,char **args);
short rexx_lister_remove_file(Lister *lister,char *args);
short rexx_lister_select_file(Lister *lister,char *args,char *result);

short rexx_get_command(char **commandptr);
void rexx_skip_space(char **command);
void rexx_skip_space_reverse(char **command,char *);
long rexx_parse_number(char **ptr,BOOL,long);
BOOL rexx_parse_number_byte(char **,UBYTE *);
STRPTR rexx_parse_word_alloch(char **ptrptr,ULONG *ret_len);
short rexx_parse_word(char **ptr,char *buffer,short bufferlen);
short rexx_match_keyword(char **ptr,char **keys,long *);
BOOL rexx_lister_valid(Lister *lister);
void rexx_set_var(struct RexxMsg *,char *,char *,ULONG,short);
BOOL rexx_get_var(struct RexxMsg *,char *,char *,char *,short);

short AddFunctionTrap(char *,char *,char *);
short RemFunctionTrap(char *,char *);
BOOL FindFunctionTrap(char *,char *,char *);
APTR LockTrapList(void);
void UnlockTrapList(void);
APTR FindTrapEntry(APTR,char  *,char *);

long rexx_add_appicon(char *,struct RexxMsg *);
void rexx_rem_appthing(char *,short);
void rexx_handle_appmsg(struct AppMessage *msg);
BOOL rexx_send_appmsg(RexxAppThing *,short,struct AppMessage *);

long rexx_lister_newprogress(Lister *lister,char *args,long *);

void rexx_set_lister_mode(Lister *lister,char *args);
void rexx_send_command(char *command,BOOL);


short rexx_handler_msg(
	char *handler,
	DirBuffer *buffer,
	short flags,
	ULONG args,...);
short __stdargs rexx_handler_msg_args(
	char *handler,
	DirBuffer *buffer,
	short flags,
	HandlerArg *args);

long rexx_set_appicon(char *str,struct RexxMsg *msg);


BOOL rexx_send_msg(struct MinList *list,IPCMessage **imsg,struct MsgPort *port);


BOOL rexx_send_rxmsg(IPCMessage *imsg,struct MsgPort *rxport);
BOOL rexx_send_rxmsg_args(RexxDespatch *desp,ULONG flags,struct MsgPort *rxport);

short rexx_lister_reload_file(Lister *,char *);

// dos notify
void handle_dos_notify(GalileoNotify *,FunctionHandle *);

void rexx_add_cmd(char *args);
BOOL rexx_lister_movement(Lister *lister,char *args);
void rexx_add_cmd_type(Att_List *list,char *type);


void rexx_remap_icons(BOOL);


void rexx_galileo_get_pen(char **,UBYTE *);
short rexx_galileo_map_pen(short);
void rexx_send_reset(ULONG,ULONG);

#endif

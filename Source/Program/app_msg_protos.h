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

#ifndef _GALILEOFM_APPMSG_PROTOS
#define _GALILEOFM_APPMSG_PROTOS

#include "app_msg.h"

void lister_receive_drop(Lister *dest,GalileoListerAppMessage *msg);

GalileoListerAppMessage *alloc_lister_appmsg(APTR memory,
					     struct MsgPort *reply,
					     WORD num);
void free_lister_appmsg(GalileoListerAppMessage *msg);
void reply_lister_appmsg(GalileoListerAppMessage *msg);
BOOL set_glarg(GalileoListerAppMessage *msg,
	       WORD num,
	       char *name,
	       APTR memory);
void set_wbargs_files(ULONG numargs,
		      struct WBArg *args,
		      struct GLAData *argsdata,
		      BPTR dir_lock,
		      DirEntry *entry,
		      DirBuffer *buffer);
GalileoListerAppMessage *alloc_lister_appmsg_files(
	DirEntry *entry,
	DirBuffer *buffer,
	BOOL multifiles);

struct AppMessage *alloc_appmsg_files(DirEntry *,DirBuffer *,BOOL);


void FreeArgArray(struct ArgArray *);
WORD FindGLArg(struct WBArg *args,WORD count,char *name);


struct ArgArray *BuildArgArray(char *,...);
struct ArgArray *BuildArgArrayA(char **);
struct ArgArray *NewArgArray(void);
struct ArgArrayEntry *NewArgArrayEntry(struct ArgArray *,char *);

#endif

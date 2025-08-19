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

#ifndef _GALILEOFM_FILETYPES_PROTOS
#define _GALILEOFM_FILETYPES_PROTOS

#include "filetypes.h"

// Prototypes
void filetype_read_list(APTR,struct ListLock *);
void filetype_default_list(APTR,struct ListLock *);
Cfg_Filetype *filetype_default_new(APTR,Cfg_FiletypeList *,char *,char *,short,char *,char *);
void filetype_default_menu(APTR,Cfg_Filetype *,char *,char *);
MatchHandle *filetype_new_handle(char *);
void filetype_free_handle(MatchHandle *);
Cfg_Filetype *filetype_identify(char *,BPTR,short,char *,unsigned short);

void parse_prot_string(char *,ULONG *);
void parse_date_string(char *,struct DateStamp *);

BOOL ftype_check_function(char *,int,Lister *,Lister *);
char *ftype_get_description(char *);
void ftype_doubleclick(Lister *,char *,char *,int);
BOOL ftype_icon_drop(struct AppMessage *,int,Lister *);

IPCData *filetype_get_sniffer(void);
void filetype_find_type(Lister *lister,DirBuffer *buffer,char *name,short flags);
void filetype_find_typelist(Lister *lister,short flags);
void __saveds filetype_sniffer_proc(void);

Cfg_Filetype *filetype_find(char *desc,short type);
BOOL filetype_check(Cfg_Filetype *match_type);

BOOL is_default_filetype(Cfg_Filetype *type);

BOOL filetype_match_type(char *,BPTR,Cfg_Filetype *);

#endif

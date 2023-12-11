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

#ifndef _GALILEOFM_ENVOY
#define _GALILEOFM_ENVOY

// Envoy stuff
struct UserInfo
{
	UBYTE	ui_UserName[32];
	UWORD	ui_UserID;
	UWORD	ui_PrimaryGroupID;
	ULONG   ui_Flags;
};

struct GroupInfo
{
	UBYTE	gi_GroupName[32];
	UWORD	gi_GroupID;
};

struct UserInfo *AllocUserInfo( void );
struct GroupInfo *AllocGroupInfo( void );
void FreeUserInfo( struct UserInfo * );
void FreeGroupInfo( struct GroupInfo * );

ULONG IDToUser( unsigned long userID, struct UserInfo *user );
ULONG IDToGroup( unsigned long groupID, struct GroupInfo *group );

#pragma libcall AccountsBase AllocUserInfo 1e 00
#pragma libcall AccountsBase AllocGroupInfo 24 00
#pragma libcall AccountsBase FreeUserInfo 2a 801
#pragma libcall AccountsBase FreeGroupInfo 30 801

#pragma libcall AccountsBase IDToUser 4e 8002
#pragma libcall AccountsBase IDToGroup 54 8002

#define ACTION_Envoy            20000

#define ACTION_NAME_TO_UID      (ACTION_Envoy+0) // arg1 cstring
#define ACTION_NAME_TO_GID      (ACTION_Envoy+1) // arg1 cstring
#define ACTION_UID_TO_USERINFO  (ACTION_Envoy+2)
#define ACTION_GID_TO_GROUPINFO (ACTION_Envoy+3)

#define NEW_ACTION_UID_TO_USERINFO      1037
#define NEW_ACTION_GID_TO_GROUPINFO     1038

#endif

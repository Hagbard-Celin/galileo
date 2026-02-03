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

#include <gfm/version.h>
#define CATCOMP_NUMBERS
#include "envoy.strings"
#include "modules_lib.h"
#include "module.h"

extern ConfigWindow _envoy_window;
extern ObjectDef _envoy_objects[];

enum
{
	ENVOY_LAYOUT=1,
	GAD_ENVOY_OWNER,
	GAD_ENVOY_OWNER_FIELD,
	GAD_ENVOY_GROUP,
	GAD_ENVOY_GROUP_FIELD,
	GAD_ENVOY_OK,
	GAD_ENVOY_CANCEL,
	GAD_ENVOY_GROUP_READ,
	GAD_ENVOY_GROUP_WRITE,
	GAD_ENVOY_GROUP_EXECUTE,
	GAD_ENVOY_GROUP_DELETE,
	GAD_ENVOY_OTHER_READ,
	GAD_ENVOY_OTHER_WRITE,
	GAD_ENVOY_OTHER_EXECUTE,
	GAD_ENVOY_OTHER_DELETE,
};

void envoy_owner_list(struct Window *,ObjectList *,struct UserInfo *,struct Library *);
void envoy_group_list(struct Window *,ObjectList *,struct GroupInfo *,struct Library *);
BOOL envoy_check_owner(struct Window *,ObjectList *,struct UserInfo *,struct Library *,UWORD *);
BOOL envoy_check_group(struct Window *,ObjectList *,struct GroupInfo *,struct Library *,UWORD *);

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
ULONG VerifyUser( STRPTR userName, STRPTR password, struct UserInfo *user );
ULONG MemberOf( struct GroupInfo *group, struct UserInfo *user );
ULONG NameToUser( STRPTR userName, struct UserInfo *user );
ULONG NameToGroup( STRPTR groupName, struct GroupInfo *group );
ULONG IDToUser( unsigned long userID, struct UserInfo *user );
ULONG IDToGroup( unsigned long groupID, struct GroupInfo *group );
ULONG NextUser( struct UserInfo *user );
ULONG NextGroup( struct GroupInfo *group );
ULONG NextMember( struct GroupInfo *group, struct UserInfo *user );
STRPTR ECrypt( STRPTR buffer, STRPTR password, STRPTR username );
ULONG VerifyUserCrypt( STRPTR userName, STRPTR password,
	struct UserInfo *user );

#pragma libcall AccountsBase AllocUserInfo 1e 00
#pragma libcall AccountsBase AllocGroupInfo 24 00
#pragma libcall AccountsBase FreeUserInfo 2a 801
#pragma libcall AccountsBase FreeGroupInfo 30 801
#pragma libcall AccountsBase VerifyUser 36 A9803
#pragma libcall AccountsBase MemberOf 3c 9802
#pragma libcall AccountsBase NameToUser 42 9802
#pragma libcall AccountsBase NameToGroup 48 9802
#pragma libcall AccountsBase IDToUser 4e 8002
#pragma libcall AccountsBase IDToGroup 54 8002
#pragma libcall AccountsBase NextUser 5a 801
#pragma libcall AccountsBase NextGroup 60 801
#pragma libcall AccountsBase NextMember 66 9802
#pragma libcall AccountsBase ECrypt 6c A9803
#pragma libcall AccountsBase VerifyUserCrypt 72 A9803

#define OLD_ACTION_Dummy            20000

#define OLD_ACTION_SET_USER         (OLD_ACTION_Dummy+0)
#define OLD_ACTION_SET_GROUP        (OLD_ACTION_Dummy+1)
#define OLD_ACTION_UID_TO_USERINFO  (OLD_ACTION_Dummy+2)
#define OLD_ACTION_GID_TO_GROUPINFO (OLD_ACTION_Dummy+3)

#define ACTION_UID_TO_USERINFO      1037
#define ACTION_GID_TO_GROUPINFO     1038


#define CATCOMP_NUMBERS
#include "envoy.strings"

#define _GALILEOFM_MODULE_DEF
#include <galileofm/modules.h>

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
BOOL envoy_check_owner(IPCData *,struct Window *,ObjectList *,struct UserInfo *,struct Library *,UWORD *);
BOOL envoy_check_group(IPCData *,struct Window *,ObjectList *,struct GroupInfo *,struct Library *,UWORD *);

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


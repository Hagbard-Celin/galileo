#ifndef CLIB_MULTIUSER_PROTOS_H
#define CLIB_MULTIUSER_PROTOS_H
/*
**      $VER: multiuser_protos.h 39.11 (29.6.94)
**      MultiUser Release 1.8
**
**      C prototypes.
**
**      (C) Copyright 1993-1994 Geert Uytterhoeven
**          All Rights Reserved
*/

#ifndef LIBRARIES_MULTIUSER_H
#include <libraries/multiuser.h>
#endif

ULONG muLogoutA(struct TagItem *taglist);
ULONG muLogout(ULONG tag1type,...);
ULONG muLoginA(struct TagItem *taglist);
ULONG muLogin(ULONG tag1type,...);
ULONG muGetTaskOwner(struct Task *task);
BOOL muPasswd(STRPTR oldpwd, STRPTR newpwd);
struct muUserInfo *muAllocUserInfo(void);
void muFreeUserInfo(struct muUserInfo *info);
struct muUserInfo *muGetUserInfo(struct muUserInfo *info, ULONG keytype);
BOOL muSetDefProtectionA(struct TagItem *taglist);
BOOL muSetDefProtection(ULONG tag1type,...);
ULONG muGetDefProtection(struct Task *task);
BOOL muSetProtection(STRPTR name, LONG mask);
BOOL muLimitDOSSetProtection(BOOL flag);
BOOL muCheckPasswd(struct TagItem *taglist);
BOOL muCheckPasswdTagList(struct TagItem *taglist);
BOOL muCheckPasswdTags(ULONG tag1type,...);
BPTR muGetPasswdDirLock(void);
BPTR muGetConfigDirLock(void);
struct muExtOwner *muGetTaskExtOwner(struct Task *task);
void muFreeExtOwner(struct muExtOwner *owner);
ULONG muGetRelationshipA(struct muExtOwner *user, ULONG owner,
                         struct TagItem *taglist);
ULONG muGetRelationship(struct muExtOwner *user, ULONG owner,
                        ULONG tag1type,...);
struct muExtOwner *muUserInfo2ExtOwner(struct muUserInfo *info);
struct muGroupInfo *muAllocGroupInfo(void);
void muFreeGroupInfo(struct muGroupInfo *info);
struct muGroupInfo *muGetGroupInfo(struct muGroupInfo *info, ULONG keytype);
BOOL muAddMonitor(struct muMonitor *monitor);
void muRemMonitor(struct muMonitor *monitor);
BOOL muKill(struct Task *task);
BOOL muFreeze(struct Task *task);
BOOL muUnfreeze(struct Task *task);


  /*
   *     Private functions
   *
   *     Do NOT call them!!
   */

BOOL muFSRendezVous(void);


#endif  /* CLIB_MULTIUSER_PROTOS_H */

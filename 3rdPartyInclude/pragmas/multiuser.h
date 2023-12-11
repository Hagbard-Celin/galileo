/* "multiuser.library"*/
#pragma libcall muBase muLogoutA 1E 801
#ifdef __SASC_60
#pragma tagcall muBase muLogout 1E 801
#endif
#pragma libcall muBase muLoginA 24 801
#ifdef __SASC_60
#pragma tagcall muBase muLogin 24 801
#endif
#pragma libcall muBase muGetTaskOwner 2A 001
#pragma libcall muBase muPasswd 30 9802
#pragma libcall muBase muAllocUserInfo 36 00
#pragma libcall muBase muFreeUserInfo 3C 801
#pragma libcall muBase muGetUserInfo 42 0802
/*#pragma libcall muBase OBSOLETEmuSetLibFlush 48 001*/
#pragma libcall muBase muSetDefProtectionA 4E 801
#ifdef __SASC_60
#pragma tagcall muBase muSetDefProtection 4E 801
#endif
#pragma libcall muBase muGetDefProtection 54 001
#pragma libcall muBase muSetProtection 5A 2102
#pragma libcall muBase muLimitDOSSetProtection 60 001
#pragma libcall muBase muCheckPasswd 66 801
#pragma libcall muBase muCheckPasswdTagList 66 801
#ifdef __SASC_60
#pragma tagcall muBase muCheckPasswdTags 66 801
#endif
/*#pragma libcall muBase muFSRendezVous 6C 00*/
#pragma libcall muBase muGetPasswdDirLock 72 00
#pragma libcall muBase muGetConfigDirLock 78 00
#pragma libcall muBase muGetTaskExtOwner 7E 001
#pragma libcall muBase muFreeExtOwner 84 801
#pragma libcall muBase muGetRelationshipA 8A 81003
#ifdef __SASC_60
#pragma tagcall muBase muGetRelationship 8A 81003
#endif
#pragma libcall muBase muUserInfo2ExtOwner 90 801
#pragma libcall muBase muAllocGroupInfo 96 00
#pragma libcall muBase muFreeGroupInfo 9C 801
#pragma libcall muBase muGetGroupInfo A2 0802
#pragma libcall muBase muAddMonitor A8 801
#pragma libcall muBase muRemMonitor AE 801
#pragma libcall muBase muKill B4 001
#pragma libcall muBase muFreeze BA 001
#pragma libcall muBase muUnfreeze C0 001

* multiuser_lib.i

	IFND	MULTIUSER_LIB_I
MULTIUSER_LIB_I	SET	1

* "multiuser.library"
_LVOmuLogoutA	EQU	-30
_LVOmuLoginA	EQU	-36
_LVOmuGetTaskOwner	EQU	-42
_LVOmuPasswd	EQU	-48
_LVOmuAllocUserInfo	EQU	-54
_LVOmuFreeUserInfo	EQU	-60
_LVOmuGetUserInfo	EQU	-66
* _LVOOBSOLETEmuSetLibFlush	EQU	-72
_LVOmuSetDefProtectionA	EQU	-78
_LVOmuGetDefProtection	EQU	-84
_LVOmuSetProtection	EQU	-90
_LVOmuLimitDOSSetProtection	EQU	-96
_LVOmuCheckPasswd	EQU	-102
* _LVOmuFSRendezVous	EQU	-108
_LVOmuGetPasswdDirLock	EQU	-114
_LVOmuGetConfigDirLock	EQU	-120
_LVOmuGetTaskExtOwner	EQU	-126
_LVOmuFreeExtOwner	EQU	-132
_LVOmuGetRelationshipA	EQU	-138
_LVOmuUserInfo2ExtOwner	EQU	-144
_LVOmuAllocGroupInfo	EQU	-150
_LVOmuFreeGroupInfo	EQU	-156
_LVOmuGetGroupInfo	EQU	-162
_LVOmuAddMonitor	EQU	-168
_LVOmuRemMonitor	EQU	-174
_LVOmuKill	EQU	-180
_LVOmuFreeze	EQU	-186
_LVOmuUnfreeze	EQU	-192

	ENDC

/*-----init-----*/
#pragma libcall SysInfoBase InitSysInfo 1e 0
#pragma libcall SysInfoBase FreeSysInfo 24 801
/*-----load average-----*/
#pragma libcall SysInfoBase GetLoadAverage 2a 9802
/*-----id-----*/
#pragma libcall SysInfoBase GetPid 30 801
#pragma libcall SysInfoBase GetPpid 36 801
#pragma libcall SysInfoBase GetPgrp 3c 801
/*-----nice-----*/
#pragma libcall SysInfoBase GetNice 42 10803
#pragma libcall SysInfoBase SetNice 48 210804
/*-----notify-----*/
#pragma libcall SysInfoBase AddNotify 4e 10803
#pragma libcall SysInfoBase RemoveNotify 54 9802
/*-----cpu usage-----*/
#pragma libcall SysInfoBase GetCpuUsage 5a 9802
#pragma libcall SysInfoBase GetTaskCpuUsage 60 A9803

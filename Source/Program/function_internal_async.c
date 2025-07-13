/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 2023 Hagbard Celine

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

This program is built from the sources for Directory Opus Magellan II,
released by GPSoftware under the APL license in 2012. Re-licensed under GPL by
permission of Dr Greg Perry, Managing Director of GPSoftware.

Opus® and Directory Opus® and associated images are registered tradataemarks of GPSoftware.
DOpus(TM), DirOpus(TM) and Opus File Manager(TM) are tradataemarked 1991 by GPSoftware.
Opus Magellan (TM) is tradataemarked 1996 GPSoftware.
All other tradataemarks are the property of their respective owners.

The release of Directory Opus 5 under the GPL in NO WAY affects
the existing commercial status of Directory Opus for Windows.

For more information on Directory Opus for Windows please see:

		 http://www.gpsoft.com.au

*/

#include "galileofm.h"
#include "/Modules/modules.h"
#include "/Modules/modules_protos.h"

extern CONST GalileoCallbackInfo CallBackInfo;


function_internal_async(AsyncData *adata)
{
    IPCData *async_proc = NULL;
    int ret;

    // call IPC_Launch
    ret=IPC_Launch(0,
	           &async_proc,
		   adata->command->name,
		   (ULONG)async_command,
		   STACK_DEFAULT,
		   (ULONG)adata,
		   (struct Library *)DOSBase);

    if (!async_proc)
	 ret = 0;
    return ret;
}

ULONG __asm __saveds async_command_startup(register __a0 IPCData *ipc,
					   register __a1 AsyncData *adata)
{
    // make sure we do not get expunged
    adata->module->lib_OpenCnt++;

    return TRUE;
}


void __saveds async_command(void)
{
    AsyncData             *adata;
    IPCData               *ipc;
    struct Library        *ModuleBase;
    APTR                  memhandlep;

    // do IPC startup
    ipc = IPC_ProcStartup((ULONG *) &adata, async_command_startup);

    // did we get ipc?
    if (!ipc)
    {
	if (adata)
	{
	    FreeVec(adata);
	}

	return;
    }

    // Set async flag
    ipc->flags|=IPCF_ASYNC;

    // Swap in new IPCData
    ipc->userdata = adata->handle;
    adata->handle->ipc = ipc;

    // Save ModuleBase for later
    ModuleBase=adata->module;

    // Save memhandlepointer for later
    memhandlep = adata->memhandlep;

    // Call module
    Module_Entry((adata->args)?(struct List *)adata->args:0,
		 adata->screen,
		 ipc,
		 adata->main_ipc,
		 adata->command->function,
		 &CallBackInfo);


    // Free mem
    FreeMemHandle(*adata->memhandlep);
    FreeVec(memhandlep);

    // Free ipc
    IPC_Free(ipc);

    // Put opencount back
    ModuleBase->lib_OpenCnt--;
}

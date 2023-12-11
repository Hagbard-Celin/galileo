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

Opus� and Directory Opus� and associated images are registered trademarks of GPSoftware.
DOpus(TM), DirOpus(TM) and Opus File Manager(TM) are trademarked 1991 by GPSoftware.
Opus Magellan (TM) is trademarked 1996 GPSoftware.
All other trademarks are the property of their respective owners.

The release of Directory Opus 5 under the GPL in NO WAY affects
the existing commercial status of Directory Opus for Windows.

For more information on Directory Opus for Windows please see:

                 http://www.gpsoft.com.au

*/

#include "galileofm.h"

// StopSniff internal function
GALILEOFM_FUNC(function_stopsniffer)
{
	Lister *lister;

	// Get current lister
	if (lister=function_lister_current(&handle->source_paths))
	{
		IPCData *ipc;

		// Lock process list
		lock_listlock(&GUI->process_list,FALSE);

		// See if filetypes process is present
		if (ipc=IPC_FindProc(&GUI->process_list,"galileo_filetype_sniffer",FALSE,0))
		{
			// Send abort command
			IPC_Command(ipc,IPC_ABORT,0,lister,0,0);

			// Send break
			Signal((struct Task *)ipc->proc,SIGBREAKF_CTRL_C);
		}

		// Unlock process list
		unlock_listlock(&GUI->process_list);
	}

	return 1;
}

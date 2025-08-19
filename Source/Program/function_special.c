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

#include "galileofm.h"
#include "misc_protos.h"
#include "function_protos.h"
#include "menu_data.h"

// Quit, Hide, Reveal
GALILEOFM_FUNC(function_special)
{
	// Look at command
	switch (command->function)
	{
		// Quit
		case FUNC_QUIT:

			// Force quit?
			if (instruction->ipa_funcargs &&
				instruction->ipa_funcargs->FA_Arguments[0])
			{
				// Send quit message directory
				IPC_Quit(&main_ipc,0,FALSE);
			}

			// Otherwise, start quit process
			else
			{
				misc_startup("galileo_quit",MENU_QUIT,GUI->window,0,1);
			}
			return 0;


		// Hide
		case FUNC_HIDE:

			// Send hide command
			IPC_Command(&main_ipc,IPC_HIDE,0,0,0,0);
			break;


		// Reveal
		case FUNC_REVEAL:

			// Send show command
			IPC_Command(&main_ipc,IPC_SHOW,0,0,0,0);
			break;
	}

	return 1;
}

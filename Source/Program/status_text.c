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
#include "status_text_protos.h"

// Display an error message
status_display_error(Lister *lister,int errcode)
{
	static char error_text[80];

	// Need to get error code ourselves?
	if (errcode==-1) errcode=IoErr();

	// Valid code?
	if (errcode==0) return 0;

	// Get error message
	Fault(errcode,GetString(&locale,MSG_DOS_ERROR),error_text,80);

	// Print error
	status_text(lister,error_text);
	return 1;
}


// Display status text
void status_text(Lister *lister,char *text)
{
	if (lister)
	{
		char *copy;

		// Copy text
		if (copy=AllocVec(strlen(text)+1,0))
			strcpy(copy,text);

		IPC_Command(
			lister->ipc,
			LISTER_STATUS,
			0,
			text,
			copy,
			0);
	}
}


// Display OK status text
void status_okay(Lister *lister)
{
	status_text(lister,GetString(&locale,MSG_OKAY));
}


// Display Aborted status text
void status_abort(Lister *lister)
{
	status_text(lister,GetString(&locale,MSG_ABORTED));
}

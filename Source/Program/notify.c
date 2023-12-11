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

// Start notify on a file
struct NotifyRequest *start_file_notify(
	char *filename,
	ULONG userdata,
	struct MsgPort *port)
{
	struct NotifyRequest *notify;

	// Create request
	if (!(notify=AllocMemH(global_memory_pool,sizeof(struct NotifyRequest)+strlen(filename)+1)))
		return 0;

	// Fill out request structure
	notify->nr_Name=(char *)(notify+1);
	strcpy(notify->nr_Name,filename);
	notify->nr_UserData=userdata;
	notify->nr_Flags=NRF_SEND_MESSAGE;
	notify->nr_stuff.nr_Msg.nr_Port=port;

	// Start notify
	if (!(StartNotify(notify)))
	{
		// Failed to start
		FreeMemH(notify);
		return 0;
	}

	// No worries
	return notify;
}


// Stop notify on a file
void stop_file_notify(struct NotifyRequest *notify)
{
	// Valid request structure?
	if (notify)
	{
		// End notify
		EndNotify(notify);

		// Free stuff
		FreeMemH(notify);
	}
}

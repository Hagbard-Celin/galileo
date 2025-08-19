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
#include "lister_protos.h"

/*
APTR __stdargs OpenProgressWindowTags(Tag tag1,...);
void __stdargs SetProgressWindowTags(APTR prog,Tag tag1,...);
*/

// Display the progress indicator
void lister_progress_on(Lister *lister,ProgressPacket *packet)
{
	// Valid window?
	if (!lister->window) return;

	// Is the progress indicator already displayed?
	if (lister->progress_window)
	{
		// Close existing window
		CloseProgressWindow(lister->progress_window);
	}

	// Open progress indicator
	lister->progress_window=
		OpenProgressWindowTags(
			PW_Window,lister->window,
			PW_Title,packet->operation,
			PW_FileCount,packet->total,
			PW_Flags,packet->flags,
			PW_SigTask,lister->ipc->proc,
			PW_SigBit,lister->abort_signal,
			TAG_END);
}


// Remove progress indicator
void lister_progress_off(Lister *lister)
{
	CloseProgressWindow(lister->progress_window);
	lister->progress_window=0;
}


// Set progress total
void lister_progress_total(Lister *lister,long total,long count)
{
	SetProgressWindowTags(lister->progress_window,
		PW_FileCount,total,
		PW_FileNum,count,
		TAG_END);
}

// Set file total
void lister_progress_filetotal(Lister *lister,long total)
{
	SetProgressWindowTags(lister->progress_window,
		PW_FileSize,total,
		TAG_END);
}

// Update file progress
void lister_progress_fileprogress(Lister *lister,long count)
{
	SetProgressWindowTags(lister->progress_window,
		PW_FileDone,count,
		TAG_END);
}

// Set file progress
void lister_progress_file(Lister *lister,long total,long count)
{
	SetProgressWindowTags(lister->progress_window,
		PW_FileSize,total,
		PW_FileDone,count,
		TAG_END);
}


// Update file progress info
void lister_progress_info(Lister *lister,char *info)
{
	SetProgressWindowTags(lister->progress_window,
		PW_Info,info,
		TAG_END);
}


// Update progress title
void lister_progress_title(Lister *lister,char *info)
{
	SetProgressWindowTags(lister->progress_window,
		PW_Title,info,
		TAG_END);
}

/*
APTR __stdargs __saveds OpenProgressWindowTags(Tag tag1,...)
{
	return OpenProgressWindow((struct TagItem *)&tag1);
}

void __stdargs __saveds SetProgressWindowTags(APTR prog,Tag tag1,...)
{
	SetProgressWindow(prog,(struct TagItem *)&tag1);
}
*/

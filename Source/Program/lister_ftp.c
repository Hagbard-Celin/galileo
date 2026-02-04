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
#include <gfm/function_flags.h>
#include "lister_protos.h"
#include "function_launch_protos.h"
#include "commands.h"
#include "menu_data.h"
#include "lsprintf_protos.h"

// See if a path entered means go to FTP
BOOL lister_check_ftp(Lister *lister,char *path)
{
	char *ptr;
	short pos;
	Cfg_Function *function;

	if (!path || !path[0])
	    return 0;

	// Look for ftp:// string
	if (strnicmp(path,"ftp://",6)!=0) return 0;

	// Copy the string
	ptr=lister->work_buffer+512;
	stccpy(ptr,path+6,450);

	// Refresh the path field
	lister_update_pathfield(lister);

	// Find first slash
	for (pos=0;ptr[pos];pos++)
		if (ptr[pos]=='/') break;

	// Got a slash?
	if (ptr[pos]=='/')
	{
		// Change to a null
		ptr[pos]=0;
		++pos;
	}
	else pos=0;

	// Build command
	stccpy(lister->work_buffer,"FTPConnect",512);

	// Valid host?
	if (*ptr)
	{
		strcat(lister->work_buffer," HOST=");
		strcat(lister->work_buffer,ptr);
	}

	// Got a directory?
	if (pos>0 && ptr[pos])
	{
		BOOL suffix=0,ok=0;

		// Bump pointer to path
		ptr+=pos;

		// Try and be smart; if path has a suffix it probably points to a file
		for (pos=strlen(ptr)-1;pos>0;pos--)
		{
			if (ptr[pos]=='/' || ptr[pos]=='\\') break;
			else
			if (ptr[pos]=='.')
			{
				suffix=1;
			}
		}

		// Got a slash?
		if (ptr[pos]=='/' || ptr[pos]=='\\')
		{
			// If the path had a suffix, strip the last element
			if (suffix) ptr[pos]=0;

			// Set flag
			if (pos>0) ok=1;
		}

		// Still got directory?
		if ((ok && pos>0) || (!ok && !suffix))
		{
			// Add directory field
			strcat(lister->work_buffer," DIR=\"");
			strcat(lister->work_buffer,ptr);
			strcat(lister->work_buffer,"\"");
		}
	}

	// Add lister
	strcat(lister->work_buffer," LISTER=");
	lsprintf(lister->work_buffer+strlen(lister->work_buffer),"%ld",lister);

	// Create new function
	if (function=new_default_function(lister->work_buffer,global_memory_pool))
	{
		// Set flag to free function
		function->function.flags2|=FUNCF2_FREE_FUNCTION;

		// Execute function
		function_launch_quick(
			FUNCTION_RUN_FUNCTION,
			function,
			lister);
	}

	return 1;
}

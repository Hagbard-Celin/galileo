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
#include "/Modules/modules.h"

extern short revision_num, version_num;
extern char *version_string;

// Show about requester
void show_about(struct Screen *screen,IPCData *ipc)
{
	char buf[80],*buffer;
	struct Library *ModuleBase;
	Att_List *list;
	Att_Node *node;

	// Create list of text
	if (!(list=Att_NewList(0)))
		return;

	// Build lines
	lsprintf(buf,"%s %ld.%ld %s",galileo_name,version_num,revision_num,version_string);
	Att_NewNode(list,buf,2,0);
	Att_NewNode(list,about_1,1,0);
	Att_NewNode(list,about_2,1,0);
    Att_NewNode(list,about_3,1,0);
    Att_NewNode(list,about_4,2,0);

	// Name of the translator
	Att_NewNode(list,GetString(&locale,MSG_TRANSLATION_BY_YOUR_NAME),3,0);

    Att_NewNode(list,"This program is licensed under the terms of the GNU",1,0);
    Att_NewNode(list,"General Public License version 2 or later.",1,0);
    Att_NewNode(list,"See the file COPYING in the distribution archive.",1,0);
    Att_NewNode(list,"",1,0);
    Att_NewNode(list,"The release of Directory Opus 5 under the GPL in NO",1,0);
    Att_NewNode(list,"WAY affects the existing commercial status of Directory",1,0);
    Att_NewNode(list,"Opus for Windows.",1,0);
    Att_NewNode(list,"",1,0);
    Att_NewNode(list,"For more information on Directory Opus for Windows please",1,0);
    Att_NewNode(list,"see: http://www.gpsoft.com.au",1,0);
    Att_NewNode(list,"",1,0);
    Att_NewNode(list,"Directory Opus® is a registered trademarks of GPSoftware.",1,0);

	// Try for external about library
	if ((ModuleBase=OpenLibrary("PROGDIR:modules/about.gfmmodule",0)))
	{
		short ret;

		// Show about
		ret=Module_Entry((struct List *)list,screen,ipc,&main_ipc,0,0);
		CloseLibrary(ModuleBase);

		// If it displayed ok, return
		if (ret)
		{
			Att_RemList(list,0);
			return;
		}
	}

	// Allocate buffer
	if (buffer=AllocVec(1024,0))
	{
		// Clear buffer
		buffer[0]=0;

		// Build about text
		for (node=(Att_Node *)list->list.lh_Head;
			node->node.ln_Succ;
			node=(Att_Node *)node->node.ln_Succ)
		{
			// Skip?
			if (node->att_data==3) continue;

			// Add to string
			strcat(buffer,node->node.ln_Name);
			if (node->att_data>0)
			{
				strcat(buffer,(node->att_data==2)?"\n\n":"\n");
			}
		}

		// Display about text
		super_request_args(
			screen,
			buffer,
			SRF_SCREEN_PARENT|SRF_IPC,
			ipc,
			GetString(&locale,MSG_OKAY),0);

		// Free buffer
		FreeVec(buffer);
	}

	// Free list
	Att_RemList(list,0);
}

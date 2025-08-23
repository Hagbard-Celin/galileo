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

#include "config_lib.h"
#include "config_environment.h"
#include "//Modules/modules.h"

// Show list of status bar options
void _config_env_status_list(ObjectList *objlist,ULONG id,long first,long last)
{
	Att_List *list;
	short a,b;
	char name[80];

	// Build list
	if (!(list=Att_NewList(0))) return;
	for (a=first;a<=((last)?last:32767);a++)
	{
		stccpy(name,GetString(locale,a),sizeof(name));
		if (name[0]=='-' && name[1]==0)
			break;
		for (b=0;name[b] && name[b]!='\t';b++)
			if (name[b]=='*') name[b]='%';
		Att_NewNode(list,name,0,0);
	}

	// Make window busy
	SetWindowBusy(objlist->window);

	// Display selection list
	a=SelectionList(list,objlist->window,0,
		GetString(locale,MSG_ENVIRONMENT_LISTER_SELECT_STATUS),
		-1,
		0,
		0,
		GetString(locale,MSG_OKAY),
		GetString(locale,MSG_CANCEL),0,0);

	// Clear busy
	ClearWindowBusy(objlist->window);

	// Selection?
	if (a!=-1)
	{
		Att_Node *node;

		// Get node
		if (node=Att_FindNode(list,a))
		{
			char *ptr,buf[10];

			// Get string pointer
			ptr=strchr(node->node.ln_Name,'\t');
			if (ptr) stccpy(buf,node->node.ln_Name,(ptr-node->node.ln_Name)+1);
			else stccpy(buf,node->node.ln_Name,sizeof(buf));

			// Insert into gadget
			funced_edit_insertstring(objlist,id,buf);
		}
	}

	// Free list
	Att_RemList(list,0);
}

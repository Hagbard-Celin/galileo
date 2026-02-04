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

#include <ctype.h>
#include <stdlib.h>
#include <proto/intuition.h>
#include <proto/asl.h>
#include <gfm/suboptionhandle.h>
#include <gfm/list_management.h>
#include <gfm/objectlist.h>
#include <gfm/gadtools.h>
#include <gfm/cfg_function.h>
#include <gfm/cfg_instruction.h>
#include <gfm/windowdata.h>
#include <gfm/config_command.h>
#include <gfm/icon_cache_protos.h>
#include <gfm/configuration.h>
#include "funced_internal_protos.h"
#include "config_lib.h"

#define CATCOMP_NUMBERS
#include "config.strings"


// Build a sub-option list
Att_List *build_sub_options(SubOptionHandle *list)
{
	Att_List *options;

	if (options=Att_NewList(0))
	{
		int a;

		// Add names to list
		for (a=0;list[a].num>-1;a++)
			Att_NewNode(options,GetString(locale,list[a].name),a,ADDNODE_SORT);
	}
	return options;
}

// Get line from coordinates
short functioned_get_line(
	struct Window *window,
	GL_Object *object,
	unsigned short x,unsigned short y)
{
	ULONG storage;

	// Get window-relative coordinates
	x-=window->LeftEdge;
	y-=window->TopEdge;

	// Send coordinates
	storage=(x<<16)|y;

	// Get selection
	GetAttr(GLV_GetLine,GADGET(object),&storage);

	return (short)storage;
}


// Get pointer to function label
char *function_label(Cfg_ButtonFunction *func)
{
	// Valid function?
	if (!func) return 0;

	// Got name?
	if (func->node.ln_Name && *func->node.ln_Name) return func->node.ln_Name;

	// Got label?
	if (func->label && *func->label) return func->label;

	// No label
	return 0;
}


// Check if a path is valid for saving
BOOL config_valid_path(char *path)
{
	char *ptr;

	// Nothing at all?
	if (!path || !*path) return 0;

	// Get file part
	if (!(ptr=FilePart(path))) return 0;

	// Untitled?
	if (strcmp(ptr,GetString(locale,MSG_UNTITLED))==0) return 0;

	// Ok
	return 1;
}


// File requester
BOOL config_filereq(struct Window *window,ULONG title,char *path,char *defpath,short flags)
{
	char *ptr=0;
	char file[40];
	BOOL ok;

	// Invalid path?
	if (!*path) stccpy(path,defpath,256);

	// Get filename
	else
	if (ptr=FilePart(path))
	{
		stccpy(file,ptr,sizeof(file));
		*ptr=0;
		ptr=file;
	}

	// Display file requester
	if (ok=
		AslRequestTags(
			DATA(window)->request,
			ASLFR_Window,window,
			ASLFR_TitleText,GetString(locale,title),
			ASLFR_InitialFile,(ptr)?ptr:"",
			ASLFR_InitialDrawer,path,
			ASLFR_Flags1,FRF_PRIVATEIDCMP|((flags&1)?FRF_DOSAVEMODE:0),
			TAG_END))
	{
		// Get new path
		stccpy(path,DATA(window)->request->fr_Drawer,256);
		AddPart(path,DATA(window)->request->fr_File,256);
	}

	return ok;
}


// Create instruction from dropped file
Cfg_Instruction *instruction_from_wbarg(struct WBArg *arg,APTR memory)
{
	char buf[256],buffer[300];
	short type;
	BOOL quotes=0;

	// Check argument
	if ((type=funced_appmsg_arg(arg,buf))==-1)
		return 0;

	// Clear buffer
	*buffer=0;

	// Directory?
	if (type==INST_DIR)
	{
		// Scandir command
		strcpy(buffer,"ScanDir NEW ");
		type=INST_COMMAND;
	}

	// Project?
	else
	if (type==INST_PROJECT)
	{
		struct DiskObject *icon;

		// Try to get icon
		if (icon=GetCachedDiskObjectNew(buf,1))
		{
			// Got default tool?
			if (icon->do_DefaultTool)
			{
				stccpy(buffer,icon->do_DefaultTool,sizeof(buffer)-1);
				strcat(buffer," ");
			}

			// Free icon
			FreeCachedDiskObject(icon);

			// Change type
			type=INST_WORKBENCH;
		}

		// If no default tool, use SmartRead
		if (!*buffer)
		{
			// SmartRead it
			strcpy(buffer,"SmartRead ");

			// Change type
			type=INST_COMMAND;
		}
	}

	// Any spaces in name?
	if (strchr(buf,' '))
	{
		// Add quote
		strcat(buffer,"\"");
		quotes=1;
	}

	// Add filename to buffer
	strcat(buffer,buf);

	// Terminating quote
	if (quotes) strcat(buffer,"\"");

	// Add instruction
	return NewInstruction(memory,type,buffer);
}


void parse_number(char **ptr,unsigned short *val)
{
	// Digit?
	if (isdigit(*(*ptr)))
	{
		*val=atoi(*ptr);
		while (*(*ptr) && isdigit(*(*ptr))) ++*ptr;
		++*ptr;
	}
}


void store_window_pos(struct Window *window,struct IBox *pos)
{
	pos->Left=window->LeftEdge;
	pos->Top=window->TopEdge;
	pos->Width=window->Width-window->BorderLeft-window->BorderRight;
	pos->Height=window->Height-window->BorderTop-window->BorderBottom;
}

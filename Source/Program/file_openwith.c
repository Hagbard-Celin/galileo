/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 1993-2012 Jonathan Potter & GP Software
Copyright 2025 Hagbard Celine

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
#include "file_openwith.h"
#include "lsprintf_protos.h"

void file_open_with(struct Window *window,char *name,BPTR parent_lock, ULONG flags)
{
	char tmp, *buffer, *pos;
	struct DiskObject *icon;
	BPTR org_dir, tool_parent = 0;
	ULONG stack_size;
	short len,wb=0,ok=0;

	// Allocate buffer
	if (!(buffer=AllocVec(1400,MEMF_CLEAR)))
		return;

	// App already selected?
	if (flags&(1<<30))
	{
		short num;
		struct Node *node;

		// Get application number in list
		num=flags&~(1<<30);

		// Lock the list
		lock_listlock(&GUI->open_with_list,0);

		// Find the node
		for (node=GUI->open_with_list.list.lh_Head;
			node->ln_Succ;
			node=node->ln_Succ)
		{
			// Matched?
			if (num--==0) break;
		}

		// Valid node?
		if (node->ln_Succ)
		{
			// Get the application name
			strcpy(buffer+1,node->ln_Name);
			ok=1;
		}

		// Unlock the list
		unlock_listlock(&GUI->open_with_list);
	}

	// Need to ask?
	if (!ok)
	{
		BPTR lock;
		char *fullpath = 0;

		// Build title
		lsprintf(buffer+1024,GetString(&locale,MSG_SELECT_APP),FilePart(name));

		// Ask for file
		if (!(request_file(
			window,
			buffer+1024,
			buffer+1,
			0,
			0,0)) || !*(buffer+1))
		{
			FreeVec(buffer);
			return;
		}

		if (!(lock = LockFromPath(buffer+1, NULL, FALSE)))
		{
			FreeVec(buffer);
			return;
		}

		// Relative path? convert to absolute
		if (!(strchr(buffer + 1, ':')))
		    fullpath = PathFromLock(NULL, lock, NULL, NULL);

		// Add App to OpenWith list
		add_open_with(fullpath?fullpath:buffer + 1);

		tool_parent = ParentDir(lock);

		if (fullpath)
		    FreeMemH(fullpath);

		UnLock(lock);
	}

	pos = FilePart(buffer+1);
	tmp = *pos;
	*pos = 0;

	if (!tool_parent && !(tool_parent = LockFromPath(buffer + 1, NULL, FALSE)))
	{
	    FreeVec(buffer);
	    return;
	}

	org_dir = CurrentDir(tool_parent);

	*pos = tmp;

	// See if app has an icon
	if (icon=GetDiskObject(pos))
	{
		// Is it a tool?
		if (icon->do_Type==WBTOOL)
		{
		    // Launch as Workbench App
		    wb=1;
		}

		// Free icon
		else
		FreeDiskObject(icon);
	}

	CurrentDir(org_dir);

	if (wb)
	{
	    struct WBArg *args;
	    LONG argsnum = 1;

	    ok = 0;

	    if (args = AllocVec(sizeof(struct WBArg) * 2 + 1, MEMF_CLEAR|MEMF_PUBLIC))
	    {
		if (args[0].wa_Name = AllocVec(strlen(pos) + 1, MEMF_PUBLIC))
		{
		    strcpy(args[0].wa_Name, pos);
		    args[0].wa_Lock = tool_parent;

		    if (args[1].wa_Name = AllocVec(strlen(name) + 1, MEMF_PUBLIC))
		    {
			strcpy(args[1].wa_Name, name);
			args[1].wa_Lock = parent_lock;
		    }   ok = 1;
		}
	    }

	    if (ok)
	    {
		struct LaunchWBArg wbargs;

		if (icon->do_StackSize > environment->env->default_stack)
		    stack_size = icon->do_StackSize;
		else
		    stack_size = environment->env->default_stack;

	        // Fill in data for launch-packet.
	        wbargs.lwba_args = args;
		wbargs.lwba_numargs = 2;
	        wbargs.lwba_stack = stack_size;
		wbargs.lwba_default_tool = 0;
	        if (icon->do_ToolWindow)
		    wbargs.lwba_toolwindow = icon->do_ToolWindow;
	        else
		    wbargs.lwba_toolwindow = 0;

	        if (!(WB_LaunchArg(&wbargs,GUI->screen_pointer,0,0,0,0)))
	        {
		    FreeLaunchWBArgs(&args,argsnum);
	        }
	    }
	    else
	    {
		if (args)
		{
		    if (args[0].wa_Name)
			FreeVec(args[0].wa_Name);

		    //if (args[0].wa_Lock)
			//UnLock(args[0].wa_Lock);

		    FreeVec(args);

		}

		UnLock(tool_parent);

		if (parent_lock)
		    UnLock(parent_lock);
	    }

	    FreeDiskObject(icon);

	}
	else
	{
	    char output[180];
	    BPTR file;

	    if (pos != buffer + 1)
	     strcpy(buffer + 1, pos);

	    // Fill in quotes
	    buffer[0]='\"';
	    buffer[(len=strlen(buffer))]='\"';

	    // Add filename in quotes
	    buffer[len+1]=' ';
	    buffer[len+2]='\"';
	    strcpy(buffer+len+3,name);
	    buffer[(len=strlen(buffer))]='\"';
	    buffer[len+1]=0;

	    // Output window
	    lsprintf(output,"%s%s/AUTO/CLOSE/WAIT/SCREEN %s",
		    environment->env->output_device,
		    environment->env->output_window,
		    get_our_pubscreen());

	    // Open output
	    if (!(file=Open(output,MODE_OLDFILE)))
		    file=Open("nil:",MODE_OLDFILE);

	    // Launch the function
	    CLI_Launch(buffer,(struct Screen *)-1,parent_lock,file,0,LAUNCHF_USE_STACK,environment->env->default_stack,tool_parent);
	    //file_launch(buffer,wb,"ram:");
	}

	// Free buffer
	FreeVec(buffer);
}


// Launch a function from a string
BOOL file_launch(char *name,short wb,char *curdir)
{
	char output[180];
	BPTR file;

	// Workbench launch?
	if (wb) return WB_Launch(name,(struct Screen *)-1,0,environment->env->default_stack,0,0,0,0);

	// Output window
	lsprintf(output,"%s%s/AUTO/CLOSE/WAIT/SCREEN %s",
		environment->env->output_device,
		environment->env->output_window,
		get_our_pubscreen());

	// Open output
	if (!(file=Open(output,MODE_OLDFILE)))
		file=Open("nil:",MODE_OLDFILE);

	// Run command
	return CLI_Launch(name,(struct Screen *)-1,Lock(curdir,ACCESS_READ),file,0,LAUNCHF_USE_STACK,environment->env->default_stack,NULL);
}


// Read the OpenWith file
void startup_read_openwith(void)
{
	APTR file;

	// Lock list
	lock_listlock(&GUI->open_with_list,1);

	// Open file
	if (file=OpenBuf("PROGDIR:System/OpenWith",MODE_OLDFILE,4000))
	{
		char line[256];
		short len;

		// Read lines
		while ((len=ReadBufLine(file,line,255))>0)
		{
			struct Node *node;

			// Allocate node
			if (node=AllocMemH(global_memory_pool,sizeof(struct Node)+len+1))
			{
				// Initialise node and copy string
				node->ln_Name=(char *)(node+1);
				strcpy(node->ln_Name,line);

				// Add to Open With list
				AddTail((struct List *)&GUI->open_with_list,node);
			}
		}

		// Close file
		CloseBuf(file);
	}

	// Unlock list
	unlock_listlock(&GUI->open_with_list);
}


// Add an entry to the OpenWith file
void add_open_with(char *line)
{
	APTR file;
	struct Node *node;

	// Lock list
	lock_listlock(&GUI->open_with_list,1);

	// Too many nodes?
	if (Att_NodeCount((Att_List *)&GUI->open_with_list)>=environment->env->settings.max_openwith)
	{
		// List is empty?
		if (IsListEmpty((struct List *)&GUI->open_with_list.list))
			return;

		// Get first node
		node=GUI->open_with_list.list.lh_Head;

		// Remove first node and free it
		RemHead((struct List *)&GUI->open_with_list);
		FreeMemH(node);
	}

	// Allocate a new node
	if (node=AllocMemH(global_memory_pool,sizeof(struct Node)+strlen(line)+1))
	{
		// Initialise node and copy string
		node->ln_Name=(char *)(node+1);
		strcpy(node->ln_Name,line);

		// Add to Open With list
		AddTail((struct List *)&GUI->open_with_list,node);
	}

	// Open file for output
	if (file=OpenBuf("PROGDIR:System/OpenWith",MODE_NEWFILE,4000))
	{
		// Write lines
		for (node=GUI->open_with_list.list.lh_Head;node->ln_Succ;node=node->ln_Succ)
		{
			// Write line
			WriteBuf(file,node->ln_Name,strlen(node->ln_Name));
			WriteBuf(file,"\n",1);
		}

		// Close file
		CloseBuf(file);
	}

	// Unlock list
	unlock_listlock(&GUI->open_with_list);
}

/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 1993-2012 Jonathan Potter & GP Software
Copyright 2023 Hagbard Celine

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

#include "galileofmlib.h"

// Places to look for a path
static const char __far
	*path_places[]={
		"Workbench",
		"Initial CLI",
		"Shell Process",
		"New_WShell",
		"Background CLI",0};

typedef struct
{
	BPTR	next;
	BPTR	lock;
} PathListEntry;


// Get a usable path list
BPTR __asm __saveds L_GetDosPathList(register __a0 BPTR copy_list)
{
    short num;
    struct Process *proc;
    struct CommandLineInterface *cli;
    PathListEntry *path,*new_entry,*last_entry=0;
    BPTR new_path=0;
    APTR file;
    BOOL duplicate=FALSE;

    if (copy_list)
	duplicate=TRUE;

    // See if path environment variable exists
    if ((!duplicate) && (file=L_OpenBuf("env:Galileo/paths",MODE_OLDFILE,4096)))
    {
	char buf[300];

	// Read paths from file
	while (L_ReadBufLine(file,buf,sizeof(buf)-1)>0)
	{
	    // Allocate a new path entry
	    if (new_entry=AllocVec(sizeof(PathListEntry),0))
	    {
		// Link to previous
		if (!new_path) new_path=MKBADDR(new_entry);
		else last_entry->next=MKBADDR(new_entry);
		last_entry=new_entry;
		new_entry->next=0;

		// Get lock on path
		new_entry->lock=Lock(buf,ACCESS_READ);
	    }
	}

	// Close file
	L_CloseBuf(file);

	// If we got a path then return it
	if (new_path)
	{

    	    // Add Galileo:C before returning
#ifdef RESOURCE_TRACKING
    	    if (new_entry=NRT_AllocVec(sizeof(PathListEntry),0))
#else
	    if (new_entry=AllocVec(sizeof(PathListEntry),0))
#endif
    	    {
		// Add to head of list
		new_entry->next=new_path;
		new_path=MKBADDR(new_entry);
	    	new_entry->lock=Lock("Galileo:C",ACCESS_READ);
    	    }

    	    return new_path;
	}
    }

    // Start with current process
    proc=(struct Process *)FindTask(0);

    // Forbid so processes do not disappear on us
    Forbid();

    // Go through all possible places
    for (num=0;;num++)
    {
	// Get CLI structure, see if it has a path
	if (copy_list ||
	    (proc &&
	    (cli=(struct CommandLineInterface *)BADDR(proc->pr_CLI)) &&
	    (copy_list=cli->cli_CommandDir)))
	{
	    // Go through path list
	    for (path=(PathListEntry *)BADDR(copy_list);
		 path;
		 path=BADDR(path->next))
	    {
		// Allocate a new entry
#ifdef RESOURCE_TRACKING
		if (new_entry=NRT_AllocVec(sizeof(PathListEntry),0))
#else
		if (new_entry=AllocVec(sizeof(PathListEntry),0))
#endif
		{
		    // Link to previous
		    if (!new_path) new_path=MKBADDR(new_entry);
		    else last_entry->next=MKBADDR(new_entry);
		    last_entry=new_entry;
		    new_entry->next=0;

		    // Duplicate lock
#ifdef RESOURCE_TRACKING
		    new_entry->lock=NRT_DupLock(path->lock);
#else
		    new_entry->lock=DupLock(path->lock);
#endif
		}
	    }


	    // Did we get a path
	    if (new_path)
	    {  
		// If not duplicating, add Galileo:C
    	        if (!duplicate)
    	        {
#ifdef RESOURCE_TRACKING
		    if (new_entry=NRT_AllocVec(sizeof(PathListEntry),0))
#else
		    if (new_entry=AllocVec(sizeof(PathListEntry),0))
#endif
		    {
			// Add to head of list
			new_entry->next=new_path;
			new_path=MKBADDR(new_entry);
#ifdef RESOURCE_TRACKING
			new_entry->lock=NRT_Lock("Galileo:C",ACCESS_READ);
#else
			new_entry->lock=Lock("Galileo:C",ACCESS_READ);
#endif
		    }
	    	}

		// Return it
		break;
	    }
	}

	// Nowhere else to look?
	if (!path_places[num])
	{
    	    break;
	}

	// Reset copy_list
	copy_list=0;

	// Find next process
	proc=(struct Process *)FindTask(path_places[num]);
    }

    // Enable multitasking
    Permit();

    // Return new path (if we got one)
    return new_path;
}


/// Update our path list
void __asm __saveds L_UpdatePathList(
	register __a6 struct MyLibrary *libbase)
{
    struct LibData *data;

    // Get library data
    data=(struct LibData *)libbase->ml_UserData;

    // Lock path list
    L_GetSemaphore(&data->path_lock,SEMF_EXCLUSIVE,0);

    // Free path list
    L_FreeDosPathList(data->path_list);

    // Get new path list
    data->path_list=L_GetDosPathList(0);

    // Unlock path list
    L_FreeSemaphore(&data->path_lock);

    // Send command to launcher to reset it
    L_IPC_Command(launcher_ipc,IPC_RESET,0,0,0,NO_PORT_IPC);
}
///

/// Update a process path list
void __asm __saveds L_UpdateMyPaths(
	register __a6 struct MyLibrary *libbase)
{
    BPTR pathlist;
    struct Process *proc;
    struct LibData *data;
    struct CommandLineInterface *cli;

    // Get library data
    data=(struct LibData *)libbase->ml_UserData;

    // Get this process
    proc=(struct Process *)FindTask(0);

    // Get CLI structure
    if (!(cli=(struct CommandLineInterface *)BADDR(proc->pr_CLI)))
	return;

    // Lock path list
    L_GetSemaphore(&data->path_lock,SEMF_SHARED,0);

    // Get path list copy
    pathlist=L_GetDosPathList(data->path_list);

    // Unlock path list
    L_FreeSemaphore(&data->path_lock);

    // Got valid path list?
    if (pathlist)
    {
	// Free existing process path list
	if (cli->cli_CommandDir)
	    L_FreeDosPathList(cli->cli_CommandDir);

	// Store new path list
	cli->cli_CommandDir=pathlist;
    }
}
///

/// Free a path list
void __asm __saveds L_FreeDosPathList(register __a0 BPTR list)
{
    PathListEntry *path;

    // Valid list?
    if (!(path=(PathListEntry *)BADDR(list)))
	return;

    // Go through list
    for (;path;)
    {
	PathListEntry *next=(PathListEntry *)BADDR(path->next);

	// Unlock this lock
#ifdef RESOURCE_TRACKING
	NRT_UnLock(path->lock);
#else
	UnLock(path->lock);
#endif

	// Free this entry
#ifdef RESOURCE_TRACKING
	NRT_FreeVec(path);
#else
	FreeVec(path);
#endif

	// Get next
	path=next;
    }
}
///

// Copy local environment variables to current process
void __asm __saveds L_CopyLocalEnv(register __a0 struct Library *DOSBase)
{
    short num;
    struct Process *proc=0;
    struct LocalVar *var;

    // Go through all possible places
    for (num=0;;num++)
    {
	// Any variables set?
	if (proc &&
	    !(IsListEmpty((struct List *)&proc->pr_LocalVars)))
	{
	    // Better forbid for this
	    Forbid();

	    // Go through variable list
	    for (var=(struct LocalVar *)proc->pr_LocalVars.mlh_Head;
		 var->lv_Node.ln_Succ;
		 var=(struct LocalVar *)var->lv_Node.ln_Succ)
	    {
		// Is this a variable?
		if (var->lv_Node.ln_Type==LV_VAR)
		{
		    // Copy this variable
		    SetVar(var->lv_Node.ln_Name,
			   var->lv_Value,
			   var->lv_Len,
			   var->lv_Flags|GVF_LOCAL_ONLY);
		}
	    }

	    // Enable multitasking
	    Permit();
	    break;
	}

	// Nowhere else to look?
	if (!path_places[num]) break;

	// Find next process
	else
	{
	    Forbid();
    	    proc=(struct Process *)FindTask(path_places[num]);
	    Permit();
	}
    }
}


// Get a copy of the Galileo path list
BPTR __asm __saveds L_GetGalileoPathList(register __a6 struct MyLibrary *libbase)
{
    BPTR copy;
    struct LibData *data;

    // Get library data
    data=(struct LibData *)libbase->ml_UserData;

    // Lock path list
    L_GetSemaphore(&data->path_lock,SEMF_SHARED,0);

    // Copy it
    copy=L_GetDosPathList(data->path_list);

    // Unlock path list
    L_FreeSemaphore(&data->path_lock);

    return copy;
}

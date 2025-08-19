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

#include "fixicons.h"

/*void kprintf( const char *, ... );*/

#ifndef DEBUG
#define kprintf ;   /##/
#endif

char *version="$VER: fixicons.gfmmodule 0.2 "__AMIGADATE__" ";

/************************************************************************
*
* Module designed to fix icon problems. Adds FixIcons command with the following template:
*
*	FILE/M/A,ALLOWNOBORDERS=ANB/S,ALLOWNOLABELS=ANL/S,NOFIXOFFSET=NFO/S,SYNCWBTOGALILEO=SWG/S,SYNCGALILEOTOWB=SGW/S
*
*	Function will act on all selected files and directories recursively, making the
*	following changes to icons:
*
*	- Clearing the ICONF_BORDER_ON and ICONF_BORDER_OFF flags
*	  (unless ALLOWNOBORDERS switch is set)
*
*	- Clearing the ICONF_NO_LABEL flag
*	  (unless ALLOWNOLABELS switch is set)
*
*	- Resetting the dd_CurrentX and dd_CurrentY values of drawer/disk icons to
*	  0,0, AND offsetting all icons within that drawer by the values of these
*	  two fields (eg, if dd_CurrentX==5 and dd_CurrentY==3, all icons within
*	  the drawer would be shifted -5,-3, and the two dd_Current fields would
*	  be set to 0)
*	  (unless NOFIXOFFSET switch is set)
*
*	- If the SYNCWBTOGALILEO switch is set, the Workbench position field within
*	  the icon will be set to the same coordinates as the Galileo position field
*
*	- If the SYNCGALILEOTOWB switch is set, the Galileo position field within the icon
*	  will be set to the same coordinates as the Workbench position field.
*	  Additionally, the ICONF_POSITION_OK flag will be cleared.
*
*	If an icon file itself is selected, function will operate on that file - if another
*	file or directory is selected, function will try to obtain an icon for that file
*	or directory. Additionally, if a directory is selected function will enter
*	directory recursively and perform actions on contents (use MatchFirst()/MatchNext())
*
*	Main module code is already written and working, all that needs to be done is the
*	code to actually scan and fix icons.
*
*	See the functions GetIconFlags(), SetIconFlags(), GetIconPosition() and
*	SetIconPosition() in icon.doc, and the ICONF_xxx defines in galileofmbase.h
*
*	--
*
*	An addition to the FixIcons spec that I forgot about - it needs to do the
*	dd_Current shifting in the root directory of a disk as well as for
*	sub-directories.  Basically, if the NFO flag is not set and the Disk.info
*	file is one of the selected files, FixIcons should read the disk.info file
*	first and use the offsets within to adjust the positions of other selected
*	icons in the root directory.
*
*	Also thinking about it, I think the NOFIXOFFSETS flag should be changed to
*	a positive (FIXOFFSETS) and be off by default.
*
*
*	--
*	15.4.98 Modified to fix multiple setting of file and file.info.
*		Now only updates .info files.
*
*	15.5.98	Added new switch REPORT to output a report on the results.
*
*	26.5.98 Fixed SYNCGALILEOTOWB option to save icons only if they have changed.
*
****************************************************************************/

// Routine copied from Jon's code
BPTR open_temp_file( char *filename, IPCData *ipc )
{
    unsigned short a;
    ULONG          micros, secs;
    int            temp_key;
    BPTR           file=NULL;

    // Get temporary key
    temp_key = (ULONG)ipc;
    CurrentTime( &secs, &micros );
    if (micros)
	temp_key *= micros;
    temp_key += secs;

    // Create a temporary script file; try up to 99 times
    for (a = 0; a < 99; a++)
    {
	// Build script name
	lsprintf( filename, "T:galileo-%lx-tmp", temp_key + a );

	// Try to open file
	if (file = Open( filename, MODE_NEWFILE ))
	    break;
    }

    return file;
}

// Print the result of a fix to the output file
void print_result( BPTR outfile, char *path, BPTR parent_lock, BOOL result, ULONG flags )
{
    int i, f = 0;
    char *lockpath;
    char buf[96] = ".info";

    FPuts( outfile, GetString( locale, result ? MSG_FIXED : MSG_COULDNT_FIX ) );

    if (parent_lock && (lockpath = PathFromLock(NULL, parent_lock, PFLF_USE_DEVICENAME|PFLF_END_SLASH, NULL)))
    {
	FPuts( outfile, lockpath );

	FreeMemH(lockpath);
    }

    FPuts( outfile, path );

    if (flags)
    {
	strcat( buf, "\t(" );

	for (i = 1; i <= 5; ++i)
	{
	    if (flags & (1 << i))
	    {
		lsprintf( buf + strlen(buf), "%s%s", f ? ", " : "", GetString(locale,MSG_ANB+i-1) );
		f = 1;
	    }
	}

	strcat( buf, ")" );
    }

    strcat( buf, "\n" );

    FPuts( outfile, buf );
}

// Set up local variables for next sub-directory
static struct locals *next_state(APTR memhandle, struct MinList *dirs, char *path, BPTR parent_lock, int xoff, int yoff)
{
    struct locals *l;

    if (!(l = AllocMemH( memhandle, sizeof(struct locals))))
	return 0;
    
    AddHead((struct List *)dirs, (struct Node *)l);

    l->path = path;
    l->xoff = xoff;
    l->yoff = yoff;
    l->parent_lock = parent_lock;

    return l;
}


// The main routine
static int fix_icon( APTR progress, char *path, BPTR parent_lock, long *args, int xoff, int yoff, BPTR outfile, int *changes )
{
    struct MinList dirs;
    APTR memhandle;
    struct locals *l;

    // Check abort
    if (CheckProgressAbort( progress ))
	return 0;

    NewList((struct List *)&dirs);

    if (!(memhandle = NewMemHandle(sizeof(struct locals) << 3, sizeof(struct locals), MEMF_CLEAR)))
	return 0;

    if (!(l = AllocMemH( memhandle, sizeof(struct locals))))
	goto fi_failure;;
    
    l->path = path;
    l->xoff = xoff;
    l->yoff = yoff;
    l->parent_lock = parent_lock;

    AddHead((struct List *)&dirs, (struct Node *)l);

    do
    {
fi_iter1:
	{
	    ULONG len;

	    // Trim .info if needed
	    if ((len = strlen(l->path)) >= 5 && !stricmp( l->path + len - 5, ".info" ))
	    {
		l->path[len - 5] = 0;
		l->isicon = 1;
	    }
	}

	if (l->parent_lock)
	    l->org_dir = CurrentDir(l->parent_lock);

	if (l->obj = GetDiskObject( l->path ))
	{
	    // Get flags
	    l->iconflags = GetIconFlags( l->obj );

	    // Get position
	    GetIconPosition( l->obj, &l->galileo_x, &l->galileo_y );

	    // Reset border flags?
	    if (!args || (args && !args[FI_ANB]))
	    {
	        if (l->iconflags & (ICONF_BORDER_ON | ICONF_BORDER_OFF))
	        {
		    l->iconflags &= ~(ICONF_BORDER_ON | ICONF_BORDER_OFF);
		    l->set = 1;
	        }
	    }

	    // Reset label flag?
	    if (!args || (args && !args[FI_ANL]))
	    {
	        if (l->iconflags & ICONF_NO_LABEL)
	        {
		    l->iconflags &= ~ICONF_NO_LABEL;
		    l->set = 1;
	        }
	    }

	    if ((!args || (args && !args[FI_NFO])))
	    {
	        if (l->obj->do_DrawerData &&
		    (l->obj->do_Type == WBDISK ||
		     l->obj->do_Type == WBDRAWER ||
		     l->obj->do_Type == WBGARBAGE ||
		     l->obj->do_Type == WBDEVICE))


	        {
		    // Need to adjust x?
		    if (l->xoff && l->obj->do_CurrentX && l->obj->do_CurrentX != NO_ICON_POSITION)
		    {
		        l->obj->do_CurrentX -= l->xoff;
		        l->put = 1;
		        l->flags |= (1<<FI_NFO);
		    }

		    // Need to adjust y?
		    if (l->yoff && l->obj->do_CurrentY && l->obj->do_CurrentY != NO_ICON_POSITION)
		    {
		        l->obj->do_CurrentY -= l->yoff;
		        l->put = 1;
		        l->flags |= (1<<FI_NFO);
		    }

		    // Remember offsets for next recursion level
		    l->newxoff = l->obj->do_DrawerData->dd_CurrentX;
		    l->newyoff = l->obj->do_DrawerData->dd_CurrentY;

		    // Need to reset offsets?
		    if (l->obj->do_DrawerData->dd_CurrentX ||
		        l->obj->do_DrawerData->dd_CurrentY)
		    {
		        l->obj->do_DrawerData->dd_CurrentX = 0;
		        l->obj->do_DrawerData->dd_CurrentY = 0;

		        l->put = 1;
		        l->flags |= (1<<FI_NFO);
		    }
	        }
	    }

	    // Sync WB to Galileo?  (Only if there is an Galileo pos)
	    if (args && args[FI_SWG] && (l->iconflags & ICONF_POSITION_OK))
	    {
	        // Need to change?
	        if (l->obj->do_CurrentX != l->galileo_x ||
		    l->obj->do_CurrentY != l->galileo_y)
	        {
		    l->obj->do_CurrentX = l->galileo_x;
		    l->obj->do_CurrentY = l->galileo_y;
		    l->put = 1;
		    l->flags |= (1<<FI_SWG);
	        }
	    }

	    // Sync Galileo to WB?  (Syncs position and resets pos flag)
	    else if (args && args[FI_SGW])
	    {
	        // Need to change position?
	        if (l->obj->do_CurrentX != l->galileo_x ||
		    l->obj->do_CurrentY != l->galileo_y)
	        {
		    SetIconPosition( l->obj, l->obj->do_CurrentX, l->obj->do_CurrentY );
		    l->put = 1;
		    l->flags |= (1<<FI_SGW);
	        }

	        // Need to reset flag?
	        if (l->iconflags & ICONF_POSITION_OK)
	        {
		    l->iconflags &= ~ICONF_POSITION_OK;
		    l->set = 1;
		    l->flags |= (1<<FI_SGW);
	        }
	    }

	    // Need to set flags?
	    if (l->set)
	    {
	        SetIconFlags( l->obj, l->iconflags );
	        l->put = 1;
	    }

	    // only write if it was an icon to start with
	    // skip if just a plain file

	    if (l->isicon && l->put)
	    {
	        register BOOL r;

		if (r = PutDiskObject(l->path, l->obj ))
		    ++ *changes;

	        if (outfile)
		    print_result( outfile, l->path, l->parent_lock, r, l->flags );
	    }

	    FreeDiskObject( l->obj );
	}

	// if not an icon then lock it and check if it is a directory
	if (!l->isicon && (l->lock = Lock( l->path, ACCESS_READ )))
	{
	    // is a directory?
	    if (Examine( l->lock, &l->fib ) && l->fib.fib_DirEntryType >= 0)
	    {
	        // yes is a directory
	        if (ExNext( l->lock, &l->fib ))
	        {
		    //int more_files;
		    l->newparent_lock = DupLock(l->lock);

		    strcpy( l->newpath, l->fib.fib_FileName );

		    l->more_files = ExNext( l->lock, &l->fib );

		    // Update progress window
		    SetProgressWindowTags(progress,PW_FileName,l->path,TAG_END);

		    l->return_to = 2;
		    if (!(l = next_state(memhandle, &dirs, l->newpath, l->newparent_lock, l->newxoff, l->newyoff)))
			goto fi_failure;
		    
		    goto fi_iter1;

fi_iter2:
		    if (l->more_files)
		    {
		        do
		        {
			    // Check abort
			    if (CheckProgressAbort( progress ))
			        break;

			    strcpy( l->newpath, l->fib.fib_FileName );

			    if (l->more_files)
			        l->more_files = ExNext( l->lock, &l->fib );

			    // Update progress window
			    SetProgressWindowTags(progress,PW_FileName,l->path,TAG_END);

			    l->return_to = 3;
			    if (!(l = next_state(memhandle, &dirs, l->newpath, l->newparent_lock, l->newxoff, l->newyoff)))
				goto fi_failure;

			    goto fi_iter1;

fi_iter3:
			    ;
			} while (l->more_files);
		    }
		    UnLock(l->newparent_lock);
	        }
	    }
	    UnLock( l->lock );
	}

	if (l->org_dir)
	    CurrentDir(l->org_dir);

	Remove((struct Node *)l);
	FreeMemH( l );
	l = (struct locals *)dirs.mlh_Head;

	if (l->node.mln_Succ)
	{
	    switch (l->return_to)
	    {
		case 3:
		    goto fi_iter3;

		case 2:
		    goto fi_iter2;

		default:
		    goto fi_iter1;
	    }
	}
	else
	    break;
    } while (TRUE);
fi_failure:

    FreeMemHandle(memhandle);

    return 0;
}

// Fix icons
int __asm __saveds L_Module_Entry(
	register __a0 char *argstring,
	register __a1 struct Screen *screen,
	register __a2 IPCData *ipc,
	register __a3 IPCData *main_ipc,
	register __d0 ULONG mod_id,
	register __d1 CONST GalileoCallbackInfo *gci)
{
    Att_List *files;
    Att_Node *node;
    PathNode *pathnode;
    APTR progress = 0;
    short count=1;
    FuncArgs *args;
    struct DiskObject *obj;
    int xoff = 0;
    int yoff = 0;
    BPTR outfile = 0;
    char temp_filename[32];
    int changes = 0;
    BPTR parent_lock = 0, org_dir = 0, arg_dir = 0;
    BOOL is_disk = FALSE;
    char *ptr, *tmp;

    // Create list
    if (!(files=Att_NewList(LISTF_POOL)))
	    return 0;

    // Get source path
    pathnode = gci->gc_GetSource(IPCDATA(ipc), NULL);

    // Parse arguments
    args=ParseArgs(arg_template,argstring);

    // Files supplied?
    if (args && args->FA_Arguments[0])
    {
	char **names;
	short a;

	// Get array pointer
	names=(char **)args->FA_Arguments[0];

	// Go through names
	for (a=0;names[a];a++)
	{
	    // Add name to list
	    Att_NewNode(files,names[a],0,0);
	}
    }

    // Otherwise
    else
    {
	FunctionEntry *entry;

	gci->gc_FirstEntry( IPCDATA(ipc), NULL );

	// Get entries
	while (entry = gci->gc_GetEntry(IPCDATA(ipc)))
	{
	    // Add name to list
	    Att_NewNode(files,entry->fe_name,0,0);

	    // End entry
	    gci->gc_EndEntry(IPCDATA(ipc), entry, TRUE);
	}
    }

    // List empty?
    if (IsListEmpty((struct List *)files))
    {
	// Free and return
	goto fi_exit;
    }

    // Need output file?
    if (args && args->FA_Arguments[FI_REPORT])
    {
	if (outfile = open_temp_file( temp_filename, ipc ))
	{
	    FPuts( outfile, GetString(locale,MSG_RESULTS) );
	    FPuts( outfile, ":\n\n" );
	}
    }

    if (pathnode)
	parent_lock = pathnode->pn_lock;

    // Need to process Disk.info first?
    if (!args || !args->FA_Arguments[FI_NFO])
    {
	BPTR test_lock = 0;

	if (parent_lock && (test_lock = ParentDir(parent_lock)))
	{
	    UnLock(test_lock);
	}

	for (node=(Att_Node *)files->list.lh_Head;node->node.ln_Succ;node=(Att_Node *)node->node.ln_Succ)
	{
	    ptr = FilePart( node->node.ln_Name );

	    if (ptr == node->node.ln_Name && ptr[strlen(ptr) - 1] == ':')
		is_disk = TRUE;

	    if (is_disk || ((!stricmp( "Disk.info", ptr )) &&
		((ptr == node->node.ln_Name && !test_lock) || ptr[-1] == ':')))
	    {
		if (is_disk)
		{
		    // Can not handle multiple arguments if one is a disk
		    if (node->node.ln_Succ->ln_Succ || node->node.ln_Pred->ln_Pred)
			goto fi_exit;

		    if (arg_dir = Lock(ptr, ACCESS_READ))
			org_dir = CurrentDir(arg_dir);
		    else
			goto fi_exit;

		}
		else
		if ((tmp = strrchr(node->node.ln_Name,'/')) || (tmp = strrchr(node->node.ln_Name,':')))
		{
		    if (tmp[0] == ':')
			tmp++;

		    tmp[0] = 0;

		    if (arg_dir = Lock(node->node.ln_Name, ACCESS_READ))
			org_dir = CurrentDir(arg_dir);
		    else
			goto fi_exit;
		}

		// Strip .info
		ptr[strlen(ptr) - 5] = 0;

		if (!org_dir && parent_lock)
		    org_dir = CurrentDir(parent_lock);

		if (arg_dir)
		    parent_lock = arg_dir;

		if (obj = GetDiskObject( ptr ))
		{
		    if (obj->do_DrawerData &&
			    obj->do_Type == WBDISK)
		    {
			xoff = obj->do_DrawerData->dd_CurrentX;
			yoff = obj->do_DrawerData->dd_CurrentY;

			if (xoff || yoff)
			{
			    BOOL result;

			    obj->do_DrawerData->dd_CurrentX = 0;
			    obj->do_DrawerData->dd_CurrentY = 0;

			    if (result = PutDiskObject( ptr, obj ))
				++ changes;

			    if (outfile)
				print_result( outfile, ptr, parent_lock, result, 1 << FI_NFO );
			}
		    }

		    FreeDiskObject( obj );
		}

		if (org_dir)
		    ParentDir(org_dir);

		if (!is_disk)
		{
		    if (arg_dir)
		    {
			UnLock(arg_dir);
			arg_dir = 0;
		    }

		    Att_RemNode( node );
		}
		break;
	    }
	}
    }

    // Open progress window
    progress=OpenProgressWindowTags(
	    PW_Screen,screen,
	    PW_Title,GetString(locale,MSG_FIXING_ICONS),
	    PW_Flags,PWF_FILENAME|PWF_GRAPH|PWF_ABORT,
	    PW_FileCount,Att_NodeCount(files),
	    TAG_END);

    // Go through files list
    for (node=(Att_Node *)files->list.lh_Head;node->node.ln_Succ;node=(Att_Node *)node->node.ln_Succ,count++)
    {
	// Check abort
	if (CheckProgressAbort(progress))
		break;

	// Update progress window
	SetProgressWindowTags(
		progress,
		PW_FileName,FilePart(node->node.ln_Name),
		PW_FileNum,count,
		TAG_END);

	if (is_disk)
	{
	    node->node.ln_Name[0] = 0;
	    ptr = node->node.ln_Name;
	}
	else
	if ((tmp = strrchr(node->node.ln_Name,'/')) || (tmp = strrchr(node->node.ln_Name,':')))
	{
	    char keep;

	    if (tmp[0] == ':')
	    {
		tmp++;
		ptr = tmp;
	    }
	    else
		ptr = tmp + 1;

	    keep = tmp[0];
	    tmp[0] = 0;

	    if (!(arg_dir = Lock(node->node.ln_Name, ACCESS_READ)))
		goto fi_exit;

	    tmp[0] = keep;
	}
	else
	    ptr = node->node.ln_Name;

	if (arg_dir)
	    parent_lock = arg_dir;

	/*********************** MAIN CODE BODY GOES HERE ****************************/

	fix_icon( progress, ptr, parent_lock, args ? args->FA_Arguments : 0, xoff, yoff, outfile, &changes );

	/*********************** MAIN CODE BODY GOES HERE ****************************/

	if (arg_dir)
	    UnLock(arg_dir);
    }

fi_exit:

    // Free stuff
    if (progress)
	CloseProgressWindow(progress);

    // Need to show results?
    if (outfile)
    {
	char                  buf[256];

	// No files changed?
	if (changes)
	    lsprintf( buf, "\n%ld %s\n", changes, GetString(locale,MSG_CHANGES_MADE) );
	else
	    lsprintf( buf, "%s %s\n", GetString(locale,MSG_NO), GetString(locale,MSG_CHANGES_MADE) );

	FPuts( outfile, buf );

	Close( outfile );

	lsprintf( buf, "galileo read delete %s", temp_filename );


	gci->gc_SendCommand( IPCDATA(ipc), buf, NULL, NULL);
    }

    Att_RemList(files,0);
    DisposeArgs(args);
    return 1;
}

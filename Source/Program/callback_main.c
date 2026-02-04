/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 1993-2012 Jonathan Potter & GP Software
Copyright 2023,2025 Hagbard Celine

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
#include <gfm/paths_protos.h>
#include "callback_protos.h"
#include "misc_protos.h"
#include "rexx_protos.h"
#include "lister_protos.h"
#include "position_protos.h"
#include "/Modules/modules.h"
#include "scripts.h"

CONST GalileoCallbackInfo CallBackInfo = {
		GALILEOFM_CALLBACK_COUNT,
		(APTR)HookCreateFileEntry,
		(APTR)HookFileSet,
		(APTR)HookSortFileList,
		(APTR)HookAddFileEntry,
		(APTR)HookResortLister,
		(APTR)HookRefreshLister,
		(APTR)HookLockFileList,
		(APTR)HookUnlockFileList,
		(APTR)HookFindFileEntry,
		(APTR)HookSetFileComment,
		(APTR)HookRemoveFileEntry,
		(APTR)HookFileQuery,
		(APTR)HookShowHelp,
		(APTR)HookConvertEntry,
		(APTR)HookGetLister,
		(APTR)HookGetSource,
		(APTR)HookNextSource,
		(APTR)HookUnlockSource,
		(APTR)HookGetDest,
		(APTR)HookEndSource,
		(APTR)HookEndDest,
		(APTR)HookGetEntry,
		(APTR)HookExamineEntry,
		(APTR)HookEndEntry,
		(APTR)HookRemoveEntry,
		(APTR)HookEntryCount,
		(APTR)HookReloadEntry,
		(APTR)HookAddFile,
		(APTR)HookDelFile,
		(APTR)HookLoadFile,
		(APTR)HookDoChanges,
		(APTR)HookCheckAbort,
		(APTR)HookGetWindow,
		(APTR)HookGetPort,
		(APTR)HookGetScreen,
		(APTR)HookGetScreenData,
		(APTR)HookFreeScreenData,
		(APTR)HookOpenProgress,
		(APTR)HookUpdateProgress,
		(APTR)HookCloseProgress,
		(APTR)HookReplaceReq,
		(APTR)HookGetPointer,
		(APTR)HookFreePointer,
		(APTR)HookSendCommand,
		(APTR)HookCheckDesktop,
		(APTR)HookGetDesktop,
		(APTR)HookScript,
		(APTR)HookDesktopPopup,
		(APTR)HookFirstEntry,
		(APTR)HookRexxCommand,
		(APTR)HookFileRequest,
		(APTR)HookGetThemes,
		(APTR)HookFreePointerDirect,
		(APTR)HookListerSet,
		(APTR)HookListerGet,
		(APTR)HookGetFileEntry,
		(APTR)HookNewLister,
		(APTR)HookReadDir,
		(APTR)HookClearDir,
};


// Run a script
BOOL __asm __saveds HookScript(
	register __a0 char *name,
	register __a1 char *data)
{
	// Run the script
	return RunScript_Name(name,data);
}


// Send an ARexx command
long __asm __saveds HookRexxCommand(
	register __a0 char *command,
	register __a1 char *result,
	register __d0 long length,
	register __a2 struct MsgPort *replyport,
	register __d1 ULONG flags)
{
	struct MsgPort *rexx,*reply;
	struct RexxMsg *msg;
	long rc=-1;

	// Clear result
	if (result) *result=0;

	// Get rexx port
	if (!GUI->rexx_proc || !(rexx=(struct MsgPort *)IPCDATA(GUI->rexx_proc)))
		return -1;

	// Create reply port if needed
	if ((reply=replyport) || (reply=CreateMsgPort()))
	{
		// Create message
		if (msg=BuildRexxMsgExTags(
					reply,
					".galileo",
					rexx->mp_Node.ln_Name,
					RexxTag_Arg0,command,
					TAG_END))
		{
			// Initialise message
			msg->rm_Action|=1|RXCOMM;
			if (result) msg->rm_Action|=RXFF_RESULT;

			// Send the message and wait for reply
			PutMsg(rexx,(struct Message *)msg);
			WaitPort(reply);
			GetMsg(reply);

			// String reply?
			if (msg->rm_Result2 && result)
				stccpy(result,(char *)msg->rm_Result2,length);
			rc=msg->rm_Result1;

		        // Free the result
		        if( msg->rm_Result1 == 0 && msg->rm_Result2 != NULL )
			        DeleteArgstring( (char *) msg->rm_Result2 );

			// Free message
			FreeRexxMsgEx(msg);
		}

		// Free message port
		if (reply!=replyport)
		{
		    struct Message *msg;
		    while (msg=GetMsg(reply))
				ReplyFreeMsg(msg);
			DeleteMsgPort(reply);
		}
	}

	return rc;
}


// Request a file
long __asm __saveds HookFileRequest(
	register __a0 struct Window *parent,
	register __a1 char *title,
	register __a2 char *initial_path,
	register __a3 char *pathname,
	register __d0 ULONG flags,
	register __d1 char *pattern)
{
	return request_file(parent,title,pathname,initial_path,flags,pattern);
}


// Create a new lister
Lister * __asm __saveds HookNewLister(
	register __a0 STRPTR path,
	register __a1 Lister *parent,
	register __a2 STRPTR toolbar_path,
	register __d0 BPTR path_lock,
	register __d1 ULONG flags,
	register __d2 WORD Left,
	register __d3 WORD Top,
	register __d4 WORD Width,
	register __d5 WORD Height)
{
    Cfg_Lister *cfg;
    Lister *new_lister = 0;
    short fromicon=0, nomode=0;
    position_rec *pos=0;

    // Get a new lister definition
    if (cfg=NewLister(path))
    {
	// Lock lister list
	lock_listlock(&GUI->lister_list,FALSE);

	// Got a parent list?
	if (parent)
	{
	    // Check if it's valid
	    if (!(rexx_lister_valid(parent))) parent=0;
	}

	// Fill out lister
	lister_init_new(cfg,parent);

	// Unlock lister list
	unlock_listlock(&GUI->lister_list);

	// Set lock
	cfg->lock = path_lock;

	// Store dimensions (if set)
	if (Left!=-1) cfg->lister.pos[0].Left=Left;
	if (Top!=-1) cfg->lister.pos[0].Top=Top;
	if (Width) cfg->lister.pos[0].Width=Width;
	if (Height) cfg->lister.pos[0].Height=Height;

	// Invisible
	if (flags&GCNLF_INVISIBLE)
	    cfg->lister.flags|=GLSTF_INVISIBLE;

	// Iconfied
	if (flags&GCNLF_ICONIFIED)
	    cfg->lister.flags|=GLSTF_ICONIFIED;

	// No activate
	if (flags&GCNLF_NOACTIVE)
	    cfg->lister.flags|=GLSTF_NOACTIVE;

	// From icon
	if (flags&GCNLF_FROMICON)
	    fromicon=1;

	// Icon?
	if (flags&GCNLF_ICON)
	{
	    cfg->lister.flags|=GLSTF_ICON;

	    // Icon Action?
	    if (flags&GCNLF_ICON_ACTION)
		cfg->lister.flags|=GLSTF_ICON_ACTION;

	    // Show All
	    if (flags&GCNLF_SHOW_ALL)
		cfg->lister.flags|=GLSTF_SHOW_ALL;

	    nomode = 1;
	}

	// Position from icon?
	if (fromicon && (path || path_lock))
	{
	    BPTR lock;

	    if (path_lock)
		lock = path_lock;
	    else
		lock = LockFromPath(path, NULL, NULL);


	    // Get full path
	    if (lock)
	    {
		D_S(struct FileInfoBlock, fib);
		STRPTR fullpath, filename;
		BPTR parent_dir, org_dir;
	        struct DiskObject *icon;
	        short mode=0;

		if (parent_dir = ParentDir(lock))
		{
		    org_dir = CurrentDir(parent_dir);
		    if (path)
			filename = FilePart(path);

		    else
		    {
			Examine(lock, fib);
			filename = fib->fib_FileName;
		    }
		}
		else
		{
		    org_dir = CurrentDir(lock);
		    filename = "Disk";
		}

		if (filename)
	        {
		    // Try and load icon
		    if (icon=GetDiskObject(filename))
		    {
		        struct DateStamp date = {0};

		        VolIdFromLock(lock, &date, NULL);
			fullpath = PathFromLock(NULL, lock, NULL, NULL);

		        // Get position
		        pos=GetListerPosition(fullpath,
				              &date,
				              0,
				              icon,
				              &cfg->lister.pos[0],
				              (nomode)?0:&mode,
				              &cfg->lister.format,
				              (parent)?parent->window:0,
				              parent,
				              0);

		        // Free icon
		        FreeDiskObject(icon);

		        // Mode set?
		        if (!nomode && mode)
		        {
			    if (mode&LISTERMODE_ICON)
				cfg->lister.flags|=GLSTF_ICON;
			    if (mode&LISTERMODE_ICON_ACTION)
				cfg->lister.flags|=GLSTF_ICON_ACTION;
			    if (mode&LISTERMODE_SHOW_ALL)
				cfg->lister.flags|=GLSTF_SHOW_ALL;
		        }

			if (fullpath)
			    FreeMemH(fullpath);
		    }

		    if (org_dir)
			CurrentDir(org_dir);

		    if (parent_dir)
			UnLock(parent_dir);
	        }

		if (!path_lock)
		    UnLock(lock);
	    }
	}

	// Create a new lister
	if (new_lister=lister_new(cfg))
	{
	    // Store position record
	    new_lister->pos_rec=pos;

	    // Custom toolbar?
	    if (toolbar_path)
	    {
		// Get new toolbar
		lister_new_toolbar(new_lister,toolbar_path,0);
	    }

	    // Initialise lister
	    IPC_Command(new_lister->ipc,
		        LISTER_INIT,
		        0,
		        GUI->screen_pointer,
		        0,
		        0);
	}
    }

    return new_lister;
}

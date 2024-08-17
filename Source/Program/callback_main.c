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

ULONG
	HookTable[GALILEOFM_HOOK_COUNT]={
		(ULONG)HookCreateFileEntry,
		(ULONG)HookFileSet,
		(ULONG)HookSortFileList,
		(ULONG)HookAddFileEntry,
		(ULONG)HookResortLister,
		(ULONG)HookRefreshLister,
		(ULONG)HookLockFileList,
		(ULONG)HookUnlockFileList,
		(ULONG)HookFindFileEntry,
		(ULONG)HookSetFileComment,
		(ULONG)HookRemoveFileEntry,
		(ULONG)HookFileQuery,
		(ULONG)HookShowHelp,
		(ULONG)HookConvertEntry,
		(ULONG)HookGetLister,
		(ULONG)HookGetSource,
		(ULONG)HookNextSource,
		(ULONG)HookUnlockSource,
		(ULONG)HookGetDest,
		(ULONG)HookEndSource,
		(ULONG)HookEndDest,
		(ULONG)HookGetEntry,
		(ULONG)HookExamineEntry,
		(ULONG)HookEndEntry,
		(ULONG)HookRemoveEntry,
		(ULONG)HookEntryCount,
		(ULONG)HookReloadEntry,
		(ULONG)HookAddFile,
		(ULONG)HookDelFile,
		(ULONG)HookLoadFile,
		(ULONG)HookDoChanges,
		(ULONG)HookCheckAbort,
		(ULONG)HookGetWindow,
		(ULONG)HookGetPort,
		(ULONG)HookGetScreen,
		(ULONG)HookGetScreenData,
		(ULONG)HookFreeScreenData,
		(ULONG)HookOpenProgress,
		(ULONG)HookUpdateProgress,
		(ULONG)HookCloseProgress,
		(ULONG)HookReplaceReq,
		(ULONG)HookGetPointer,
		(ULONG)HookFreePointer,
		(ULONG)HookSendCommand,
		(ULONG)HookCheckDesktop,
		(ULONG)HookGetDesktop,
		(ULONG)HookScript,
		(ULONG)HookDesktopPopup,
		(ULONG)HookFirstEntry,
		(ULONG)HookRexxCommand,
		(ULONG)HookFileRequest,
		(ULONG)HookGetThemes,
        (ULONG)HookFreePointerDirect,
        (ULONG)HookIsSourceDestLock,
        (ULONG)HookFakeDir,
        (ULONG)HookIsFakeDir,
	};


long HookInitHooks(GalileoCallbackInfo *info)
{
	ULONG *srcptr,*dstptr;
	short num;

	// Get pointer to start of function table
	srcptr=(ULONG *)HookTable;

	// Get pointer to storage space in structure
	dstptr=(ULONG *)&info->gc_CreateFileEntry;

	// Fill it out, up to count entries
	for (num=0;num<info->gc_Count;num++,srcptr++,dstptr++)
	{
		// Copy entry if valid
		if (num<GALILEOFM_HOOK_COUNT) *dstptr=*srcptr;

		// Just clear entry in table
		else *dstptr=0;
	}

	return (num>GALILEOFM_HOOK_COUNT)?GALILEOFM_HOOK_COUNT:num;
}


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

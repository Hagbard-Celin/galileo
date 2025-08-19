/*
  XADbrowser.gfmmodule 1.22 - Galileo plugin to browse through XAD archives
  Copyright (C) 1999,2000 Mladen Milinkovic <mladen.milinkovic@ri.tel.hr>
  Copyright 2013 Roman Kargin <kas1e@yandex.ru>
  Copyright 2013 Szilard Biro
  Copyright 2023-2025 Hagbard Celine

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "XADbrowser.h"

static const char *version="\0$VER: XADbrowser.gfmmodule 0.2 "__AMIGADATE__" Written by Mladen Milinkovic <max2@iname.com>\0";

char *viewcmds="Read Play Show HexRead AnsiRead IconInfo";
 
ULONG __asm ProgressHookTr(register __a0 struct Hook *, register __a1 struct xadProgressInfo *);


/// Error Requester
void ErrorReq(struct XADbrowserData *xbdata, char *Mess)
{
    AsyncRequestTags(
	    xbdata->ipc,
	    REQTYPE_SIMPLE,
	    0,
	    0,
	    0,
	    AR_Screen, xbdata->screen,
	    AR_Message, Mess,
	    AR_Title, GetString(locale,MSG_ERROR),
	    AR_Button, GetString(locale,MSG_OK),
	    (xbdata->source_window)?AR_Window:TAG_END, xbdata->source_window,
	    TAG_END);
}  
///

BOOL PasswordReq(struct XADbrowserData *xbdata)
{
    int retval;
    BOOL result=TRUE;

    memset(xbdata->password,0,512);


    retval = AsyncRequestTags(xbdata->ipc,
		              REQTYPE_SIMPLE,
		              0,
		              0,
		              0,
			      AR_Window, xbdata->source_window,
			      AR_Message, GetString(locale,MSG_ENTER_PASSWORD),
			      AR_Buffer, xbdata->password,
			      AR_BufLen, 512,
			      AR_Button, GetString(locale,MSG_OK),
			      AR_Button, GetString(locale,MSG_ABORT),
			      AR_Flags, SRF_SECURE,
		              TAG_END);

    if (!retval)
    {
	result=FALSE;
	memset(xbdata->password,0,512);
    }

    return (result);
}

/// ChangeDir
static void ChangeDir(struct XADbrowserData *xbdata, struct Tree *cur)
{
    struct List list;
    UWORD dc=0, fc=0;

    xbdata->gci->gc_LockFileList(xbdata->source_lister,TRUE);
	
    if (xbdata->cur)
    {
	xbdata->gci->gc_ClearDir(xbdata->source_lister, CBCDF_CLEAR_PATH|CBCDF_CLEAR_EXPANDEDPATH);
    }
	
    xbdata->cur=cur;
    cur=cur->Child;

    NewList(&list);
	
    while (cur)
    {
	cur->entry=xbdata->gci->gc_CreateFileEntry(xbdata->source_lister, (struct FileInfoBlock *)&cur->fib, NULL);
	AddTail(&list,cur->entry);

	if (cur->fib.sfib_DirEntryType<0) fc++;
	else dc++;

	cur=cur->Next;
    }

    xbdata->gci->gc_SortFileList(xbdata->source_lister, &list, fc, dc);
    xbdata->gci->gc_UnlockFileList(xbdata->source_lister);
    xbdata->gci->gc_RefreshLister(xbdata->source_lister, HOOKREFRESH_DATE|HOOKREFRESH_FULL, FALSE);
}
///

static void PopulateFib(struct XADbrowserData *xbdata, struct xadFileInfo *xfi, struct Tree *tree)
{
    struct xadMasterBase *xadMasterBase = xbdata->xadMasterBase;

    if (xfi->xfi_Comment)
    {
	strcpy(tree->fib.sfib_Comment,xfi->xfi_Comment);
    }
    xadConvertDates(XAD_DATEXADDATE, &xfi->xfi_Date,
		    XAD_GETDATEDATESTAMP, &tree->fib.sfib_Date,
		    TAG_DONE);

    tree->fib.sfib_Protection=xfi->xfi_Protection;

    if (!tree->fib.sfib_DirEntryType && (tree->fib.sfib_Size = xfi->xfi_Size) &&
	!(xfi->xfi_Flags&(XADFIF_DIRECTORY|XADFIF_LINK|XADFIF_INFOTEXT)))
    {
	struct Tree *tmp;

	tmp = tree->Parent;
	do
	{
	    tmp->FileCount++;
	    tmp = tmp->Parent;
	} while (tmp);
    }

    if (xfi->xfi_Flags&XADFIF_DIRECTORY)
	tree->fib.sfib_DirEntryType=1;
    else
    if (!tree->fib.sfib_DirEntryType)
	tree->fib.sfib_DirEntryType=-1;

    tree->fib.sfib_OwnerUID=xfi->xfi_OwnerUID;
    tree->fib.sfib_OwnerGID=xfi->xfi_OwnerGID;
    tree->xfi=xfi;
}


static struct Tree *FindDrw(struct XADbrowserData *xbdata, char *path, struct xadFileInfo *xfi)
{
    struct Tree *cur=xbdata->root;
    UBYTE x, *buf = xbdata->buf;

    if (xfi)
	path = xfi->xfi_FileName;

    while (1)
    {
	for(x=0;(buf[x] = *path) && (*path++ != '/');x++);

	if (!buf[x]) return(cur);

	buf[x]=0;

	if (!cur->Child)
	{
	    cur->Child=AllocMemH(xbdata->treehandle,sizeof(struct Tree));
	    cur->Child->Parent = cur;
	    cur=cur->Child;
	    strcpy(cur->fib.sfib_FileName,buf);
	    cur->fib.sfib_DirEntryType=1;
	    PopulateFib(xbdata, xfi, cur);
	}
	else
	{
	    cur=cur->Child;
	    while (strcmp(cur->fib.sfib_FileName,buf))
	    {
		if (cur->Next)
		    cur=cur->Next;
		else
		{
		    cur->Next=AllocMemH(xbdata->treehandle,sizeof(struct Tree));
		    cur->Next->Parent = cur->Parent;
		    cur=cur->Next;
		    strcpy(cur->fib.sfib_FileName,buf);
		    cur->fib.sfib_DirEntryType=1;
		    PopulateFib(xbdata, xfi, cur);
		}
	    }
	}
    }
}

/// BuildTree
static void BuildTree(struct XADbrowserData *xbdata, struct xadMasterBase *xadMasterBase)
{
    struct xadFileInfo *xfi;
    struct Tree *tree, *tmp;
    char *name;
    BOOL dup;

    if (!(xbdata->root = AllocMemH(xbdata->treehandle,sizeof(struct Tree))))
	return;

    xfi = xbdata->ArcInf->xai_FileInfo;

    while (xfi)
    {
	tree=FindDrw(xbdata, NULL, xfi);
	name=FilePart(xfi->xfi_FileName);
	if (!name[0])
	{
	    xfi = xfi->xfi_Next;
	    continue;
	}
	dup=FALSE;

	if (tree->Child)
	{
	    tmp=tree->Child;
	    while (tmp)
	    {
		if (!strcmp(name,tmp->fib.sfib_FileName))
		{
		    dup=TRUE;
		    break;
		}
		tmp=tmp->Next;
	    }
	}

	if (!dup)
	{
	    if (tree->Child==NULL)
	    {
		tree->Child=AllocMemH(xbdata->treehandle,sizeof(struct Tree));
		tree->Child->Parent = tree;
		tree=tree->Child;
	    }
	    else
	    {
		tree=tree->Child;
		while (tree->Next!=NULL) tree=tree->Next;
		tree->Next=AllocMemH(xbdata->treehandle,sizeof(struct Tree));
		tree->Next->Parent = tree->Parent;
		tree=tree->Next;
	    }

	    strcpy(tree->fib.sfib_FileName,name);
	    PopulateFib(xbdata, xfi, tree);
	}

	xfi = xfi->xfi_Next;
    }
}
///

/// MsgPort
static BOOL AllocPort(struct XADbrowserData *xbdata)
{
    if (xbdata->mp=CreateMsgPort())
    {
	(xbdata->mp->mp_Node.ln_Pri) = 2;
	(xbdata->mp->mp_Node.ln_Name) = xbdata->mp_name;

	// This is guaranteed to be unique
	sprintf(xbdata->mp_name,"gfm_XADbrowser_%p",xbdata->mp);
	AddPort(xbdata->mp);

	sprintf(xbdata->buf,"lister set %lu handler %s quotes synctraps", xbdata->source_lister, xbdata->mp_name);
	xbdata->gci->gc_RexxCommand(xbdata->buf, NULL, NULL, NULL, NULL);

	sprintf(xbdata->buf,"galileo addtrap * %s",xbdata->mp_name);
	xbdata->gci->gc_RexxCommand(xbdata->buf, NULL, NULL, NULL, NULL);

	sprintf(xbdata->buf,"galileo remtrap All %s",xbdata->mp_name);
	xbdata->gci->gc_RexxCommand(xbdata->buf, NULL, NULL, NULL, NULL);

	sprintf(xbdata->buf,"galileo remtrap None %s",xbdata->mp_name);
	xbdata->gci->gc_RexxCommand(xbdata->buf, NULL, NULL, NULL, NULL);

	sprintf(xbdata->buf,"galileo remtrap Select %s",xbdata->mp_name);
	xbdata->gci->gc_RexxCommand(xbdata->buf, NULL, NULL, NULL, NULL);

	sprintf(xbdata->buf,"galileo remtrap Toggle %s",xbdata->mp_name);
	xbdata->gci->gc_RexxCommand(xbdata->buf, NULL, NULL, NULL, NULL);

	return(TRUE);
    }
    return(FALSE);
}

static void FreePort(struct XADbrowserData *xbdata)
{
    struct Message *msg;

    RemPort(xbdata->mp);

    while (msg=GetMsg(xbdata->mp))
    ReplyMsg(msg);

    DeleteMsgPort(xbdata->mp);
}
///

/// Temp Files
static void RemoveTemp(struct XADbrowserData *xbdata)
{
    struct TempFile *tf;
	
    while (tf=(struct TempFile *)RemHead((struct List *)&xbdata->Temp))
    {
	DeleteFile(tf->FileName);
	FreeVec(tf);
    }
}
///

/// Events

static void LaunchCommand(struct XADbrowserData *xbdata, char *cmd, char *name)
{
    sprintf(xbdata->buf,"command wait %s %s", cmd, name);
    xbdata->gci->gc_RexxCommand(xbdata->buf, NULL, NULL, NULL, NULL);
}


static void _cd(struct XADbrowserData *xbdata, struct Tree *cur)
{
    ChangeDir(xbdata,cur);

    IPC_Command(*((IPCData **)xbdata->source_lister), LISTER_SET_PATH, 0, (APTR)xbdata->path, 0, REPLY_NO_PORT);
}

static void _doubleclick(struct XADbrowserData *xbdata, APTR entry, struct xadMasterBase *xadMasterBase)
{
    struct Tree *tmp=xbdata->cur->Child;
    struct TempFile *tf;
    xadERROR err;
    BOOL retry = TRUE;

    xbdata->over=FALSE;

    while ((tmp->entry != entry) && !xbdata->over) tmp=tmp->Next;

    if (tmp->fib.sfib_DirEntryType>0)
    {
	STRPTR newpath;

	if (newpath = JoinString(xbdata->memhandle, xbdata->path, tmp->fib.sfib_FileName, NULL, JSF_FS_ADDPART))
	{
	    FreeMemH(xbdata->path);
	    xbdata->path = newpath;
	}

	_cd(xbdata,tmp);
    }
    else
    if (tf=AllocVec(sizeof(struct TempFile),NULL))
    {
	AddTail((struct List *)&xbdata->Temp,(struct Node *)tf);
	sprintf(tf->FileName,"T:%s",tmp->fib.sfib_FileName);
	if ((tmp->xfi->xfi_Flags & XADFIF_CRYPTED) && (*xbdata->password == 0))
	{
	    if (!(PasswordReq(xbdata)))
		xbdata->over = TRUE;

	}

	while (retry && !xbdata->over)
	{
	    xbdata->retry = retry = FALSE;

	    err = xadFileUnArc(xbdata->ArcInf,
			       XAD_ENTRYNUMBER, tmp->xfi->xfi_EntryNumber,
			       XAD_OUTFILENAME, tf->FileName,
			       XAD_OVERWRITE, TRUE,
			       (*xbdata->password)?XAD_PASSWORD:TAG_IGNORE, xbdata->password,
			       TAG_DONE);

	    if (err == XADERR_PASSWORD)
	    {
		retry = PasswordReq(xbdata);
		xbdata->over=!retry;
	    }
	    else
	    if (xbdata->retry)
		retry = TRUE;
	}

	LaunchCommand(xbdata,"DoubleClick",tf->FileName);
    }
}

static void RemoveHandler(struct XADbrowserData *xbdata)
{
    sprintf(xbdata->buf, "galileo remtrap * %s", xbdata->mp_name);
    xbdata->gci->gc_RexxCommand(xbdata->buf, NULL, NULL, NULL, NULL);

    sprintf(xbdata->buf, "lister clear %lu handler", xbdata->source_lister);
    xbdata->gci->gc_RexxCommand(xbdata->buf, NULL, NULL, NULL, NULL);

    sprintf(xbdata->buf, "lister wait %lu quick", xbdata->source_lister);
    xbdata->gci->gc_RexxCommand(xbdata->buf, NULL, NULL, NULL, NULL);
}

static BOOL _parent(struct XADbrowserData *xbdata)
{
    struct Tree *tmp;
    char *path=xbdata->path;

    if ((!xbdata->newlister) && ((strlen(path))==(strlen(xbdata->root_path))) &&
       (!stricmp(xbdata->path,xbdata->root_path)))
    {
	RemoveHandler(xbdata);

	xbdata->gci->gc_ReadDir(xbdata->source_lister, NULL, NULL, DupLock(xbdata->sourcedir_lock));

	return TRUE;
    }
    else
    {
	while (*path++!=':');

	if (*path)
	{
	    tmp=FindDrw(xbdata, path, NULL);
	    *((char *)PathPart(path))=0;
	    _cd(xbdata,tmp);
	}
	return FALSE;
    }
}

static void _root(struct XADbrowserData *xbdata)
{
    STRPTR tmp;

    if (tmp = CopyString(xbdata->memhandle, xbdata->root_path))
    {
	FreeMemH(xbdata->path);
	xbdata->path = tmp;
	IPC_Command(*((IPCData **)xbdata->source_lister), LISTER_SET_PATH, 0, (APTR)xbdata->path, 0, REPLY_NO_PORT);
    }

    ChangeDir(xbdata,xbdata->root);
}

static void _viewcommand(struct XADbrowserData *xbdata, char *com, APTR handle, struct xadMasterBase *xadMasterBase)
{
    APTR function_entry, entry;
    struct Tree *tmp;
    struct TempFile *tf;
    xadERROR err;
    BOOL retry;

    xbdata->over=FALSE;

    while ((function_entry = xbdata->gci->gc_GetEntry(handle)) && !xbdata->over)
    {
	entry = xbdata->gci->gc_ConvertEntry(function_entry);
	tmp=xbdata->cur->Child;
	while (tmp && (tmp->entry != entry))
	        tmp=tmp->Next;

	if (tmp && !xbdata->over)
	{
	    if (tf=AllocVec(sizeof(struct TempFile),NULL))
	    {
	        AddTail((struct List *)&xbdata->Temp,(struct Node *)tf);
	        sprintf(tf->FileName,"T:%s",tmp->fib.sfib_FileName);
	        if ((tmp->xfi->xfi_Flags & XADFIF_CRYPTED) && (*xbdata->password == 0))
	        {
	            if (!(PasswordReq(xbdata)))
		        xbdata->over = TRUE;

	        }

		retry = TRUE;

	        while (retry && !xbdata->over)
	        {
		    xbdata->retry = retry = FALSE;

	            err = xadFileUnArc(xbdata->ArcInf,
			               XAD_ENTRYNUMBER,        tmp->xfi->xfi_EntryNumber,
			               XAD_OUTFILENAME,        tf->FileName,
			               XAD_OVERWRITE,          TRUE,
			               (*xbdata->password)?XAD_PASSWORD:TAG_IGNORE, xbdata->password,
			               TAG_DONE);

	            if (err == XADERR_PASSWORD)
	            {
		        retry = PasswordReq(xbdata);
		        xbdata->over=!retry;
	            }
	            else
	            if (xbdata->retry)
		        retry = TRUE;
	        }

	        LaunchCommand(xbdata,com,tf->FileName);
	    }
	}
	xbdata->gci->gc_EndEntry(handle, function_entry, TRUE);
    }
}



///
static void Extract(struct XADbrowserData *xbdata, struct Tree *file, STRPTR dest_file, APTR handle, struct Hook *prhk, struct xadMasterBase *xadMasterBase)
{
    BOOL retry = TRUE;
    xadERROR err;

    while (retry && !xbdata->over)
    {
	retry = FALSE;
	xbdata->retry = FALSE;

	err = xadFileUnArc(xbdata->ArcInf,
	                   XAD_ENTRYNUMBER, file->xfi->xfi_EntryNumber,
	                   XAD_OUTFILENAME, dest_file,
		           XAD_PROGRESSHOOK, prhk,
	                   XAD_OVERWRITE, FALSE,
	                   (*xbdata->password)?XAD_PASSWORD:TAG_IGNORE, xbdata->password,
	                   TAG_DONE);

	if (!err)
	{
	    struct DateStamp date;

	    if (!(file->xfi->xfi_Flags & XADFIF_NODATE) &&
	        !xadConvertDates(XAD_DATEXADDATE,
	                         &file->xfi->xfi_Date,
	                         XAD_GETDATEDATESTAMP,
	                         &date,
	                         TAG_DONE))
	    {
	        SetFileDate(dest_file, &date);
	    }

	    SetProtection(dest_file, file->xfi->xfi_Protection);

	    if (file->xfi->xfi_Comment)
	        SetComment(dest_file, file->xfi->xfi_Comment);

	    // Add entry to destiantion lister
	    if (file->Parent == xbdata->cur)
	    {
	        if (handle)
	        {
		    xbdata->gci->gc_AddFile(handle, NULL, (struct FileInfoBlock *)&file->fib, xbdata->dest_lister, xbdata->destdir_lock);
		    xbdata->gci->gc_DoChanges(handle);
	        }
	        else
	        if (xbdata->dest_lister)
	        {
		    APTR new_entry;

		    xbdata->gci->gc_LockFileList(xbdata->dest_lister, TRUE);

		    if (new_entry = xbdata->gci->gc_CreateFileEntry(xbdata->dest_lister, (struct FileInfoBlock *)&file->fib, NULL))
		    {
		        xbdata->gci->gc_AddFileEntry(xbdata->dest_lister, new_entry, TRUE);
			    xbdata->gci->gc_RefreshLister(xbdata->dest_lister, HOOKREFRESH_FULL, FALSE);
		    }

		    xbdata->gci->gc_UnlockFileList(xbdata->dest_lister);
	        }
	    }
	}

	if (!xbdata->over && err != XADERR_SKIP)
	{
	    if (xbdata->retry)
	    {
	        retry = TRUE;
	    }
	    else
	    if (err==XADERR_BREAK || CheckProgressAbort(xbdata->progress))
	    {
	        xbdata->over=TRUE;
	    }
	    else
	    if (err == XADERR_PASSWORD)
	    {
	        retry = PasswordReq(xbdata);
	        xbdata->over=!retry;
	    }
	    else
	    if (err)
	    {
	        ErrorReq(xbdata,xadGetErrorText(err));
	        xbdata->over = TRUE;
	    }
	}
    }
}

/// Copy
static void Copy(struct XADbrowserData *xbdata, struct Tree *tmp, struct Hook *prhk, APTR handle, BPTR cur_dir, BOOL CopyAs, struct xadMasterBase *xadMasterBase)
{
    BOOL skip;
    char *filename = xbdata->cpy_filename;


    skip=FALSE;
    filename[0] = 0;

    if (CopyAs)
    {
	char message[192], drawer[108];

	sprintf(message, GetString(locale,MSG_RENAME_FORM), tmp->fib.sfib_FileName);
	strcpy(drawer, tmp->fib.sfib_FileName);
	switch(AsyncRequestTags(xbdata->ipc, REQTYPE_SIMPLE, NULL, NULL, NULL,
			        AR_Window, xbdata->source_window,
			        AR_Message, message,
			        AR_Buffer, drawer,
			        AR_BufLen, 108,
			        AR_Button, GetString(locale,MSG_RENAME),
			        AR_Button, GetString(locale,MSG_SKIP),
			        AR_Button, GetString(locale,MSG_ABORT),
			        AR_Flags, SRF_PATH_FILTER,
			        TAG_END))
	{
	    case 0:
	            xbdata->over=TRUE;
	            break;
	    case 2:
	            skip=TRUE;
	    default:
		    strcpy(filename, drawer);
	}
    }
    else
     strcpy(filename, tmp->fib.sfib_FileName);

    if (!skip)
    {
	if (tmp->fib.sfib_DirEntryType<0)
	{
	    SetProgressWindowTags(xbdata->progress,
	            PW_FileName, tmp->fib.sfib_FileName,
	            PW_FileNum, ++xbdata->count,
	            PW_FileDone, 0,
	            PW_FileSize, tmp->xfi->xfi_Size,
	            TAG_DONE);

	    if ((tmp->xfi->xfi_Flags & XADFIF_CRYPTED) && (*xbdata->password == 0))
	    {
	        if (!(PasswordReq(xbdata)))
		    xbdata->over = TRUE;

	    }

	    Extract(xbdata, tmp, filename, handle, prhk, xadMasterBase);
	}
	else
	{
	    SetProgressWindowTags(xbdata->progress,
	            PW_FileName, tmp->fib.sfib_FileName,
	            PW_FileNum, ++xbdata->count,
	            PW_FileDone, 0,
		    PW_FileSize, 1,
	            TAG_DONE);

	    while (tmp && !xbdata->over)
	    {
	        if (tmp->fib.sfib_DirEntryType>0)
	        {
		    BPTR dir;

		    if (tmp->Parent != xbdata->cur)
		    {
	                SetProgressWindowTags(xbdata->progress,
			                      PW_FileName, tmp->fib.sfib_FileName,
			                      PW_FileDone, 0,
					      PW_FileSize, 1,
			                      TAG_DONE);
		    }

	            if (dir=CreateDir(tmp->fib.sfib_FileName))
	            {
		        UnLock(dir);

		        if (tmp->xfi)
		        {
		            SetProtection(tmp->fib.sfib_FileName, tmp->xfi->xfi_Protection);

		            if (tmp->xfi->xfi_Comment)
		                SetComment(tmp->fib.sfib_FileName, tmp->xfi->xfi_Comment);

			    // Mark for setting date
			    if (tmp->Child)
				tmp->fib.sfib_EntryType = TRUE;
		        }

			// Add entry to destiantion lister
			if (tmp->Parent == xbdata->cur)
		        {
			    if (handle)
			    {
				xbdata->gci->gc_AddFile(handle, NULL, (struct FileInfoBlock *)&tmp->fib, xbdata->dest_lister, xbdata->destdir_lock);
				xbdata->gci->gc_DoChanges(handle);
			    }
			    else
			    if (xbdata->dest_lister)
			    {
				APTR entry;

				xbdata->gci->gc_LockFileList(xbdata->dest_lister, TRUE);

				if (entry = xbdata->gci->gc_CreateFileEntry(xbdata->dest_lister, (struct FileInfoBlock *)&tmp->fib, xbdata->destdir_lock))
				{
				    xbdata->gci->gc_AddFileEntry(xbdata->dest_lister, entry, TRUE);
				    xbdata->gci->gc_RefreshLister(xbdata->dest_lister, HOOKREFRESH_FULL, FALSE);
				}

				xbdata->gci->gc_UnlockFileList(xbdata->dest_lister);
			    }
		        }
	            }

		    SetProgressWindowTags(xbdata->progress,
				          PW_FileDone, 1,
		                          TAG_DONE);
		    if (tmp->Child)
		    {
		        UnLock(CurrentDir(cur_dir = Lock(tmp->fib.sfib_FileName, ACCESS_READ)));
		        tmp = tmp->Child;
			continue;
		    }
		    else
		    {
			SetFileDate(tmp->fib.sfib_FileName, &tmp->fib.sfib_Date);
		    }
	        }
	        else
	        {
		    if (xbdata->mod_id == 1 && tmp->fib.sfib_Size &&
			!(tmp->xfi->xfi_Flags&(XADFIF_LINK|XADFIF_INFOTEXT)))
		    {
			xbdata->count++;
		    }


	            SetProgressWindowTags(xbdata->progress,
		            PW_FileName, tmp->fib.sfib_FileName,
			    PW_FileNum, xbdata->count,
		            PW_FileDone, 0,
		            PW_FileSize, tmp->xfi->xfi_Size,
		            TAG_DONE);

	            if ((tmp->xfi->xfi_Flags & XADFIF_CRYPTED) && (*xbdata->password == 0))
		    {
		        if (!(PasswordReq(xbdata)))
			    xbdata->over = TRUE;

		    }

		    Extract(xbdata, tmp, tmp->fib.sfib_FileName, handle, prhk, xadMasterBase);
	        }

	        while (!(tmp->Next))
	        {
	            if (tmp->Parent->Parent != xbdata->cur)
	            {
		        UnLock(CurrentDir(cur_dir = ParentDir(cur_dir)));

		        tmp = tmp->Parent;

		        // Set dir date?
		        if (tmp->fib.sfib_EntryType)
		        {
		            // Clear mark
		            tmp->fib.sfib_EntryType = FALSE;

			    SetFileDate(tmp->fib.sfib_FileName, &tmp->fib.sfib_Date);
		        }
	            }
	            else
	            {
		        UnLock(CurrentDir(cur_dir = ParentDir(cur_dir)));
		        break;
	            }
	        }
	        tmp = tmp->Next;
	    }
	}
    }
}

static void _copy_dropfrom(struct XADbrowserData *xbdata, ULONG numargs, struct WBArg *args, struct xadMasterBase *xadMasterBase)
{
    struct WBArg *cur_arg;
    BPTR org_dir, cur_dir;
    struct Hook prhk;
    struct Tree *tmp;
    struct TagItem tags[] = {HLT_CURBUFFER_LOCK, 0 , TAG_END};

    if (!args || !numargs)
    return;

    if (!xbdata->destdir_lock)
    {
	if (xbdata->dest_lister)
	{
	    tags[0].ti_Data = (ULONG)&xbdata->destdir_lock;
	    xbdata->gci->gc_ListerGet(xbdata->dest_lister, tags);
	    if (!xbdata->destdir_lock)
		return;
	}
	else
	    return;
    }

    if (xbdata->dest_lister)
    {
	IPC_Command(*((IPCData **)xbdata->dest_lister), LISTER_BUSY, 0, (APTR)1, 0, REPLY_NO_PORT);
    }

    prhk.h_Entry=(ULONG (*)()) ProgressHookTr;
    prhk.h_Data=xbdata;

    xbdata->All=5;
    xbdata->over=FALSE;

    xbdata->count = 0;

    xbdata->progress=OpenProgressWindowTags(PW_Window,      xbdata->source_window,
	                                  PW_Title,       GetString(locale,MSG_EXTRACTING),
					  PW_Info,        "",
	                                  PW_FileCount,   numargs,
	                                  PW_FileSize,    1,
	                                  PW_Flags,       PWF_FILENAME|PWF_FILESIZE|PWF_GRAPH|PWF_ABORT|PWF_INFO,
	                                  TAG_DONE);
    cur_dir = DupLock(xbdata->destdir_lock);

    org_dir = CurrentDir(cur_dir);

    while (xbdata->count < numargs && !xbdata->over)
    {
	cur_arg = &args[xbdata->count];

	tmp=xbdata->cur->Child;
	while (tmp && (tmp->entry != (APTR)cur_arg->wa_Lock))
	        tmp=tmp->Next;

	if (!tmp)
	    break;

	Copy(xbdata, tmp, &prhk, NULL, cur_dir, NULL, xadMasterBase);
    }

    if (xbdata->dest_lister)
    {
	IPC_Command(*((IPCData **)xbdata->dest_lister), LISTER_BUSY, 0, (APTR)0, 0, REPLY_NO_PORT);
    }

    xbdata->gci->gc_RefreshLister(xbdata->dest_lister, HOOKREFRESH_DATE, FALSE);

    UnLock(CurrentDir(org_dir));

    if (xbdata->progress)
    {
	CloseProgressWindow(xbdata->progress);
	xbdata->progress = 0;
    }
}

static void _copy_cmd(struct XADbrowserData *xbdata, APTR handle, BOOL CopyAs, struct xadMasterBase *xadMasterBase)
{
    APTR function_entry, entry;
    BPTR org_dir, cur_dir;
    struct Hook prhk;
    LONG total;
    struct Tree *tmp;
    struct TagItem tags[] = {HLT_CURBUFFER_LOCK, 0 , TAG_END};

    if (!handle || !(total = xbdata->gci->gc_EntryCount(handle)))
    return;

    if (!xbdata->destdir_lock)
    {
	if (xbdata->dest_lister)
	{
	    tags[0].ti_Data = (ULONG)&xbdata->destdir_lock;
	    xbdata->gci->gc_ListerGet(xbdata->dest_lister, tags);
	    if (!xbdata->destdir_lock)
		return;
	}
	else
	    return;
    }

    prhk.h_Entry=(ULONG (*)()) ProgressHookTr;
    prhk.h_Data=xbdata;

    xbdata->All=5;
    xbdata->over=FALSE;

    xbdata->count = 0;

    xbdata->gci->gc_FirstEntry(handle, NULL);

    xbdata->progress=OpenProgressWindowTags(PW_Window,      xbdata->source_window,
	                                  PW_Title,       GetString(locale,MSG_EXTRACTING),
					  PW_Info,        "",
	                                  PW_FileCount,   total,
	                                  PW_FileSize,    1,
	                                  PW_Flags,       PWF_FILENAME|PWF_FILESIZE|PWF_GRAPH|PWF_ABORT|PWF_INFO,
	                                  TAG_DONE);
    cur_dir = DupLock(xbdata->destdir_lock);

    org_dir = CurrentDir(cur_dir);

    while ((function_entry = xbdata->gci->gc_GetEntry(handle)) && !xbdata->over)
    {
	entry = xbdata->gci->gc_ConvertEntry(function_entry);
	tmp=xbdata->cur->Child;
	while (tmp && (tmp->entry != entry))
	        tmp=tmp->Next;

	if (!tmp)
	    break;

	Copy(xbdata, tmp, &prhk, handle, cur_dir, CopyAs, xadMasterBase);

	xbdata->gci->gc_EndEntry(handle, function_entry, TRUE);
    }

    UnLock(CurrentDir(org_dir));

    if (xbdata->progress)
    {
	CloseProgressWindow(xbdata->progress);
	xbdata->progress = 0;
    }
}
///

/// Path
static BOOL _path(struct XADbrowserData *xbdata, char *path)
{
    char *old=path, *listerpath=xbdata->path;
    struct Tree *tmp, *cur=xbdata->root;
    char x, *buf = xbdata->buf;
    STRPTR newpath;

    while ((*path!=':') && (*listerpath == *path))
    {
	path++;
	listerpath++;
    }

    if (*path!=':')
    {
	RemoveHandler(xbdata);

	xbdata->gci->gc_ReadDir(xbdata->source_lister, old, NULL, NULL);

	return(TRUE);
    }

    *(++listerpath)=0;
    x=strlen(path);
    if (path[x-1]=='/')
	    path[x-1]=0;

    while (*path)
    {
	tmp=cur;
	path++;
	for(x=0;(buf[x] = *path) && (*path != '/');x++,path++);
	buf[x]=0;

	if (!cur->Child)
	{
	    ErrorReq(xbdata,GetString(locale,MSG_NO_PATH_ERR));
	    _cd(xbdata,tmp);
	    return(FALSE);
	}
	else
	{
	    cur=cur->Child;
	    while ((cur->fib.sfib_DirEntryType<0) || stricmp(cur->fib.sfib_FileName,buf))
	    {
		if (cur->Next)
		    cur=cur->Next;
		else
		{
		    // Do not show requester if the path is the archive root
		    if (!(((strlen(xbdata->path))==(strlen(xbdata->root_path))) &&
			(!stricmp(xbdata->path,xbdata->root_path))))
		    {
		        ErrorReq(xbdata,GetString(locale,MSG_NO_PATH_ERR));
		    }

		    _cd(xbdata,tmp);
		    return(FALSE);
		}
	    }
	}

	if (newpath = JoinString(xbdata->memhandle, xbdata->path, cur->fib.sfib_FileName, NULL, JSF_FS_ADDPART))
	{
	    STRPTR tmppath = xbdata->path;
	    xbdata->path = newpath;
	    FreeMemH(tmppath);
	}
    }

    _cd(xbdata,cur);

    return(FALSE);
}
///

/// Extract Archive
static BOOL ExtractD(struct XADbrowserData *xbdata, struct xadMasterBase *xadMasterBase)
{
    struct DosList *dl;
    struct DosEnvec *de;
    struct xadDiskInfo *xdi=xbdata->ArcInf->xai_DiskInfo;
    struct xadDeviceInfo *dev;
    struct Hook prhk;
    WORD sel;
    Att_List *list;

    prhk.h_Entry=(ULONG (*)()) ProgressHookTr;
    prhk.h_Data=xbdata;

    if (list=Att_NewList(NULL))
    {
	if (dl=LockDosList(LDF_DEVICES|LDF_READ))
	{
	    while (dl=NextDosEntry(dl,LDF_DEVICES))
	    {
		de=BADDR(((struct FileSysStartupMsg *)
			BADDR(dl->dol_misc.dol_handler.dol_Startup))->fssm_Environ);
		if ((xdi->xdi_Heads == de->de_Surfaces) &&
		    (xdi->xdi_Cylinders == (de->de_HighCyl - de->de_LowCyl + 1)) &&
		    (xdi->xdi_TrackSectors == (de->de_SectorPerBlock * de->de_BlocksPerTrack)))
		{
		    Att_NewNode(list,(((char *)BADDR(dl->dol_Name))+1),NULL,ADDNODE_SORT);
		}
	    }
	    UnLockDosList(LDF_DEVICES|LDF_READ);
		sel=SelectionList(list,xbdata->source_window, NULL, GetString(locale,MSG_SELECT_DEST), -1,
						  NULL, NULL, GetString(locale,MSG_OK), GetString(locale,MSG_ABORT), 0, 0);
	    if ((sel>=0) && (dev=xadAllocObject(XADOBJ_DEVICEINFO,NULL)))
	    {
		SetProgressWindowTags(xbdata->progress,
				      PW_FileName, FilePart(xbdata->arc_filename),
				      PW_Info, "",
				      PW_FileDone, 0,
				      PW_FileSize, (xdi->xdi_TotalSectors*xdi->xdi_SectorSize),
				      TAG_DONE);
		dev->xdi_DOSName=Att_NodeName(list,sel);
		xadDiskUnArc(xbdata->ArcInf,
			      XAD_ENTRYNUMBER, 1,
			      XAD_OUTDEVICE, dev,
			      XAD_VERIFY, TRUE,
			      XAD_PROGRESSHOOK, &prhk,
			      TAG_DONE);
		xadFreeObject(dev,NULL);
	    }
	}
	Att_RemList(list,NULL);
    }

    return(FALSE);
}


static BOOL _extract_cmd(struct XADbrowserData *xbdata, APTR handle, struct xadMasterBase *xadMasterBase)
{
    BPTR org_dir, cur_dir;
    struct Hook prhk;
    LONG total;
    struct Tree *tmp;

    if (!handle || !xbdata->root)
    {
	xbdata->over = TRUE;
    }
    else
    {
	prhk.h_Entry=(ULONG (*)()) ProgressHookTr;
	prhk.h_Data=xbdata;

	xbdata->All=5;
	xbdata->over=FALSE;

	xbdata->count = 0;

	total = xbdata->root->FileCount;

	SetProgressWindowTags(xbdata->progress,
			      PW_FileCount, total,
			      PW_FileSize, 0,
			      TAG_DONE);

	cur_dir = DupLock(xbdata->destdir_lock);

	org_dir = CurrentDir(cur_dir);

	tmp = xbdata->root->Child;

	while (tmp && !xbdata->over)
	{
	    Copy(xbdata, tmp, &prhk, handle, cur_dir, FALSE, xadMasterBase);

	    tmp = tmp->Next;
	}

	UnLock(CurrentDir(org_dir));
    }

    return(xbdata->over);
}
///

static BOOL is_dest_customhandler(struct XADbrowserData *xbdata, APTR lister)
{
    char result[32];
    BOOL ret = FALSE;

    result[0] = 0;

    if (lister)
    {
	sprintf(xbdata->buf,"lister query %lu handler", lister);
	xbdata->gci->gc_RexxCommand(xbdata->buf, result, sizeof(result), NULL, NULL);

	if (result[0])
	    ret = TRUE;
    }
    // TODO: handle desktop folder correctly
    else
	ret = TRUE;

    return ret;
}

int __asm __saveds L_Module_Entry(register __a0 char *args,
				  register __a1 struct Screen *screen,
				  register __a2 IPCData *ipc,
				  register __a3 IPCData *main_ipc,
				  register __d0 ULONG mod_id,
				  register __d1 CONST GalileoCallbackInfo *gci)
{
    struct xadMasterBase *xadMasterBase;
    struct XADbrowserData *xbdata;

    char **archives=0;
    short archnum=0;

    BOOL async=FALSE;

    FuncArgs *func_args = 0;
	
    FunctionEntry *Entry=0;
    BPTR dest_lock = 0, org_dir = 0;

    ULONG err, total;
    STRPTR filename=0;
	
    BOOL over = FALSE;

    if (!(xbdata = AllocVec(sizeof(struct XADbrowserData), MEMF_CLEAR))) return(0);

    if (!(xbdata->memhandle = NewMemHandle(512, 256, MEMF_CLEAR))) goto end_1;

    if (!(xbdata->treehandle = NewMemHandle(sizeof(struct Tree)*8, sizeof(struct Tree), MEMF_CLEAR))) goto end_1;

    xbdata->gci = gci;

    xbdata->screen = screen;
    xbdata->ipc = ipc;
    xbdata->mod_id = mod_id;

    if (FUNCASYNC(ipc))
    {
	async=TRUE;
    }
    else if (mod_id == 0)
    {
	ErrorReq(xbdata,GetString(locale,MSG_NEEDS_ASYNC_ERR));
	return 0;
    }

    if (args && args[0])
    {

	if (mod_id == 1)
	{
	    if (func_args=ParseArgs(XADEXTRACT_ARGS,args))
	    {
	        if (func_args->FA_Arguments[0])
	        {
		    archives=(char **)func_args->FA_Arguments[0];

		    if (!(xbdata->arc_filename = CopyString(xbdata->memhandle, archives[archnum])))
			goto end_1;

		    archnum=1;
	        }

	        if (func_args->FA_Arguments[1])
	        {
		    if (!(xbdata->path = CopyString(xbdata->memhandle, (char *)func_args->FA_Arguments[1])) ||
			!(xbdata->destdir_lock = LockFromPath(xbdata->path, NULL, NULL)))
			goto end_1;
	        }
	    }
	}
	else
	{
	    if (func_args=ParseArgs(XADOPEN_ARGS,args))
	    {
	        if (func_args->FA_Arguments[0])
	        {
		    if (!(xbdata->arc_filename = CopyString(xbdata->memhandle, (char *)func_args->FA_Arguments[0])) ||
		    !(xbdata->root_path = CopyString(xbdata->memhandle, (char *)func_args->FA_Arguments[0])))
			    goto end_1;

		    // Always use new listerwindow when archivename comes from commandline
		    xbdata->newlister=TRUE;
	        }
	        else
	        {
		    if (func_args->FA_Arguments[1])
		        xbdata->newlister=TRUE;
		    else
		        xbdata->newlister=FALSE;
	        }
	        DisposeArgs(func_args);
	    }
	}
    }

    NewList((struct List *)&xbdata->Temp);

    // Open xadmaster.library
    if (!(xadMasterBase=(struct xadMasterBase *)OpenLibrary(XADNAME,13)))
    {
	ErrorReq(xbdata,GetString(locale,MSG_XAD_OPEN_ERR));
	goto end_1;
    }

    xbdata->xadMasterBase = xadMasterBase;

    // Ignore selected files in listers if archivenames on commandline
    if (!xbdata->arc_filename)
    {
	if ( !(xbdata->source_node = gci->gc_GetSource(IPCDATA(ipc), NULL)) ) goto end_2;

	gci->gc_FirstEntry(IPCDATA(ipc), NULL);

	if ( !(Entry=gci->gc_GetEntry(IPCDATA(ipc))) ) goto end_2;

	if (xbdata->source_node->pn_lock)
	{
		xbdata->sourcedir_lock = DupLock(xbdata->source_node->pn_lock);
		org_dir = CurrentDir(xbdata->sourcedir_lock);
	}

	filename = (STRPTR)gci->gc_ExamineEntry(Entry, EE_NAME);

	xbdata->arc_filename = CopyString(xbdata->memhandle, filename);
    }

/// Main Extract
    if (mod_id == 1)
    {
	if (!xbdata->path && !(xbdata->dest_node = gci->gc_GetDest(IPCDATA(ipc), NULL))) goto end_2;

	if (xbdata->dest_node)
	{
	    if (xbdata->dest_node->pn_lock)
		xbdata->destdir_lock = DupLock(xbdata->dest_node->pn_lock);
	    xbdata->dest_lister = xbdata->dest_node->pn_lister;
	}

	if (xbdata->source_node)
	{
	    xbdata->source_lister = xbdata->source_node->pn_lister;
	    xbdata->source_window = gci->gc_GetWindow(xbdata->source_lister);

	    total = gci->gc_EntryCount(IPCDATA(ipc));
	}
	else
	{
	    short a=archnum;

	    while (archives[a+1])
	    {
		a++;
	    }
	    total=a;
	}

	xbdata->progress = OpenProgressWindowTags(PW_Window, xbdata->source_window,
						  (xbdata->source_window)?TAG_IGNORE:PW_Screen, xbdata->screen,
				                  PW_Title, GetString(locale,MSG_EXTRACTING),
				                  PW_FileCount, total,
				                  PW_Flags, PWF_FILENAME|PWF_FILESIZE|PWF_GRAPH|PWF_ABORT|PWF_INFO,
				                  TAG_DONE);

	if (xbdata->ArcInf = xadAllocObject(XADOBJ_ARCHIVEINFO, NULL))
	{
	    ULONG count=0;
	    while ((Entry || archnum) && (!over))
	    {
			SetProgressWindowTags(xbdata->progress,
					      PW_Info, FilePart(xbdata->arc_filename),
					      PW_FileName, GetString(locale,MSG_OPENING_ARC),
					      PW_FileNum, ++count,
					      PW_FileCount, total,
					      TAG_DONE);

			if (!(err=xadGetInfo(xbdata->ArcInf,XAD_INFILENAME, xbdata->arc_filename, TAG_DONE)))
				xbdata->ArcMode = xbdata->ArcInf->xai_Client->xc_Flags & XADCF_DISKARCHIVER;

			if (err == XADERR_FILETYPE)
			{
			    // *** DISKIMAGE
			    if (!(err=xadGetDiskInfo(xbdata->ArcInf, XAD_INFILENAME, xbdata->arc_filename, TAG_DONE)))
			    {
				xbdata->ArcMode=XADCF_DISKARCHIVER;
				BuildTree(xbdata, xadMasterBase);

				if (xbdata->root)
			        {
				    xbdata->cur = xbdata->root;
				    over = _extract_cmd(xbdata, IPCDATA(ipc), xadMasterBase);
				    ClearMemHandle(xbdata->treehandle);
				    xbdata->root = 0;
				}
				xadFreeInfo(xbdata->ArcInf);
			    }
			}
			else
			if ((!err) && (xbdata->ArcMode == XADCF_DISKARCHIVER))
			{
			    // *** DISKARCHIVE
			    struct TagItem ti[2];
				switch(AsyncRequestTags(ipc, REQTYPE_SIMPLE, NULL, NULL, NULL,
							AR_Window, xbdata->source_window,
							AR_Message, GetString(locale,MSG_DISKARC_EXTRACT),
							AR_Button, GetString(locale,MSG_FILES),
							AR_Button, GetString(locale,MSG_DISK),
							AR_Button, GetString(locale,MSG_ABORT),
						        TAG_END))
			    {
				case 0:
					break;
				case 2:
					over=ExtractD(xbdata, xadMasterBase);
					break;
				default:
					xadFreeInfo(xbdata->ArcInf);
					ti[0].ti_Tag  = XAD_INFILENAME;
					ti[0].ti_Data = (ULONG) xbdata->arc_filename;
					ti[1].ti_Tag  = TAG_DONE;
					if (!(err = xadGetDiskInfo(xbdata->ArcInf, XAD_INDISKARCHIVE, ti, TAG_DONE)))
					{
					    BuildTree(xbdata, xadMasterBase);

					    if (xbdata->root)
				            {
					        xbdata->cur = xbdata->root;
						over = _extract_cmd(xbdata, IPCDATA(ipc), xadMasterBase);
					        ClearMemHandle(xbdata->treehandle);
					        xbdata->root = 0;
					    }
					}
					break;
			    }
			}
			else
			if (!err)
			{
			    // *** FILEARCHIVE
			    BuildTree(xbdata, xadMasterBase);

			    if (xbdata->root)
			    {
				xbdata->cur = xbdata->root;
				over = _extract_cmd(xbdata, IPCDATA(ipc), xadMasterBase);
				ClearMemHandle(xbdata->treehandle);
				xbdata->root = 0;
			    }
			}

			if (!err)
			    xadFreeInfo(xbdata->ArcInf);
			else
			    ErrorReq(xbdata,xadGetErrorText(err));

			if (filename)
			{
			    gci->gc_EndEntry(IPCDATA(ipc), Entry, TRUE);
			    Entry = gci->gc_GetEntry(IPCDATA(ipc));
			    if (Entry)
			    {
				FreeMemH(xbdata->arc_filename);
				xbdata->arc_filename = CopyString(xbdata->memhandle, Entry->fe_name);
			    }
			}
			else
			{
			    if (archives[archnum])
			    {
				FreeMemH(xbdata->arc_filename);
				xbdata->arc_filename = CopyString(xbdata->memhandle, archives[archnum]);
				archnum++;
			    }
			    else archnum=0;
			}
	    }

	    if (func_args) DisposeArgs(func_args);

	    xadFreeObject(xbdata->ArcInf,NULL);
	}

	if (xbdata->progress)
	    CloseProgressWindow(xbdata->progress);

	if (org_dir)
	    CurrentDir(org_dir);

	if (xbdata->sourcedir_lock)
	    UnLock(xbdata->sourcedir_lock);

	if (xbdata->destdir_lock)
	    UnLock(xbdata->destdir_lock);

	FreeMemHandle(xbdata->treehandle);
	FreeMemHandle(xbdata->memhandle);
	FreeVec(xbdata);

	return(1);
    }
///

/// Main Open

    if (!xbdata->root_path)
	xbdata->root_path = JoinString(xbdata->memhandle, filename, ":", NULL, NULL);

    // Do we have a destination lister?
    if (xbdata->dest_node = gci->gc_GetDest(IPCDATA(ipc), NULL))
    {
	// Store lister for restoring deststination status later
	if (xbdata->dest_lister = xbdata->dest_node->pn_lister)

	// Have to do this, or destination lister will be locked busy
	IPC_Command(*((IPCData **)xbdata->dest_lister), LISTER_BUSY, 0, (APTR)0, 0, REPLY_NO_PORT);
    }

    if (xbdata->source_node && !xbdata->newlister)
    {
	xbdata->source_lister = xbdata->source_node->pn_lister;
	xbdata->source_window = gci->gc_GetWindow(xbdata->source_lister);
    }

    if (filename)
    {
	gci->gc_EndEntry(IPCDATA(ipc), Entry, TRUE);
    }

    if (xbdata->newlister)
    {
	APTR new_lister;

	if (new_lister = gci->gc_NewLister(NULL, NULL, NULL, NULL, NULL , -1, -1, 0, 0))
	{
	    xbdata->source_lister = new_lister;
	}
    }

    if (xbdata->source_lister && AllocPort(xbdata))
    {
	GalileoHandlerMessage *pkt;
	struct TagItem tags[] = {HLT_FAKEDIR, TRUE , TAG_END};
	gci->gc_UnlockSource(IPCDATA(ipc));

	// Free the FunctionHandle, detaching completely from originating lister
	gci->gc_FreePointerDirect(IPCDATA(ipc),MODPTR_HANDLE,POINTERF_DELPORT);


	// To avoid usless snapshot attempt when quitting
	gci->gc_ListerSet(xbdata->source_lister, tags);

	gci->gc_ClearDir(xbdata->source_lister, CBCDF_CLEAR_PATH|CBCDF_CLEAR_EXPANDEDPATH|CBCDF_CLEAR_LOCK);

	if (xbdata->path = CopyString(xbdata->memhandle, xbdata->root_path))
	{
	    IPC_Command(*((IPCData **)xbdata->source_lister), LISTER_SET_PATH, 0, (APTR)xbdata->path, 0, REPLY_NO_PORT);
	}
	else
	    goto end_3;


	sprintf(xbdata->buf, "lister set %lu title %s", xbdata->source_lister, GetString(locale, MSG_READING_ARCHIVE));
	gci->gc_RexxCommand(xbdata->buf, NULL, NULL, NULL, NULL);

	if (xbdata->newlister)
	{
	    IPC_Command(*((IPCData **)xbdata->source_lister), LISTER_MAKE_SOURCE, 0, (APTR)0, 0, REPLY_NO_PORT);
	}

	// Quit and cleanup if user closed window
	while (pkt = (GalileoHandlerMessage *)GetMsg(xbdata->mp))
	{
	    if (!strcmp((char *)pkt->ghm_Command, "inactive"))
	    {
	        sprintf(xbdata->buf, "galileo remtrap * %s", xbdata->mp_name);
	        gci->gc_RexxCommand(xbdata->buf, NULL, NULL, NULL, NULL);

	        ReplyHandlerMessage(pkt);

	        goto end_3;
	    }
	    else ReplyHandlerMessage(pkt);
	}

	gci->gc_RefreshLister(xbdata->source_lister, HOOKREFRESH_FULL, TRUE);

	// Block lister until we are ready to handle events
	IPC_Command(*((IPCData **)xbdata->source_lister), LISTER_BUSY, 0, (APTR)1, 0, REPLY_NO_PORT);

	if (xbdata->newlister)
	{
	    // Did we have a destination lister to begin with?
	    if (xbdata->dest_lister)
	    {
	        BOOL sdlock;

	        tags[0].ti_Tag = HLT_SOURCEDEST_LOCK;
	        tags[0].ti_Data = (ULONG)&sdlock;
	        gci->gc_ListerGet(xbdata->dest_lister, tags);

	        // Restore destination status
	        if (!sdlock)
	        {
		    IPC_Command(*((IPCData **)xbdata->dest_lister), LISTER_MAKE_DEST, 0, (APTR)0, 0, REPLY_NO_PORT);
	        }
	    }

	    xbdata->source_window = gci->gc_GetWindow(xbdata->source_lister);

	}

	if (xbdata->ArcInf = xadAllocObject(XADOBJ_ARCHIVEINFO, NULL))
	{
	    err = xadGetInfo(xbdata->ArcInf, XAD_INFILENAME, xbdata->arc_filename, TAG_DONE);
	    if (!err)
	    {
	        xbdata->ArcMode = xbdata->ArcInf->xai_Client->xc_Flags & XADCF_DISKARCHIVER;
	    }
	    else
	    if (err == XADERR_FILETYPE)
	    {
	        if (!(err = xadGetDiskInfo(xbdata->ArcInf, XAD_INFILENAME, xbdata->arc_filename, TAG_DONE)))
	        {
		    xbdata->ArcMode = XADCF_DISKARCHIVER;
	        }
	    }
	    else
	    if ((!err) && xbdata->ArcMode)
	    {
	        struct TagItem ti[2];

	        xadFreeInfo(xbdata->ArcInf);
	        ti[0].ti_Tag  = XAD_INFILENAME;
	        ti[0].ti_Data = (ULONG) xbdata->arc_filename;
	        ti[1].ti_Tag  = TAG_DONE;
	        err = xadGetDiskInfo(xbdata->ArcInf, XAD_INDISKARCHIVE, ti, TAG_DONE);
	    }

	    if (!err)
	    {
		BuildTree(xbdata, xadMasterBase);
		ChangeDir(xbdata, xbdata->root);

		sprintf(xbdata->buf, "lister set %lu title XADbrowser: %s", xbdata->source_lister, FilePart(xbdata->arc_filename));
	        gci->gc_RexxCommand(xbdata->buf, NULL, NULL, NULL, NULL);

		gci->gc_RefreshLister(xbdata->source_lister, HOOKREFRESH_FULL, FALSE);

	        // Ready to handle input, unblock lister
		IPC_Command(*((IPCData **)xbdata->source_lister), LISTER_BUSY, 0, (APTR)0, 0, REPLY_NO_PORT);

	        while (!over)
	        {
		    Wait(1L<<xbdata->mp->mp_SigBit);
		    while ((!over) && (pkt = (GalileoHandlerMessage *)GetMsg(xbdata->mp)))
		    {
			BOOL devicelist;
			STRPTR args = 0;

			if ((devicelist = (!strcmp(pkt->ghm_Command, "DeviceList"))) ||
			    !strcmp(pkt->ghm_Command, "CacheList"))
		        {
			    if (pkt->ghm_Args)
				args = CopyString(xbdata->memhandle, pkt->ghm_Args);

			    ReplyHandlerMessage(pkt);
			    pkt = 0;

			    RemoveHandler(xbdata);

			    sprintf(xbdata->buf,"command wait source %lu %s %s",
				    xbdata->source_lister, (devicelist)?"DeviceList":"CacheList",
				    (args)?args:(STRPTR)"");
			    gci->gc_RexxCommand(xbdata->buf, NULL, NULL, NULL, NULL);

			    over=TRUE;
		        }
			else
		        if (!strcmp(pkt->ghm_Command, "inactive"))
		        {
			    RemoveHandler(xbdata);

			    over=TRUE;
		        }
		        else if (!stricmp(pkt->ghm_Command, "doubleclick"))
		        {
			    _doubleclick(xbdata, pkt->ghm_Entry, xadMasterBase);
		        }
		        else if (!stricmp(pkt->ghm_Command, "parent"))
		        {
			    ReplyHandlerMessage(pkt);
			    pkt = 0;
			    over = _parent(xbdata);
		        }
		        else if (!stricmp(pkt->ghm_Command, "root"))
		        {
			    _root(xbdata);
		        }
			else if (!strcmp(pkt->ghm_Command,"path"))
		        {
			    if (args = CopyString(xbdata->memhandle, (pkt->ghm_Args &&pkt->ghm_Args[0])?pkt->ghm_Args:xbdata->path))
			    {
			        ReplyHandlerMessage(pkt);
			        pkt = 0;

				over = _path(xbdata, args);
				FreeMemH(args);
			    }

		        }
		        else if (strstr(viewcmds,pkt->ghm_Command))
		        {
			    _viewcommand(xbdata, pkt->ghm_Command, pkt->ghm_FunctionHandle, xadMasterBase);
		        }
		        else if (!strcmp(pkt->ghm_Command, "dropfrom"))
		        {
			    GalileoHandlerAppMessage *msg;

			    msg = (GalileoHandlerAppMessage *)pkt;

			    if (!(is_dest_customhandler(xbdata, msg->gham_DestLister)))
			    {
				xbdata->dest_lister = msg->gham_DestLister;
				xbdata->destdir_lock = msg->gham_DestLock;
			        _copy_dropfrom(xbdata, msg->gham_NumArgs, msg->gham_ArgList, xadMasterBase);
				xbdata->dest_lister = 0;
				xbdata->destdir_lock = 0;
			    }
		        }
		        else if ((!stricmp(pkt->ghm_Command, "copy")) || (!stricmp(pkt->ghm_Command, "move")))
		        {
			    if (is_dest_customhandler(xbdata, pkt->ghm_DestLister))
			        DisplayBeep(screen);
			    else
			    {
				xbdata->dest_lister = pkt->ghm_DestLister;
				xbdata->destdir_lock = pkt->ghm_DestLock;
			        _copy_cmd(xbdata, pkt->ghm_FunctionHandle, FALSE, xadMasterBase);
				xbdata->dest_lister = 0;
				xbdata->destdir_lock = 0;
			    }
		        }
		        else if ((!stricmp(pkt->ghm_Command, "copyas")) || (!stricmp(pkt->ghm_Command, "moveas")))
		        {
			    if (is_dest_customhandler(xbdata, pkt->ghm_DestLister))
			        DisplayBeep(screen);
			    else
			    {
				xbdata->dest_lister = pkt->ghm_DestLister;
				xbdata->destdir_lock = pkt->ghm_DestLock;
				_copy_cmd(xbdata, pkt->ghm_FunctionHandle, TRUE, xadMasterBase);
				xbdata->dest_lister = 0;
				xbdata->destdir_lock = 0;
			    }
		        }

		        if (pkt)
			    ReplyHandlerMessage(pkt);
		    }
	        }
	        xadFreeInfo(xbdata->ArcInf);
	    }
	    else
	    {
	        sprintf(xbdata->buf, "galileo remtrap * %s", xbdata->mp_name);
	        gci->gc_RexxCommand(xbdata->buf, NULL, NULL, NULL, NULL);

		sprintf(xbdata->buf, "lister clear %lu handler", xbdata->source_lister);
	        gci->gc_RexxCommand(xbdata->buf, NULL, NULL, NULL, NULL);

	        ErrorReq(xbdata, xadGetErrorText(err));

	        // unblock lister
		IPC_Command(*((IPCData **)xbdata->source_lister), LISTER_BUSY, 0, (APTR)0, 0, REPLY_NO_PORT);
	    }
	    xadFreeObject(xbdata->ArcInf,NULL);
	}
	FreePort(xbdata);
    }
    else
    {
	gci->gc_UnlockSource(IPCDATA(ipc));

	// Free the FunctionHandle, detaching completely from originating lister
	gci->gc_FreePointerDirect(IPCDATA(ipc),MODPTR_HANDLE,POINTERF_DELPORT);
    }

    RemoveTemp(xbdata);

    // Do not leave password in memory.
    memset(xbdata->password,0,512);

    if (org_dir)
	CurrentDir(org_dir);

    if (dest_lock)
	UnLock(dest_lock);

    if (xbdata->sourcedir_lock)
	UnLock(xbdata->sourcedir_lock);

    if (xadMasterBase) CloseLibrary((struct Library *)xadMasterBase);

    FreeMemHandle(xbdata->treehandle);
    FreeMemHandle(xbdata->memhandle);
    FreeVec(xbdata);

    return(1);

    //Error Handling
    end_3:

    if (org_dir)
	CurrentDir(org_dir);

    if (dest_lock)
	UnLock(dest_lock);

    if (xbdata->sourcedir_lock)
	UnLock(xbdata->sourcedir_lock);

    FreePort(xbdata);

    if (xadMasterBase) CloseLibrary((struct Library *)xadMasterBase);

    return (0);

    end_2:

    if (org_dir)
	CurrentDir(org_dir);
    if (xadMasterBase) CloseLibrary((struct Library *)xadMasterBase);

    end_1:

    if (async)
    {
	// Get deatination pathnode
	if (xbdata->dest_node = gci->gc_GetDest(IPCDATA(ipc), xbdata->path))
	{
	    // Get lister pointer
		xbdata->dest_lister = xbdata->dest_node->pn_lister;

	    // Have to do this, or destination lister will be locked busy
	    IPC_Command(*((IPCData **)xbdata->dest_lister), LISTER_BUSY, 0, (APTR)0, 0, REPLY_NO_PORT);
	}
	gci->gc_UnlockSource(IPCDATA(ipc));
	gci->gc_FreePointerDirect(IPCDATA(ipc), MODPTR_HANDLE, NULL);
    }

    FreeMemHandle(xbdata->treehandle);
    FreeMemHandle(xbdata->memhandle);
    FreeVec(xbdata);

    return (0);
}
///

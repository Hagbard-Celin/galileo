/*
  XADbrowser.gfmmodule 1.22 - Galileo plugin to browse through XAD archives
  Copyright (C) 1999,2000 Mladen Milinkovic <mladen.milinkovic@ri.tel.hr>
  Copyright 2013 Roman Kargin <kas1e@yandex.ru>
  Copyright 2013 Szilard Biro
  Copyright 2023,2024 Hagbard Celine

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
//#include "compiler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <exec/memory.h>
#include <dos/filehandler.h>

#include <proto/exec.h>
#include <proto/dos.h>

#include <clib/alib_protos.h>
#include <proto/xadmaster.h>
#include <libraries/xadmaster.h>
/*
#include <exec/types.h>
#include <exec/libraries.h>
#include <exec/execbase.h>
#include <exec/resident.h>
#include <exec/initializers.h>
*/

//void KPrintF __ARGS((char *,...));

static const char *version="\0$VER: XADbrowser.gfmmodule 0.1 "__AMIGADATE__" Written by Mladen Milinkovic <max2@iname.com>\0";

struct xadMasterBase *xadMasterBase = NULL;
//extern struct xadMasterBase *xadMasterBase;

char *viewcmds="Read Play Show HexRead AnsiRead IconInfo";
 
ULONG __asm __saveds ProgressHook(register __a0 struct Hook *, register __a1 struct xadProgressInfo *);


/// Error Requester
void
ErrorReq(struct ModuleData *data, char *Mess)
{
	AsyncRequestTags(
		data->ipc,
		REQTYPE_SIMPLE,
		0,
		0,
		0,
		AR_Screen,data->screen,
		AR_Message,Mess,
		AR_Title,GetString(locale,MSG_ERROR),
		AR_Button,GetString(locale,MSG_OK),
		(data->listw)?AR_Window:TAG_END,data->listw,
		TAG_END);
}  
///

BOOL PasswordReq(struct ModuleData *data)
{
	int retval;
	BOOL result=TRUE;

	memset(data->password,0,512);


	retval = AsyncRequestTags(
			data->ipc,
			REQTYPE_SIMPLE,
			0,
			0,
			0,
			AR_Screen,data->screen,
			AR_Message,GetString(locale,MSG_ENTER_PASSWORD),
			AR_Buffer,data->password,
			AR_BufLen,512,
		    AR_Button,GetString(locale,MSG_OK),
		    AR_Button,GetString(locale,MSG_ABORT),
			AR_Flags,SRF_SECURE,
		    (data->listw)?AR_Window:TAG_END,data->listw,
			TAG_END);

	if (!retval)
	{
		result=FALSE;
		memset(data->password,0,512);
	}

	return (result);
}

/// ChangeDir
void
ChangeDir(struct ModuleData *data, struct Tree *cur)
{
	struct Tree *tmp;
	struct List list;
	UWORD dc=0, fc=0;

	data->hook.gc_LockFileList(data->listh,TRUE);
	
	if(data->cur) {
		tmp=data->cur->Child;
		while(tmp) {
			data->hook.gc_RemoveFileEntry(data->listh,tmp->entry);
			tmp=tmp->Next;
		}
	}
	
	data->cur=cur;
	cur=cur->Child;

	NewList(&list);
	
	while(cur) {
		cur->entry=data->hook.gc_CreateFileEntry(data->listh, &cur->fib, NULL);
		AddTail(&list,cur->entry);

		if(cur->fib.fib_DirEntryType<0) fc++;
		else dc++;

		cur=cur->Next;
	}

	data->hook.gc_SortFileList(data->listh, &list, fc, dc);
	data->hook.gc_UnlockFileList(data->listh);
	data->hook.gc_RefreshLister(data->listh, HOOKREFRESH_DATE|HOOKREFRESH_FULL);
}
///

struct Tree *
FindDrw(struct ModuleData *data, UBYTE *path)
{
	struct Tree *cur=data->root;
	UBYTE x,buf[50];

	while(1) {
		for(x=0;(buf[x] = *path) && (*path++ != '/');x++);
		if(!buf[x]) return(cur);
		buf[x]=0;

		if(!cur->Child) {
			cur->Child=AllocMemH(data->memhandle,sizeof(struct Tree));
			cur=cur->Child;
			strcpy(cur->fib.fib_FileName,buf);
			cur->fib.fib_DirEntryType=1;
			*cur->fib.fib_Comment=cur->fib.fib_Size=cur->fib.fib_Protection=0;
			DateStamp(&cur->fib.fib_Date);
		} else {
			cur=cur->Child;
			while(strcmp(cur->fib.fib_FileName,buf)) {
				if(cur->Next)
					cur=cur->Next;
				else {
					cur->Next=AllocMemH(data->memhandle,sizeof(struct Tree));
					cur=cur->Next;
					strcpy(cur->fib.fib_FileName,buf);
					cur->fib.fib_DirEntryType=1;
					*cur->fib.fib_Comment=cur->fib.fib_Size=cur->fib.fib_Protection=0;
					DateStamp(&cur->fib.fib_Date);
				}
			}
		}
	}
}

/// BuildTree
void BuildTree(struct ModuleData *data)
{
	struct xadFileInfo *xfi;
	struct Tree *tree, *tmp;
	char *name;
	BOOL dup;

	xfi = data->ArcInf->xai_FileInfo;

	while(xfi)
	{
		tree=FindDrw(data, xfi->xfi_FileName);
		name=FilePart(xfi->xfi_FileName);
		dup=FALSE;

		if(tree->Child)
		{
			tmp=tree->Child;
			while(tmp)
			{
				if(!strcmp(name,tmp->fib.fib_FileName))
				{
					dup=TRUE;
					break;
				}
				tmp=tmp->Next;
			}
		}

		if(!dup)
		{
			if(tree->Child==NULL)
			{
				tree->Child=AllocMemH(data->memhandle,sizeof(struct Tree));
				tree=tree->Child;
			}
			else
			{
				tree=tree->Child;
				while(tree->Next!=NULL) tree=tree->Next;
				tree->Next=AllocMemH(data->memhandle,sizeof(struct Tree));
				tree=tree->Next;
			}

			strcpy(tree->fib.fib_FileName,name);
			if(xfi->xfi_Comment)
			{
				strcpy(tree->fib.fib_Comment,xfi->xfi_Comment);
			}
			else
			{
				*tree->fib.fib_Comment=0;
			}
			xadConvertDates(XAD_DATEXADDATE,	&xfi->xfi_Date,
					XAD_GETDATEDATESTAMP,	&tree->fib.fib_Date,
					TAG_DONE);
			tree->fib.fib_Protection=xfi->xfi_Protection;
			if(xfi->xfi_Flags & XADFIF_DIRECTORY)
				tree->fib.fib_DirEntryType=1;
			else
				tree->fib.fib_DirEntryType = -1;
			tree->fib.fib_Size=xfi->xfi_Size;
			tree->fib.fib_OwnerUID=xfi->xfi_OwnerUID;
			tree->fib.fib_OwnerGID=xfi->xfi_OwnerGID;
			tree->fib.fib_NumBlocks=tree->fib.fib_DiskKey=tree->fib.fib_EntryType=NULL;
			tree->xfi=xfi;
		}

		xfi = xfi->xfi_Next;
	}
}
///

/// MsgPort
BOOL
AllocPort(struct ModuleData *data)
{

//	  UWORD c=0;

	if(data->mp=CreateMsgPort()) {
		(data->mp->mp_Node.ln_Pri) = 2;
		(data->mp->mp_Node.ln_Name) = data->mp_name;  
		sprintf(data->mp_name,"XADbrowser_%p",data->mp); // this is guaranteed to be unique
		AddPort(data->mp);

		sprintf(data->buf,"lister set %s handler %s quotes",data->lists,data->mp_name);
		data->hook.gc_RexxCommand(data->buf, NULL, NULL, NULL, NULL);

		sprintf(data->buf,"galileo addtrap * %s",data->mp_name);
		data->hook.gc_RexxCommand(data->buf, NULL, NULL, NULL, NULL);

		sprintf(data->buf,"galileo remtrap All %s",data->mp_name);
		data->hook.gc_RexxCommand(data->buf, NULL, NULL, NULL, NULL);

		sprintf(data->buf,"galileo remtrap None %s",data->mp_name);
		data->hook.gc_RexxCommand(data->buf, NULL, NULL, NULL, NULL);

		sprintf(data->buf,"galileo remtrap Select %s",data->mp_name);
		data->hook.gc_RexxCommand(data->buf, NULL, NULL, NULL, NULL);

		sprintf(data->buf,"galileo remtrap Toggle %s",data->mp_name);
		data->hook.gc_RexxCommand(data->buf, NULL, NULL, NULL, NULL);

		sprintf(data->buf,"galileo remtrap CacheList %s",data->mp_name);
		data->hook.gc_RexxCommand(data->buf, NULL, NULL, NULL, NULL);

		sprintf(data->buf,"galileo remtrap DeviceList %s",data->mp_name);
		data->hook.gc_RexxCommand(data->buf, NULL, NULL, NULL, NULL);

		return(TRUE);
	}
	return(FALSE);
}

void
FreePort(struct ModuleData *data)
{
	struct Message *msg;

	RemPort(data->mp);

	while(msg=GetMsg(data->mp))
		ReplyMsg(msg);

	DeleteMsgPort(data->mp);
}
///

/// Temp Files
void
RemoveTemp(struct ModuleData *data)
{
	struct TempFile *tf;
	
	while(tf=(struct TempFile *)RemHead((struct List *)&data->Temp)) {
		DeleteFile(tf->FileName);
		FreeVec(tf);
	}
}
///

/// Events
void
LaunchCommand(struct ModuleData *data, char *cmd, char *name, char *qual)
{
	sprintf(data->buf,"lister set %s busy on wait",data->lists);
	data->hook.gc_RexxCommand(data->buf, NULL, NULL, NULL, NULL);

	sprintf(data->buf,"galileo remtrap %s %s",cmd,data->mp_name);
	data->hook.gc_RexxCommand(data->buf, NULL, NULL, NULL, NULL);

	// Need data->lists here or command might end at wrong XADbrowser lister
	sprintf(data->buf,"command wait source %s %s %s %s", data->lists, cmd, qual, name);
	data->hook.gc_RexxCommand(data->buf, NULL, NULL, NULL, NULL);

	sprintf(data->buf,"galileo addtrap %s %s",cmd,data->mp_name);
	data->hook.gc_RexxCommand(data->buf, NULL, NULL, NULL, NULL);

	sprintf(data->buf,"lister set %s busy off wait",data->lists);
	data->hook.gc_RexxCommand(data->buf, NULL, NULL, NULL, NULL);

}

void
_scandir(struct ModuleData *data, char *listh, char *path)
{
	sprintf(data->buf,"command source %s ScanDir %s",listh,path);
	data->hook.gc_RexxCommand(data->buf, NULL, NULL, NULL, NULL);
			//data->hook.gc_SendCommand(IPCDATA(data->ipc),data->buf,NULL,NULL);
}

void
_cd(struct ModuleData *data, struct Tree *cur)
{
	sprintf(data->buf,"lister set %s path %s",data->lists,data->listpath);
	data->hook.gc_RexxCommand(data->buf, NULL, NULL, NULL, NULL);

	ChangeDir(data,cur);
}

void _doubleclick(struct ModuleData *data, char *name, char *qual)
{
	struct Tree *tmp=data->cur->Child;
	struct TempFile *tf;
	xadERROR err;
	BOOL retry;

	data->over=FALSE;

	while(strcmp(tmp->fib.fib_FileName,name) && !data->over) tmp=tmp->Next;

	if(tmp->fib.fib_DirEntryType>0)
	{
		AddPart(data->listpath, tmp->fib.fib_FileName, 512);
		_cd(data,tmp);
	}
	else if(tf=AllocVec(sizeof(struct TempFile),NULL))
	{
		AddTail((struct List *)&data->Temp,(struct Node *)tf);
		sprintf(tf->FileName,"T:%s",name);
		if ((tmp->xfi->xfi_Flags & XADFIF_CRYPTED) && (*data->password == 0))
			PasswordReq(data);

		do
		{
			retry = FALSE;

			err = xadFileUnArc(data->ArcInf,
							   XAD_ENTRYNUMBER,	   tmp->xfi->xfi_EntryNumber,
							   XAD_OUTFILENAME,	   tf->FileName,
							   XAD_OVERWRITE,	   TRUE,
							   (*data->password)?XAD_PASSWORD:TAG_IGNORE, data->password,
							   TAG_DONE);

			if (err == XADERR_PASSWORD)
			{
				retry = PasswordReq(data);
				data->over=!retry;
			}
		} while (retry && !data->over);



		LaunchCommand(data,"DoubleClick",tf->FileName,qual);

	}

}

void
_parent(struct ModuleData *data)
{
	struct Tree *tmp;
	char *path=data->listpath;

	if ((!data->newlister) && ((strlen(data->listpath))==(strlen(data->rootpath))) &&
	   (!stricmp(data->listpath,data->rootpath)))
	{
		sprintf(data->buf,"lister read %s %s",data->lists,data->orgpath);
		data->hook.gc_RexxCommand(data->buf, NULL, NULL, NULL, NULL);
	}
	else
	{
	    while(*path++!=':');

	    if(*path)
		{
		    tmp=FindDrw(data,path);
		    *((char *)PathPart(path))=0;
		    _cd(data,tmp);
	    }
	}
}

void
_root(struct ModuleData *data)
{
	strcpy(data->listpath, data->rootpath);
	sprintf(data->buf,"lister set %s path %s",data->lists,data->listpath);
	data->hook.gc_RexxCommand(data->buf, NULL, NULL, NULL, NULL);

	ChangeDir(data,data->root);
}

void
_viewcommand(struct ModuleData *data,char *com,char *name)
{
	struct Tree *tmp=data->cur->Child;
	struct TempFile *tf;
	xadERROR err;
	BOOL retry;

	data->over=FALSE;

	while(tmp && !data->over)
	{
		while(tmp && (sprintf(data->buf,"\"%s\"",tmp->fib.fib_FileName)) && (!strstr(name,data->buf)) )
			tmp=tmp->Next;

		if(tmp && !data->over)
		{
			if(tf=AllocVec(sizeof(struct TempFile),NULL))
			{
				AddTail((struct List *)&data->Temp,(struct Node *)tf);
				sprintf(tf->FileName,"T:%s",tmp->fib.fib_FileName);
				if ((tmp->xfi->xfi_Flags & XADFIF_CRYPTED) && (*data->password == 0))
					PasswordReq(data);

				do
				{
					retry = FALSE;

					err = xadFileUnArc(data->ArcInf,
									   XAD_ENTRYNUMBER,	   tmp->xfi->xfi_EntryNumber,
									   XAD_OUTFILENAME,	   tf->FileName,
									   XAD_OVERWRITE,	   TRUE,
									   (*data->password)?XAD_PASSWORD:TAG_IGNORE, data->password,
									   TAG_DONE);

					if (err == XADERR_PASSWORD)
					{
						retry = PasswordReq(data);
						data->over=!retry;
					}
				} while (retry && !data->over);

				sprintf(data->buf,"lister select %s \"%s\" off",data->lists,tmp->fib.fib_FileName);
				data->hook.gc_RexxCommand(data->buf, NULL, NULL, NULL, NULL);

				data->hook.gc_RefreshLister(data->listh, NULL);
				LaunchCommand(data,com,tf->FileName,"");
			}
			tmp=tmp->Next;
		}
	}
}

ULONG
countchar(char *buf)
{
	ULONG c=0;

	while(*buf)
		if(*buf++=='"')
			c++;
	return(c);
}
///
/// Copy
void
_copy(struct ModuleData *data,char *name, char *Dest, BOOL CopyAs)
{
	struct Tree *tmp=data->cur->Child;
	struct xadFileInfo *xfi;
	char *FileName, *TreeName, *Drawer, *Drawer2;
	struct Hook prhk;
	LONG count=0,total;
	BOOL skip;
	BPTR dir;    
	xadERROR err;
	BOOL retry;
	struct DirDate *toSet;

	BPTR parent;

	char dirtest[256];

	FileName=AllocVec(1024,0);
	TreeName=AllocVec(1024,0);
	Drawer=AllocVec(1024,0);
	Drawer2=AllocVec(1024,0);
	
	prhk.h_Entry=(ULONG (*)()) ProgressHook;
	prhk.h_Data=data;

	total=countchar(name)/2;

	data->All=5;
	data->over=FALSE;

	data->ptr=OpenProgressWindowTags(
		PW_Window,	data->listw,
		PW_Title,	GetString(locale,MSG_EXTRACTING),
		PW_FileCount,	total,
		PW_FileSize,	1,
		PW_Flags,	PWF_FILENAME|PWF_FILESIZE|PWF_GRAPH|PWF_ABORT|PWF_INFO,
		TAG_DONE);

	// Init list for storing dates of directories
	NewList((struct List *)&data->DirDates);

	while(tmp && (!data->over))
	{
		while(tmp && (sprintf(data->buf,"\"%s\"",tmp->fib.fib_FileName)) && (!strstr(name,data->buf)) )
			tmp=tmp->Next;

		if(tmp)
		{
			strcpy(FileName,Dest);
			skip=FALSE;
			if(CopyAs)
			{
				sprintf(TreeName,GetString(locale,MSG_RENAME_FORM),tmp->fib.fib_FileName);
				strcpy(Drawer,tmp->fib.fib_FileName);
				switch(AsyncRequestTags(data->ipc,REQTYPE_SIMPLE,NULL,NULL,NULL,
							AR_Window,data->listw,
							AR_Message,TreeName,
							AR_Buffer,Drawer,
							AR_BufLen,200,
							AR_Button,GetString(locale,MSG_RENAME),
							AR_Button,GetString(locale,MSG_SKIP),
							AR_Button,GetString(locale,MSG_ABORT),
							AR_Flags,SRF_PATH_FILTER,
							TAG_END))
				{
					case 0:
						data->over=TRUE;
					case 2:
						skip=TRUE;
						break;
					default:
						AddPart(FileName,Drawer,1024);
						break;
				}
			} else AddPart(FileName,tmp->fib.fib_FileName,1024);

			if(!skip)
			{

				if(tmp->fib.fib_DirEntryType<0)
				{
					SetProgressWindowTags(data->ptr,
						PW_FileName,   tmp->fib.fib_FileName,
						PW_FileNum,		++count,
						PW_FileDone,	0,
						PW_FileSize,   tmp->xfi->xfi_Size,
						PW_Info,			"",
						TAG_DONE);
							if ((tmp->xfi->xfi_Flags & XADFIF_CRYPTED) && (*data->password == 0))
									PasswordReq(data);
							do
							{
								retry = FALSE;

								err=xadFileUnArc(data->ArcInf,
									XAD_ENTRYNUMBER,	tmp->xfi->xfi_EntryNumber,
									XAD_OUTFILENAME,	FileName,
									XAD_MAKEDIRECTORY, TRUE,
									XAD_PROGRESSHOOK, &prhk,
									XAD_OVERWRITE, FALSE,
									(*data->password)?XAD_PASSWORD:TAG_IGNORE, data->password,
									TAG_DONE);

								if (!err)
								{
									struct DateStamp date;

									if (!(tmp->xfi->xfi_Flags & XADFIF_NODATE) &&
										!xadConvertDates(XAD_DATEXADDATE,
													 	 &tmp->xfi->xfi_Date,
													 	 XAD_GETDATEDATESTAMP,
													 	 &date,
													 	 TAG_DONE))
									{
	                        			SetFileDate(FileName, &date);
									}

									SetProtection(FileName, tmp->xfi->xfi_Protection);

									if (tmp->xfi->xfi_Comment)
	                        			SetComment(FileName, tmp->xfi->xfi_Comment);
								}
								if(err==XADERR_BREAK || CheckProgressAbort(data->ptr))
								{
									data->over=TRUE;
								}
								else if (err == XADERR_PASSWORD)
								{
									retry = PasswordReq(data);
									data->over=!retry;
								}
							} while (retry && !data->over);
				}
				else
				{

					if((parent = Lock(FileName, SHARED_LOCK)))
		  		    {
						UnLock(parent);

					}

					xfi=data->ArcInf->xai_FileInfo;
					strcpy(Drawer,&data->listpath[strlen(data->rootpath)]);  
					AddPart(Drawer,tmp->fib.fib_FileName,1024);
					strcat(Drawer,"/");                  
					total=strlen(Drawer);
					
					SetProgressWindowTags(data->ptr,
						PW_FileName,   tmp->fib.fib_FileName,
						PW_FileNum,		++count,
						TAG_DONE);
					while(xfi && (!data->over)) {
				  
						strcpy(TreeName,FileName);
						if(!strncmp(Drawer,xfi->xfi_FileName,total))
						{
							AddPart(TreeName,&xfi->xfi_FileName[total],1024);
						}
						
						SetProgressWindowTags(data->ptr,
							PW_Info,   	&xfi->xfi_FileName[total],
							PW_FileDone,	0,
							PW_FileSize,	xfi->xfi_Size,
							TAG_DONE);
						
						if(!(xfi->xfi_Flags & (XADFIF_INFOTEXT | XADFIF_NOFILENAME))
							&& (xfi->xfi_Flags & XADFIF_DIRECTORY))
						{
							strcpy(dirtest, TreeName);
							/* path=PathPart(dirtest);
							path='\0'; */

							if((parent = Lock(dirtest, SHARED_LOCK)))
		  		        	{
								UnLock(parent);
							}
					  
							if(dir=CreateDir(TreeName))
							{
								UnLock(dir);
						   
								// Allocate memory to store directoryname and date for setting later
								if (toSet=AllocMemH(data->memhandle, sizeof(struct DirDate)+strlen(TreeName)))
								{
									// Get date
									if (!(xfi->xfi_Flags & XADFIF_NODATE) &&
								    	!(xadConvertDates(XAD_DATEXADDATE,
														  &xfi->xfi_Date,
												          XAD_GETDATEDATESTAMP,
													      &toSet->date,
														  TAG_DONE)))
									{                                           	
										// Fill inn directory name
										strcpy(toSet->name,TreeName);

										// Add to list for setting date later
										AddTail((struct List *)&data->DirDates,(struct Node *)toSet);
									}
									
	                    		}

								SetProtection(TreeName, xfi->xfi_Protection);
	                    		
								if (xfi->xfi_Comment)
	                    			SetComment(TreeName, xfi->xfi_Comment);
							}
				   		}
						else
						if(!strncmp(Drawer,xfi->xfi_FileName,total))
						{
							if ((xfi->xfi_Flags & XADFIF_CRYPTED) && (*data->password == 0))
								PasswordReq(data);

							do
							{
								retry = FALSE;

								err=xadFileUnArc(data->ArcInf,
												 XAD_ENTRYNUMBER,	 xfi->xfi_EntryNumber,
												 XAD_OUTFILENAME,	 TreeName,
												 XAD_MAKEDIRECTORY,	 TRUE,
												 XAD_PROGRESSHOOK,	 &prhk,
												 XAD_OVERWRITE,		 FALSE,
												 (*data->password)?XAD_PASSWORD:TAG_IGNORE, data->password,
												 TAG_DONE);
								if (!err)
								{
									struct DateStamp date;

									if (!(xfi->xfi_Flags & XADFIF_NODATE) &&
								    	!xadConvertDates(XAD_DATEXADDATE,
													 	 &xfi->xfi_Date,
													 	 XAD_GETDATEDATESTAMP,
													 	 &date,
													 	 TAG_DONE))
									{
	                    				SetFileDate(TreeName, &date);
									}

									SetProtection(TreeName, xfi->xfi_Protection);

									if (xfi->xfi_Comment)
	                    				SetComment(TreeName, xfi->xfi_Comment);
								}
								if(err==XADERR_BREAK || CheckProgressAbort(data->ptr))
								{
									data->over=TRUE;
								}
								else if (err == XADERR_PASSWORD)
								{
									retry = PasswordReq(data);
									data->over=!retry;
								}

							} while (retry && !data->over);
						}

						xfi=xfi->xfi_Next;
					}
				}
				sprintf(data->buf,"lister select %s \"%s\" off",data->lists,FilePart(tmp->fib.fib_FileName));
				data->hook.gc_RexxCommand(data->buf, NULL, NULL, NULL, NULL);

				data->hook.gc_RefreshLister(data->listh, NULL);
			}
			tmp=tmp->Next;
		}
	  
	  	// Go throug list of created directories
		for (toSet=(struct DirDate *)data->DirDates.mlh_Head;
					toSet->node.mln_Succ;)
		{
			struct DirDate *tmpd;

			// Set dete from archive info
			SetFileDate(toSet->name, &toSet->date);

			// Store next node
			tmpd=(struct DirDate *)toSet->node.mln_Succ;

			Remove((struct Node *)toSet);
			FreeMemH(toSet);

			toSet=tmpd;
		}
	}

	if(data->ptr)
		CloseProgressWindow(data->ptr);

	if(FileName)
		FreeVec(FileName);
	if(TreeName)
		FreeVec(TreeName);
	if(Drawer)
		FreeVec(Drawer);
	if(Drawer2)
		FreeVec(Drawer2);
}
///
/// Path
BOOL
_path(struct ModuleData *data, char *path)
{
	char *old=path, *listerpath=data->listpath;
	struct Tree *tmp, *cur=data->root;
	char x,buf[50];

	while((*path!=':') && (*listerpath == *path)) {
		path++;
		listerpath++;
	}
	if(*path!=':') {
		sprintf(data->buf,"lister read %s %s",data->lists,old);
		data->hook.gc_RexxCommand(data->buf, NULL, NULL, NULL, NULL);

		return(TRUE);
	}

	*(++listerpath)=0;
	x=strlen(path);
	if(path[x-1]=='/')
		path[x-1]=0;

	while(*path) {
		tmp=cur;
		path++;
		for(x=0;(buf[x] = *path) && (*path != '/');x++,path++);
		buf[x]=0;

		if(!cur->Child) {
			ErrorReq(data,GetString(locale,MSG_NO_PATH_ERR));
			_cd(data,tmp);
			return(FALSE);
		} else {
			cur=cur->Child;
			while((cur->fib.fib_DirEntryType<0) || stricmp(cur->fib.fib_FileName,buf)) {
				if(cur->Next)
					cur=cur->Next;
				else {
					// Do not show requester if the path is the archive root
					if (!(((strlen(data->listpath))==(strlen(data->rootpath))) &&
					   (!stricmp(data->listpath,data->rootpath))))
					    ErrorReq(data,GetString(locale,MSG_NO_PATH_ERR));

					_cd(data,tmp);
					return(FALSE);
				}
			}
		}
		AddPart(data->listpath,cur->fib.fib_FileName,512);
	}

	_cd(data,cur);

	return(FALSE);
}
///

/// Extract Archive
BOOL
ExtractD(struct ModuleData *data)
{
	struct DosList *dl;
	struct DosEnvec *de;
	struct xadDiskInfo *xdi=data->ArcInf->xai_DiskInfo;
	struct xadDeviceInfo *dev;
	struct Hook prhk;
	WORD sel;
	Att_List *list;

	prhk.h_Entry=(ULONG (*)()) ProgressHook;
	prhk.h_Data=data;

	if(list=Att_NewList(NULL)) {
		if(dl=LockDosList(LDF_DEVICES|LDF_READ)) {
			while(dl=NextDosEntry(dl,LDF_DEVICES)) {
				de=BADDR(((struct FileSysStartupMsg *)
					BADDR(dl->dol_misc.dol_handler.dol_Startup))->fssm_Environ);
				if((xdi->xdi_Heads == de->de_Surfaces) &&
					(xdi->xdi_Cylinders == (de->de_HighCyl - de->de_LowCyl + 1)) &&
					(xdi->xdi_TrackSectors == (de->de_SectorPerBlock * de->de_BlocksPerTrack)))
						Att_NewNode(list,(((char *)BADDR(dl->dol_Name))+1),NULL,ADDNODE_SORT);
			}
			UnLockDosList(LDF_DEVICES|LDF_READ);
			sel=SelectionList(list,data->listw,NULL,GetString(locale,MSG_SELECT_DEST),-1,
				NULL,NULL,GetString(locale,MSG_OK),GetString(locale,MSG_ABORT),0,0);
			if((sel>=0) && (dev=xadAllocObject(XADOBJ_DEVICEINFO,NULL))) {
				SetProgressWindowTags(data->ptr,
						PW_FileName,	FilePart(data->arcname),
						PW_Info,	"",
						PW_FileDone,	0,
						PW_FileSize,	(xdi->xdi_TotalSectors*xdi->xdi_SectorSize),
						TAG_DONE);
				dev->xdi_DOSName=Att_NodeName(list,sel);
				xadDiskUnArc(data->ArcInf,
						XAD_ENTRYNUMBER,	1,
						XAD_OUTDEVICE,		dev,
						XAD_VERIFY,		TRUE,
						XAD_PROGRESSHOOK,	&prhk,
						TAG_DONE);  
				xadFreeObject(dev,NULL);
			}
		}
		Att_RemList(list,NULL);
	}

	return(FALSE);
}

BOOL
ExtractF(struct ModuleData *data)
{
	struct xadFileInfo *xfi;
	struct Hook prhk;
	char FileName[1024];
	BPTR dir;
	ULONG total=1;
	xadERROR err;
	BOOL retry;
	struct DirDate *toSet;

	prhk.h_Entry=(ULONG (*)()) ProgressHook;
	prhk.h_Data=data;

	data->All=5;
	data->over=FALSE;

	xfi=data->ArcInf->xai_FileInfo;
	while(xfi->xfi_Next) {
		total++;
		xfi=xfi->xfi_Next;
	}
	xfi=data->ArcInf->xai_FileInfo;
	
	SetProgressWindowTags(data->ptr,
		PW_FileCount, total,
		PW_FileSize, 0,
		TAG_DONE);

	total=0;

	NewList((struct List *)&data->DirDates);

	while(xfi && (!data->over)) {
		strcpy(FileName,xfi->xfi_FileName);
		*((char *)FilePart(FileName))=0;

		SetProgressWindowTags(data->ptr,
			PW_FileName,	FilePart(xfi->xfi_FileName),
			PW_FileNum,	++total,
			PW_Info,   	FileName,
			PW_FileDone,	0,
			PW_FileSize,	xfi->xfi_Size,
			TAG_DONE);
		
		strcpy(FileName,data->listpath);
		AddPart(FileName,xfi->xfi_FileName,1024);

		if ((xfi->xfi_Flags & XADFIF_CRYPTED) && (*data->password == 0))
			PasswordReq(data);

		if(xfi->xfi_Flags & (XADFIF_INFOTEXT | XADFIF_NOFILENAME))
			;
		else if(xfi->xfi_Flags & XADFIF_DIRECTORY) {
			if(dir=CreateDir(FileName))
			{
				UnLock(dir);

				// Allocate memory to store directoryname and date for setting later
				if (toSet=AllocMemH(data->memhandle, sizeof(struct DirDate)+strlen(FileName)))
				{
					// Get date
					if (!(xfi->xfi_Flags & XADFIF_NODATE) &&
						!(xadConvertDates(XAD_DATEXADDATE,
										  &xfi->xfi_Date,
								          XAD_GETDATEDATESTAMP,
									      &toSet->date,
										  TAG_DONE)))
					{
						// Fill inn directory name
						strcpy(toSet->name,FileName);

						// Add to list for setting date later
						AddTail((struct List *)&data->DirDates,(struct Node *)toSet);
					}

	            }

				if (xfi->xfi_Protection)
					SetProtection(FileName, xfi->xfi_Protection);

				if (xfi->xfi_Comment)
	            	SetComment(FileName, xfi->xfi_Comment);
			}
 		} else do {
			retry = FALSE;

			err=xadFileUnArc(data->ArcInf,
					XAD_ENTRYNUMBER,	xfi->xfi_EntryNumber,
					XAD_OUTFILENAME,	FileName,
					XAD_MAKEDIRECTORY, TRUE,
					XAD_PROGRESSHOOK, &prhk,
					XAD_OVERWRITE, FALSE,
					(*data->password)?XAD_PASSWORD:TAG_IGNORE, data->password,
					TAG_DONE);

			if (!err)
			{
				struct DateStamp date;

				if (!(xfi->xfi_Flags & XADFIF_NODATE) &&
					!xadConvertDates(XAD_DATEXADDATE,
									 &xfi->xfi_Date,
									 XAD_GETDATEDATESTAMP,
								 	 &date,
								 	 TAG_DONE))
				{

					SetFileDate(FileName, &date);
				}

				SetProtection(FileName, xfi->xfi_Protection);

				if (xfi->xfi_Comment)
				SetComment(FileName, xfi->xfi_Comment);
			}
			if(err==XADERR_BREAK || CheckProgressAbort(data->ptr))
			{
				data->over=TRUE;
			}
			else if (err == XADERR_PASSWORD)
			{
				retry = PasswordReq(data);
				data->over=!retry;
			}
		} while (retry && !data->over);
		xfi=xfi->xfi_Next;
	}

	// Go through list of created directories
	for (toSet=(struct DirDate *)data->DirDates.mlh_Head;
				toSet->node.mln_Succ;)
	{
		struct DirDate *tmpd;

		// Set dete from archive info
		SetFileDate(toSet->name, &toSet->date);

		// Store next node
		tmpd=(struct DirDate *)toSet->node.mln_Succ;

		Remove((struct Node *)toSet);
		FreeMemH(toSet);

		toSet=tmpd;
	}

	return(data->over);
}                       
///

int __saveds __asm L_Module_Entry(
	register __a0 char *args,
	register __a1 struct Screen *screen,
	register __a2 IPCData *ipc,
	register __a3 IPCData *main_ipc,
	register __a4 APTR *memhandlep,
	register __d0 ULONG mod_id,
	register __d1 EXT_FUNC(func_callback)
	)
{
	char arcname[512];
	char buf[512];
	char result[100];
	char **archives=0;
	short archnum=0;

	struct ModuleData data;
	BOOL async=FALSE;

	struct Tree root;

	int openwinret;
	
	struct function_entry *Entry=0;

	ULONG err, total;
	STRPTR filename=0;
	
	struct MyPacket *pkt;
	BOOL over = FALSE;
	char *a0, *a2, *a3, *a5, *a6, *a7;

	root.xfi = root.entry = root.Next = root.Child = NULL;
	data.ipc = ipc;
	data.root = &root;
	data.arcname = arcname;
	data.buf = buf;
	data.listw = NULL;
	data.cur = NULL;
	data.locale = locale;
	data.GalileoFMBase = GalileoFMBase;
	data.DOSBase = DOSBase;
	data.UtilityBase = UtilityBase;

	data.screen = screen;

	arcname[0]=0;

	if (memhandlep && *memhandlep)
	{
		async=TRUE;
	}
	else if (mod_id == 0)
	{
		ErrorReq(&data,GetString(locale,MSG_NEEDS_ASYNC_ERR));
		return 0;
	}

	if(mod_id == 1)
	{
		if (data.args=ParseArgs(XADEXTRACT_ARGS,args))
		{
			if (data.args->FA_Arguments[0])
			{
				archives=(char **)data.args->FA_Arguments[0];

				strcpy(arcname,archives[archnum]);
				archnum=1;
			}

			if (data.args->FA_Arguments[1])
			{
				strcpy(data.listpath,(char *)data.args->FA_Arguments[1]);
			}
		}
	}
	else
	{
		if (data.args=ParseArgs(XADOPEN_ARGS,args))
		{
			if (data.args->FA_Arguments[0])
			{
				strcpy(arcname, (char *)data.args->FA_Arguments[0]);
				strcpy(data.rootpath, FilePart((char *)data.args->FA_Arguments[0]));

				// Always use new listerwindow when archivename comes from commandline
				data.newlister=TRUE;
			}
			else
			{
				if (data.args->FA_Arguments[1])
					data.newlister=TRUE;
				else
					data.newlister=FALSE;
			}
			DisposeArgs(data.args);
		}
	}

	NewList((struct List *)&data.Temp);

	data.hook.gc_Count=GALILEOFM_HOOK_COUNT;

	if(IPC_Command(main_ipc, HOOKCMD_GET_CALLBACKS, 0, &data.hook, 0, REPLY_NO_PORT) != GALILEOFM_HOOK_COUNT)
	{
		ErrorReq(&data, GetString(locale, MSG_NO_HOOKS_ERR));
		return 0;
	}

	if(async)
		data.memhandle = *memhandlep;
	else
		if(!(data.memhandle = NewMemHandle(NULL, NULL, MEMF_CLEAR))) return(0);

	if (!(data.password=AllocMemH(data.memhandle,512))) goto end_1;

	// Open xadmaster.library
	if (!(xadMasterBase=(struct xadMasterBase *)OpenLibrary(XADNAME,13))) goto end_1;

	data.xadMasterBase = xadMasterBase;

	// Ignore selected files in listers if archivenames on commandline
	if (!(arcname[0]))
	{
		if( !(data.listp2 = data.hook.gc_GetSource(IPCDATA(ipc), data.orgpath)) ) goto end_2;

		data.hook.gc_FirstEntry(IPCDATA(ipc));

		if( !(Entry=data.hook.gc_GetEntry(IPCDATA(ipc))) ) goto end_2;

		filename = (STRPTR)data.hook.gc_ExamineEntry(Entry, EE_NAME);

		strcpy(arcname, data.orgpath);

		AddPart(arcname, filename, 512);
	}

/// Main Extract
	if(mod_id == 1)
	{
		if (!(data.listpath[0]))
		{
			if(!(data.destp = data.hook.gc_GetDest(IPCDATA(ipc), data.listpath))) goto end_2;

			data.hook.gc_EndDest(IPCDATA(ipc), 0);
		}

		if (data.listp2)
		{
			data.listh = (ULONG)data.listp2->lister;
			data.listw = data.hook.gc_GetWindow(data.listp2);
			sprintf(data.lists, "%lu", data.listh);

			sprintf(buf,"lister query %s numselentries", data.lists);
			total=data.hook.gc_SendCommand(IPCDATA(ipc),buf,NULL,NULL);
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

		data.ptr=OpenProgressWindowTags(
				PW_Window, data.listw,
				(data.listw)?TAG_IGNORE:PW_Screen,data.screen,
				PW_Title,	GetString(locale,MSG_EXTRACTING),
				PW_FileCount,	total,
				PW_Flags,	PWF_FILENAME|PWF_FILESIZE|PWF_GRAPH|PWF_ABORT|PWF_INFO,
				TAG_DONE);

		if(data.ArcInf=xadAllocObject(XADOBJ_ARCHIVEINFO,NULL)) {
			ULONG count=0;
			while((Entry || archnum) && (!over))
			{
				SetProgressWindowTags(data.ptr,
					PW_Info,	FilePart(arcname),
					PW_FileName,	GetString(locale,MSG_OPENING_ARC),
					PW_FileNum,	++count,
					PW_FileCount,	total,
					TAG_DONE);

				if(!(err=xadGetInfo(data.ArcInf,XAD_INFILENAME, arcname, TAG_DONE)))
					data.ArcMode=data.ArcInf->xai_Client->xc_Flags & XADCF_DISKARCHIVER;
				
				if(err == XADERR_FILETYPE)
				{
					// *** DISKIMAGE
					if(!(err=xadGetDiskInfo(data.ArcInf, XAD_INFILENAME, arcname, TAG_DONE)))
					{
						data.ArcMode=XADCF_DISKARCHIVER;
						over=ExtractF(&data);
						xadFreeInfo(data.ArcInf);
					}
				}
				else if((!err) && (data.ArcMode == XADCF_DISKARCHIVER))
				{
					// *** DISKARCHIVE
					struct TagItem ti[2];
					switch(AsyncRequestTags(ipc,REQTYPE_SIMPLE,NULL,NULL,NULL,
								(data.listw)?AR_Window:TAG_IGNORE,data.listw,
								(data.listw)?TAG_IGNORE:AR_Screen,data.screen,
								AR_Message,GetString(locale,MSG_DISKARC_EXTRACT),
								AR_Button,GetString(locale,MSG_FILES),
								AR_Button,GetString(locale,MSG_DISK),
								AR_Button,GetString(locale,MSG_ABORT),
								AR_Flags,SRF_PATH_FILTER,
								TAG_END))
					{
						case 0:
							break;
						case 2:
							over=ExtractD(&data);
							break;
						default:
							xadFreeInfo(data.ArcInf);
							ti[0].ti_Tag  = XAD_INFILENAME;
							ti[0].ti_Data = (ULONG) arcname;
							ti[1].ti_Tag  = TAG_DONE;
							if(!(err=xadGetDiskInfo(data.ArcInf, XAD_INDISKARCHIVE, ti, TAG_DONE)))
								over=ExtractF(&data);
							break;
					}
				}
				else if(!err)
				// *** FILEARCHIVE
					over=ExtractF(&data);
				
				if(!err)
					xadFreeInfo(data.ArcInf);
				else
					ErrorReq(&data,xadGetErrorText(err));

				if (filename)
				{
					data.hook.gc_EndEntry(IPCDATA(ipc),Entry,TRUE);
					Entry=data.hook.gc_GetEntry(IPCDATA(ipc));
					if (Entry)
					{
						*((char *)PathPart(arcname))=0;
						AddPart(arcname,Entry->name,512);
					}
				}
				else
				{
					if (archives[archnum])
					{
						strcpy(arcname,archives[archnum]);
						archnum++;
					}
					else archnum=0;
				}
			}

			if (data.args) DisposeArgs(data.args);

			if (data.destp)
			{
				// Needed for the following scandir not to open a new window
				sprintf(buf,"lister set %lu busy off wait",data.destp->lister);
				data.hook.gc_SendCommand(IPCDATA(ipc),buf,NULL,NULL);

	  		    sprintf(buf,"command source %lu ScanDir %s",data.destp->lister,data.destp->path);
				data.hook.gc_SendCommand(IPCDATA(ipc),buf,NULL,NULL);
			}
			xadFreeObject(data.ArcInf,NULL);
		}
		
		if(data.ptr)
			CloseProgressWindow(data.ptr);

		// Plug memory leak
		FreeMemHandle(data.memhandle);

		return(1);
	}
///

/// Main Open

	if (!(data.rootpath[0]))
		strcpy(data.rootpath, filename);

	strcat(data.rootpath, ":");

	// Do we have a destination lister?
	if(data.destp=data.hook.gc_GetDest(IPCDATA(ipc), data.listpath))
	{
		// Store lister for restoring deststination status later
		data.desth=(ULONG)data.destp->lister;

		data.hook.gc_EndDest(IPCDATA(ipc), 0);

		// Have to do this, or destination lister will be locked busy
		sprintf(buf,"lister set %lu busy off wait",data.desth);
		data.hook.gc_RexxCommand(buf, NULL, NULL, NULL, NULL);
	}

	if (data.listp2)
	{
		data.listh2=(ULONG)data.listp2->lister;

		if(!data.newlister)
		{
			 sprintf(data.lists, "%lu", data.listh2);
			 data.listh=data.listh2;
			 data.listw=data.hook.gc_GetWindow(data.listp2);
		}
	}

	if (filename)
	{
		data.hook.gc_EndEntry(IPCDATA(ipc), Entry, TRUE);
		data.hook.gc_EndSource(IPCDATA(ipc), 1);
	}

	if(data.newlister)
	{
		openwinret=data.hook.gc_RexxCommand("lister new", result, sizeof(result), NULL, NULL);
#ifdef _DEBUG
		KPrintF("lister new returned: %ld result: %s\n", openwinret, result);
#endif
	}

	if (result || !data.newlister)
	{
		if(data.newlister)
		{
			strcpy(data.lists, result);
		    data.listh = strtoul(result,NULL,10);
		}

//		ErrorReq(&data, data.lists); // *********************

		if(AllocPort(&data))
		{
			struct _PathNode listp;

			data.hook.gc_UnlockSource(IPCDATA(ipc));

			// Free the FunctionHandle, detaching completely from originating lister
			data.hook.gc_FreePointerDirect(IPCDATA(ipc),MODPTR_HANDLE,POINTERF_DELPORT);

			data.listp=&listp;

			// To avoid usless snapshot attempt when quitting
			data.hook.gc_FakeDir(data.listh,TRUE);

			strcpy(data.listpath, data.rootpath);
			sprintf(buf, "lister set %s path %s", data.lists, data.listpath);
			data.hook.gc_RexxCommand(buf, NULL, NULL, NULL, NULL);

			sprintf(buf, "lister clear %s", data.lists);
			data.hook.gc_RexxCommand(buf, NULL, NULL, NULL, NULL);

			sprintf(buf, "lister set %s title %s", data.lists, GetString(locale, MSG_READING_ARCHIVE));
			data.hook.gc_RexxCommand(buf, NULL, NULL, NULL, NULL);

			if(data.newlister)
			{
				sprintf(buf, "lister set %s source", data.lists);
			    data.hook.gc_RexxCommand(buf, NULL, NULL, NULL, NULL);
			}

			// Quit and cleanup if user closed window
			while (pkt = (struct MyPacket *)GetMsg(data.mp))
			{
				if (!strcmp((char *)pkt->sp_Pkt.dp_Arg1, "inactive"))
				{
					sprintf(buf, "galileo remtrap * %s", data.mp_name);
					data.hook.gc_RexxCommand(buf, NULL, NULL, NULL, NULL);

					ReplyMsg((struct Message *)pkt);

					goto end_3;
				}
				else ReplyMsg((struct Message *)pkt);
			}

			data.hook.gc_RefreshLister(data.listh, HOOKREFRESH_FULL);

			sprintf(buf, "lister wait %s quick", data.lists);
			data.hook.gc_RexxCommand(buf, NULL, NULL, NULL, NULL);

			// Block lister until we are ready to handle events
			sprintf(buf, "lister set %s busy on wait", data.lists);
			data.hook.gc_RexxCommand(buf, NULL, NULL, NULL, NULL);

			if(data.newlister)
			{
				// Did we have a destination lister to begin with?
				if(data.desth)
				{
#ifdef _DEBUG
					KPrintF("4 org: %lu dst: %lu\n", data.listh2, data.desth);
#endif
					// Turn originating lister off
					if (!(data.hook.gc_IsSourceDestLock(data.listh2)))
					{
						sprintf(buf, "lister set %lu off", data.listh2);
			            data.hook.gc_RexxCommand(buf, NULL, NULL, NULL, NULL);
					}

					// Restore destination status
					if (!(data.hook.gc_IsSourceDestLock(data.desth)))
					{
						sprintf(buf, "lister set %lu dest", data.desth);
			            data.hook.gc_RexxCommand(buf, NULL, NULL, NULL, NULL);
					}
				}


			    data.listp->lister = (APTR)data.listh;
			    *data.listp->path_buf = data.listp->flags = 0;
			    data.listp->path = data.listp->path_buf;
			    data.listw = data.hook.gc_GetWindow(data.listp);
			}

			if(data.ArcInf = xadAllocObject(XADOBJ_ARCHIVEINFO, NULL))
			{
				err = xadGetInfo(data.ArcInf, XAD_INFILENAME, arcname, TAG_DONE);
				if(!err)
				{
					data.ArcMode = data.ArcInf->xai_Client->xc_Flags & XADCF_DISKARCHIVER;
				}
				else if(err == XADERR_FILETYPE)
				{
					if(!(err = xadGetDiskInfo(data.ArcInf, XAD_INFILENAME, arcname, TAG_DONE)))
					{
						data.ArcMode = XADCF_DISKARCHIVER;
					}
				}
				else if((!err) && data.ArcMode)
				{
					struct TagItem ti[2];

					xadFreeInfo(data.ArcInf);
					ti[0].ti_Tag  = XAD_INFILENAME;
					ti[0].ti_Data = (ULONG) arcname;
					ti[1].ti_Tag  = TAG_DONE;
					err = xadGetDiskInfo(data.ArcInf, XAD_INDISKARCHIVE, ti, TAG_DONE);
				}

				if(!err)
				{
					BuildTree(&data);
					ChangeDir(&data, &root);

					sprintf(buf, "lister set %s title XADbrowser: %s", data.lists, FilePart(arcname));
					data.hook.gc_RexxCommand(buf, NULL, NULL, NULL, NULL);

					data.hook.gc_RefreshLister(data.listh, HOOKREFRESH_FULL);

					// Ready to handle input, unblock lister
					sprintf(buf, "lister set %s busy off wait", data.lists);
					data.hook.gc_RexxCommand(buf, NULL, NULL, NULL, NULL);

					while(!over)
					{
						Wait(1L<<data.mp->mp_SigBit);
						while((!over) && (pkt = (struct MyPacket *)GetMsg(data.mp)))
						{
							a0 = (char *)pkt->sp_Pkt.dp_Arg1;
							a2 = (char *)pkt->sp_Pkt.dp_Arg3;
							a3 = (char *)pkt->sp_Pkt.dp_Arg4;
							a5 = (char *)pkt->sp_Pkt.dp_Arg6;
							a6 = (char *)pkt->sp_Pkt.dp_Arg7;
							a7 = pkt->a7;

							if(!strcmp(a0, "inactive"))
							{
								sprintf(buf, "galileo remtrap * %s", data.mp_name);
								data.hook.gc_RexxCommand(buf, NULL, NULL, NULL, NULL);

								over=TRUE;
							}
							else if(!stricmp(a0, "doubleclick"))
							{
								_doubleclick(&data, a2, a6);
							}
							else if(!stricmp(a0, "parent"))
							{
								_parent(&data);
							}
							else if(!stricmp(a0, "root"))
							{
								_root(&data);
							}
							else if(!strcmp(a0,"path"))
							{
								over = _path(&data, a2);
							}
							else if(strstr(viewcmds,a0))
							{
								_viewcommand(&data,a0,a2);
							}
							else if(!strcmp(a0, "dropfrom"))
							{
								_copy(&data, a2, a5, FALSE);
								_scandir(&data, a3, a5);
							}
							else if((!stricmp(a0, "copy")) || (!stricmp(a0, "move")))
							{
								_copy(&data, a2, a7, FALSE);
								_scandir(&data, a3, a7);
							}
							else if((!stricmp(a0, "copyas")) || (!stricmp(a0, "moveas")))
							{
								_copy(&data, a2, a7, TRUE);
								_scandir(&data, a3, a7);
							}
							ReplyMsg((struct Message *)pkt);
						}
					}
					xadFreeInfo(data.ArcInf);
				}
				else
				{
					sprintf(buf, "galileo remtrap * %s", data.mp_name);
					data.hook.gc_RexxCommand(buf, NULL, NULL, NULL, NULL);

					ErrorReq(&data, xadGetErrorText(err));

					// unblock lister
					sprintf(buf, "lister set %s busy off wait", data.lists);
					data.hook.gc_RexxCommand(buf, NULL, NULL, NULL, NULL);

				}
				xadFreeObject(data.ArcInf,NULL);
			}
			FreePort(&data);
		}
		else
		{
			data.hook.gc_UnlockSource(IPCDATA(ipc));

			// Free the FunctionHandle, detaching completely from originating lister
			data.hook.gc_FreePointerDirect(IPCDATA(ipc),MODPTR_HANDLE,POINTERF_DELPORT);
		}
	}
#ifdef _DEBUG
	KPrintF("XAD normal END!! \n");
#endif
	RemoveTemp(&data);

	// Do not leave password in memory.
	memset(data.password,0,512);

	if(xadMasterBase) CloseLibrary((struct Library *)xadMasterBase);
 	
	if(!async)
		FreeMemHandle(data.memhandle);
	return(1);

	//Error Handling
	end_3:

	FreePort(&data);
	if(xadMasterBase) CloseLibrary((struct Library *)xadMasterBase);
	return (0);

	end_2:
#ifdef _DEBUG
	KPrintF("XAD fail2 END!! \n");
#endif
	if(xadMasterBase) CloseLibrary((struct Library *)xadMasterBase);

	end_1:
#ifdef _DEBUG
	KPrintF("XAD fail1 END!! \n");
#endif
	if (async)
		data.hook.gc_FreePointerDirect(IPCDATA(ipc),GETPTR_HANDLE,NULL);

	if(!async)
		FreeMemHandle(data.memhandle);
	end_0:

	return (0);
}
///

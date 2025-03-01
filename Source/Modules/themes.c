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

#include "themes.h"

char *version="$VER: themes.gfmmodule 0.2 "__AMIGADATE__" ";

int __asm __saveds L_Module_Entry(
	register __a0 char *argstring,
	register __a1 struct Screen *screen,
	register __a2 IPCData *ipc,
	register __a3 IPCData *main_ipc,
	register __a4 APTR *memhandlep,
	register __d0 ULONG mod_id,
	register __d1 EXT_FUNC(func_callback))
{
    GalileoCallbackInfo info;
	char filename[300];
	FuncArgs *args;

	// Get callbacks
    info.gc_Count=GALILEOFM_HOOK_COUNT;
    info.gc_RexxCommand=0;
    info.gc_FileRequest=0;
    info.gc_GetThemes=0;
    info.gc_UnlockSource=0;
    IPC_Command(main_ipc,HOOKCMD_GET_CALLBACKS,0,&info,0,REPLY_NO_PORT);

	// Must be able to send rexx commands and show file requester
	if (!info.gc_RexxCommand || !info.gc_FileRequest) return 0;

	// See if filename is supplied
	filename[0]=0;
	if ((args=ParseArgs(func_templates[mod_id],argstring)) && args->FA_Arguments[0])
	{
		strcpy(filename,(char *)args->FA_Arguments[0]);
		if (!strchr(filename,'/') && !strchr(filename,':'))
			lsprintf(filename,"G_THEMES:%s",(char *)args->FA_Arguments[0]);
		if (mod_id!=CONVERTTHEME && (strlen(filename)<7 || stricmp(filename+strlen(filename)-6,".theme")!=0))
			strcat(filename,".theme");
	}

	// No filename?
	if (!*filename && (mod_id==LOADTHEME || mod_id==CONVERTTHEME))
	{
		FunctionEntry *entry;

		// Get first entries
		if (entry=(FunctionEntry *)func_callback(EXTCMD_GET_ENTRY,IPCDATA(ipc),0))
		{
			struct endentry_packet packet;

			// Build filename
			func_callback(EXTCMD_GET_SOURCE,IPCDATA(ipc),filename);
			AddPart(filename,entry->fe_name,256);

			// Fill out packet to end entry
			packet.entry=entry;
			packet.deselect=1;
			func_callback(EXTCMD_END_ENTRY,IPCDATA(ipc),&packet);
		}
	}

	// Unlock source lister
	if (info.gc_UnlockSource)
		info.gc_UnlockSource(IPCDATA(ipc));

	// Save theme?
	if (mod_id==SAVETHEME || mod_id==BUILDTHEME)
	{
		char buf[256];

		// Get themes path
		if (info.gc_GetThemes)
			info.gc_GetThemes(buf);
		else
			strcpy(buf,"G_THEMES:");

		// Get filename
		if (filename[0] ||
			info.gc_FileRequest(
				(struct Window *)screen,
				GetString(locale,(mod_id==SAVETHEME)?MSG_SAVE_THEME_MSG:MSG_BUILD_THEME_MSG),
				buf,
				filename,
				(1<<30)|(1<<31)|FRF_DOSAVEMODE,0))
		{
			long res;
			char *ptr;

			// Check .theme suffix
			if (strlen(filename)<7 || stricmp(filename+strlen(filename)-6,".theme")!=0)
				strcat(filename,".theme");

			// Remove spaces
			for (ptr=FilePart(filename);*ptr;ptr++)
				if (*ptr==' ') *ptr='_';

			// Save theme
			if (res=save_theme(screen,&info,filename,(mod_id==BUILDTHEME)?TRUE:FALSE))
			{
				// Build error
				lsprintf(filename,GetString(locale,MSG_SAVE_ERROR),res);

				// Show error
				AsyncRequestTags(
					ipc,
					REQTYPE_SIMPLE,
					0,
					0,
					0,
					AR_Screen,screen,
					AR_Message,filename,
					AR_Button,GetString(locale,MSG_OK),
					TAG_END);
			}
		}
	}

	// Load theme
	else
	if (mod_id==LOADTHEME)
	{
		ULONG apply_flags=0;

		// Flags supplied?
		if (args)
		{
			short num;
			for (num=0;num<4;num++)
			{
				if (args->FA_Arguments[num+1])
					apply_flags|=1<<num;
			}
		}

		// No name supplied?
		if (!filename[0])
		{
			BPTR lock;
			Att_List *list;
			Att_Node *node;
			char *apply_switches[5];
			char temp[20];
			char path[256];
			long err;

			// Get old apply flags
			if (!apply_flags && (GetVar("Galileo/taf",temp,16,GVF_GLOBAL_ONLY))>0)
				apply_flags=atoi(temp);

			// Initial theme path
			strcpy(path,"G_THEMES:");
			if (lock=Lock(path,ACCESS_READ))
			{
				NameFromLock(lock,path,256);
				UnLock(lock);
			}
			
			// Get list of themes
			while (list=theme_build_list(path))
			{
				// Switches for what to apply
				apply_switches[0]=GetString(locale,MSG_THEME_APPLY_PALETTE);
				apply_switches[1]=GetString(locale,MSG_THEME_APPLY_FONTS);
				apply_switches[2]=GetString(locale,MSG_THEME_APPLY_PICTURES);
				apply_switches[3]=GetString(locale,MSG_THEME_APPLY_SOUNDS);
				apply_switches[4]=0;

				// Show selection list
				strcpy(filename,path);
				if ((err=SelectionList(
							list,
							0,
							screen,
							GetString(locale,MSG_LOAD_THEME_MSG),
							-1,
							SLF_DIR_FIELD|SLF_USE_INITIAL|SLF_SWITCHES|SLF_RETURN_PATH,
							filename,
							GetString(locale,MSG_OK),
							GetString(locale,MSG_CANCEL),
							apply_switches,
							&apply_flags))>-1)
				{
					// Name selected?
					if (node=Att_FindNode(list,err))
					{
						char *ptr;
						for (ptr=node->node.ln_Name;*ptr;ptr++)
							if (*ptr==' ') *ptr='_';
						lsprintf(filename,"G_THEMES:%s.theme",node->node.ln_Name);
					}
					else
						filename[0]=0;

					// Save apply flags
					lsprintf(temp,"%ld",apply_flags);
					SetEnv("Galileo/taf",temp,TRUE);
				}
				else
				if (err!=-1 && filename[0]!=0 && filename[strlen(filename)-1]=='/') filename[0]=0;

				// Free list
				Att_RemList(list,0);

				// Break out?
				if (err!=-1) break;

				// Empty path returns to default
				if (!filename[0])
				{
					// Get themes path
					if (info.gc_GetThemes)
						info.gc_GetThemes(filename);
					else
						strcpy(filename,"PROGDIR:Themes/");
				}

				// Re-open with new path
				strcpy(path,filename);

				// Assign D5THEMES to the new directory
				if ((lock=Lock(filename,ACCESS_READ)) &&
					!(AssignLock("G_THEMES",lock)))
					UnLock(lock);
			}
		}

		// File to load?
		if (filename[0])
		{
			short num;
			char apply[20],*ptr,command[400],port[40];

			// Build apply flags
			for (num=0,ptr=apply;num<APPLY_LAST;num++)
			{
				if (apply_flags&(1<<num))
					*(ptr++)=apply_lookup[num];
			}
			*ptr=0;

			// Get ARexx port name
			info.gc_GetPort(port);

			// Build ARexx command to send to Galileo
			lsprintf(command,"%s %s %s",filename,port,apply);
			info.gc_SendCommand(0,command,0,COMMANDF_RUN_SCRIPT);
		}
	}

	// Convert theme
	else
	if (mod_id==CONVERTTHEME)
	{
		// Get filename
		if (filename[0] ||
			info.gc_FileRequest(
				(struct Window *)screen,
				GetString(locale,MSG_CONVERT_THEME_MSG),
				0,
				filename,
				(1<<30)|(1<<31)|(1<<29)|FRF_DOPATTERNS,
				"#?.theme"))
		{
			char dest[256],buf[256],*ptr;

			dest[0]=0;
			if ((args=ParseArgs(func_templates[mod_id],argstring)) && args->FA_Arguments[1])
			{
				strcpy(dest,(char *)args->FA_Arguments[1]);
				if (!strchr(dest,'/') && !strchr(dest,':'))
					lsprintf(dest,"G_THEMES:%s",(char *)args->FA_Arguments[0]);
			}

			// Ask for path?
			if (!dest[0])
			{
				// Get themes path
				if (info.gc_GetThemes)
					info.gc_GetThemes(buf);
				else
					strcpy(buf,"G_THEMES:");
			}	

			// Get save filename
			if (dest[0] ||
				(info.gc_FileRequest(
					(struct Window *)screen,
					GetString(locale,MSG_SAVE_THEME_MSG),
					buf,
					dest,
					(1<<30)|(1<<31)|FRF_DOSAVEMODE,0)))
			{
				short res;

				// Check .theme suffix
				if (strlen(dest)<7 || stricmp(dest+strlen(dest)-6,".theme")!=0)
					strcat(dest,".theme");

				// Remove spaces
				for (ptr=FilePart(dest);*ptr;ptr++)
					if (*ptr==' ') *ptr='_';

				// Convert the file
				if (res=convert_theme(&info,filename,dest))
				{
					// Build error
					lsprintf(filename,GetString(locale,MSG_SAVE_ERROR),res);

					// Show error
					AsyncRequestTags(
						ipc,
						REQTYPE_SIMPLE,
						0,
						0,
						0,
						AR_Screen,screen,
						AR_Message,filename,
						AR_Button,GetString(locale,MSG_OK),
						TAG_END);
				}
			}
		}
	}

	DisposeArgs(args);
	return 1;
}


// Save theme file
long save_theme(struct Screen *screen,GalileoCallbackInfo *info,char *filename,BOOL build)
{
	APTR file;
	APTR progress=0;
	char build_path[300];
	struct MsgPort *reply_port;
	struct GetPointerPkt pkt;
	Att_List *list;
	long res=0;
	short a;

	// Building theme?
	if (build)
	{
		BPTR lock;
		short len;
		long count;

		// Copy filename to get path
		strcpy(build_path,filename);
		if ((len=strlen(build_path))>6 && stricmp(build_path+len-6,".theme")==0)
			build_path[len-6]=0;
		else
			strcat(build_path,".dir");

		// Create theme directory
		if ((lock=CreateDir(build_path)) || IoErr()==ERROR_OBJECT_EXISTS)
			UnLock(lock);
		else
			return 0;

		// Get file count
		count=3;
		pkt.gpp_Type=MODPTR_SCRIPTS;
		pkt.gpp_Ptr=0;
		if (list=info->gc_GetPointer(&pkt))
		{
			count+=Att_NodeCount(list);
			info->gc_FreePointer(&pkt);
		}

		// Open progress indicator
		progress=OpenProgressWindowTags(
					PW_Screen,screen,
					PW_Title,"Galileo Themes",
					PW_Info,GetString(locale,MSG_BUILDING_THEME),
					PW_Flags,PWF_INFO|PWF_GRAPH,
					PW_FileCount,count,
					PW_FileNum,0,
					TAG_END);
	}
	else build_path[0]=0;

	// Open output file
	if (!(file=OpenBuf(filename,MODE_NEWFILE,4096)))
	{
		CloseProgressWindow(progress);	
		return 0;
	}

	// Create a reply port
	if (!(reply_port=CreateMsgPort()))
    {
        res=1;
        goto cleanup;
    }

	// Write file introduction
	write_theme_intro(file,filename);

	// Background pictures
	WriteBuf(file,	"/* Set background pictures */\n", -1);
	WriteBuf(file,	"if index( apply_flags , \"B\") ~= 0 then do\n", -1);
	WriteBuf(file,	"\tgalileo  set  background on\n", -1);
	if (!save_theme_background(file,info,"desktop",reply_port,build_path,progress) ||
		!save_theme_background(file,info,"lister",reply_port,build_path,progress) ||
		!save_theme_background(file,info,"req",reply_port,build_path,progress))
	{
		res=IoErr();
		goto cleanup;
	}
	WriteBuf(file,	"end\n\n",-1);

	// Sounds
	WriteBuf(file,	"/* Set sound events */\n", -1);
	WriteBuf(file,	"if index( apply_flags , \"S\") ~= 0 then do\n", -1);

	// Get script list (for sounds)
	pkt.gpp_Type=MODPTR_SCRIPTS;
	pkt.gpp_Ptr=0;
	if (list=info->gc_GetPointer(&pkt))
	{
		Att_Node *node;

		// Go through scripts, find sounds	
		for (node=(Att_Node *)list->list.lh_Head;node->node.ln_Succ;node=(Att_Node *)node->node.ln_Succ)
		{
			// No sound?
			if (node->att_data&(1<<1))
			{
				if (progress)
					SetProgressWindowTags(progress,PW_FileInc,1,TAG_END);
				continue;
			}

			// Save the sound
			if (!save_theme_sound(file,info,node->node.ln_Name,reply_port,build_path,progress))
				break;
		}

		// Failed?
		if (node->node.ln_Succ) res=IoErr();

		// Free list
		info->gc_FreePointer(&pkt);

		// Failed?
		if (res) goto cleanup;

	}
	WriteBuf(file,	"end\n\n",-1);

	// Fonts
	WriteBuf(file,	"/* Set fonts */\n", -1);
	WriteBuf(file,	"if index( apply_flags , \"F\") ~= 0 then do\n", -1);
	if (!save_theme_font(file,info,"screen",reply_port) ||
		!save_theme_font(file,info,"listers",reply_port) ||
		!save_theme_font(file,info,"iconsd",reply_port) ||
		!save_theme_font(file,info,"iconsw",reply_port))
	{
		res=IoErr();
		goto cleanup;
	}
	WriteBuf(file,	"end\n\n",-1);

	// Colour settings
	WriteBuf(file,	"/* Set colour settings*/\n",-1);
	WriteBuf(file,	"if index( apply_flags , \"P\") ~= 0 then do\n", -1);
	for (a=0;pen_settings[a];a++)
	{
		if (!save_theme_pens(file,info,pen_settings[a],reply_port))
			break;
	}
	if (pen_settings[a] || !(save_theme_palette(file,info,reply_port)))
	{
		// Failure
		res=IoErr();
		goto cleanup;
	}
	WriteBuf(file,	"end\n\n",-1);

	// Write file outroduction
	write_theme_outro(file);

    cleanup:
	// Close file	
	CloseBuf(file);
	CloseProgressWindow(progress);	

	// Free message port
	if (reply_port)
    {
        struct Message *msg;
        while (msg=GetMsg(reply_port))
		ReplyFreeMsg(msg);
    	DeleteMsgPort(reply_port);
    }

	return res;
}


// Write introduction
void write_theme_intro(APTR file,char *filename)
{
	char buf[400];
	lsprintf(buf,	"/* D5THEME\n\n"
					"   %s\n\n"
					"   Galileo Theme File\n"
					"*/\n\n",	FilePart(filename));
	WriteBuf(file,buf,-1);

	// Bit that gets the ARexx port
	WriteBuf(file,	"parse arg galileo_port apply_flags\n"
					"if galileo_port='' then\n"
					"\tgalileo_port='GALILEO.1'\n"
					"address value galileo_port\n\n", -1);
	WriteBuf(file,	"if apply_flags='' then\n"
					"\tapply_flags='PFBS'\n"
					"else\n"
					"\tapply_flags=upper(apply_flags)\n\n", -1);

	// Set results and failat
	WriteBuf(file,	"options results\n"
					"options failat 21\n\n", -1);
}


// Write outroduction
void write_theme_outro(APTR file)
{
	// Refresh Galileo
	WriteBuf(file,	"/* Refresh Galileo */\n",-1);
	WriteBuf(file,	"galileo refresh all\n",-1);
}


// Save background picture
BOOL save_theme_background(APTR file,GalileoCallbackInfo *info,char *type,struct MsgPort *reply_port,char *build_path,APTR progress)
{
	char buf[400],buf2[340],temp[340],*ptr;
	BOOL ret=1;
	short len;

	// Increment progress
	if (progress)
		SetProgressWindowTags(progress,PW_FileInc,1,TAG_END);

	// Get settings
	lsprintf(buf,"galileo query background %s",type);
	info->gc_RexxCommand(buf,buf2,sizeof(buf2),reply_port,0);

	// Build command
	ptr=buf2;
	rexx_parse_word(&ptr,temp,254);
	if ((len=strlen(temp))>0)
	{
		// Building theme?
		if (*build_path && temp[len-1]!='/' && !strchr(temp,'*') && !strchr(temp,'#') && !strchr(temp,'?'))
		{
			BPTR lock;
			char dest[340];

			// Try to create in screens sub-directory
			strcpy(dest,build_path);
			AddPart(dest,"Screens",340);
			if ((lock=CreateDir(dest)) || IoErr()==ERROR_OBJECT_EXISTS)
				UnLock(lock);
			else
				strcpy(dest,build_path);

			// Add filename
			AddPart(dest,FilePart(temp),340);

			// Try to copy file
			if (theme_copy_file(temp,dest))
				lsprintf(temp,"G_THEMES:%s/Screens/%s",FilePart(build_path),FilePart(dest));
			else
				ret=0;
		}
		lsprintf(buf,"\tgalileo  set  background \"\'%s\'\"",temp);
	}
	else
	{
		lsprintf(buf,"\tgalileo clear background %s",type);
		ptr=0;
	}

	// Write command to rexx script
	WriteBuf(file,buf,-1);
	if (ptr) WriteBuf(file,ptr,-1);
	WriteBuf(file,"\n",1);
	return ret;
}


// Save sound event
BOOL save_theme_sound(APTR file,GalileoCallbackInfo *info,char *type,struct MsgPort *reply_port,char *build_path,APTR progress)
{
	char buf[400],buf2[340],temp[340],*ptr;
	BOOL ret=1;
	short len;

	// Increment progress
	if (progress)
		SetProgressWindowTags(progress,PW_FileInc,1,TAG_END);

	// Get settings
	lsprintf(buf,"galileo query sound \"%s\"",type);
	info->gc_RexxCommand(buf,buf2,sizeof(buf2),reply_port,0);

	// Build command
	ptr=buf2;
	rexx_parse_word(&ptr,temp,254);
	if ((len=strlen(temp))>0)
	{
		// Building theme?
		if (*build_path && temp[len-1]!='/' && !strchr(temp,'*') && !strchr(temp,'#') && !strchr(temp,'?'))
		{
			BPTR lock;
			char dest[340];

			// Try to create in sounds sub-directory
			strcpy(dest,build_path);
			AddPart(dest,"Sounds",340);
			if ((lock=CreateDir(dest)) || IoErr()==ERROR_OBJECT_EXISTS)
				UnLock(lock);
			else
				strcpy(dest,build_path);

			// Add filename
			AddPart(dest,FilePart(temp),340);

			// Try to copy file
			if (theme_copy_file(temp,dest))
				lsprintf(temp,"G_THEMES:%s/Sounds/%s",FilePart(build_path),FilePart(dest));
			else
				ret=0;
		}
		lsprintf(buf,"\tgalileo  set  sound \"\'%s\'\" \"\'%s\'\"",type,temp);
	}
	else
	{
		lsprintf(buf,"\tgalileo clear sound \"\'%s\'\"",type);
		ptr=0;
	}

	// Write command to rexx script
	WriteBuf(file,buf,-1);
	if (ptr) WriteBuf(file,ptr,-1);
	WriteBuf(file,"\n",1);
	return ret;
}


// Save font setting
BOOL save_theme_font(APTR file,GalileoCallbackInfo *info,char *type,struct MsgPort *reply_port)
{
	char buf[200],buf2[140],temp[80],*ptr;

	// Get settings
	lsprintf(buf,"galileo query font %s",type);
	info->gc_RexxCommand(buf,buf2,sizeof(buf2),reply_port,0);

	// Put result in quotes
	ptr=buf2;
	rexx_parse_word(&ptr,temp,78);

	// Write command to rexx script
	lsprintf(buf,"\tgalileo set font %s \"\'%s\'\"",type,temp);
	WriteBuf(file,buf,-1);
	WriteBuf(file,ptr,-1);
	WriteBuf(file,"\n",1);
	return 1;
}


// Save pens setting
BOOL save_theme_pens(APTR file,GalileoCallbackInfo *info,char *type,struct MsgPort *reply_port)
{
	char buf[150],buf2[100];

	// Get settings
	lsprintf(buf,"galileo query pens %s",type);
	info->gc_RexxCommand(buf,buf2,sizeof(buf2),reply_port,0);

	// Write command to rexx script
	lsprintf(buf,"\tgalileo set pens %s %s\n",type,buf2);
	WriteBuf(file,buf,-1);
	return 1;
}


// Save palette
BOOL save_theme_palette(APTR file,GalileoCallbackInfo *info,struct MsgPort *reply_port)
{
	char buf[300],buf2[260];

	// Get settings
	info->gc_RexxCommand("galileo query palette",buf2,sizeof(buf2),reply_port,0);

	// Write command to rexx script
	lsprintf(buf,"\tgalileo set palette %s\n",buf2);
	WriteBuf(file,buf,-1);
	return 1;
}


// Skip whitespaces
void rexx_skip_space(char **command)
{
	while (*(*command) && is_space(*(*command))) ++(*command);
}

// Parse a word out of a string
short rexx_parse_word(char **ptrptr,char *buffer,short bufferlen)
{
	short len=0,quote=0;
	char *ptr;

	// Skip preceeding spaces
	rexx_skip_space(ptrptr);
	ptr=*ptrptr;

	// Clear buffer
	*buffer=0;

	// Copy until end, whitespace or buffer full
	while (*ptr)
	{
		// Quote?
		if (*ptr=='\"' && quote!=2)
		{
			if (quote==0) quote=1;
			else quote=0;
		}

		// Other quote?
		else if (*ptr=='\'' && quote!=1)
		{
			if (quote==0) quote=2;
			else quote=0;
		}

		// White space?
		else if (is_space(*ptr) && !quote)
			break;

		// Store character?
		else if (len<bufferlen-1)
			buffer[len++]=*ptr;

		// Increment pointer
		++ptr;
		++*ptrptr;
	}

	// If not on a whitespace, find next space
	while (*ptr && !(is_space(*ptr)))
	{
		++*ptrptr;
		++ptr;
	}

	// Null-terminate buffer
	buffer[len]=0;
	return len;
}


// Copy a file for the build operation
BOOL theme_copy_file(char *source,char *dest)
{
	BPTR in,out;
	struct FileInfoBlock __aligned fib;
	BOOL ret=0;

	// See if source exists
	if (in=Lock(source,ACCESS_READ))
	{
		Examine(in,&fib);	
		UnLock(in);

		// Won't try to copy a directory
		if (fib.fib_DirEntryType>0) return 1;
	}
	else return 0;

	// Open files
	if ((in=Open(source,MODE_OLDFILE)) &&
		(out=Open(dest,MODE_NEWFILE)))
	{
		char *buf;

		// Allocate buffer
		if (buf=AllocVec(8192,0))
		{
			long len;

			// Copy file
			while ((len=Read(in,buf,8192))>0)
				Write(out,buf,len);
			FreeVec(buf);
			ret=1;
		}
		Close(out);
	}
	if (in) Close(in);
	return ret;
}


// Build theme list
Att_List *theme_build_list(char *path)
{
	Att_List *list;

	// Create list
	if (list=Att_NewList(LISTF_POOL))
	{
		long err;
		char buf[280];
		struct AnchorPath __aligned anchor;

		anchor.ap_BreakBits=0;
		anchor.ap_Flags=APF_DOWILD;
		anchor.ap_Strlen=0;
		strcpy(buf,path);
		AddPart(buf,"#?.theme",280);
		err=MatchFirst(buf,&anchor);
		while (!err)
		{
			char *ptr;
			if (anchor.ap_Info.fib_DirEntryType>0)
			{
				anchor.ap_Flags&=~APF_DODIR;
				continue;
			}
			anchor.ap_Info.fib_FileName[strlen(anchor.ap_Info.fib_FileName)-6]=0;
			for (ptr=anchor.ap_Info.fib_FileName;*ptr;ptr++)
				if (*ptr=='_') *ptr=' ';
			Att_NewNode(list,anchor.ap_Info.fib_FileName,0,ADDNODE_SORT);
			err=MatchNext(&anchor);
		}
		MatchEnd(&anchor);
	}

	return list;
}


// Convert Windows95 theme
short convert_theme(GalileoCallbackInfo *info,char *source,char *dest)
{
	APTR in,file;
	BPTR lock,old;
	char buf[500],*ptr;
	char wallpaper[256];
	short stretch=0,tile=0;
	Att_Node *node=0;
	Att_List *sounds;
	short err=0;

	// Initialise
	wallpaper[0]=0;
	sounds=Att_NewList(LISTF_POOL);

	// Get source path
	strcpy(buf,source);
	if (ptr=FilePart(buf)) *ptr=0;
	if (lock=Lock(buf,ACCESS_READ))
	{
		old=CurrentDir(lock);
		source=FilePart(source);
	}

	// Open files
	if (!(in=OpenBuf(source,MODE_OLDFILE,4096)) ||
		!(file=OpenBuf(dest,MODE_NEWFILE,4096)))
	{
		err=IoErr();
		CloseBuf(in);
		if (lock) UnLock(CurrentDir(old));
		return err;
	}

	// Write file introduction
	write_theme_intro(file,dest);

	// Read theme lines
	while (ReadBufLine(in,buf,sizeof(buf)-1)>-1)
	{
		// Wallpaper?
		if (strnicmp(buf,"Wallpaper=",10)==0)
		{
			// Write wallpaper information
			convert_theme_file(buf);
			strcpy(wallpaper,buf);
		}

		// Tile/Stretch?
		else
		if (strnicmp(buf,"TileWallpaper=",14)==0)
		{
			tile=atoi(buf+14);
		}
		else
		if (strnicmp(buf,"Stretch=",7)==0)
		{
			stretch=atoi(buf+7);
		}

		// Sound intro
		else
		if (strnicmp(buf,"[AppEvents",10)==0 && sounds)
		{
			char *ptr;
			if (ptr=strchr(buf,'\r')) *ptr=0;
			node=Att_NewNode(sounds,buf,0,0);
		}

		// Sound entry
		else
		if (strnicmp(buf,"DefaultValue=",13)==0 && node)
		{
			// Save filename for later
			convert_theme_file(buf);
			if (node->att_data=(ULONG)AllocVec(strlen(buf)+1,MEMF_CLEAR))
				strcpy((char *)node->att_data,buf);
			node=0;
		}
	}

	// Write wallpaper info
	WriteBuf(file,	"/* Set background pictures */\n", -1);
	WriteBuf(file,	"if index( apply_flags , \"B\") ~= 0 then do\n", -1);
	WriteBuf(file,	"\tgalileo  set  background on\n", -1);
	if (wallpaper[0])
		lsprintf(buf,	"\tgalileo  set  background \"'%s'\" desktop %s precision exact\n",
						wallpaper,
						(stretch)?"stretch":
						(tile)?"tile":"center");
	else
		lsprintf(buf,	"\tgalileo clear background desktop\n");
	WriteBuf(file,	buf, -1);
	WriteBuf(file,	"\tgalileo clear background lister\n", -1);
	WriteBuf(file,	"\tgalileo clear background req\n", -1);
	WriteBuf(file,	"end\n\n",-1);

	// Convert sounds
	if (sounds)
	{
		short num;

		// Sounds
		WriteBuf(file,	"/* Set sound events */\n", -1);
		WriteBuf(file,	"if index( apply_flags , \"S\") ~= 0 then do\n", -1);

		// Go through sound lookup table
		for (num=0;sound_lookup[num];num+=2)
		{
			// Build full name
			lsprintf(buf,"[AppEvents\\Schemes\\Apps\\%s\\%s\\.Current]",
				(num==0)?"Explorer":".Default",
				sound_lookup[num+1]);

			// Find this sound
			node=(Att_Node *)FindNameI(&sounds->list,buf);

			// Default to no sound
			lsprintf(buf,	"\tgalileo clear sound \"'%s'\"\n",sound_lookup[num]);

			// Sound found?
			if (node)
			{
				char *name=(char *)node->att_data;
				if (name && *name)
				{
					lsprintf(buf,	"\tgalileo  set  sound \"'%s'\" \"'%s'\" 64\n",
									sound_lookup[num],
									name);
				}
			}

			// Write string
			WriteBuf(file,	buf, -1);
		}

		WriteBuf(file,	"end\n\n",-1);

		// Free sound list
		Att_RemList(sounds,REMLIST_FREEDATA);
	}

	// Write file outroduction
	write_theme_outro(file);

	// Close files
	CloseBuf(file);
	CloseBuf(in);
	if (lock) UnLock(CurrentDir(old));
	return err;
}

void convert_theme_file(char *file)
{
	char buf[512],*ptr;
	short pos=0;
	BPTR lock;

	// Extract filename
	if (ptr=strchr(file,'='))
	{
		++ptr;
		if (*ptr=='%')
		{
			while (*(++ptr)!='%' && *ptr);
			if (*ptr=='%') ++ptr;
		}
		strcpy(buf,ptr);
		strcpy(file,buf);
	}
	for (ptr=file;*ptr;ptr++)
	{
		if (*ptr=='\\') *ptr='/';
		else
		if (*ptr=='\r')
		{
			*ptr=0;
			break;
		}
	}
	if (!*file) return;
	strcpy(buf,file);
	if (ptr=FilePart(buf))
	{
		BPTR lock;
		if (lock=Lock(ptr,ACCESS_READ))
		{
			UnLock(lock);
			strcpy(file,ptr);
		}
		else
		{
			*(PathPart(buf))=0;
			strcpy(buf+256,FilePart(buf));
			AddPart(buf+256,FilePart(file),256);
			if (lock=Lock(buf+256,ACCESS_READ))
			{
				UnLock(lock);
				strcpy(file,buf+256);
			}
		}
	}

	// Get full pathname
	if (lock=Lock(file,ACCESS_READ))
	{
		NameFromLock(lock,file,256);	
		UnLock(lock);
	}
	else
	{
		// Try removing apostrophes
		for (ptr=file;*ptr;ptr++)
		{
			if (*ptr!='\'')
				file[pos++]=*ptr;
		}
		file[pos]=0;

		// Get full pathname
		if (lock=Lock(file,ACCESS_READ))
		{
			NameFromLock(lock,file,256);	
			UnLock(lock);
		}
	}

	// Fix apostrophes in filename
	strcpy(buf,file);
	pos=0;
	for (ptr=buf;*ptr;ptr++)
	{
		if ((file[pos++]=*ptr)=='\'')
			file[pos++]='\'';
	}
	file[pos]=0;
}

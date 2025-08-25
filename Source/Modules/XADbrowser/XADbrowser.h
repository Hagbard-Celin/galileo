/*
  XADbrowser.gfmmodule 1.22 - Galileo plugin to browse through XAD archives
  Copyright (C) 1999,2000 Mladen Milinkovic <mladen.milinkovic@ri.tel.hr>
  Copyright 2013 Roman Kargin <kas1e@yandex.ru>
  Copyright 2023,2025 Hagbard Celine

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

#define CATCOMP_NUMBERS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <exec/memory.h>
#include <dos/filehandler.h>

#include <proto/exec.h>
#include <proto/dos.h>

#include <clib/alib_protos.h>
#define __NOLIBBASE__
#include <proto/xadmaster.h>
#undef __NOLIBBASE__

#include "XADbrowser.strings"
#include "modules_lib.h"
#include "//Program/lister_ipc.h"
#include "module.h"

#define XADOPEN_ARGS  "ARCHIVE,NEWLISTER/S"
#define XADEXTRACT_ARGS "ARCHIVE/M,TO/K"

#define IPC_CMD_PORT(lister) ((*(IPCData **)lister)->command_port)

extern struct DosLibrary *DOSBase;
extern struct Library *GalileoFMBase;
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;
extern struct Library *IconBase;
extern struct LocaleBase *LocaleBase;
extern struct Library *UtilityBase;
extern struct Library *LayersBase;
extern struct Library *WorkbenchBase;
extern struct Library *GadToolsBase;
extern struct Library *AslBase;
extern struct Library *DiskfontBase;
extern struct Device *TimerBase;
extern struct RxsLib *RexxSysBase;

extern struct GalileoLocale *locale;
void init_locale_data(struct GalileoLocale *);

#ifdef RESOURCE_TRACKING
extern ULONG callerid;
#endif

#define XBF_SKIP_ALL	    (1<<1)
#define XBF_OVERWRITE_ALL   (1<<2)

struct SmallFileInfoBlock
{
   LONG	  sfib_DiskKey;
   LONG	  sfib_DirEntryType;

   TEXT	  sfib_FileName[108];
   LONG	  sfib_Protection;
   LONG	  sfib_EntryType;
   LONG	  sfib_Size;
   LONG	  sfib_NumBlocks;
   struct DateStamp sfib_Date;
   TEXT	  sfib_Comment[80];
   UWORD  sfib_OwnerUID;
   UWORD  sfib_OwnerGID;
};


struct Tree
{
	struct SmallFileInfoBlock fib;
	struct xadFileInfo *xfi;
	APTR entry;
	ULONG FileCount;
	struct Tree *Next, *Child, *Parent;
};


// Structures used with callback commands
struct function_entry
{
	ULONG			pad[2];
	char			*name;	// File name
	APTR			entry;	// Entry pointer (don't touch!)
	short			type;	// Type of file
	short			flags;	// File flags
};

struct XADbrowserData
{
    IPCData 			    *ipc;
    ULONG			    mod_id;
    APTR			    memhandle;
    APTR			    treehandle;
    struct Tree 		    *root;
    struct Tree 		    *cur;
    BOOL			    newlister;
    BOOL			    All;
    ULONG 			    count;
    BOOL			    over;
    BOOL			    retry;
    struct xadArchiveInfo 	    *ArcInf;
    ULONG			    ArcMode;
    STRPTR			    arc_filename;
    STRPTR			    root_path;
    STRPTR			    path;
    PathNode 			    *source_node;
    PathNode 			    *dest_node;
    BPTR			    sourcedir_lock;
    BPTR			    destdir_lock;
    APTR			    source_lister;
    APTR			    dest_lister;
    IPCData			    *source_ipc;
    IPCData			    *dest_ipc;
    struct Window		    *source_window;
    struct Screen 		    *screen;
    APTR 			    progress;
    struct xadMasterBase	    *xadMasterBase;
    CONST GalileoCallbackInfo	    *gci;
    TEXT			    password[512];
    struct MsgPort		    *mp;
    struct MinList 		    Temp;
    TEXT			    cpy_filename[108];
    TEXT			    buf[160];
    char			    mp_name[24];
};

struct TempFile
{
	struct MinNode node;
	char FileName[110];
};

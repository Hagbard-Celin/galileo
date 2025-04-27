/*
  XADbrowser.gfmmodule 1.22 - Galileo plugin to browse through XAD archives
  Copyright (C) 1999,2000 Mladen Milinkovic <mladen.milinkovic@ri.tel.hr>
  Copyright 2013 Roman Kargin <kas1e@yandex.ru>
  Copyright 2023 Hagbard Celine

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
#include "XADbrowser.strings"
//#define _GALILEOFM_MODULE_DEF
//#include <galileofm/modules.h>
#include "modules_lib.h"

// FIXME: This makes a mix of internal and external interfaces.
#include <galileofm/hooks.h>

#define XADOPEN_ARGS  "ARCHIVE,NEWLISTER/S"
#define XADEXTRACT_ARGS "ARCHIVE/M,TO/K"

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
extern struct xadMasterBase *xadMasterBase;

extern struct GalileoLocale *locale;
void init_locale_data(struct GalileoLocale *);
//extern ModuleInfo module_info;
//extern ModuleFunction module_func;

#ifdef RESOURCE_TRACKING
extern ULONG callerid;
#endif


struct DirDate
{
    struct MinNode      node;
    struct DateStamp    date;
    char                name[1];
};

struct FList
{
	struct List list;
	APTR entry;
   UWORD dirs, files;
	struct FList *Parent,*Next;
};

struct Tree
{
	struct FileInfoBlock fib;
	struct xadFileInfo *xfi;
	APTR entry;
	struct Tree *Next, *Child;
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

struct ModuleData
{
	ULONG	ArcMode, listh, listh2, desth;
	char lists[20], listpath[512], rootpath[108], orgpath[512];
	struct xadArchiveInfo *ArcInf;
	struct MsgPort *mp;
	char mp_name[20];
	IPCData *ipc;
	struct Screen *screen;
	struct Tree *root, *cur;
	APTR memhandle;
	char *arcname, *buf;
	struct Window *listw;
	struct GalileoLocale *locale;
	struct Library *GalileoFMBase;
	struct DosLibrary *DOSBase;
    struct Library *UtilityBase;
    struct xadMasterBase *xadMasterBase;
	struct _PathNode *listp2, *destp;
	APTR ptr;
	GalileoCallbackInfo hook;
	struct MinList Temp, DirDates;
	struct _PathNode *listp;
	BOOL All, over, newlister;
    FuncArgs		*args;
    //struct _dirdate *dirdatep;
    char *password;
//    char password[512];
};

struct TempFile
{
	struct MinNode node;
	char FileName[110];
};

struct MyPacket
{
	struct Message sp_Msg;
	struct DosPacket sp_Pkt;
	char *a7, *a8;
};


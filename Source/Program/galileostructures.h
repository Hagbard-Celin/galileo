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

#ifndef GALILEOFM_STRUCTURES
#define GALILEOFM_STRUCTURES

#define OLD_CONFIG_VERSION 9999
#define CONFIG_VERSION 10000
#define CONFIG_MAGIC 0xFACE

#define MENU_NUM			5					// Number of custom menus
#define ITEM_NUM			20					// Number of items per menu
#define MENUCOUNT			MENU_NUM*ITEM_NUM	// Total number of custom menus
#define GADCOUNT			84					// Number of buttons per bank
#define DRIVECOUNT			32					// Number of custom drives
#define NUMFONTS			16					// Number of fonts
#define FILETYPE_FUNCNUM	16					// Number of filetype functions

struct Help {
	struct Help *next;
	char *lookup,*message;
};

// used ????????
typedef struct galileofuncpar {
	ULONG	flags;	// Flags
	ULONG	stack;	// Stack size
	UBYTE	key;	// Key
	UBYTE	qual;	// Qualifier
	BYTE	type;	// Function type
	BYTE	pri;	// Priority
	BYTE	delay;	// Close delay
} FuncPars;

struct galileofmfunction {
	char name[16];
	int which,stack;
	unsigned char key,qual;
	char type,pri,delay;
	char fpen,bpen;
	char pad;
	char *function;
};

typedef struct newgalileofmfunction {
	BYTE	*name;
	ULONG	pad2[3];
	ULONG	flags;
	ULONG	stack;
	UBYTE	key,qual;
	UBYTE	type;
	BYTE	pri;
	BYTE	delay;
	UBYTE	fpen,bpen;
	BYTE	pad;
	BYTE	*function;
} old_NewFunction;

typedef struct galileofiletype {
	struct galileofiletype *next;
	char type[32];
	char typeid[8];
	char actionstring[FILETYPE_FUNCNUM][40];
	ULONG flags[FILETYPE_FUNCNUM],stack[FILETYPE_FUNCNUM];
	char pri[FILETYPE_FUNCNUM],delay[FILETYPE_FUNCNUM];
	unsigned char *recognition;
	char *function[FILETYPE_FUNCNUM];
	char *iconpath;
} Filetype;

typedef struct galileogadgetbanks {
	old_NewFunction gadgets[GADCOUNT];
	struct galileogadgetbanks *next;
	struct galileogadgetbanks *prev;
} GadgetBank;

typedef struct galileohotkey {
	struct galileohotkey *next;
	UWORD code,qualifier;
	char name[40];
	old_NewFunction func;
} Hotkey;

struct Config {
	USHORT version;
	USHORT magic;

	char copyflags,deleteflags,errorflags,generalflags,iconflags;
	char existflags,_invalid_sepflags,_invalid_sortflags,dynamicflags;
	char _invalid_sortmethod[2];

	char hotkeyflags;

	char menutit[5][16];
	struct newgalileofmfunction menu[MENUCOUNT];

	struct galileofmfunction drive[DRIVECOUNT];

	char outputcmd[80],output[80];
	int gadgetrows;

	char separatemethod[2];

	char language[30];

	char _invalid_displaypos[2][16];
	char _invalid_displaylength[2][16];

	char pubscreen_name[80];

	USHORT Palette[16];
	char gadgettopcol,gadgetbotcol;
	char statusfg,statusbg;
	char _invalid_filesfg,_invalid_filesbg,_invalid_filesselfg,_invalid_filesselbg;
	char _invalid_dirsfg,_invalid_dirsbg,_invalid_dirsselfg,_invalid_dirsselbg;
	char clockfg,clockbg;
	char requestfg,requestbg;
	char disknamefg,disknamebg,disknameselfg,disknameselbg;
	char slidercol,arrowfg,arrowbg,littlegadfg,littlegadbg;

	char drive_position;

	char scrdepth;
	char screenflags;
	int screenmode;
	int scrw,scrh;
	char fontbuf[40];
	char arrowpos[3];

	char pad4;

	char startupscript[80];
	char dirflags;
	unsigned char bufcount;

	char pad5[2];

	char autodirs[2][30];
	char pad5a[80];
	UWORD hotkeycode,hotkeyqual;

	char toolicon[80],projecticon[80],drawericon[80],defaulttool[80];
	char priority;
	char showdelay,viewbits,fadetime,tabsize;

	char pad7[2];

	char hiddenbit;
	char _invalid_showpat[40],_invalid_hidepat[40];
	char _invalid_showpatparsed[40],_invalid_hidepatparsed[40];
	char icontype,scrclktype,_invalid_showfree;

	char pad8;

	short iconx,icony;
	short wbwinx,wbwiny;

	char configreturnscript[80];

	char fontsizes[NUMFONTS];
	char fontbufs[NUMFONTS][40];

	char uniconscript[80];
	char sliderbgcol;

	char pad_foo;

	short scr_winx,scr_winy;
	short scr_winw,scr_winh;

	char morepadding[231];

	char old_displaypos[2][8];
	char dateformat,addiconflags;
	char stringfgcol,stringbgcol;
	char namelength[2];
	char sliderwidth,sliderheight;
	char formatflags;
	short iconbutx,iconbuty;
	char stringheight;
	char stringselfgcol,stringselbgcol;
	char generalscreenflags;

	struct Rectangle scrollborders[2];

	char old_displaylength[2][8];

	char shellstartup[30];

	char windowdelta;

	char pad9a[397];

	int loadexternal;

	ULONG new_palette[48];

	char arrowsize[3];

	char slider_pos;

	short config_x;
	short config_y;

	char pad10[1414];
};

// used ??????
struct galileotaskmsg {
	struct Message msg;
	int command;
	int total,value;
	int flag;
	char *data;
};

struct RLEinfo {
	unsigned char *sourceptr;
	UBYTE **destplanes;
	USHORT imagebpr,imageheight,imagedepth;
	USHORT destbpr,destheight,destdepth;
	char masking,compression;
	int offset;
};

#include "galileomessage.h"

#endif

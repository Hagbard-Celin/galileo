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

#include "galileofm.h"

// SAS Detach information
long __stack=STACK_LARGE;
long __priority=0;
long __BackGroundIO=0;
char *__procname="galileo";

// Library bases
struct Library			*GalileoFMBase;
struct IntuitionBase	*IntuitionBase=0;
struct GfxBase			*GfxBase=0;
struct Library			*IconBase=0;
struct Library			*DiskfontBase=0;
struct Library			*LayersBase=0;
struct Library			*WorkbenchBase=0;
struct Library			*CxBase=0;
struct RxsLib			*RexxSysBase=0;
struct Library			*UtilityBase=0;
struct Library			*GadToolsBase=0;
struct Library			*AslBase=0;
struct Library			*DataTypesBase=0;
struct Device			*InputBase=0;
struct Device			*TimerBase=0;
struct Library			*ConsoleDevice=0;
struct Library			*AmigaGuideBase=0;
//struct Library		  *CyberGfxBase=0;
struct Library 			*P96Base=0;
struct NewIconBase		*NewIconBase=0;
struct muBase			*muBase=0;
struct Library 			*ResTrackBase=0;

struct Process			*main_proc;				// Main process pointer
IPCData					main_ipc={0};			// Main IPC data
APTR					global_memory_pool=0;	// Global memory pool for anything to use
char					*str_space_string;		// A string of spaces
struct GalileoLocale		locale;					// Locale data
APTR					main_status=0;			// Main status window
Cfg_Environment			*environment=0;			// Environment data
GUI_Glue				*GUI=0;					// GUI data
BPTR					old_current_dir=0;		// Old current directory
BOOL                    rem_galileo_assign=0;   // Remove assigns...
BOOL                    rem_g_themes_assign=0;  // ...if we made them.
struct IOStdReq			input_req;
struct IOStdReq			timer_req;
struct IOStdReq			console_req;

struct GalileoSemaphore	pub_semaphore;

long main_lister_count=0;

// Internal function definitions
Cfg_Function	*def_function_leaveout=0;
Cfg_Function	*def_function_iconinfo=0;
Cfg_Function	*def_function_format=0;
Cfg_Function	*def_function_diskcopy=0;
Cfg_Function	*def_function_select=0;
Cfg_Function	*def_function_diskinfo=0;
Cfg_Function	*def_function_devicelist=0;
Cfg_Function	*def_function_cachelist=0;
Cfg_Function	*def_function_all=0;
Cfg_Function	*def_function_rename=0;
Cfg_Function	*def_function_delete=0;
Cfg_Function	*def_function_delete_quiet=0;
Cfg_Function	*def_function_makedir=0;
Cfg_Function	*def_function_copy=0;
Cfg_Function	*def_function_move=0;
Cfg_Function	*def_function_assign=0;
Cfg_Function	*def_function_configure=0;
Cfg_Function	*def_function_cli=0;
Cfg_Function	*def_function_devicelist_full=0;
Cfg_Function	*def_function_devicelist_brief=0;
Cfg_Function	*def_function_loadtheme=0;
Cfg_Function	*def_function_savetheme=0;
Cfg_Function	*def_function_buildtheme=0;
Cfg_Filetype	*default_filetype=0;
Cfg_Filetype	*run_filetype=0;
Cfg_Filetype	*command_filetype=0;

char *config_name="galileoconfig.gfmmodule";
char *galileo_name="Galileo";

char *about_1="© 1998 Jonathan Potter & GPSoftware";
char *about_2="© 1999-2000 Mladen Milinkovic, © 2012-2013 Roman Kargin";
char *about_3="© 2013-2014 Szilard Biro, © 2020 Mathias Heyer";
char *about_4="© 2023-2024 Hagbard Celine";

char *string_no_owner;
char *string_no_group;
char *string_empty;

// Shortcut arrow
USHORT __chip
	arrow_hi_data[]={
		0xffe0,0x8020,0x8fa0,0x87a0,0x8fa0,0x9ea0,0xbc20,0xb820,
		0xb820,0x8020,0xffe0,
		0x0000,0x7fc0,0x7040,0x7840,0x7040,0x6140,0x43c0,0x47c0,
		0x47c0,0x7fc0,0x0000},

	arrow_lo_data[]={
		0xfff8,0x8008,0x8fe8,0x87e8,0x8fe8,0x9f68,0xbe08,0xbc08,
		0xbc08,0x8008,0xfff8,
		0x0000,0x7ff0,0x7010,0x7810,0x7010,0x6090,0x41f0,0x43f0,
		0x43f0,0x7ff0,0x0000};

struct Image
	arrow_image[2]={
		{0,0,11,11,2,arrow_hi_data,3,0,0},
		{0,0,13,11,2,arrow_lo_data,3,0,0}};

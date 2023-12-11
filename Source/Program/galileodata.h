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

#ifndef GALILEOFM_DATA
#define GALILEOFM_DATA

// Library bases
extern struct ExecBase			*SysBase;
extern struct Library			*GalileoFMBase;
extern struct IntuitionBase		*IntuitionBase;
extern struct GfxBase			*GfxBase;
extern struct Library			*IconBase;
extern struct Library			*DiskfontBase;
extern struct Library			*LayersBase;
extern struct Library			*WorkbenchBase;
extern struct Library			*CxBase;
extern struct RxsLib			*RexxSysBase;
extern struct Library			*UtilityBase;
extern struct Library			*GadToolsBase;
extern struct Library			*IFFParseBase;
extern struct Library			*AslBase;
extern struct Library			*DataTypesBase;
extern struct Device			*InputBase;
extern struct Device			*TimerBase;
extern struct Library			*AmigaGuideBase;
//extern struct Library			  *CyberGfxBase;
extern struct Library 			*P96Base;
extern struct NewIconBase		*NewIconBase;
extern struct muBase			*muBase;
extern struct Library           *ResTrackBase;

extern struct Process			*main_proc;			// Main process pointer
extern struct MsgPort			*arexx_port;			// ARexx port
extern IPCData					main_ipc;			// Main IPC data
extern APTR						global_memory_pool;		// Global memory pool for anything to use
extern char						*str_space_string;		// A string of spaces
extern struct GalileoLocale		locale;				// Locale data
extern APTR						main_status;			// Main status window
extern Cfg_Environment			*environment;			// Environment data
extern GUI_Glue					*GUI;				// GUI data
extern BPTR						old_current_dir;		// Old current directory
extern BOOL                     rem_galileo_assign;     // Remove assigns...
extern BOOL                     rem_g_themes_assign;    // ...if we made them.
extern struct IOStdReq			input_req;
extern struct IOStdReq			timer_req;
extern struct IOStdReq			console_req;

extern struct GalileoSemaphore	pub_semaphore;

extern long						main_lister_count;

// Internal function definitions
extern Cfg_Function	*def_function_leaveout;
extern Cfg_Function	*def_function_iconinfo;
extern Cfg_Function	*def_function_format;
extern Cfg_Function	*def_function_diskcopy;
extern Cfg_Function	*def_function_select;
extern Cfg_Function	*def_function_diskinfo;
extern Cfg_Function	*def_function_devicelist;
extern Cfg_Function	*def_function_devicelist_full;
extern Cfg_Function	*def_function_devicelist_brief;
extern Cfg_Function	*def_function_cachelist;
extern Cfg_Function	*def_function_all;
extern Cfg_Function	*def_function_rename;
extern Cfg_Function	*def_function_delete;
extern Cfg_Function	*def_function_delete_quiet;
extern Cfg_Function	*def_function_makedir;
extern Cfg_Function	*def_function_copy;
extern Cfg_Function	*def_function_move;
extern Cfg_Function	*def_function_assign;
extern Cfg_Function	*def_function_configure;
extern Cfg_Function	*def_function_cli;
extern Cfg_Function	*def_function_loadtheme;
extern Cfg_Function	*def_function_savetheme;
extern Cfg_Function	*def_function_buildtheme;
extern Cfg_Filetype	*default_filetype;
extern Cfg_Filetype	*run_filetype;
extern Cfg_Filetype	*command_filetype;

extern char *config_name;
extern char *galileo_name;

extern char *string_no_owner;
extern char *string_no_group;
extern char *string_empty;

extern struct Image arrow_image[2];

#endif

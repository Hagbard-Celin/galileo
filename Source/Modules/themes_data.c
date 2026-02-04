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

#include <exec/types.h>
#include "module_func_flags.h"
#include "moduleinfo.h"
#include "themes_data.h"

#define ARG_1	"AS=TO"
#define ARG_2	"FILE=FROM,APPLYPALETTE=AP/S,APPLYFONTS=AF/S,APPLYBACKGROUNDS=AB/S,APPLYSOUNDS=AS/S"
#define ARG_3	"FILE=FROM,AS=TO"

char *func_templates[]={ARG_1,ARG_2,ARG_1,ARG_3};


ModuleInfo
	module_info={
		1,
		"themes.gfmmodule",
		"themes.catalog",
		0,
		4,
		{SAVETHEME,"SaveTheme",MSG_SAVE_THEME_DESC,0,ARG_1}};

ModuleFunction
	more_functions[]={
		{LOADTHEME,"LoadTheme",MSG_LOAD_THEME_DESC,FUNCF_WANT_ENTRIES,ARG_2},
		{BUILDTHEME,"BuildTheme",MSG_BUILD_THEME_DESC,0,ARG_1},
		{CONVERTTHEME,"ConvertTheme",MSG_CONVERT_THEME_DESC,0,ARG_3}};

char
	*pen_settings[]={
		"icons",
		"files",
		"dirs",
		"selfiles",
		"seldirs",
		"devices",
		"assigns",
		"source",
		"dest",
		"gauge",
		"user",
		0};

char apply_lookup[]={'p','f','b','s'};

char
	*sound_lookup[]={
		"Bad disk inserted",	"EmptyRecycleBin",
		"Startup",				"SystemStart",
		"Shutdown",				"SystemExit",
		"Hide",					"SystemExclamation",
		"Reveal",				".Default",
		"Disk inserted",		"SystemQuestion",
		"Disk removed",			"SystemDefault",
		"Open lister",			"Maximize",
		"Open buttons",			"MenuPopup",
		"Open group",			"MenuCommand",
		"Close lister",			"Minimize",
		"Close buttons",		"SystemAsterisk",
		"Close group",			"SystemHand",
		"FTP close connection",	"Close",
		"FTP connect fail",		"RestoreDown",
		"FTP connect success",	"RestoreUp",
		"FTP copy fail",		"AppGPFault",
		"FTP copy success",		".Default",
		"FTP error",			"SystemAsterisk",
		0,0};

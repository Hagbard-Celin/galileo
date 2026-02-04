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

#ifndef _GALILEOFM_CONFIGWIN_H
#include <gfm/configwin.h>
#endif
#ifndef _GFM_SUBOPTIONHANDLE_H
#include <gfm/suboptionhandle.h>
#endif

extern ConfigWindow
	_config_settings_window,
	_config_environment_window,
	_config_buttons_window,
	_config_buttons_clip_window,
	_config_buttons_visual_window,
	_config_convert_window,
	_config_filetypes_window,
	_filetype_editor_window,
	_fileclass_editor_window;

extern SubOptionHandle
	_settings_options[],
	_environment_options[];

extern ObjectDef
	_config_settings_objects[],
	_config_environment_objects[],
	_config_buttons_objects[],
	_config_buttons_clip_objects[],
	_config_buttons_visual_objects[],
	_config_convert_objects[],
	_config_filetypes_objects[],
	_filetype_editor_objects[],
	_fileclass_editor_objects[];

/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 1993-2012 Jonathan Potter & GP Software
Copyright 2024,2025 Hagbard Celine

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

#include "galileofmlib.h"

struct DosLibrary	*DOSBase;
struct ExecBase		*SysBase;

struct IntuitionBase	*IntuitionBase;
struct GfxBase		*GfxBase;

struct Library		*GadToolsBase;
struct Library		*AslBase;
struct Library		*UtilityBase;
struct LocaleBase	*LocaleBase;
struct Library		*IFFParseBase;
struct Library		*LayersBase;
struct Library		*CxBase;
struct Library		*KeymapBase;
struct Library		*IconBase;
struct Library		*WorkbenchBase;
struct Library		*DataTypesBase;
struct Library		*P96Base;
struct NewIconBase	*NewIconBase;
struct RxsLib		*RexxSysBase;
#ifdef RESOURCE_TRACKING
struct Library		*ResTrackBase;
#endif
struct Library __far	*MyLibBase;

struct TextFont		*topaz_font;

struct List		image_list;
struct SignalSemaphore	image_lock;
APTR			image_memory;

Class
	*listview_class,*image_class,*button_class,*palette_class,*string_class,
	*check_class,*view_class,*frame_class,*gauge_class,*propgadget_class,
	*pathg_class,*path_class;

struct LibData gfmlib_data;

char	decimal_point='.';

IPCData	*launcher_ipc;

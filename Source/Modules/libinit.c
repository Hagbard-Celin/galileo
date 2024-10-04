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

#include "modules_lib.h"

#define LIB_VER	0

int __saveds __UserLibInit(void);
void __saveds __UserLibCleanup(void);

struct DosLibrary *DOSBase;
struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;

struct Library *GalileoFMBase;
struct Library *IconBase;
struct LocaleBase *LocaleBase;
struct Library *UtilityBase;
struct Library *LayersBase;
struct Library *WorkbenchBase;
struct Library *GadToolsBase;
struct Library *AslBase;
struct Library *DiskfontBase;
struct Device  *TimerBase;
struct RxsLib  *RexxSysBase;
struct GalileoLocale *locale;

#ifdef RESOURCE_TRACKING
struct Library *ResTrackBase;

ULONG callerid;
#endif

//extern struct ModuleData data;

// Initialise libraries we need
__saveds __UserLibInit()
{

#ifdef RESOURCE_TRACKING
	callerid=(ULONG)&__UserLibInit;

	if (ResTrackBase=REALS_OpenLibrary("g_restrack.library",0))
	{
		StartResourceTracking (RTL_ALL);
	}
#endif

	// Initialise
	GalileoFMBase=0;
	IntuitionBase=0;
	GfxBase=0;
	IconBase=0;
	LocaleBase=0;
	UtilityBase=0;
	LayersBase=0;
	WorkbenchBase=0;
	GadToolsBase=0;
	DiskfontBase=0;
	AslBase=0;
	RexxSysBase=0;
	locale=0;

	// DOS
	DOSBase=(struct DosLibrary *)OpenLibrary("dos.library",0);

	// Other libraries we want
	if (!(GalileoFMBase=OpenLibrary("galileofm.library",LIB_VER)) ||
		!(IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",37)) ||
		!(GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",37)) ||
		!(IconBase=OpenLibrary("icon.library",37)) ||
		!(LayersBase=OpenLibrary("layers.library",37)) ||
		!(GadToolsBase=OpenLibrary("gadtools.library",37)) ||
		!(AslBase=OpenLibrary("asl.library",37)) ||
		!(DiskfontBase=OpenLibrary("diskfont.library",37)) ||
		!(TimerBase=(struct Device *)GetTimerBase()) ||
		!(UtilityBase=OpenLibrary("utility.library",37))) return 1;

	// Unnecessary libraries
	WorkbenchBase=OpenLibrary("workbench.library",0);
	RexxSysBase=(struct RxsLib *)OpenLibrary("rexxsyslib.library",0);
  
	// Allocate and open locale data
	if (!(locale=AllocVec(sizeof(struct GalileoLocale),MEMF_CLEAR)))
		return 1;
	init_locale_data(locale);

	if (LocaleBase=(struct LocaleBase *)OpenLibrary("locale.library",38))
	{
		locale->li_LocaleBase=LocaleBase;
		if (module_info.locale_name)
		{
			struct TagItem tags[2];
			tags[0].ti_Tag=(module_info.flags&MODULEF_CATALOG_VERSION)?OC_Version:TAG_IGNORE;
			tags[0].ti_Data=module_info.ver;
			tags[1].ti_Tag=TAG_END;
			locale->li_Catalog=OpenCatalogA(NULL,module_info.locale_name,tags);
		}
		locale->li_Locale=OpenLocale(0);
	}

	// Succeeded
	return 0;
}


// Clean up
void __saveds __UserLibCleanup()
{
	// Free locale stuff
	if (locale)
	{
		if (LocaleBase)
		{
			CloseLocale(locale->li_Locale);
			CloseCatalog(locale->li_Catalog);
			CloseLibrary((struct Library *)LocaleBase);
		}
		FreeVec(locale);
	}

#ifdef RESOURCE_TRACKING
	KPrintF("galileofm.library opencount: %ld: ",GalileoFMBase->lib_OpenCnt);
#endif
	// Close libraries
	CloseLibrary(GalileoFMBase);
	CloseLibrary(GadToolsBase);
	CloseLibrary((struct Library *)IntuitionBase);
	CloseLibrary((struct Library *)GfxBase);
	CloseLibrary(IconBase);
	CloseLibrary(LayersBase);
	CloseLibrary(UtilityBase);
	if (WorkbenchBase) CloseLibrary(WorkbenchBase);
	CloseLibrary(DiskfontBase);
	if (RexxSysBase) CloseLibrary((struct Library *)RexxSysBase);
	CloseLibrary(AslBase);
	CloseLibrary((struct Library *)DOSBase);
 
#ifdef RESOURCE_TRACKING
	KPrintF("%s Quitting......\n", module_info.name);
	if (ResTrackBase->lib_OpenCnt==2)
	    EndResourceTracking(); /* Generate a memory usage report */

	REALS_CloseLibrary(ResTrackBase);
//    PrintTrackedResources();
	//EndResourceTracking(); /* Generate a memory usage report */
#endif

}

ModuleInfo *__asm __saveds L_Module_Identify(register __d0 int num)
{
	// Return module information
	if (num==-1) return &module_info;

	// Valid function number?
	if (num>module_info.function_count || !(module_info.function[num].desc)) return 0;

	// Return function description
	return (ModuleInfo *)GetString(locale,module_info.function[num].desc);
}

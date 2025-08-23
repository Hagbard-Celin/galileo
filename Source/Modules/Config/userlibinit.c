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

#include "config_lib.h"

#define LIB_VER 0

int __asm __saveds __UserLibInit(register __a6 struct Library *libbase);
void __asm __saveds __UserLibCleanup(void);
char *_ProgramName="galileoconfig.gfmmodule";

char *version="$VER: galileoconfig.gfmmodule 0.2 "__AMIGADATE__" ";

struct DosLibrary *DOSBase;
struct Library *GalileoFMBase;
struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct Library *LayersBase;
struct Library *DiskfontBase;
struct Library *GadToolsBase;
struct Library *WorkbenchBase;
struct Library *IconBase;
struct Library *AslBase;
struct Library *CxBase;
struct Library *UtilityBase;
struct Library *P96Base;
struct RxsLib *RexxSysBase;
struct LocaleBase *LocaleBase;
struct GalileoLocale *locale;

struct Library __far	*MyLibBase;

#ifdef RESOURCE_TRACKING
struct Library *ResTrackBase;

ULONG callerid;
#endif


// Initialise libraries we need
__asm __saveds __UserLibInit(register __a6 struct Library *libbase)
{
#ifdef RESOURCE_TRACKING
	callerid=(ULONG)&__UserLibInit;

	if (ResTrackBase=REALS_OpenLibrary("g_restrack.library",0))
		     StartResourceTracking (RTL_ALL);
#endif
	// Initialise
	DOSBase = 0;
	GalileoFMBase = 0;
	IntuitionBase = 0;
	GfxBase = 0;
	LayersBase = 0;
	DiskfontBase = 0;
	GadToolsBase = 0;
	WorkbenchBase = 0;
	IconBase = 0;
	AslBase = 0;
	CxBase = 0;
	UtilityBase = 0;
	P96Base = 0;
	RexxSysBase = 0;
	LocaleBase = 0;
	locale = 0;

	// DOS
	DOSBase=(struct DosLibrary *)OpenLibrary("dos.library",0);

	// Other libraries we want
	if (!(GalileoFMBase=OpenLibrary("galileofm.library",LIB_VER)) ||
		!(IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",37)) ||
		!(GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",37)) ||
		!(LayersBase=OpenLibrary("layers.library",37)) ||
		!(GadToolsBase=OpenLibrary("gadtools.library",37)) ||
		!(DiskfontBase=OpenLibrary("diskfont.library",37)) ||
		!(WorkbenchBase=OpenLibrary("workbench.library",37)) ||
		!(IconBase=OpenLibrary("icon.library",37)) ||
		!(AslBase=OpenLibrary("asl.library",37)) ||
		!(CxBase=OpenLibrary("commodities.library",37)) ||
		!(UtilityBase=OpenLibrary("utility.library",37)))
	{
	    __UserLibCleanup();
	    return 1;
	}

	// Under 39?
	if (GfxBase->LibNode.lib_Version>=39)
	{
	    P96Base=OpenLibrary("Picasso96API.library",0);
	}

	RexxSysBase = (struct RxsLib *)OpenLibrary("rexxsyslib.library",0);

	MyLibBase = libbase;

	// Allocate and open locale data
	if (!(locale=AllocVec(sizeof(struct GalileoLocale),MEMF_CLEAR)))
	{
		__UserLibCleanup();
		return 1;
	}

	init_locale_data(locale);

	if (LocaleBase=(struct LocaleBase *)OpenLibrary("locale.library",38))
	{
		locale->li_LocaleBase=LocaleBase;
		locale->li_Catalog=OpenCatalogA(0,"galileoconfig.catalog",0);
	}



	// Succeeded
	return 0;
}


// Clean up
void __asm __saveds __UserLibCleanup()
{
	// Free locale stuff
	if (locale)
	{
		if (LocaleBase)
		{
			CloseCatalog(locale->li_Catalog);
			CloseLibrary((struct Library *)LocaleBase);
		}
		FreeVec(locale);
	}


	if (RexxSysBase) CloseLibrary((struct Library *)RexxSysBase);
	if (P96Base) CloseLibrary(P96Base);

	// Close libraries
	if (UtilityBase) CloseLibrary(UtilityBase);
	if (CxBase) CloseLibrary(CxBase);
	if (AslBase) CloseLibrary(AslBase);
	if (IconBase) CloseLibrary(IconBase);
	if (WorkbenchBase) CloseLibrary(WorkbenchBase);
	if (DiskfontBase) CloseLibrary(DiskfontBase);
	if (GadToolsBase) CloseLibrary(GadToolsBase);
	if (LayersBase) CloseLibrary(LayersBase);
	if (GfxBase) CloseLibrary((struct Library *)GfxBase);
	if (IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
	if (GalileoFMBase) CloseLibrary(GalileoFMBase);
	if (DOSBase) CloseLibrary((struct Library *)DOSBase);

#ifdef RESOURCE_TRACKING
	KPrintF("Config Quitting......\n");
	if (ResTrackBase->lib_OpenCnt==2)
	    EndResourceTracking(); /* Generate a memory usage report */
	REALS_CloseLibrary(ResTrackBase);
#endif

}

void __stdargs _XCEXIT(long poo) {}
void __stdargs _CXFERR(int code) {}


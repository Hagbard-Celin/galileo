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

int __saveds __UserLibInit(void);
void __saveds __UserLibCleanup(void);
char *_ProgramName="galileoconfig.gfmmodule";

char *version="$VER: galileoconfig.gfmmodule 0.1 "__AMIGADATE__" ";

struct DosLibrary *DOSBase=0;
struct Library *GalileoFMBase=0;
struct IntuitionBase *IntuitionBase=0;
struct GfxBase *GfxBase=0;
struct Library *LayersBase=0;
struct Library *DiskfontBase=0;
struct Library *GadToolsBase=0;
struct Library *WorkbenchBase=0;
struct Library *IconBase=0;
struct Library *AslBase=0;
struct Library *CxBase=0;
struct Library *UtilityBase=0;
struct Library *P96Base=0;
struct LocaleBase *LocaleBase=0;
struct GalileoLocale *locale=0;

#if RESOURCE_TRACKING
struct Library *ResTrackBase;

char *callerid;

#endif


// Initialise libraries we need
__saveds __UserLibInit()
{
#if RESOURCE_TRACKING
   callerid=_ProgramName;

   if (ResTrackBase=REALS_OpenLibrary("restrack.library",0))
        StartResourceTracking (RTL_ALL);
#endif

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
		!(UtilityBase=OpenLibrary("utility.library",37))) return 1;

    // Under 39?
	if (GfxBase->LibNode.lib_Version>=39)
	{
	    P96Base=OpenLibrary("Picasso96API.library",0);
    }

	// Allocate and open locale data
	if (!(locale=AllocVec(sizeof(struct GalileoLocale),MEMF_CLEAR)))
		return 1;
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
void __saveds __UserLibCleanup()
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

    if (P96Base)
        CloseLibrary(P96Base);

	// Close libraries
	CloseLibrary(GalileoFMBase);
	CloseLibrary((struct Library *)IntuitionBase);
	CloseLibrary((struct Library *)GfxBase);
	CloseLibrary(LayersBase);
	CloseLibrary(GadToolsBase);
	CloseLibrary(DiskfontBase);
	CloseLibrary(WorkbenchBase);
	CloseLibrary(IconBase);
	CloseLibrary(AslBase);
	CloseLibrary(CxBase);
	CloseLibrary(UtilityBase);
	CloseLibrary((struct Library *)DOSBase);

#if RESOURCE_TRACKING
    KPrintF("Config Quitting......\n");
    PrintTrackedResources();
    //EndResourceTracking(); /* Generate a memory usage report */
#endif

}

/*
libdata *init_libs(void)
{
	libdata *data;

	// Allocate library data
	if (!(data=AllocVec(sizeof(libdata),MEMF_CLEAR)))
		return 0;

	// Get libraries we need
	if (!(data->DOSBase=OpenLibrary("dos.library",0)) ||
		!(data->GalileoFMBase=OpenLibrary("galileofm.library",40)) ||
		!(data->IntuitionBase=OpenLibrary("intuition.library",37)) ||
		!(data->GfxBase=OpenLibrary("graphics.library",37)) ||
		!(data->LayersBase=OpenLibrary("layers.library",37)) ||
		!(data->GadToolsBase=OpenLibrary("gadtools.library",37)) ||
		!(data->DiskfontBase=OpenLibrary("diskfont.library",37)) ||
		!(data->WorkbenchBase=OpenLibrary("workbench.library",37)) ||
		!(data->IconBase=OpenLibrary("icon.library",37)) ||
		!(data->AslBase=OpenLibrary("asl.library",37)) ||
		!(data->CxBase=OpenLibrary("commodities.library",37)) ||
		!(data->UtilityBase=OpenLibrary("utility.library",37)))
	{
		free_libs(data);
		return 0;
	}

	// Open locale data
	init_locale_data(&data->locale);
	if (data->LocaleBase=OpenLibrary("locale.library",38))
	{
		data->locale.li_LocaleBase=LocaleBase;
		data->locale.li_Catalog=OpenCatalogA(NULL,"GalileoConfig.catalog",0);
	}

	return data;
}

void free_libs(libdata *data)
{
	if (data)
	{
		// Free locale stuff
		if (data->LocaleBase)
		{
			CloseCatalog(data->locale.li_Catalog);
			CloseLibrary(data->LocaleBase);
		}

		// Close libraries
		if (data->GalileoFMBase) CloseLibrary(data->GalileoFMBase);
		if (data->IntuitionBase) CloseLibrary(data->IntuitionBase);
		if (data->GfxBase) CloseLibrary(data->GfxBase);
		if (data->LayersBase) CloseLibrary(data->LayersBase);
		if (data->GadToolsBase) CloseLibrary(data->GadToolsBase);
		if (data->DiskfontBase) CloseLibrary(data->DiskfontBase);
		if (data->WorkbenchBase) CloseLibrary(data->WorkbenchBase);
		if (data->IconBase) CloseLibrary(data->IconBase);
		if (data->AslBase) CloseLibrary(data->AslBase);
		if (data->CxBase) CloseLibrary(data->CxBase);
		if (data->UtilityBase) CloseLibrary(data->UtilityBase);
		if (data->DOSBase) CloseLibrary(data->DOSBase);

		// Free data
		FreeVec(data);
	}
}
*/

void __stdargs _XCEXIT(long poo) {}
void __stdargs _CXFERR(int code) {}


/****************************************************************************

 modinit.c - standard initialisation for Galileo modules

 ****************************************************************************/

#define _GALILEOFM_MODULE_DEF
#include <galileofm/modules.h>

#ifndef CLIB_EXEC_PROTOS_H
#include <clib/exec_protos.h>
#include <pragmas/exec_pragmas.h>
#endif

#ifndef EXEC_MEMORY_H
#include <exec/memory.h>
#endif

#ifndef CLIB_LOCALE_PROTOS_H
#include <clib/locale_protos.h>
#include <pragmas/locale_pragmas.h>
#endif

// SAS/C stuff
int __saveds __UserLibInit(void);
void __saveds __UserLibCleanup(void);

// The libraries we open
struct DosLibrary    *DOSBase;
struct Library       *GalileoFMBase;
struct IntuitionBase *IntuitionBase;
struct GfxBase       *GfxBase;
struct Library       *IconBase;
struct LocaleBase    *LocaleBase;
struct Library       *UtilityBase;
struct Library       *LayersBase;
struct Library       *WorkbenchBase;
struct Library       *GadToolsBase;
struct Library       *AslBase;
struct Library       *DiskfontBase;
struct Device        *TimerBase;
struct RxsLib        *RexxSysBase;
#ifdef _FFP
struct Library *MathBase;
struct Library *MathTransBase;
#endif

// Locale pointer
struct GalileoLocale *locale;

// Library initialisation code
__saveds __UserLibInit()
{
	// Initialise pointers
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
#ifdef _FFP
	MathBase=0;
	MathTransBase=0;
#endif
	locale=0;

	// Get DOS library (can't really fail)
	DOSBase=(struct DosLibrary *)OpenLibrary("dos.library",0);

	// Open other libraries we need
	if (!(GalileoFMBase=OpenLibrary("galileofm.library",55)) ||
		!(IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",37)) ||
		!(GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",37)) ||
		!(IconBase=OpenLibrary("icon.library",37)) ||
		!(LayersBase=OpenLibrary("layers.library",37)) ||
		!(GadToolsBase=OpenLibrary("gadtools.library",37)) ||
		!(AslBase=OpenLibrary("asl.library",37)) ||
		!(DiskfontBase=OpenLibrary("diskfont.library",37)) ||
		!(TimerBase=(struct Device *)GetTimerBase()) ||
#ifdef _FFP
		!(MathBase=OpenLibrary("mathffp.library",37)) ||
		!(MathTransBase=OpenLibrary("mathtrans.library",37)) ||
#endif
		!(UtilityBase=OpenLibrary("utility.library",37)))
	{
	    __UserLibCleanup();
	    return 1;
	}

	// Libraries we don't need but want
	WorkbenchBase=OpenLibrary("workbench.library",0);
	RexxSysBase=(struct RxsLib *)OpenLibrary("rexxsyslib.library",0);

	// Allocate and open locale data
	if (!(locale=AllocVec(sizeof(struct GalileoLocale),MEMF_CLEAR)))
	{
	        __UserLibCleanup();
	        return 1;
	}

	init_locale_data(locale);

	// Open locale library
	if (LocaleBase=(struct LocaleBase *)OpenLibrary("locale.library",38))
	{
		// Store library pointer
		locale->li_LocaleBase=LocaleBase;

		// Open catalog if name supplied
		if (module_info.locale_name)
		{
			struct TagItem tags[2];

			// If MODULEF_CATALOG_VERSION is set, we do version checking
			tags[0].ti_Tag=(module_info.flags&MODULEF_CATALOG_VERSION)?OC_Version:TAG_IGNORE;
			tags[0].ti_Data=module_info.ver;
			tags[1].ti_Tag=TAG_END;

			// Open catalog
			locale->li_Catalog=OpenCatalogA(NULL,module_info.locale_name,tags);
		}

		// Get default lolale
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

	// Close libraries
	if (RexxSysBase) CloseLibrary((struct Library *)RexxSysBase);
	if (WorkbenchBase) CloseLibrary(WorkbenchBase);
	if (UtilityBase) CloseLibrary(UtilityBase);
#ifdef _FFP
	if (MathTransBase) CloseLibrary(MathTransBase);
	if (MathBase) CloseLibrary(MathBase);
#endif
	if (DiskfontBase) CloseLibrary(DiskfontBase);
	if (AslBase) CloseLibrary(AslBase);
	if (GadToolsBase) CloseLibrary(GadToolsBase);
	if (LayersBase) CloseLibrary(LayersBase);
	if (IconBase) CloseLibrary(IconBase);
	if (GfxBase) CloseLibrary((struct Library *)GfxBase);
	if (IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
	if (GalileoFMBase) CloseLibrary(GalileoFMBase);
	CloseLibrary((struct Library *)DOSBase);
}


// This routine is called by Galileo to find out what the module does
ModuleInfo *__asm __saveds L_Module_Identify(register __d0 int num)
{
	// Return module information
	if (num==-1) return &module_info;

	// Valid function number?
	if (num>module_info.function_count || !(module_info.function[num].desc)) return 0;

	// Return function description
	return (ModuleInfo *)GalileoGetString(locale,module_info.function[num].desc);
}

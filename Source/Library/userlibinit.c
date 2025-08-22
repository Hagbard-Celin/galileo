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
#include "boopsi.h"

int __asm __saveds __UserLibInit(register __a6 struct Library *libbase);
void __asm __saveds __UserLibCleanup(register __a6 struct Library *libbase);
__asm low_mem_handler(register __a0 struct MemHandlerData *);

char *_ProgramName="galileofm.library";

void init_locale_data(struct GalileoLocale *locale);
void free_locale_data(struct GalileoLocale *locale);
void free_libdata(void);

static const struct TextAttr topaz_attr={"topaz.font",8,0,0};

char *version="$VER: galileofm.library 0.2 "__AMIGADATE__" ";

#ifdef RESOURCE_TRACKING
ULONG callerid;

#endif

#define ExecLib		((struct ExecBase *)*((ULONG *)4))
#define SysBase		((struct ExecBase *)*((ULONG *)4))


void free_libdata(void)
{
    WB_Data *wb_data;

    // Get workbench data
    wb_data=&gfmlib_data.wb_data;

    // Remove low-memory handler
    if (gfmlib_data.low_mem_handler.is_Node.ln_Pri==50)
	    RemMemHandler(&gfmlib_data.low_mem_handler);

    // Remove patch information
    if (wb_data)
    {
	    if (wb_data->patch_count>0)
	    {
		    wb_data->patch_count=1;
		    L_WB_Remove_Patch();
	    }
    }

    // Free path list
    if (gfmlib_data.path_list) L_FreeDosPathList(gfmlib_data.path_list);

    // Free locale stuff
    free_locale_data(&gfmlib_data.locale);

    // Free memory
    if (gfmlib_data.memory) L_FreeMemHandle(gfmlib_data.memory);
    if (gfmlib_data.dos_list_memory) L_FreeMemHandle(gfmlib_data.dos_list_memory);

    // Close timer
    if (gfmlib_data.TimerBase) CloseDevice(&gfmlib_data.timer_io);

    if (listview_class) FreeClass(listview_class);
    if (path_class) FreeClass(path_class);
    if (pathg_class) FreeClass(pathg_class);
    if (propgadget_class) FreeClass(propgadget_class);
    if (button_class) FreeClass(button_class);
    if (string_class) FreeClass(string_class);
    if (check_class) FreeClass(check_class);
    if (view_class) FreeClass(view_class);
    if (frame_class) FreeClass(frame_class);
    if (palette_class) FreeClass(palette_class);
    if (gauge_class) FreeClass(gauge_class);
    if (image_class) FreeClass(image_class);
    if (topaz_font) CloseFont(topaz_font);
    if (NewIconBase) CloseLibrary((struct Library *)NewIconBase);
    if (RexxSysBase) CloseLibrary((struct Library *)RexxSysBase);
    if (P96Base) CloseLibrary(P96Base);
    if (DataTypesBase) CloseLibrary(DataTypesBase);
    if (IconBase) CloseLibrary(IconBase);
    if (KeymapBase) CloseLibrary(KeymapBase);
    if (CxBase) CloseLibrary(CxBase);
    if (LayersBase) CloseLibrary(LayersBase);
    if (UtilityBase) CloseLibrary(UtilityBase);
    if (AslBase) CloseLibrary(AslBase);
    if (GadToolsBase) CloseLibrary(GadToolsBase);
    if (WorkbenchBase) CloseLibrary(WorkbenchBase);
    if (GfxBase) CloseLibrary((struct Library *)GfxBase);
    if (IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
    if (DOSBase) CloseLibrary((struct Library *)DOSBase);

#ifdef _DEBUG
#ifdef RESOURCE_TRACKING
    KPrintF("Main Library\n");
    //PrintTrackedResources();
    if (ResTrackBase->lib_OpenCnt==2)
	EndResourceTracking(); /* Generate a memory usage report */

    REALL_CloseLibrary(ResTrackBase);

    KPrintF("Quitting......\n");
#endif
#endif

}


// Initialise libraries we need
__asm __saveds __UserLibInit(register __a6 struct Library *libbase)
{
	char buf[16];

#ifdef RESOURCE_TRACKING

	callerid=(ULONG)&__UserLibInit;

	if (ResTrackBase=REALL_OpenLibrary("g_restrack.library",0))
	{
		StartResourceTracking (RTL_ALL);
		SetMyLibBase ((struct Library *)libbase);
	}
#endif
#ifdef _DEBUG_USERLIBINIT
	KPrintF("Library GalileoFMBase: 0x%08.lx  &gfmlib_data.notify_lock: 0x%08.lx\n", libbase, &gfmlib_data.notify_lock);
#endif
#ifdef _DEBUG_IPCPROC
	KPrintF("Library ResTrackBase: 0x%08.lx at: 0x%08.lx SysBase: 0x%08.lx  GalileoFMBase: 0x%08.lx\n",ResTrackBase, &ResTrackBase, SysBase, libbase);
#endif

	// Initialise
	GfxBase=0;
	GadToolsBase=0;
	AslBase=0;
	UtilityBase=0;
	LayersBase=0;
	LocaleBase=0;
	IconBase=0;
	CxBase=0;
	KeymapBase=0;
	WorkbenchBase=0;
	topaz_font=0;
	NewList(&image_list);
	InitSemaphore(&image_lock);
	image_memory=0;
	propgadget_class=0;
	listview_class=0;
	image_class=0;
	button_class=0;
	string_class=0;
	check_class=0;
	view_class=0;
	palette_class=0;
	gauge_class=0;
	launcher_ipc=0;

	// DOS
	DOSBase=(struct DosLibrary *)OpenLibrary("dos.library",0);

	// Need 37
	if (!(IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",37)) ||
		!(GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",37)) ||
		!(GadToolsBase=OpenLibrary("gadtools.library",37)) ||
		!(AslBase=OpenLibrary("asl.library",37)) ||
		!(UtilityBase=OpenLibrary("utility.library",37)) ||
		!(LayersBase=OpenLibrary("layers.library",37)) ||
		!(CxBase=OpenLibrary("commodities.library",37)) ||
		!(KeymapBase=OpenLibrary("keymap.library",37)) ||
		!(IconBase=OpenLibrary("icon.library",37)) ||
		!(WorkbenchBase=OpenLibrary("workbench.library",37)))
	{
	    free_libdata();
	    return 1;
	}


	// Get non-necessary libraries
	DataTypesBase=OpenLibrary("datatypes.library",39);
	RexxSysBase=(struct RxsLib *)OpenLibrary("rexxsyslib.library",0);
	P96Base=OpenLibrary("Picasso96API.library",0);

	// Save our library base with in far-data to make it always available.
	MyLibBase = libbase;

	// Check for OS 3.5 icon library
	// if <44 then try for NewIcons. Don't open NewIcons under V44
	if	(IconBase->lib_Version>=44)
		gfmlib_data.flags|=LIBDF_USING_OS35;
	else
		NewIconBase=(struct NewIconBase *)OpenLibrary("newicon.library",0);


	// Open timer device
	if (OpenDevice("timer.device",UNIT_VBLANK,&gfmlib_data.timer_io,0))
		return 1;
	gfmlib_data.TimerBase=(struct Library *)gfmlib_data.timer_io.io_Device;

	// Get topaz font
	if (!(topaz_font=OpenFont(&topaz_attr)))
	{
	    free_libdata();
	    return 1;
	}

	// Initialise stuff
	gfmlib_data.low_mem_signal=-1;
	gfmlib_data.wb_data.first_app_entry=TRUE;

#ifdef RESOURCE_TRACKING
	gfmlib_data.restrack_base=ResTrackBase;
#endif

	NewList((struct List *)&gfmlib_data.wb_data.app_list);
	NewList((struct List *)&gfmlib_data.wb_data.rem_app_list);
	NewList((struct List *)&gfmlib_data.notify_list);
	NewList((struct List *)&gfmlib_data.launch_list);
	NewList((struct List *)&gfmlib_data.error_list);
	NewList(&gfmlib_data.semaphores.list);
	NewList(&gfmlib_data.device_list.list);
	NewList(&gfmlib_data.file_list.list);
	NewList(&gfmlib_data.filetype_cache.list);
	NewList(&gfmlib_data.allocbitmap_patch.list);
	NewList((struct List *)&gfmlib_data.dos_list);
	InitSemaphore(&gfmlib_data.dos_lock);
	InitSemaphore(&gfmlib_data.wb_data.patch_lock);
	InitSemaphore(&gfmlib_data.notify_lock);
	InitSemaphore(&gfmlib_data.launch_lock);
	InitSemaphore(&gfmlib_data.path_lock);
	InitSemaphore(&gfmlib_data.semaphores.lock);
	InitSemaphore(&gfmlib_data.device_list.lock);
	InitSemaphore(&gfmlib_data.file_list.lock);
	InitSemaphore(&gfmlib_data.filetype_cache.lock);
	InitSemaphore(&gfmlib_data.allocbitmap_patch.lock);
	InitSemaphore(&gfmlib_data.backfill_lock);
	gfmlib_data.popup_delay=10;

	// Initialise NewIcons settings
	gfmlib_data.NewIconsFlags=ENVNIF_ENABLE;
	gfmlib_data.NewIconsPrecision=16;

	// See if SysIHack is running
	Forbid();
	if (FindTask("« sysihack »")) gfmlib_data.flags|=LIBDF_3DLOOK;

	// Or variable is set for 3d gadgets
	else
	if (GetVar("Galileo/3DLook",buf,2,GVF_GLOBAL_ONLY)>-1)
		gfmlib_data.flags|=LIBDF_3DLOOK;
	Permit();

	// Variable set for no stippling of requesters
	if (GetVar("Galileo/OuEstLeMinibar",buf,2,GVF_GLOBAL_ONLY)>-1)
		gfmlib_data.flags|=LIBDF_NOSTIPPLE;

	// Variable set to install DOS patches
	if (GetVar("Galileo/DOSPatch",buf,2,GVF_GLOBAL_ONLY)>-1)
		gfmlib_data.flags|=LIBDF_DOS_PATCH;

	// Variable set to install filetype cache
	if (GetVar("Galileo/FiletypeCache",buf,14,GVF_GLOBAL_ONLY)>-1)
	{
		// Get cache size
		gfmlib_data.ft_cache_max=atoi(buf);
		if (gfmlib_data.ft_cache_max<10)
			gfmlib_data.ft_cache_max=10;

		// Set cache flag
		gfmlib_data.flags|=LIBDF_FT_CACHE;
	}

	// Create some memory handles
	gfmlib_data.memory=L_NewMemHandle(sizeof(IPCMessage)<<5,sizeof(IPCMessage)<<4,MEMF_CLEAR|MEMF_PUBLIC);
	gfmlib_data.dos_list_memory=L_NewMemHandle(1024,512,MEMF_CLEAR);

	// Initialise boopsi classes
	if (!(image_class=
			init_class(
				"galileoiclass",
				"imageclass",
				NULL,
				(unsigned long (*)())image_dispatchTr,
				sizeof(BoopsiImageData))) ||

		!(button_class=
			init_class(
				"galileobuttongclass",
				"gadgetclass",
				NULL,
				(unsigned long (*)())button_dispatchTr,
				sizeof(ButtonData))) ||

		!(string_class=
			init_class(
				"galileostrgclass",
				"strgclass",
				NULL,
				(unsigned long (*)())button_dispatchTr,
				sizeof(StringData))) ||

		!(check_class=
			init_class(
				"galileocheckgclass",
				"gadgetclass",
				NULL,
				(unsigned long (*)())button_dispatchTr,
				sizeof(CheckData))) ||

		!(view_class=
			init_class(
				"galileoviewgclass",
				"gadgetclass",
				NULL,
				(unsigned long (*)())button_dispatchTr,
				sizeof(ButtonData))) ||

		!(palette_class=
			init_class(
				"galileopalettegclass",
				"gadgetclass",
				NULL,
				(unsigned long (*)())palette_dispatchTr,
				sizeof(PaletteData))) ||

		!(frame_class=
			init_class(
				"galileoframeclass",
				"gadgetclass",
				NULL,
				(unsigned long (*)())button_dispatchTr,
				sizeof(ButtonData))) ||

		!(gauge_class=
			init_class(
				"galileogaugeclass",
				"gadgetclass",
				NULL,
				(unsigned long (*)())button_dispatchTr,
				sizeof(GaugeData))) ||

		!(propgadget_class=
			init_class(
				"galileopropgclass",
				"gadgetclass",
				NULL,
				(unsigned long (*)())propgadget_dispatchTr,
				sizeof(PropGadgetData))) ||

		!(pathg_class=
			init_class(
				NULL,
				"gadgetclass",
				NULL,
				(unsigned long (*)())pathgadget_dispatchTr,
				sizeof(PathGadgetData))) ||

		!(path_class=
			init_class(
				"galileopathgclass",
				NULL,
				pathg_class,
				(unsigned long (*)())button_dispatchTr,
				sizeof(PathData))) ||

		!(listview_class=
			init_class(
				"galileolistviewgclass",
				"gadgetclass",
				NULL,
				(unsigned long (*)())listview_dispatchTr,
				sizeof(ListViewData))))
	{
		free_libdata();
	        return 1;
	}

	// Set flag to identify some classes
	if (string_class) string_class->cl_UserData=CLASS_STRINGGAD;
	if (check_class) check_class->cl_UserData=CLASS_CHECKGAD;
	if (view_class) view_class->cl_UserData=CLASS_VIEW;
	if (frame_class) frame_class->cl_UserData=CLASS_FRAME;
	if (gauge_class) gauge_class->cl_UserData=CLASS_GAUGE;
	if (path_class) path_class->cl_UserData=CLASS_PATHGAD;

	// Initialise locale
	init_locale_data(&gfmlib_data.locale);

	// Did we get locale library?
	if (LocaleBase=gfmlib_data.locale.li_LocaleBase)
	{
		// Get decimal point
		if (gfmlib_data.locale.li_Locale && gfmlib_data.locale.li_Locale->loc_DecimalPoint)
				decimal_point=gfmlib_data.locale.li_Locale->loc_DecimalPoint[0];
	}

	// Get a path list
	gfmlib_data.path_list=L_GetDosPathList(0);

	// Create launcher process
	if (!(L_IPC_Launch(
		0,&launcher_ipc,
		"GALILEO_LAUNCHER",
		(ULONG)launcher_procTr,
		STACK_LARGE|IPCF_GETPATH,
		NULL)) || !launcher_ipc)
	{
	    free_libdata();
	    return 1;
	}
	gfmlib_data.launcher=launcher_ipc;

	// Initialise low-memory handler
	if (ExecLib->LibNode.lib_Version>=39)
	{
		// Initialise interrupt
		gfmlib_data.low_mem_handler.is_Node.ln_Pri=50;
		gfmlib_data.low_mem_handler.is_Node.ln_Type=NT_INTERRUPT;
		gfmlib_data.low_mem_handler.is_Node.ln_Name="galileo memhandler";
		gfmlib_data.low_mem_handler.is_Data=&gfmlib_data;
		gfmlib_data.low_mem_handler.is_Code=(void (*)())low_mem_handler;

		// Add the handler
		AddMemHandler(&gfmlib_data.low_mem_handler);
	}

	// Succeeded
	return 0;
}


// Clean up
void __asm __saveds __UserLibCleanup(register __a6 struct Library *libbase)
{
	L_FlushImages();

	// Launcher?
	if (launcher_ipc)
	{
		L_IPC_Command(launcher_ipc,IPC_QUIT,0,0,0,REPLY_NO_PORT_IPC);
	}

	free_libdata();
}


// Low memory handler
__asm low_mem_handler(register __a0 struct MemHandlerData *memh)
{
	// Is this the first time?
	if (!(memh->memh_Flags&MEMHF_RECYCLE))
	{
		// Signal the launcher to send the message
		if (gfmlib_data.low_mem_signal>-1)
			Signal((struct Task *)gfmlib_data.launcher->proc,1<<gfmlib_data.low_mem_signal);

		// Tell it to try again
		return MEM_TRY_AGAIN;
	}

	// We don't actually do anything
	return MEM_DID_NOTHING;
}

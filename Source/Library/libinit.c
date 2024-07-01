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

#include "galileofmlib.h"
#include "boopsi.h"

int __asm __saveds __UserLibInit(register __a6 struct MyLibrary *libbase);
void __asm __saveds __UserLibCleanup(register __a6 struct MyLibrary *libbase);
__asm low_mem_handler(register __a0 struct MemHandlerData *,register __a1 struct LibData *);

char *_ProgramName="galileofm.library";

void init_locale_data(struct GalileoLocale *locale);
void free_locale_data(struct GalileoLocale *locale);

static struct TextAttr topaz_attr={"topaz.font",8,0,0};

char *version="$VER: galileofm.library 0.1 "__AMIGADATE__" ";

#if RESOURCE_TRACKING
char *callerid;

#endif

#define ExecLib		((struct ExecBase *)*((ULONG *)4))
#define SysBase		((struct ExecBase *)*((ULONG *)4))


// Initialise libraries we need
__asm __saveds __UserLibInit(register __a6 struct MyLibrary *libbase)
{
	struct LibData *data;
	char buf[16];

#if RESOURCE_TRACKING

   callerid=_ProgramName;

   if (ResTrackBase=REALL_OpenLibrary("restrack.library",0))
        StartResourceTracking (RTL_ALL);
#endif

#ifdef _DEBUG_IPCPROC
    KPrintF("Library ResTrackBase: %lx at: %lx SysBase: %lx \n",ResTrackBase, &ResTrackBase, SysBase);
#endif

	// Initialise
	GfxBase=0;
	GadToolsBase=0;
	AslBase=0;
	UtilityBase=0;
	LocaleBase=0;
	IconBase=0;
	topaz_font=0;
	NewList(&image_list);
	InitSemaphore(&image_lock);
	image_memory=0;
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
		!(IconBase=OpenLibrary("icon.library",37)) ||
		!(WorkbenchBase=OpenLibrary("workbench.library",37))) return 1;


	// Get non-necessary libraries
	DataTypesBase=OpenLibrary("datatypes.library",39);
	RexxSysBase=(struct RxsLib *)OpenLibrary("rexxsyslib.library",0);
    P96Base=OpenLibrary("Picasso96API.library",0);

	// Get library data
	if (!(data=AllocVec(sizeof(struct LibData),MEMF_CLEAR)))
		return 1;

	libbase->ml_UserData=(ULONG)data;


	// Check for OS 3.5 icon library
	// if <44 then try for NewIcons. Don't open NewIcons under V44

	if	(IconBase->lib_Version>=44)
		data->flags|=LIBDF_USING_OS35;
	else
		NewIconBase=(struct NewIconBase *)OpenLibrary("newicon.library",0);


	// Open timer device
	if (OpenDevice("timer.device",UNIT_VBLANK,&data->timer_io,0))
		return 1;
	data->TimerBase=(struct Library *)data->timer_io.io_Device;

/*
	// Is CyberGfx library already in system? If so, open it for ourselves
	if (FindName(&ExecLib->LibList,"cybergraphics.library"))
		CyberGfxBase=OpenLibrary("cybergraphics.library",0);
*/
/*
    if (FindName(&ExecLib->LibList,"Picasso96API.library"))
		P96Base=OpenLibrary("Picasso96API.library",0);
*/
	// Get topaz font
	if (!(topaz_font=OpenFont(&topaz_attr))) return 1;

	// Initialise stuff
	data->low_mem_signal=-1;
    data->wb_data.first_app_entry=TRUE;
	data->wb_data.wb_base=WorkbenchBase;
	data->wb_data.int_base=(struct Library *)IntuitionBase;
	data->wb_data.galileofm_base=libbase;
	data->wb_data.utility_base=UtilityBase;
	data->dos_base=(struct Library *)DOSBase;
	data->icon_base=IconBase;
	data->new_icon_base=(struct Library *)NewIconBase;
	data->int_base=(struct Library *)IntuitionBase;
	data->galileofm_base=libbase;
	data->gfx_base=(struct Library *)GfxBase;
//    if (P96Base)
//        data->p96_base=P96Base;

#if RESOURCE_TRACKING
    data->wb_data.restrack_base=ResTrackBase;
    data->restrack_base=ResTrackBase;
#endif

	NewList((struct List *)&data->wb_data.app_list);
	NewList((struct List *)&data->wb_data.rem_app_list);
	NewList((struct List *)&data->notify_list);
	NewList((struct List *)&data->launch_list);
	NewList((struct List *)&data->error_list);
	NewList(&data->semaphores.list);
	NewList(&data->device_list.list);
	NewList(&data->file_list.list);
	NewList(&data->filetype_cache.list);
	NewList(&data->allocbitmap_patch.list);
	NewList((struct List *)&data->dos_list);
	InitSemaphore(&data->dos_lock);
	InitSemaphore(&data->wb_data.patch_lock);
	InitSemaphore(&data->notify_lock);
	InitSemaphore(&data->launch_lock);
	InitSemaphore(&data->path_lock);
	InitSemaphore(&data->semaphores.lock);
	InitSemaphore(&data->device_list.lock);
	InitSemaphore(&data->file_list.lock);
	InitSemaphore(&data->filetype_cache.lock);
	InitSemaphore(&data->allocbitmap_patch.lock);
	InitSemaphore(&data->backfill_lock);
	data->popup_delay=10;

	// Initialise NewIcons settings
	data->NewIconsFlags=ENVNIF_ENABLE;
	data->NewIconsPrecision=16;

	// See if SysIHack is running
    Forbid();
	if (FindTask("« sysihack »")) data->flags|=LIBDF_3DLOOK;

	// Or variable is set for 3d gadgets
	else
	if (GetVar("Galileo/3DLook",buf,2,GVF_GLOBAL_ONLY)>-1)
		data->flags|=LIBDF_3DLOOK;
    Permit();

	// Variable set for no stippling of requesters
	if (GetVar("Galileo/OuEstLeMinibar",buf,2,GVF_GLOBAL_ONLY)>-1)
		data->flags|=LIBDF_NOSTIPPLE;

	// Variable set to install DOS patches
	if (GetVar("Galileo/DOSPatch",buf,2,GVF_GLOBAL_ONLY)>-1)
		data->flags|=LIBDF_DOS_PATCH;

	// Variable set to install filetype cache
	if (GetVar("Galileo/FiletypeCache",buf,14,GVF_GLOBAL_ONLY)>-1)
	{
		// Get cache size
		data->ft_cache_max=atoi(buf);
		if (data->ft_cache_max<10)
			data->ft_cache_max=10;

		// Set cache flag
		data->flags|=LIBDF_FT_CACHE;
	}

	// Save a4
	data->a4=getreg(REG_A4);

#if RESOURCE_TRACKING
#define GalileoFMBase libbase
	// Create some memory handles
	data->memory=NewMemHandle(sizeof(IPCMessage)<<5,sizeof(IPCMessage)<<4,MEMF_CLEAR|MEMF_PUBLIC);
	data->dos_list_memory=NewMemHandle(1024,512,MEMF_CLEAR);

#undef GalileoFMBase
#else
	// Create some memory handles
	data->memory=L_NewMemHandle(sizeof(IPCMessage)<<5,sizeof(IPCMessage)<<4,MEMF_CLEAR|MEMF_PUBLIC);
	data->dos_list_memory=L_NewMemHandle(1024,512,MEMF_CLEAR);

#endif
	// Initialise boopsi classes
	if (!(image_class=
			init_class(
				data,
				"galileoiclass",
				"imageclass",
				(unsigned long (*)())image_dispatch,
				sizeof(BoopsiImageData))) ||

		!(button_class=
			init_class(
				data,
				"galileobuttongclass",
				"gadgetclass",
				(unsigned long (*)())button_dispatch,
				sizeof(ButtonData))) ||

		!(string_class=
			init_class(
				data,
				"galileostrgclass",
				"strgclass",
				(unsigned long (*)())button_dispatch,
				sizeof(StringData))) ||

		!(check_class=
			init_class(
				data,
				"galileocheckgclass",
				"gadgetclass",
				(unsigned long (*)())button_dispatch,
				sizeof(CheckData))) ||

		!(view_class=
			init_class(
				data,
				"galileoviewgclass",
				"gadgetclass",
				(unsigned long (*)())button_dispatch,
				sizeof(ButtonData))) ||

		!(palette_class=
			init_class(
				data,
				"galileopalettegclass",
				"gadgetclass",
				(unsigned long (*)())palette_dispatch,
				sizeof(PaletteData))) ||

		!(frame_class=
			init_class(
				data,
				"galileoframeclass",
				"gadgetclass",
				(unsigned long (*)())button_dispatch,
				sizeof(ButtonData))) ||

		!(gauge_class=
			init_class(
				data,
				"galileogaugeclass",
				"gadgetclass",
				(unsigned long (*)())button_dispatch,
				sizeof(GaugeData))) ||

		!(listview_class=
			init_class(
				data,
				"galileolistviewgclass",
				"gadgetclass",
				(unsigned long (*)())listview_dispatch,
				sizeof(ListViewData))))
		return 1;

	// Set flag to identify some classes
	if (string_class) string_class->cl_UserData=CLASS_STRINGGAD;
	if (check_class) check_class->cl_UserData=CLASS_CHECKGAD;
	if (view_class) view_class->cl_UserData=CLASS_VIEW;
	if (frame_class) frame_class->cl_UserData=CLASS_FRAME;
	if (gauge_class) gauge_class->cl_UserData=CLASS_GAUGE;

	// Initialise locale
	init_locale_data(&data->locale);

	// Did we get locale library?
	if (LocaleBase=data->locale.li_LocaleBase)
	{
		// Get decimal point
		if (data->locale.li_Locale && data->locale.li_Locale->loc_DecimalPoint)
				decimal_point=data->locale.li_Locale->loc_DecimalPoint[0];
	}

	// Get a path list
	data->path_list=L_GetDosPathList(0);

	// Create launcher process
	if (!(L_IPC_Launch(
		0,&launcher_ipc,
		"GALILEO_LAUNCHER",
		(ULONG)launcher_proc,
		STACK_LARGE|IPCF_GETPATH,
		(ULONG)data,(struct Library *)DOSBase,libbase)) || !launcher_ipc) return 1;
	data->launcher=launcher_ipc;

	// Initialise low-memory handler
	if (ExecLib->LibNode.lib_Version>=39)
	{
		// Initialise interrupt
		data->low_mem_handler.is_Node.ln_Pri=50;
		data->low_mem_handler.is_Node.ln_Type=NT_INTERRUPT;
		data->low_mem_handler.is_Node.ln_Name="galileo memhandler";
		data->low_mem_handler.is_Data=data;
		data->low_mem_handler.is_Code=(void (*)())low_mem_handler;

		// Add the handler
		AddMemHandler(&data->low_mem_handler);
	}

	// Succeeded
	return 0;
}


// Clean up
void __asm __saveds __UserLibCleanup(register __a6 struct MyLibrary *libbase)
{
	WB_Data *wb_data;
	struct LibData *data;

	L_FlushImages();

	// Launcher?
	if (launcher_ipc)
	{
		L_IPC_Command(launcher_ipc,IPC_QUIT,0,0,0,REPLY_NO_PORT_IPC);
	}

	// Library data?
	if ((data=(struct LibData *)libbase->ml_UserData))
	{
		// Get workbench data
		wb_data=&data->wb_data;

		// Remove low-memory handler
		if (data->low_mem_handler.is_Node.ln_Pri==50)
			RemMemHandler(&data->low_mem_handler);

		// Remove patch information
		if (wb_data)
		{
			if (wb_data->patch_count>0)
			{
				wb_data->patch_count=1;
				L_WB_Remove_Patch(libbase);
			}
		}

		// Free path list
		L_FreeDosPathList(data->path_list);

		// Free locale stuff
		free_locale_data(&data->locale);

#if RESOURCE_TRACKING
#define GalileoFMBase libbase
		// Free memory
		FreeMemHandle(data->memory);
		FreeMemHandle(data->dos_list_memory);
#undef GalileoFMBase
#else
		// Free memory
		L_FreeMemHandle(data->memory);
		L_FreeMemHandle(data->dos_list_memory);
#endif
		// Close timer
		if (data->TimerBase) CloseDevice(&data->timer_io);

		// Free library data
		FreeVec(data);
	}

	class_free(listview_class);
	class_free(button_class);
	class_free(string_class);
	class_free(check_class);
	class_free(view_class);
	class_free(frame_class);
	class_free(palette_class);
	class_free(gauge_class);
	class_free(image_class);
	if (topaz_font) CloseFont(topaz_font);
	if (NewIconBase) CloseLibrary((struct Library *)NewIconBase);
	if (RexxSysBase) CloseLibrary((struct Library *)RexxSysBase);
    if (P96Base) CloseLibrary(P96Base);
	if (DataTypesBase) CloseLibrary(DataTypesBase);
	CloseLibrary(IconBase);
	CloseLibrary(LayersBase);
	CloseLibrary(UtilityBase);
	CloseLibrary(AslBase);
	CloseLibrary(GadToolsBase);
	CloseLibrary(WorkbenchBase);
	CloseLibrary((struct Library *)GfxBase);
	CloseLibrary((struct Library *)IntuitionBase);
	CloseLibrary((struct Library *)DOSBase);

#if RESOURCE_TRACKING
    KPrintF("Main Library\n");
    //PrintTrackedResources();
    if (ResTrackBase->lib_OpenCnt==1)
	    EndResourceTracking(); /* Generate a memory usage report */

    REALL_CloseLibrary(ResTrackBase);

    KPrintF("Quitting......\n");
#endif
}


// Low memory handler
__asm low_mem_handler(
	register __a0 struct MemHandlerData *memh,
	register __a1 struct LibData *data)
{
	// Is this the first time?
	if (!(memh->memh_Flags&MEMHF_RECYCLE))
	{
		// Signal the launcher to send the message
		if (data->low_mem_signal>-1)
			Signal((struct Task *)data->launcher->proc,1<<data->low_mem_signal);

		// Tell it to try again
		return MEM_TRY_AGAIN;
	}

	// We don't actually do anything
	return MEM_DID_NOTHING;
}

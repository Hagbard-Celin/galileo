/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 1993-2012 Jonathan Potter & GP Software
Copyright 2024 Hagbard Celine

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

Opus� and Directory Opus� and associated images are registered trademarks of GPSoftware.
DOpus(TM), DirOpus(TM) and Opus File Manager(TM) are trademarked 1991 by GPSoftware.
Opus Magellan (TM) is trademarked 1996 GPSoftware.
All other trademarks are the property of their respective owners.

The release of Directory Opus 5 under the GPL in NO WAY affects
the existing commercial status of Directory Opus for Windows.

For more information on Directory Opus for Windows please see:

				 http://www.gpsoft.com.au

*/

/*
	icon.c for icon.gfmmodule

	gjp  Nov 99 changes for V44 icon library etc
	68.15

	68.16 - fixed problem with copying of icon type when dupdiskobject


*/

#include "icon.h"
#include <proto/newicon.h>
#include <libraries/newicon.h>

#ifdef DEBUG
void kprintf( const char *, ... );
#else
#define kprintf ; /##/
#endif

char *version="$VER: icon.gfmmodule 0.2 "__AMIGADATE__" ";

void icon_drop_44( icon_data *data, int x, int y );
BOOL icon_save_44( icon_data *data, char *save_name ,BOOL err);
static struct seglist *bptrtoseglist(BPTR bptr);
static CONST_STRPTR scanvertag(UBYTE *segment, ULONG size);
static CONST_STRPTR gethandlerver(BPTR seglist);

#define ICON_REMAP	0
#define ICON_NO_REMAP	1

#define AREA_TYPE_ICON_IMAGE 1
#define AREA_TYPE_ICON_TOOLTYPES 2

#define NewIconBase	(data->newicon_base)

int __asm __saveds L_Module_Entry_Internal(register __a0 struct List *files,
	                          register __a1 struct Screen *screen,
	                          register __a2 IPCData *ipc,
	                          register __a3 IPCData *main_ipc,
	                          register __d0 ULONG mod_id,
	                          register __d1 ULONG mod_data)
{
    icon_data *data;
    struct Node *node;
    struct Process *proc;
    APTR wsave;
    char buf[4];
    struct Message *msg;

    // Allocate data
    if (!(data = AllocVec( sizeof(icon_data), MEMF_CLEAR )))
	return 0;

    data->timer = AllocTimer( UNIT_VBLANK, 0 );

    // Create message ports
    if ((data->app_port = CreateMsgPort())
	&& (data->notify_port = CreateMsgPort()))
    {
	// Add notify request
	if (data->notify_req = AddNotifyRequest(GN_APP_WINDOW_LIST,
		                                0,
		                                data->notify_port))
	{
	    // Store IPC pointer
	    data->ipc      = ipc;
	    data->main_ipc = main_ipc;
	    data->screen   = screen;

	    // Open NewIcon library
	    data->newicon_base = OpenLibrary( "newicon.library", 0 );

	    // Get process, save error pointer
	    proc = (struct Process *)FindTask(0);
	    wsave = proc->pr_WindowPtr;

	    // Initialise popup menus
	    NewList( (struct List *)&data->menu );
	    data->menu.locale = locale;

	    data->dropmenu.locale = locale;
	    data->dropmenu.flags = POPUPMF_STICKY;

	    // Want to remap?
	    if (mod_data == ICON_REMAP)
		data->remap = 1;

	    // Initialise menu items
	    data->menu_items[IIM_EDIT].item_name=(char *)MSG_EDIT;
	    data->menu_items[IIM_EDIT].id=MENU_ICON_EDIT;
	    data->menu_items[IIM_EDIT].flags=POPUPF_LOCALE;
	    data->menu_items[IIM_PICK_EDITOR].item_name=(char *)MSG_PICK_EDITOR;
	    data->menu_items[IIM_PICK_EDITOR].id=MENU_ICON_PICK_EDITOR;
	    data->menu_items[IIM_PICK_EDITOR].flags=POPUPF_LOCALE;
	    data->menu_items[IIM_BARN1].item_name=POPUP_BARLABEL;
	    data->menu_items[IIM_DISK].item_name=(char *)MSG_ICON_DEVICE;
	    data->menu_items[IIM_DISK].id=MENU_DISK;
	    data->menu_items[IIM_DISK].flags=POPUPF_LOCALE|POPUPF_CHECKIT;
	    data->menu_items[IIM_DRAWER].item_name=(char *)MSG_ICON_DRAWER;
	    data->menu_items[IIM_DRAWER].id=MENU_DRAWER;
	    data->menu_items[IIM_DRAWER].flags=POPUPF_LOCALE|POPUPF_CHECKIT;
	    data->menu_items[IIM_TOOL].item_name=(char *)MSG_ICON_TOOL;
	    data->menu_items[IIM_TOOL].id=MENU_TOOL;
	    data->menu_items[IIM_TOOL].flags=POPUPF_LOCALE|POPUPF_CHECKIT;
	    data->menu_items[IIM_PROJECT].item_name=(char *)MSG_ICON_PROJECT;
	    data->menu_items[IIM_PROJECT].id=MENU_PROJECT;
	    data->menu_items[IIM_PROJECT].flags=POPUPF_LOCALE|POPUPF_CHECKIT;
	    data->menu_items[IIM_GARBAGE].item_name=(char *)MSG_ICON_GARBAGE;
	    data->menu_items[IIM_GARBAGE].id=MENU_GARBAGE;
	    data->menu_items[IIM_GARBAGE].flags=POPUPF_LOCALE|POPUPF_CHECKIT;
	    data->menu_items[IIM_BAR0].item_name=POPUP_BARLABEL;
	    data->menu_items[IIM_BORDERLESS].item_name=(char *)MSG_BORDER_MENU;
	    data->menu_items[IIM_BORDERLESS].id=MENU_BORDERLESS;
	    data->menu_items[IIM_BORDERLESS].flags=POPUPF_LOCALE|POPUPF_CHECKIT;
	    data->menu_items[IIM_LABEL].item_name=(char *)MSG_LABEL_MENU;
	    data->menu_items[IIM_LABEL].id=MENU_NO_LABEL;
	    data->menu_items[IIM_LABEL].flags=POPUPF_LOCALE|POPUPF_CHECKIT;
	    data->menu_items[IIM_BAR1].item_name=POPUP_BARLABEL;

	    data->menu_items[IIM_STRIP_NEWICONS].item_name=(char *)MSG_STRIP_NEWICONS;
	    data->menu_items[IIM_STRIP_NEWICONS].id=MENU_STRIP_NEWICONS;
	    data->menu_items[IIM_STRIP_NEWICONS].flags=POPUPF_LOCALE|POPUPF_DISABLED;

	    data->menu_items[IIM_STRIP_OLD].item_name=(char *)MSG_STRIP_OLD;
	    data->menu_items[IIM_STRIP_OLD].id=MENU_STRIP_OLD;
	    data->menu_items[IIM_STRIP_OLD].flags=POPUPF_LOCALE|POPUPF_DISABLED;
	    data->menu_items[IIM_BAR2].item_name=POPUP_BARLABEL;
	    data->menu_items[IIM_NEWICONS].item_name=(char *)MSG_NEWICONS;
	    data->menu_items[IIM_NEWICONS].id=MENU_NEWICONS;
	    data->menu_items[IIM_NEWICONS].flags=POPUPF_LOCALE|POPUPF_CHECKIT|POPUPF_CHECKED|POPUPF_DISABLED;
	    data->menu_items[IIM_OLDICONS].item_name=(char *)MSG_OLDICONS;
	    data->menu_items[IIM_OLDICONS].id=MENU_OLDICONS;
	    data->menu_items[IIM_OLDICONS].flags=POPUPF_LOCALE|POPUPF_CHECKIT|POPUPF_DISABLED;

	    // Add items to menu
	    AddTail((struct List *)&data->menu,(struct Node *)&data->menu_items[IIM_EDIT]);
	    AddTail((struct List *)&data->menu,(struct Node *)&data->menu_items[IIM_PICK_EDITOR]);
	    AddTail((struct List *)&data->menu,(struct Node *)&data->menu_items[IIM_BARN1]);
	    AddTail((struct List *)&data->menu,(struct Node *)&data->menu_items[IIM_DISK]);
	    AddTail((struct List *)&data->menu,(struct Node *)&data->menu_items[IIM_DRAWER]);
	    AddTail((struct List *)&data->menu,(struct Node *)&data->menu_items[IIM_TOOL]);
	    AddTail((struct List *)&data->menu,(struct Node *)&data->menu_items[IIM_PROJECT]);
	    AddTail((struct List *)&data->menu,(struct Node *)&data->menu_items[IIM_GARBAGE]);
	    AddTail((struct List *)&data->menu,(struct Node *)&data->menu_items[IIM_BAR0]);
	    AddTail((struct List *)&data->menu,(struct Node *)&data->menu_items[IIM_BORDERLESS]);
	    AddTail((struct List *)&data->menu,(struct Node *)&data->menu_items[IIM_LABEL]);

	    if (NewIconBase)
	    {
		// Add 'show' items to menu
		AddTail((struct List *)&data->menu,(struct Node *)&data->menu_items[IIM_BAR2]);
		AddTail((struct List *)&data->menu,(struct Node *)&data->menu_items[IIM_NEWICONS]);
		AddTail((struct List *)&data->menu,(struct Node *)&data->menu_items[IIM_OLDICONS]);

		// Add 'strip' items to menu
		AddTail((struct List *)&data->menu,(struct Node *)&data->menu_items[IIM_BAR1]);
		AddTail((struct List *)&data->menu,(struct Node *)&data->menu_items[IIM_STRIP_NEWICONS]);
		AddTail((struct List *)&data->menu,(struct Node *)&data->menu_items[IIM_STRIP_OLD]);
	    }

	    // Initialise dropmenu items
	    data->dropmenu_items[IDM_OLDICONS].item_name=(char *)MSG_COPY_OLDICON;
	    data->dropmenu_items[IDM_OLDICONS].id=MENU_COPY_OLDICON;
	    data->dropmenu_items[IDM_OLDICONS].flags=POPUPF_LOCALE;
	    data->dropmenu_items[IDM_NEWICONS].item_name=(char *)MSG_COPY_NEWICON;
	    data->dropmenu_items[IDM_NEWICONS].id=MENU_COPY_NEWICON;
	    data->dropmenu_items[IDM_NEWICONS].flags=POPUPF_LOCALE;
	    data->dropmenu_items[IDM_BOTHICONS].item_name=(char *)MSG_COPY_BOTHICONS;
	    data->dropmenu_items[IDM_BOTHICONS].id=MENU_COPY_BOTHICONS;
	    data->dropmenu_items[IDM_BOTHICONS].flags=POPUPF_LOCALE;
	    data->dropmenu_items[IDM_BAR].item_name=POPUP_BARLABEL;
	    data->dropmenu_items[IDM_TOOLTYPES].item_name=(char *)MSG_COPY_TOOLTYPES;
	    data->dropmenu_items[IDM_TOOLTYPES].id=MENU_COPY_TOOLTYPES;
	    data->dropmenu_items[IDM_TOOLTYPES].flags=POPUPF_LOCALE;

	    // Use Workbench.library info routine?
	    if (mod_data != 0x96604497 &&
	        WorkbenchBase && WorkbenchBase->lib_Version >= 39 &&
	        (GetVar("Galileo/UseWBInfo",buf,2,GVF_GLOBAL_ONLY))>0)
	    {
		data->wb_info = 1;
	    }

	    // Set up editor
	    if (GetVar( "Galileo/Icon Editor", data->edit_command, 256 ,GVF_GLOBAL_ONLY ) > 0)
	    {
		    char *p;

		    // Starts with number?
		    if (isdigit(*data->edit_command))
		    {
			data->use_iconedit = atoi(data->edit_command);
			for (p = data->edit_command; isdigit(*p); ++p);

			// Edit command follows?
			if (*p && *(p+1))
			    memmove( data->edit_command, p+1, strlen(p) );
			else
			    *data->edit_command = 0;
		    }
		    else
			*data->edit_command = 0;
	    }
	    else
		*data->edit_command = 0;

	    // Default editor settings if none saved
	    if (!*data->edit_command)
	    {
		strcpy( data->edit_command, "Sys:Tools/IconEdit" );
		data->use_iconedit = 1;
	    }

	    // Go through files
	    for (node=files->lh_Head;node->ln_Succ;node=node->ln_Succ)
	    {
		// Do info on this file
		if (!(node->ln_Type = icon_info(data,
			                        node->ln_Name,
			                        node->ln_Succ->ln_Succ)))
		{
		    break;
		}
	    }

	    // Restore error pointer
	    proc->pr_WindowPtr = wsave;

	    // Clean up
	    icon_free( data );

	    // Close window
	    closewindow( data );

	    // Close libraries
	    if (data->newicon_base)
		CloseLibrary( data->newicon_base );

	    RemoveNotifyRequest( data->notify_req );
	}
    }

    // Free and flush notify port
    if (data->notify_port)
    {
	while (msg = GetMsg( data->notify_port ))
	    ReplyFreeMsg( msg );

	DeleteMsgPort( data->notify_port );
    }

    // Free and flush app port
    if (data->app_port)
    {
	while (msg = GetMsg( data->app_port ))
	    ReplyFreeMsg( msg );

	DeleteMsgPort( data->app_port );
    }

    FreeTimer( data->timer );

    // Free data
    FreeVec( data );

    return 0;
}


// close window
void closewindow(icon_data *data)
{
    if (data)
    {
	// Remove AppWindow
	if (data->app_window)
	{
	    RemoveAppWindow( data->app_window );
	    data->app_window = 0;
	}

	// Close window
	CloseConfigWindow( data->window );
	data->window = 0;

	// Free lists
	Att_RemList( data->prot_list[0], 0 );
	Att_RemList( data->prot_list[1], 0 );
	Att_RemList( data->prot_list[2], 0 );
	Att_RemList( data->prot_list[3], 0 );
	Att_RemList( data->tool_list, 0 );
	data->prot_list[0] = 0;
	data->prot_list[1] = 0;
	data->prot_list[2] = 0;
	data->prot_list[3] = 0;
	data->tool_list = 0;
    }
}


// Free icon data
void icon_free( icon_data *data )
{
    if (data)
    {
	// Free icon
	if (data->icon)
	{
	    // Free remapping
	    RemapIcon( data->icon, (data->window) ? data->window->WScreen : 0, 1 );

	    // Free icon
	    FreeCachedDiskObject( data->icon );
	    data->icon = 0;
	}

	// Free new oldicon
	if (data->new_oldicon)
	{
	    if (data->new_oldicon == data->new_newicon)
		data->new_newicon = 0;

	    // Free remapping
	    RemapIcon( data->new_oldicon, (data->window) ? data->window->WScreen : 0, 1 );

	    // Free icon
	    FreeCachedDiskObject( data->new_oldicon );
	    data->new_oldicon = 0;
	}

	// Free new newicon
	if (data->new_newicon)
	{
	    // Free remapping
	    RemapIcon( data->new_newicon, (data->window) ? data->window->WScreen : 0, 1 );

	    // Free icon
	    FreeCachedDiskObject( data->new_newicon );
	    data->new_newicon = 0;
	}
    }
}

// Build tooltype list
void icon_build_tooltypes( icon_data *data )
{
    if (data->tool_list = Att_NewList( LISTF_POOL ))
    {
	short num;
	Att_List *list_ptr = data->tool_list;

	// Any tooltypes?
	if (data->icon->do_ToolTypes)
	{
	    // Go through tooltypes
	    for (num = 0; data->icon->do_ToolTypes[num]; ++num)
	    {
		// Add to list
		tooltype_newnode( list_ptr, data->icon->do_ToolTypes[num], 0, 0 );
	    }
	}
    }
}


// Build protection list
void icon_build_protlist( icon_data *data )
{
    short     a,b=0,c=0;
    Att_Node *node;

    if (data->prot_list[0] = Att_NewList( 0 ))
    {
	if (data->prot_list[1] = Att_NewList( 0 ))
	{
	    if (data->prot_list[2] = Att_NewList( 0 ))
	    {
		if (data->prot_list[3] = Att_NewList( 0 ))
		{
		    // Go through 8 bits
		    for (a = 0; a < 8; ++a)
		    {
			// Create node
			if (node = Att_NewNode( data->prot_list[b], GetString(locale,MSG_ICON_HOLD+a), a, 0 ))
			{
			    // Set selection flag
			    if (data->fib.fib_Protection & protect_lookup[a])
			    {
				if (a<4)
				    node->node.lve_Flags |= LVEF_SELECTED;
			    }
			    else
			    if (a>3)
				node->node.lve_Flags |= LVEF_SELECTED;
			}
			if (++c>=2)
			{
			    // Add list to listview
			    SetGadgetChoices( data->list, GAD_ICON_PROTECTION_HS+b, data->prot_list[b] );
			    b++;
			    c=0;
			}
		    }
		}
		else goto protfail3;
	    }
	    else goto protfail2;
	}
	else goto protfail1;
    }
    data->protection=TRUE;
    return;

    protfail3:
	Att_RemList(data->prot_list[2],NULL);
    protfail2:
	Att_RemList(data->prot_list[1],NULL);
    protfail1:
	Att_RemList(data->prot_list[0],NULL);
}

// Remap icon
void icon_remap( icon_data *data )
{
    // Remap icon
    if (RemapIcon( data->icon, data->window->WScreen, 0 ))
    {
	struct DiskObject *icon;
	// Has to be a valid old image too
	if ((icon = GetOriginalIcon(data->icon)) &&
	        icon->do_Gadget.Width  > 1 &&
		        icon->do_Gadget.Height > 1)
	{
	    // Both images available
	    data->which_images = 1;

	    // Enable old and new icon menu entries
	    data->menu_items[IIM_OLDICONS].flags       &= ~POPUPF_DISABLED;
	    data->menu_items[IIM_NEWICONS].flags       &= ~POPUPF_DISABLED;
	    data->menu_items[IIM_STRIP_OLD].flags      &= ~POPUPF_DISABLED;
	    data->menu_items[IIM_STRIP_NEWICONS].flags &= ~POPUPF_DISABLED;
	}
	else
	{
	    // Only newicon image available
	    data->which_images = 2;
	    data->icon_mode = 1;
	}
    }
    else
    {
	// Only original image available
	data->which_images = 0;

	// No newicon image present
	data->icon_mode = 0;
    }
}


static struct seglist *bptrtoseglist(BPTR bptr)
{
    if (bptr != 0)
	return (struct seglist *)((ULONG *)BADDR(bptr) - 1);
    else
	return NULL;
}

static CONST_STRPTR scanvertag(UBYTE *segment, ULONG size)
{
    UBYTE cmp[5];
    CONST_STRPTR vtag = NULL;
    ULONG i, j;

    cmp[0] = '$';
    cmp[1] = 'V';
    cmp[2] = 'E';
    cmp[3] = 'R';
    cmp[4] = ':';

    for (i = j = 0; i < size; i++)
    {
	if (segment[i] != cmp[j])
		j = 0;

	if (segment[i] == cmp[j] && ++j == sizeof(cmp))
	{
	    vtag = &segment[i - sizeof(cmp) + 1];
	    break;
	}
    }

    if (vtag)
	return vtag+5;

    return NULL;
}

static CONST_STRPTR gethandlerver(BPTR seglist)
{
    struct seglist *seg;
    CONST_STRPTR vtag = NULL;

    for (seg = bptrtoseglist(seglist); seg != NULL; seg = bptrtoseglist(seg->Next))
    {
	vtag = scanvertag(&seg->Code, seg->Size);
	if (vtag != NULL)
	    break;
    }
    return vtag;
}

// open window
int openwindow( icon_data *data, int next )
{
    char *ptr;
    BPTR  lock;
    short a;
    BOOL  bigfile=FALSE;
    struct Rectangle rect;
    ULONG size;
    icon_temp *temp;

    if (temp=AllocVec(sizeof(icon_temp),MEMF_CLEAR))
    {

	if (data->icon_type == WBDEVICE || data->icon_type == WBKICK)
	    data->icon_type = WBDISK;

	// If this is a disk icon, strip away everything after the colon
	if (data->icon_type == WBDISK &&
	   (ptr = strchr( data->prog_name, ':' )) &&
	   !(strchr( ptr, '/' )))
	{
	    *(ptr+1) = 0;
	}

	// Get file information
	if (lock = Lock( data->prog_name, ACCESS_READ ))
	{
	    Info( lock, &data->info );
	    data->disktype=((struct DosList *)BADDR(data->info.id_VolumeNode))->dol_misc.dol_volume.dol_DiskType;

	    // Build object name
	    strcpy( data->object_name, data->fib.fib_FileName );
	    if (data->icon_type == WBDISK)
		strcat( data->object_name, ":" );

	    // Get locale flags
	    data->locale_flags = GetLocaleSettings(0);

	    // Get decimal separator
	    if (data->locale_flags&DATE_1000SEP)
	    {
	        if (locale->li_Locale)
		    data->decimal_sep = locale->li_Locale->loc_GroupSeparator[0];
	        else
		    data->decimal_sep = ',';
	    }

	    if (data->icon_type == WBDISK)
	        DateToStrings(&((struct DeviceList *)BADDR(((struct FileLock *)BADDR(lock))->fl_Volume))->dl_VolumeDate,  data->datebuf, data->timebuf, 1);
	    else
	        DateToStrings(&data->fib.fib_Date,  data->datebuf, data->timebuf, 1);

	    if (data->locale_flags&DATE_12HOUR)
	        strcat(data->timebuf,"m");

	    if (data->icon_type == WBDISK)
	    {
		struct DosList *volume, *device;

		// Get volume node pointer
		volume=(struct DosList *)BADDR(data->info.id_VolumeNode);

		// Get device name
		device=DeviceFromLock(lock,data->path);

		// Got device?
		if (device)
		{
		    struct FileSysStartupMsg *startup;
		    struct DosEnvec		    *denv;
		    char *name = 0;
		    CONST_STRPTR htmp = 0;

		    if ( device->dol_Name)
			name = BADDR(device->dol_Name);

		    if ( (name[0] == 3) && (!(strncmp(name+1,"RAM",3))) )
		    {
			struct Resident *ramhandler;

			lsprintf(temp->buf,"%s",GetString(locale,MSG_RAM_DEVICE));

			if (ramhandler = FindResident("ram-handler"))
			{
			    htmp = ramhandler->rt_IdString;
			}
		    }

		    if (device->dol_misc.dol_handler.dol_Handler)
		    {
			lsprintf(temp->handler, "%b", device->dol_misc.dol_handler.dol_Handler);
		    }

		    if (!htmp)
		    {
			htmp = gethandlerver(device->dol_misc.dol_handler.dol_SegList);
		    }

		    if (htmp)
		    {
			WORD i;

			// Skip leading space
			if (htmp[0] == ' ') htmp++;

			// Strip unprintable
			for (i=0; htmp[i] != '\n' && htmp[i] != '\r' && htmp[i] != '\0'; i++)
			    temp->handler[i]=htmp[i];

			temp->handler[i]='\0';
		    }

		    // Got startup message?
		    if (startup=(struct FileSysStartupMsg *)BADDR(device->dol_misc.dol_handler.dol_Startup))
		    {
			// Get device name
			lsprintf(temp->buf+128,"%b",startup->fssm_Device);
			lsprintf(temp->buf,"%s %s %ld",
				 temp->buf+128,
				 GetString(locale,MSG_UNIT),
				 startup->fssm_Unit);

			denv=(struct DosEnvec *)BADDR(startup->fssm_Environ);
			if (denv->de_TableSize >= 16)
			data->filesystem = denv->de_DosType;
			data->taskport=device->dol_Task->mp_SigTask;
		    }

		    // Get name
		    lsprintf(data->volume,"%b",volume->dol_Name);
		}

		// Didn't get anything?
		if (!*temp->buf)
		{
		    // Get name of task
		    if (device &&
			((struct Task *)device->dol_Task->mp_SigTask)->tc_Node.ln_Name &&
			TypeOfMem(((struct Task *)device->dol_Task->mp_SigTask)->tc_Node.ln_Name))
		    {
			strcpy(temp->buf,((struct Task *)device->dol_Task->mp_SigTask)->tc_Node.ln_Name);
		    }

		    // Unknown handler
		    else
			strcpy(temp->buf,GetString(locale,MSG_UNKNOWN));
		}
	    }

	    if (data->fib.fib_Size == 0xFFFFFFFF)
	    {

		if (!(GetFileSize64(lock,&data->f_size64)))
		    bigfile=TRUE;
	    }
	    else
		UnLock(lock);
	}

	// Can't lock object
	else
	{
	    strcpy( data->object_name, FilePart( data->prog_name ) );
	    strcpy( data->datebuf, "???" );
	    data->timebuf[0] = 0;
	}

	// Build tooltype list
	icon_build_tooltypes( data );

	// Find type label
	for (data->label = 0; icon_type_labels[data->label]; data->label += 2)
	    if (icon_type_labels[data->label] == data->icon_type)
		break;

	// Not matched?
	if (!icon_type_labels[data->label])
	    data->label = 0;

	// Fill out new window
	data->new_win.parent = data->screen;
	data->new_win.dims   = icon_windows[data->icon_type-1];
	data->new_win.locale = locale;
	data->new_win.port   = 0;
	data->new_win.flags  = WINDOW_SCREEN_PARENT|WINDOW_VISITOR|WINDOW_AUTO_KEYS;//|WINDOW_REQ_FILL;//|WINDOW_SIMPLE;
	data->new_win.font   = 0;

	if (data->icon_type != WBDISK)
	    data->new_win.flags  |= WINDOW_SIZE_BOTTOM;

	// Open icon window
	if (!(data->window = OpenConfigWindow( &data->new_win )))
	    return 0;

	// OS 39?
	if (GfxBase->LibNode.lib_Version>=39)
	{
	    short a,fail=0;
	    struct TagItem tags[2];

	    // Set tags
	    tags[0].ti_Tag=OBP_FailIfBad;
	    tags[0].ti_Data=TRUE;
	    tags[1].ti_Tag=TAG_END;

	    // Try to allocate pens
	    for (a=0;a<4;a++)
	    {
		// Allocate it
		if ((a>=2 && fail) || (data->pen_alloc[a]=
		    ObtainBestPenA(data->window->WScreen->ViewPort.ColorMap,
				   diskinfo_colours[a][0]<<24,
				   diskinfo_colours[a][1]<<24,
				   diskinfo_colours[a][2]<<24,
				   tags))==-1)
		{
		    // Couldn't allocate; is this a shadow pen?
		    if (a>=2)
		    {
			// Use main pen
			data->pens[a]=data->pens[a-2];
		    }

		    // Otherwise, find best pen
		    else
			data->pens[a]=
			    FindColor(data->window->WScreen->ViewPort.ColorMap,
				      diskinfo_colours[a][0]<<24,
				      diskinfo_colours[a][1]<<24,
				      diskinfo_colours[a][2]<<24,
				      -1);

		    // Clear allocation
		    data->pen_alloc[a]=-1;

		    // Set 'fail' flag
		    fail=1;
		}

		// Use it
		else data->pens[a]=data->pen_alloc[a];
	    }
	}

	// Otherwise
	else
	{
	    // Get default pens
	    data->pens[USEDPEN]=PEN_C2;
	    data->pens[FREEPEN]=PEN_C3;
	    data->pens[USEDSHADOWPEN]=PEN_C2;
	    data->pens[FREESHADOWPEN]=PEN_C3;
	}

	// limit to vertical sizing only
	WindowLimits( data->window, data->window->Width, data->window->Height, data->window->Width, -1 );

	// Set window ID
	SetWindowID( data->window, 0, WINDOW_ICONINFO, (struct MsgPort *)data->ipc );

	// Done initial remap yet?
	if (!data->first_remap)
	{
	    icon_remap( data );

	    data->first_remap = 1;
	}

	//build_title( data );
	strcpy(data->title,data->path);
	SetWindowTitles( data->window, data->title, (UBYTE *)-1 );

	// Set error pointer
	((struct Process *)FindTask(0))->pr_WindowPtr = data->window;

	// Add AppWindow
	if (data->app_port)
	    data->app_window = AddAppWindowA( 0, 0, data->window, data->app_port, 0 );

	// Add common objects
	data->list = AddObjectList( data->window, icon_info_objects );

	// Need next?
	if (next)
	    AddObjectList( data->window, icon_info_next_object );

	// Popup menu type
	data->menu_items[IIM_DISK].flags    &= ~POPUPF_CHECKED;
	data->menu_items[IIM_DRAWER].flags  &= ~POPUPF_CHECKED;
	data->menu_items[IIM_TOOL].flags    &= ~POPUPF_CHECKED;
	data->menu_items[IIM_PROJECT].flags &= ~POPUPF_CHECKED;
	data->menu_items[IIM_GARBAGE].flags &= ~POPUPF_CHECKED;

	// Tool/project objects
	if (data->icon_type == WBTOOL)
	{
	    AddObjectList( data->window, icon_toolproj_right );
	    AddObjectList( data->window, icon_file_info_objects );
	    AddObjectList( data->window, icon_toolproj_objects );
	    AddObjectList( data->window, icon_tooldir_objects );
	    AddObjectList( data->window, icon_tool_objects );
	    data->menu_items[IIM_TOOL].flags |= POPUPF_CHECKED;
	}
	// Project objects
	if (data->icon_type == WBPROJECT)
	{
	    AddObjectList( data->window, icon_toolproj_right );
	    AddObjectList( data->window, icon_file_info_objects );
	    AddObjectList( data->window, icon_toolproj_objects );
	    AddObjectList( data->window, icon_project_objects );
	    data->menu_items[IIM_PROJECT].flags |= POPUPF_CHECKED;
	}

	// Drawer objects
	else if (data->icon_type == WBDRAWER)
	{
	    AddObjectList( data->window, icon_toolproj_right );
	    AddObjectList( data->window, icon_toolproj_objects );
	    AddObjectList( data->window, icon_tooldir_objects );
	    data->menu_items[IIM_DRAWER].flags |= POPUPF_CHECKED;
	}

	// Garbage objects
	else if (data->icon_type == WBGARBAGE)
	{
	    AddObjectList( data->window, icon_toolproj_right );
	    AddObjectList( data->window, icon_toolproj_objects );
	    AddObjectList( data->window, icon_tooldir_objects );
	    data->menu_items[IIM_GARBAGE].flags |= POPUPF_CHECKED;
	}

	// Disk objects
	else if (data->icon_type == WBDISK)
	{
	    AddObjectList( data->window, icon_disk_objects );
	    data->menu_items[IIM_DISK].flags |= POPUPF_CHECKED;
	}

	// Get icon image pointers
	data->image[0] = (struct Image *)data->icon->do_Gadget.GadgetRender;
	data->image[1] = (struct Image *)data->icon->do_Gadget.SelectRender;

	// Get icon display area
	GetObjectRect( data->list, GAD_ICON_IMAGE, &data->icon_area );

	// Get size of icon file
	get_icon_fsize(data);

	// Display icon
	data->image_num = 0;
	icon_switch_image( data, data->icon_mode );

	// Device
	if (data->icon_type == WBDISK)
	    if (temp->buf)
		SetGadgetValue(data->list,GAD_DEVICE,(ULONG)temp->buf);

	if (data->icon_type != WBDISK)
	{
	    // Build protection list
	    icon_build_protlist( data );
	}
	else
	{
	    data->protection=FALSE;

	    // Initialise Area
	    InitArea(&data->areainfo,data->areabuf,AREAVERTEX);
	    data->window->RPort->AreaInfo=&data->areainfo;

	    // Get graph rectangle
	    GetObjectRect(data->list,GAD_GRAPH,&rect);

	    // Allocate TmpRas buffer
	    size=RASSIZE((RECTWIDTH(&rect)+16),(RECTHEIGHT(&rect)+1));

	    if (data->rasbuf=AllocVec(size,MEMF_CHIP))
	    {
		UBYTE a, full=0, empty=0;
		char decimal_point;

		// Initialise TmpRas
		InitTmpRas(&data->tmpras,data->rasbuf,size);
		data->window->RPort->TmpRas=&data->tmpras;

		// Show graph
		diskinfo_show_graph(data,&rect,data->info.id_NumBlocksUsed,data->info.id_NumBlocks);

		// Free TmpRas
		data->window->RPort->TmpRas=0;
		FreeVec(data->rasbuf);

		// Get decimal point
		if (locale->li_Locale)
		    decimal_point = locale->li_Locale->loc_DecimalPoint[0];
		else
		    decimal_point = '.';

		// Get used percent
		sprintf(temp->buf,"%1.2f%%",data->used_percent);

		// Check for 100.00 or 0.00
		for (a = 0; temp->buf[a] != '%'; a++)
	        {
		    if (full && a == full && temp->buf[a] == '0')
		    {
			full++;
		    }
		    else
		    if (empty > 1 && temp->buf[a] == '0')
		    {
			empty++;
		    }
		    else
		    if (temp->buf[a] == '.')
		    {
			if (full == 3)
			    break;

			if (empty)
			    empty++;

			full = 0;

			// Fix decimal point
			if (decimal_point != '.')
			{
			    temp->buf[a] = decimal_point;
			}
		    }
		    else
		    if (!a)
		    {
			if (temp->buf[a] == '1')
			    full++;
			else
			if (temp->buf[a] == '0')
			    empty++;
		    }
		}

		// Not empty, show percent used
		if (empty < 4)
		{
		    AddObjectList( data->window, icon_pcent_used_objects );
		    DisplayObject(data->window,GetObject(data->list,GAD_PCENT_USED_KEY),0,data->pens[USEDPEN],0);

		    // Full ?
		    if (full == 3)
			SetGadgetValue( data->list, GAD_PCENT_USED, (ULONG)"100% ");
		    else
			SetGadgetValue( data->list, GAD_PCENT_USED, (ULONG)temp->buf );
		}

		// Not full, show percent free
		if (full < 3)
		{
		    AddObjectList( data->window, icon_pcent_free_objects );
		    DisplayObject(data->window,GetObject(data->list,GAD_PCENT_FREE_KEY),0,data->pens[FREEPEN],0);

		    // Empty ?
		    if (empty == 4)
		    {
			SetGadgetValue( data->list, GAD_PCENT_FREE, (ULONG)"100% ");
		    }
		    else
		    {
			sprintf(temp->buf,"%1.2f%%",SPSub(data->used_percent, (FLOAT)100));
			if (decimal_point != '.')
			{
			    for (a = 0; temp->buf[a];a++)
			    {
				// Fix decimal point
				if (temp->buf[a] == '.')
				{
				    temp->buf[a] = decimal_point;
				    break;
				}
			    }
			}
			SetGadgetValue( data->list, GAD_PCENT_FREE, (ULONG)temp->buf );
		    }
		}
	    }

	    // Fill handler
	    if (temp->handler)
		SetGadgetValue(data->list,GAD_HANDLER,(ULONG)temp->handler);

	    // Fill the key areas
	    DisplayObject(data->window,GetObject(data->list,GAD_USED_KEY),0,data->pens[USEDPEN],0);
	    DisplayObject(data->window,GetObject(data->list,GAD_FREE_KEY),0,data->pens[FREEPEN],0);

	    // Fill in used blocks
	    ItoaU( data->info.id_NumBlocksUsed, temp->buf, data->decimal_sep );
	    strcat(temp->buf,GetString(locale,MSG_BLOCKS_POSTFIX));
	    SetGadgetValue( data->list, GAD_USED, (ULONG)temp->buf );

	    // Fill in used bytes
	    GeometryToString(data->info.id_NumBlocksUsed, data->info.id_BytesPerBlock, temp->buf, 1, data->decimal_sep);
	    SetGadgetValue( data->list, GAD_USED_MB, (ULONG)temp->buf );

	    // Fill in free blocks
	    ItoaU( data->info.id_NumBlocks - data->info.id_NumBlocksUsed, temp->buf, data->decimal_sep );
	    strcat(temp->buf,GetString(locale,MSG_BLOCKS_POSTFIX));
	    SetGadgetValue( data->list, GAD_FREE, (ULONG)temp->buf );

	    // Fill in free bytes
	    GeometryToString(data->info.id_NumBlocks - data->info.id_NumBlocksUsed, data->info.id_BytesPerBlock, temp->buf, 1, data->decimal_sep);
	    SetGadgetValue( data->list, GAD_FREE_MB, (ULONG)temp->buf );

	    // Fill in total blocks
	    ItoaU( data->info.id_NumBlocks, temp->buf, data->decimal_sep );
	    strcat(temp->buf,GetString(locale,MSG_BLOCKS_POSTFIX));
	    SetGadgetValue( data->list, GAD_CAPACITY, (ULONG)temp->buf );

	    // Fill in total bytes
	    GeometryToString(data->info.id_NumBlocks, data->info.id_BytesPerBlock, temp->buf, 1, data->decimal_sep);
	    SetGadgetValue( data->list, GAD_CAPACITY_MB, (ULONG)temp->buf );

	    // Fill in device name
	    lsprintf(temp->buf,"%s %s", GetString(locale,MSG_DEV_NAME), data->prog_name);
	    SetGadgetValue( data->list, GAD_DEV_NAME, (ULONG)temp->buf );

	    // Fill in errors;
	    ItoaU( data->info.id_NumSoftErrors, temp->buf, data->decimal_sep );
	    SetGadgetValue( data->list, GAD_ERRORS, (ULONG)temp->buf );


	    // Fill in blocksize
	    ItoaU( data->info.id_BytesPerBlock, temp->buf+64, data->decimal_sep );

	    temp->buf[128]=0;
	    if (data->info.id_BytesPerBlock > 1024)
	    {
		DivideToString(temp->buf+192, data->info.id_BytesPerBlock, 1024, 1, data->decimal_sep);
		lsprintf(temp->buf+128, " (%sKB)", temp->buf+192);
	    }
	    lsprintf(temp->buf, "%s%s%s", temp->buf+64, GetString(locale,MSG_BYTES_POSTFIX), temp->buf+128);
	    SetGadgetValue( data->list, GAD_BLOCKSIZE, (ULONG)temp->buf );
	}

	// Fill out name field
	if (data->volume)
		SetGadgetValue(data->list,GAD_VOL_NAME,(ULONG)data->volume);

	if (data->icon_type != WBDISK)
	{
	    // Blocks
	    if (lock)
		Itoa( data->fib.fib_NumBlocks, temp->buf, data->decimal_sep );
	    else
		strcpy( temp->buf, "---" );

	    SetGadgetValue( data->list, GAD_ICON_BLOCKS, (ULONG)temp->buf );

	    // Bytes
	    if (lock)
	    {
		WORD len;

		if (bigfile)
		{
		    QtoaU( &data->f_size64, temp->buf, data->decimal_sep );
		    strcat(temp->buf,GetString(locale,MSG_BYTES_POSTFIX));
		    len=strlen(temp->buf);
		    temp->buf[len]=' ';
		    temp->buf[len+1]='(';
		    BytesToString64( &data->f_size64, temp->buf+len+2, 1, data->decimal_sep );
		}
		else
		{
		    ItoaU( data->fib.fib_Size, temp->buf, data->decimal_sep );
		    strcat(temp->buf,GetString(locale,MSG_BYTES_POSTFIX));
		    len=strlen(temp->buf);
		    temp->buf[len]=' ';
		    temp->buf[len+1]='(';
		    BytesToString( data->fib.fib_Size, temp->buf+len+2, 1, data->decimal_sep );
		}
		len=strlen(temp->buf);
		temp->buf[len]=')';
		temp->buf[len+1]=0;
	    }
	    SetGadgetValue( data->list, GAD_ICON_BYTES, (ULONG)temp->buf );

	    // Stack
	    if ((data->icon_type == WBPROJECT) || (data->icon_type == WBTOOL))
		SetGadgetValue( data->list, GAD_ICON_STACK, (data->icon->do_StackSize == 0) ? 4000 : data->icon->do_StackSize );

	    // Comment
	    if (lock)
		SetGadgetValue( data->list, GAD_ICON_COMMENT, (ULONG)data->fib.fib_Comment );
	    else
		DisableObject( data->list, GAD_ICON_COMMENT, TRUE );

	    // Name
	    {
	        UWORD a;

	        for (a = 0; data->object_name[a]; a++)
		    temp->buf[a] = data->object_name[a];

	        temp->buf[a] = 0;

	        if (a > 72)
	        {
		    SetGadgetValue( data->list, GAD_ICON_NAME3, (ULONG)(temp->buf + 72));
		    temp->buf[72] = 0;
	        }
	        if (a > 36)
	        {
		    SetGadgetValue( data->list, GAD_ICON_NAME2, (ULONG)(temp->buf + 36));
		    temp->buf[36] = 0;
	        }

	        SetGadgetValue( data->list, GAD_ICON_NAME1, (ULONG)temp->buf );

	    }

	    // Type
	    lsprintf( temp->buf, "%s", GetString(locale,icon_type_labels[data->label+1]) );
	    SetGadgetValue( data->list, GAD_ICON_TYPE, (ULONG)temp->buf );
	}

	// Date
	lsprintf( temp->buf, "%s %s", (data->datebuf[0]==' ')?data->datebuf+1:data->datebuf, data->timebuf );
	SetGadgetValue( data->list, GAD_ICON_LAST_CHANGED, (ULONG)temp->buf );

	// Default tool
	if (data->icon_type == WBPROJECT)
	    SetGadgetValue( data->list, GAD_ICON_DEFAULT_TOOL, (ULONG)data->icon->do_DefaultTool );

	if (data->icon_type == WBDISK)
	{

	    short id;

	    // Disk type
	    for (a = 0; disktype_lookup[a]; a += 2)
	    {
		if (data->filesystem)
		{
		    if (data->filesystem == disktype_lookup[a])
			break;
		}
		else
		if (data->info.id_DiskType == disktype_lookup[a] ||
		    data->disktype == disktype_lookup[a])
		{
		    break;
		}
	    }

	    SetGadgetValue( data->list, GAD_FILESYSTEM, (ULONG)GetString(locale,disktype_lookup[a+1]) );

	    // Get state
	    switch (data->info.id_DiskState)
	    {
		    // Validating
		    case ID_VALIDATING:
			    id=MSG_VALIDATING;
			    break;

		    // Write protected
		    case ID_WRITE_PROTECTED:
			    id=MSG_READ_ONLY;

			    // Disable name field
			    DisableObject(data->list,GAD_DEV_NAME,TRUE);
			    break;

		    // Read/write
		    default:
			    id=MSG_READ_WRITE;
			    break;
	    }

	    // Display state
	    SetGadgetValue(data->list,GAD_STATE,(ULONG)GetString(locale,id));
	}

	if ((data->icon_type == WBPROJECT) || (data->icon_type == WBTOOL))
	{
	    // Set tooltype list
	    SetGadgetChoices( data->list, GAD_ICON_TOOLTYPES, data->tool_list );

	    // Fix tooltype gadgets
	    icon_fix_toolgads( data, 0 );
	}
    #if 0
	// If invalid, disable save gadget
	if (!data->icon_name[0])
		DisableObject( data->list, GAD_ICON_SAVE, TRUE );
    #endif
	// Borderless?
	if (GetIconFlags(data->icon)&ICONF_BORDER_OFF)
	    data->menu_items[IIM_BORDERLESS].flags &= ~POPUPF_CHECKED;
	else
	if (GetIconFlags(data->icon)&ICONF_BORDER_ON)
	    data->menu_items[IIM_BORDERLESS].flags |= POPUPF_CHECKED;
	else
	if (GetLibraryFlags()&LIBDF_BORDERS_OFF)
	    data->menu_items[IIM_BORDERLESS].flags &= ~POPUPF_CHECKED;
	else
	    data->menu_items[IIM_BORDERLESS].flags |= POPUPF_CHECKED;

	// No label?
	if (GetIconFlags(data->icon)&ICONF_NO_LABEL)
	    data->menu_items[IIM_LABEL].flags &= ~POPUPF_CHECKED;
	else
	    data->menu_items[IIM_LABEL].flags |= POPUPF_CHECKED;
    }
    else return 0;

    FreeVec(temp);

    return 1;
}


// Show graph
void diskinfo_show_graph(icon_data *data,struct Rectangle *rect,ULONG size,ULONG total)
{
    long p,xp,yp,height,a,b,cx,cy,pc,y,chunkx=0,chunky=0;
    FLOAT rads,sin,cos,rx,ry,pcent,quot=0;
    struct RastPort *rp;
    short step,stop;

    // Reduce values if more than 2gb to fix rounding problems
    if (total&(1<<31))
    {
	total>>=1;
	size>>=1;
    }

    // Set pen
    rp=data->window->RPort;
    SetAPen(rp,DRAWINFO(data->window)->dri_Pens[TEXTPEN]);

    // Get height of pie, minimum 10
    height=rp->TxHeight;
    if (height<10) height=10;

    // Get center and radii
    a=(RECTWIDTH(rect))>>1;
    cx=rect->MinX+a;
    rx=SPFlt(a);
    b=((RECTHEIGHT(rect)-height))>>1;
    cy=rect->MinY+b;
    ry=SPFlt(b);

    // Get size of chunk as a percentage of the total, and convert to degrees
    pcent=(total<1)?(FLOAT)0:SPMul(quot = SPDiv(SPFlt(total),SPFlt(size)),(FLOAT)360);
    pcent=SPSub(pcent,(FLOAT)180);

    // Get as long and round to nearest 2
    pc=SPFix(pcent);
    if (pc&1) ++pc;
    if (pc>178) pc=-180;
    else
    if (pc<-177) pc=-179;

    // Set pen for areafill
    SafeSetOutlinePen(rp,rp->FgPen);
    SetAPen(rp,data->pens[(pc==-180)?FREEPEN:USEDPEN]);

    // Draw start
    AreaMove(rp,(pc<=-179)?rect->MinX:cx,cy);

    // Draw ellipse
    for (p=180;p>=-180;p-=2)
    {
	// Convert degrees to radians
	rads=SPDiv((FLOAT)180,SPMul((FLOAT)PI,SPFlt(p)));

	// Get sine and cosine
	sin=SPSincos(&cos,rads);

	// Get coordinates
	xp=SPFix(SPMul(rx,cos));
	yp=SPFix(SPMul(ry,sin));

	// Draw line
	AreaDraw(rp,cx+xp,cy+yp);

	// Is this the chunk position?
	if (p==pc)
	{
	    // End this area
	    AreaEnd(rp);

	    // Save position
	    chunkx=cx+xp;
	    chunky=cy+yp;

	    // All full?
	    if (pc<=-179) break;

	    // Begin for new area
	    AreaMove(rp,cx,cy);
	    AreaDraw(rp,chunkx,chunky);

	    // Set new colour
	    SetAPen(rp,data->pens[FREEPEN]);
	}
    }

    // End area
    AreaEnd(rp);

    // Start for bottom rim
    SetAPen(rp,data->pens[(pc==-180)?FREESHADOWPEN:USEDSHADOWPEN]);
    AreaMove(rp,rect->MinX,cy);

    // Draw bottom rim of ellipse
    p=180;
    step=-2;
    y=cy+height;
    stop=180;
    while (1)
    {
	// Convert degrees to radians
	rads=SPDiv((FLOAT)180,SPMul((FLOAT)PI,SPFlt(p)));

	// Get sine and cosine
	sin=SPSincos(&cos,rads);

	// Get coordinates
	xp=SPFix(SPMul(rx,cos));
	yp=SPFix(SPMul(ry,sin));

	// Draw line
	AreaDraw(rp,cx+xp,yp+y);

	// Finished?
	if (step>0 && p==stop)
	{
	    // End this area
	    AreaEnd(rp);

	    // Done the second bit?
	    if (stop<180 || pc<=0) break;

	    // Set new stop position
	    stop=pc;
	    step=-2;

	    // New chunk position is at the end
	    p=pc;
	    pc=0;

	    // Reset y-position
	    y=cy+height;

	    // Start for new chunk
	    SetAPen(rp,data->pens[FREESHADOWPEN]);
	    AreaMove(rp,chunkx,chunky);
	    continue;
	}

	// Is this the chunk position?
	if (step<0 && (p==pc || p<=0))
	{
	    // Move upwards and start going back
	    y-=height;
	    step=2;
	}

	// Increment position
	else p+=step;
    }

    // Turn off outline
    BNDRYOFF(rp);

    // Save used percent
    data->used_percent = (total<1)?(FLOAT)0:SPMul(quot, (FLOAT)100);
}


// Start dragging the icon image
static DragInfo *drag_start_window( icon_data *data, int xoff, int yoff )
{
    struct Image  *image;
    short          x, y;
    DragInfo      *draginf;
    struct BitMap  bitmap;

    data->drag_item = -1;

    image = data->image[data->image_num];

    // In case we pointed to invalid second image
    if (!image)
	    image = data->image[0];

    x = (1 + data->icon_area.MaxX-data->icon_area.MinX - image->Width)  >> 1;
    y = (1 + data->icon_area.MaxY-data->icon_area.MinY - image->Height) >> 1;

    if (draginf = GetDragInfo(
		    data->window,
		    0,
		    image->Width,
		    image->Height,
		    DRAGF_NEED_GELS | DRAGF_CUSTOM))	    // need_gels
    {
	draginf->flags |= (DRAGF_OPAQUE | DRAGF_TRANSPARENT);

	// Get icon as bitmap
	image_to_bitmap( data, image, &bitmap, data->window->RPort->BitMap->Depth );

	// Draw icon into drag info
	BltBitMapRastPort(&bitmap, 0, 0,
		          &draginf->drag_rp, 0, 0,
		          image->Width, image->Height,
		          0xc0 );

	// Make mask
	GetDragMask( draginf );

	// Get drag offset
	data->drag_x = xoff+x;
	data->drag_y = yoff+y;

	// Set pointer to draginfo so we know we're dragging
	data->draginfo = draginf;

	// Show initial drag image
	ShowDragImage(draginf,
		      data->window->WScreen->MouseX + data->drag_x,
		      data->window->WScreen->MouseY + data->drag_y );

	// Save old flags
	data->old_winflags = data->window->Flags;
	data->old_idcmp = data->window->IDCMPFlags;

	// Set mousemove reporting and ticks
	data->window->Flags |= WFLG_REPORTMOUSE | WFLG_RMBTRAP;
	data->window->Flags &= ~WFLG_WINDOWTICKED;
	ModifyIDCMP( data->window, data->window->IDCMPFlags | IDCMP_INTUITICKS | IDCMP_MOUSEBUTTONS );

	// Start deadlock timer
	StartTimer( data->timer, 0, 500000 );

	// Initialise tick count
	data->tick_count = 1;
	data->last_tick = 0;
    }

    return draginf;
}


// Start dragging a tooltype
static DragInfo *drag_start( icon_data *data, int item, int width, int height, int xoff, int yoff )
{
    DragInfo          *draginf;
    short              len;
    struct TextExtent  extent;
    Att_Node          *drag_node;

    data->drag_item = item;

    // Find drag node

    if (!(drag_node = Att_FindNode( data->tool_list, item )))
	return 0;

    // We want to drag outside the window.
    // Docs say pass NULL for window, RastPort * for rastport.
    // In fact, we still pass our window, and NULL for rastport.

    if (draginf = GetDragInfo(data->window,   // window,
		              0,		      // rastport
		              width,
		              height,
		              DRAGF_NEED_GELS | DRAGF_CUSTOM))	      // need_gels
    {
	// Set pens and font
	SetAPen(&draginf->drag_rp,DRAWINFO(data->window)->dri_Pens[FILLTEXTPEN]);
	SetDrMd(&draginf->drag_rp,JAM1);
	SetRast(&draginf->drag_rp,DRAWINFO(data->window)->dri_Pens[FILLPEN]);
	SetFont(&draginf->drag_rp,data->window->RPort->Font);

	// Get length that will fit
	len=TextFit(data->window->RPort,
		    drag_node->node.ln_Name,
		    strlen(drag_node->node.ln_Name),
		    &extent,
		    0,1,
		    draginf->width-2,draginf->height);

	// Draw text
	Move(&draginf->drag_rp,2,draginf->drag_rp.TxBaseline);
	Text(&draginf->drag_rp,
	     drag_node->node.ln_Name,
	     len);

	// Build the drag shadow mask
	// Don't just overwrite the flags.  They already contain DRAGF_TRANSPARENT.
	// And the system will barf without it.

	draginf->flags |= DRAGF_OPAQUE;

	// Create mask
	GetDragMask( draginf );

	// Get drag offset
	data->drag_x = xoff;
	data->drag_y = yoff;

	// Set pointer to draginfo so we know we're dragging
	data->draginfo = draginf;

	// Show initial drag image
	ShowDragImage(draginf,
		      data->window->WScreen->MouseX + xoff,
		      data->window->WScreen->MouseY + yoff );

	// Save old flags
	data->old_winflags = data->window->Flags;
	data->old_idcmp = data->window->IDCMPFlags;

	// Set mousemove reporting and ticks
	data->window->Flags |= WFLG_REPORTMOUSE | WFLG_RMBTRAP;
	data->window->Flags &= ~WFLG_WINDOWTICKED;
	ModifyIDCMP( data->window, data->window->IDCMPFlags | IDCMP_INTUITICKS | IDCMP_MOUSEBUTTONS );

	// Start deadlock timer
	StartTimer( data->timer, 0, 500000 );

	// Initialise tick count
	data->tick_count = 1;
	data->last_tick = 0;
    }

    return draginf;
}


static void drag_arrange( icon_data *data, int swap )
{
    GL_Object *obj;
    int        x, y;
    ULONG      drop_item;
    Att_Node   *drag_node, *drop_node;

    if (obj = GetObject( data->list, GAD_ICON_TOOLTYPES ))
    {
	x = data->screen->MouseX - data->window->LeftEdge;
	y = data->screen->MouseY - data->window->TopEdge;

	// Dropped on listview?
	if (CheckObjectArea( obj, x, y ))
	{
	    drop_item = (x << 16) | y;

	    GetAttr( GLV_GetLine, GADGET(obj), &drop_item );

	    // Item dragged onto itself?
	    if (data->drag_item == drop_item)
		    return;

	    // Find drag node
	    drag_node = Att_FindNode( data->tool_list, data->drag_item );

	    if (drag_node)
	    {
		// Detach item list using SetGadgetChoices
		SetGadgetChoices(data->list,GAD_ICON_TOOLTYPES,(APTR)-1 );

		if (!swap)
		    Remove( (struct Node *)drag_node );

		// Find drop node
		if (drop_item != -1)
		    drop_node = Att_FindNode( data->tool_list, drop_item );
		else
		    drop_node = 0;

		if (drop_node)
		{
		    if (swap)
		    {
			// Swap two entries?
			SwapListNodes((struct List *)data->tool_list,
				      (struct Node *)drag_node,
				      (struct Node *)drop_node );
		    }
		    else
			Insert( (struct List *)data->list, (struct Node *)drag_node, (struct Node *)drop_node->node.ln_Pred );
		}
		// Otherwise drag entry to end of list
		else
		    AddTail( (struct List *)data->tool_list, (struct Node *)drag_node );

		// Reattach item list using SetGadgetChoices
		SetGadgetChoices( data->list, GAD_ICON_TOOLTYPES, data->tool_list );
	    }
	}
    }
}


static int drag_to_iconinfo( icon_data *data, IPCData *ipc, int x, int y )
{
    Att_Node *node;
    Point    *pos;
    int       ok = 0;

    if (pos = AllocVec( sizeof(Point), MEMF_CLEAR ))
    {
	pos->x = x;
	pos->y = y;

	if (data->drag_item == -1)
	{
	    IPC_Command( ipc, ICONINFO_ICON, 0, data->name, pos, 0 );
	    ok = 1;
	}
	else if (node = Att_FindNode( data->tool_list, data->drag_item ))
	{
	    IPC_Command( ipc, ICONINFO_TOOLTYPE, 0, node->node.ln_Name, pos, 0 );
	    ok = 1;
	}

	if (!ok)
	    FreeVec( pos );
    }

    return ok;
}


// Set the WBArg for the icon editor
BOOL icon_set_wbarg( GalileoAppMessage *msg, char *name )
{
    BPTR  lock;
    BOOL  success = FALSE;
    char *buf, *file;

    if (name[0])
    {
	if (buf = AllocVec( strlen(name) + 2, MEMF_ANY ))
	{
	    strcpy( buf, name );

	    file = FilePart( buf );

	    memmove( file + 1, file, strlen(file) + 1 );

	    *file++ = 0;

	    if (lock = Lock( buf, ACCESS_READ ))
	    {
		if (SetWBArg( msg, 0, lock, file, 0 ))
		    success = TRUE;

		UnLock( lock );
	    }
	    FreeVec( buf );
	}
    }

    return success;
}


// Open a temporary file (taken from Jon's code somewhere)
BPTR open_temp_file( char *filename, IPCData *ipc )
{
    unsigned short a;
    ULONG          micros, secs;
    int            temp_key;
    BPTR           file=0;

    // Get temporary key
    temp_key = (ULONG)ipc;
    CurrentTime( &secs, &micros );
    if (micros)
	temp_key *= micros;
    temp_key += secs;

    // Create a temporary icon file; try up to 99 times
    for (a = 0; a < 99; a++)
    {
	// Build icon name
	lsprintf( filename, "T:galileo-%lx-tmp.info", temp_key + a );

	// Try to open file
	if (file = Open( filename, MODE_NEWFILE ))
	    break;
    }

    return file;
}


// Save a temporary copy of the icon
BOOL icon_save_temp( icon_data *data, char *name )
{
    BPTR tempfile;
    BOOL ok = FALSE;

    if (tempfile = open_temp_file( data->tempname, data->ipc ))
    {
	Close( tempfile );

	// Strip .info from name
	data->tempname[strlen(data->tempname)-5] = 0;

	ok = icon_save( data, data->tempname ,FALSE);
    }

    return ok;
}


// Send an app message
BOOL icon_send_appmsg( icon_data *data, struct Window *window, char *name )
{
    struct AppWindow  *appwin;
    struct MsgPort    *replyport, *port;
    GalileoAppMessage   *msg;
    BOOL               ok = FALSE;

    if (appwin = WB_FindAppWindow( window ))
    {
	if (replyport = CreateMsgPort())
	{
	    if (msg = AllocAppMessage( 0, replyport, 1 ))
	    {
		if (icon_set_wbarg( msg, name ))
		{
		    ok = TRUE;

		    port = WB_AppWindowData(appwin,
			                    &msg->ga_Msg.am_ID,
			                    &msg->ga_Msg.am_UserData);

		    // Fill out AppMessage info
		    msg->ga_Msg.am_Type = MTYPE_APPWINDOW;
		    msg->ga_Msg.am_MouseX = 30;
		    msg->ga_Msg.am_MouseY = 30;

		    PutMsg( port, (struct Message *)msg );

		    while (1)
		    {
			WaitPort( replyport );

			while (1)
			{
			    if (GetMsg( replyport ))
				goto breakout;
			}
		    }
		    breakout:;
		}
		FreeAppMessage( msg );
	    }
	    DeleteMsgPort( replyport );
	}
	// FindAppWindow left us in (nested) Forbid()
	Permit();
    }

    return ok;
}


static void drag_end( icon_data *data, int option )
{
    int x, y;
    int ok = 0;

    x = data->screen->MouseX;
    y = data->screen->MouseY;

    // Free drag stuff
    FreeDragInfo( data->draginfo );
    data->draginfo = 0;

    // Restore window flags & IDCMP
    data->window->Flags = data->old_winflags;
    ModifyIDCMP( data->window, data->old_idcmp );

    // Abort timer
    StopTimer( data->timer );

    // Drag dropped?
    if (option)
    {
	struct Window *win = 0;
	struct Layer  *layer;

	LockLayerInfo( &data->screen->LayerInfo );

	if (layer = WhichLayer( &data->screen->LayerInfo, x, y ))
	    win = layer->Window;

	UnlockLayerInfo( &data->screen->LayerInfo );

	if (win)
	{
	    // Tooltype dropped same window?
	    if (win == data->window)
	    {
		if (data->drag_item != -1)
		    drag_arrange( data, option == 2 ? 1 : 0 );

		ok = 1;
	    }

	    // Icon or tooltype dropped on some other window
	    else
	    {
		IPCData *win_ipc;
		ULONG win_id;

		// Get window ID
		win_id = GetWindowID( win );

		Forbid();

		// Got IPC?
		if (win_ipc = (IPCData *)GetWindowAppPort( win ))
		{
		    if (win_id == WINDOW_ICONINFO)
			ok = drag_to_iconinfo( data, win_ipc, x, y );
		}

		// Icon dropped on non-Galileo window?
		else if (data->drag_item == -1)
		{
		    short res = 2;

		    if (data->modified)
		    {
			data->busy = TRUE;
			SetWindowBusy( data->window );

			res = AsyncRequestTags(
			data->ipc,
			REQTYPE_SIMPLE,
			win,
			0,
			0,
			AR_Window,	win,
			AR_Message,	GetString(locale,MSG_ICON_MODIFIED),
			AR_Button,	GetString(locale,MSG_ICON_SAVE),
			AR_Button,	GetString(locale,MSG_ICON_DONT_SAVE),
			AR_Button,	GetString(locale,MSG_ICON_CANCEL),
			TAG_DONE );

			ClearWindowBusy( data->window );
			data->busy = FALSE;
		    }

		    switch (res)
		    {
			case 1:
			    if (!icon_save( data, data->icon_name,TRUE))
				break;
			case 2:
			    ok = icon_send_appmsg( data, win, data->name );
			    break;
			case 0:
			default:
			    ok = 1;
			    break;
		    }
		}

		Permit();
	    }
	}
    }

    // Drag aborted?
    else
	ok = 1;

    if (!ok)
	DisplayBeep( data->screen );
}


// Drop icon on our window
void icon_drop( icon_data *data, int x, int y )
{
    struct IntuiMessage *msg;
    struct DiskObject   *icon;
    short                len;
    int                  failed = 0;

    if (IconBase->lib_Version>=44)
    {
	icon_drop_44(data,x,y);
	return;
    }


    // Disk icon?
    if (data->name[strlen(data->name)-1]==':')
	strcat(data->name,"Disk");

    // Already have a .info suffix? Strip it if so
    if ((len = strlen(data->name)) > 5 &&
	stricmp(data->name+len-5,".info") == 0)
    {
	data->name[len-5] = 0;
    }

    // Try and get icon
    if (icon = GetCachedDiskObject( data->name, GCDOF_NOCACHE ))
    {
	int   area = 0;
	ULONG old_idcmp = data->window->IDCMPFlags;
	int   image_changed = 0;
	short res = -1;

	// Dropped in icon area?
	if (CheckObjectArea( GetObject( data->list, GAD_ICON_IMAGE ), x, y ))
	    area = AREA_TYPE_ICON_IMAGE;

	// Dropped in tooltypes area?
	else if (CheckObjectArea( GetObject( data->list, GAD_ICON_TOOLTYPES ), x, y ))
	    area = AREA_TYPE_ICON_TOOLTYPES;

	// Initialize poup menu options
	NewList((struct List *)&data->dropmenu);

	// Add icon options if not in tooltypes area
	if (area != AREA_TYPE_ICON_TOOLTYPES)
	{
	    data->dropmenu_items[IDM_OLDICONS].flags  |= POPUPF_DISABLED;
	    data->dropmenu_items[IDM_NEWICONS].flags  |= POPUPF_DISABLED;
	    data->dropmenu_items[IDM_BOTHICONS].flags |= POPUPF_DISABLED;
	    AddTail((struct List *)&data->dropmenu,(struct Node *)&data->dropmenu_items[IDM_OLDICONS]);
	    AddTail((struct List *)&data->dropmenu,(struct Node *)&data->dropmenu_items[IDM_NEWICONS]);
	    AddTail((struct List *)&data->dropmenu,(struct Node *)&data->dropmenu_items[IDM_BOTHICONS]);
	}

	// Remap icon
	if (RemapIcon( icon, data->window->WScreen, 0 ))
	{
	    struct DiskObject *old;

	    // Add 'copy new' to drop menu
	    if (area != AREA_TYPE_ICON_TOOLTYPES)
		    data->dropmenu_items[IDM_NEWICONS].flags &= ~POPUPF_DISABLED;

	    // Does it have a valid old image?
	    if ((old = GetOriginalIcon( icon )) &&
		    old->do_Gadget.Width  > 1 &&
		    old->do_Gadget.Height > 1)
	    {
		// Add 'copy both' to drop menu
		if (area != AREA_TYPE_ICON_TOOLTYPES)
		    data->dropmenu_items[IDM_BOTHICONS].flags &= ~POPUPF_DISABLED;

		// Enable 'copy old' in drop menu
		if (area != AREA_TYPE_ICON_TOOLTYPES)
		    data->dropmenu_items[IDM_OLDICONS].flags &= ~POPUPF_DISABLED;

		// Fix selection
		data->menu_items[IIM_NEWICONS].flags |= POPUPF_CHECKED;
		data->menu_items[IIM_OLDICONS].flags &= ~POPUPF_CHECKED;
	    }
	}

	// Otherwise clear NewIcon path
	else
	{
	    // add 'copy old' to drop menu
	    if (area != AREA_TYPE_ICON_TOOLTYPES)
		data->dropmenu_items[IDM_OLDICONS].flags &= ~POPUPF_DISABLED;
	}

	// Add 'copy tooltypes' to dropmenu
	if (area == 0 && data->icon_type != WBDISK && icon->do_ToolTypes && icon->do_ToolTypes[0])
	{
	    AddTail((struct List *)&data->dropmenu,(struct Node *)&data->dropmenu_items[IDM_BAR]);
	    AddTail((struct List *)&data->dropmenu,(struct Node *)&data->dropmenu_items[IDM_TOOLTYPES]);
	}

	// Dropped in tooltypes area?
	if (area == AREA_TYPE_ICON_TOOLTYPES)
	{
	    if (icon->do_ToolTypes && icon->do_ToolTypes[0])
		res = MENU_COPY_TOOLTYPES;
	    else
		failed = 1;
	}

	// Else show popup
	else
	{
	    ActivateWindow(data->window);

	    // Disable intuiticks
	    ModifyIDCMP( data->window, data->window->IDCMPFlags & ~ IDCMP_INTUITICKS );

	    // Flush port
	    while (msg = GetWindowMsg( data->window->UserPort ))
		ReplyWindowMsg( msg );

	    // Re-enable intuiticks
	    ModifyIDCMP( data->window, old_idcmp );

	    // Popup menu
	    res = DoPopUpMenu( data->window, &data->dropmenu, 0, SELECTDOWN );
	}

	switch (res)
	{
	    // Popup aborted
	    case -1:

		    break;

	    case MENU_COPY_OLDICON:

		    image_changed = 1;

		    data->icon_mode = 0;

		    // The old image is no longer stripped
		    data->strip_oldicon = 0;

		    if (data->which_images == 2)
			    data->which_images = 1;

		    // Free old oldicon
		    if (data->new_oldicon && data->new_oldicon != data->new_newicon)
		    {
			// Free remapping
			RemapIcon( data->new_oldicon, (data->window) ? data->window->WScreen : 0, 1 );

			// Free icon
			FreeCachedDiskObject( data->new_oldicon );
		    }

		    // Store custom icon
		    data->new_oldicon = icon;

		    // Enable oldicon menu entries
		    if (data->which_images == 1)
		    {
			data->menu_items[IIM_OLDICONS].flags       &= ~POPUPF_DISABLED;
			data->menu_items[IIM_STRIP_OLD].flags      &= ~POPUPF_DISABLED;
			data->menu_items[IIM_NEWICONS].flags       &= ~POPUPF_DISABLED;
			data->menu_items[IIM_STRIP_NEWICONS].flags &= ~POPUPF_DISABLED;
		    }

		    break;

	    case MENU_COPY_NEWICON:

		    image_changed = 1;

		    data->icon_mode = 1;

		    // The new image is no longer stripped
		    data->strip_newicon = 0;

		    if (data->which_images == 0)
			    data->which_images = 1;

		    // Free old newicon
		    if (data->new_newicon && data->new_newicon != data->new_oldicon)
		    {
			// Free remapping
			RemapIcon( data->new_newicon, (data->window) ? data->window->WScreen : 0, 1 );

			// Free icon
			FreeCachedDiskObject( data->new_newicon );
		    }

		    // Store custom icon
		    data->new_newicon = icon;

		    // Enable newicon menu entries
		    if (data->which_images == 1)
		    {
			data->menu_items[IIM_OLDICONS].flags       &= ~POPUPF_DISABLED;
			data->menu_items[IIM_STRIP_OLD].flags      &= ~POPUPF_DISABLED;
			data->menu_items[IIM_NEWICONS].flags       &= ~POPUPF_DISABLED;
			data->menu_items[IIM_STRIP_NEWICONS].flags &= ~POPUPF_DISABLED;
		    }

		    // Save path
		    strcpy( data->newicon_path, data->name );

		    break;

	    case MENU_COPY_BOTHICONS:

		    image_changed = 1;

		    // The old and new images are no longer stripped
		    data->strip_oldicon = 0;
		    data->strip_newicon = 0;

		    data->which_images = 1;

		    // Free old oldicon
		    if (data->new_oldicon)
		    {
			if (data->new_oldicon == data->new_newicon)
				data->new_newicon = 0;

			// Free remapping
			RemapIcon( data->new_oldicon, (data->window) ? data->window->WScreen : 0, 1 );

			// Free icon
			FreeCachedDiskObject( data->new_oldicon );
		    }

		    // Free old newicon
		    if (data->new_newicon)
		    {
			// Free remapping
			RemapIcon( data->new_newicon, (data->window) ? data->window->WScreen : 0, 1 );

			// Free icon
			FreeCachedDiskObject( data->new_newicon );
		    }

		    // Store custom icon
		    data->new_oldicon = icon;
		    data->new_newicon = icon;

		    // Enable both old and new icon menu entries
		    data->menu_items[IIM_OLDICONS].flags       &= ~POPUPF_DISABLED;
		    data->menu_items[IIM_STRIP_OLD].flags      &= ~POPUPF_DISABLED;
		    data->menu_items[IIM_NEWICONS].flags       &= ~POPUPF_DISABLED;
		    data->menu_items[IIM_STRIP_NEWICONS].flags &= ~POPUPF_DISABLED;

		    break;

	    case MENU_COPY_TOOLTYPES:
		    {
			int num;

			data->modified = TRUE;

			// Remove list from lister
			SetGadgetChoices( data->list, GAD_ICON_TOOLTYPES, (APTR)~0 );

			// Add tooltypes
			for (num = 0; icon->do_ToolTypes[num]; ++num)
				tooltype_newnode( data->tool_list, icon->do_ToolTypes[num], 0, 0 );

			// Attach list to lister
			SetGadgetChoices( data->list, GAD_ICON_TOOLTYPES, data->tool_list );
		    }
		    break;
	}

	// Did we get a new icon image?
	if (image_changed)
	{
	    data->modified = TRUE;

	    // Get icon image pointers
	    icon_switch_image( data, data->icon_mode );
	}

	// Free icon if we're not using it
	if (icon && icon != data->new_oldicon && icon != data->new_newicon)
	{
	    // Free remapping
	    RemapIcon( icon, (data->window) ? data->window->WScreen : 0, 1 );

	    // Free icon
	    FreeCachedDiskObject( icon );
	}
    }
    else
	failed = 1;

    // Failed
    if (failed)
	DisplayBeep(data->window->WScreen);
}


// Reload icon after editing
BOOL icon_edit_reload( icon_data *data )
{
    BOOL               result = FALSE;
    struct DiskObject *obj;
    long               old_type = data->icon_type;

    // Temporary name set?
    if (*data->tempname)
    {
	// Get icon from Galileo
	if (obj = (struct DiskObject *)IPC_Command( data->main_ipc, MAINCMD_GET_ICON, GCDOF_NOCACHE, data->tempname, 0, REPLY_NO_PORT ))
	{
	    data->modified = 0;

	    // Free old icon data
	    icon_free( data );

	    data->icon = obj;

	    // Store type
	    data->icon_type = data->icon->do_Type;

	    if (data->icon_type == WBDEVICE || data->icon_type == WBKICK)
		data->icon_type = WBDISK;

	    // Need to reopen window?
	    if (data->icon_type != old_type)
	    {
		closewindow( data );

		openwindow( data, /*next ? 1 :*/ 0 );
	    }
	    else
	    {

		// Remove old tooltypes
		Att_RemList( data->tool_list, 0 );

		// Build new tooltype list
		icon_build_tooltypes( data );

		// Disable old and new icon menu entries by default
		data->menu_items[IIM_OLDICONS].flags       |= POPUPF_DISABLED;
		data->menu_items[IIM_NEWICONS].flags       |= POPUPF_DISABLED;
		data->menu_items[IIM_STRIP_OLD].flags      |= POPUPF_DISABLED;
		data->menu_items[IIM_STRIP_NEWICONS].flags |= POPUPF_DISABLED;

		// Remap icon
		icon_remap( data );

		// Get icon image pointers
		data->image[0] = (struct Image *)data->icon->do_Gadget.GadgetRender;
		data->image[1] = (struct Image *)data->icon->do_Gadget.SelectRender;

		// Get icon display area
		GetObjectRect( data->list, GAD_ICON_IMAGE, &data->icon_area );

		// Display icon
		icon_switch_image( data, data->icon_mode );

		if (data->icon_type != WBDISK)
		{
		    // Remove old protections
		    Att_RemList( data->prot_list[0], 0 );
		    Att_RemList( data->prot_list[1], 0 );
		    Att_RemList( data->prot_list[2], 0 );
		    Att_RemList( data->prot_list[3], 0 );

		    // Build protection list
		    icon_build_protlist( data );
		}

		// Stack
		SetGadgetValue( data->list, GAD_ICON_STACK, (data->icon->do_StackSize == 0) ? 4000 : data->icon->do_StackSize );

		// Default tool
		SetGadgetValue( data->list, GAD_ICON_DEFAULT_TOOL, (ULONG)data->icon->do_DefaultTool );

		// Set tooltype list
		SetGadgetChoices( data->list, GAD_ICON_TOOLTYPES, data->tool_list );

		// Fix tooltype gadgets
		icon_fix_toolgads( data, 0 );

		// Borderless?
		if (GetIconFlags( data->icon ) & ICONF_BORDER_OFF)
		    data->menu_items[IIM_BORDERLESS].flags &= ~POPUPF_CHECKED;
		else
		if (GetIconFlags( data->icon ) & ICONF_BORDER_ON)
		    data->menu_items[IIM_BORDERLESS].flags |= POPUPF_CHECKED;
		else
		if (GetLibraryFlags() & LIBDF_BORDERS_OFF)
		    data->menu_items[IIM_BORDERLESS].flags &= ~POPUPF_CHECKED;
		else
		    data->menu_items[IIM_BORDERLESS].flags |= POPUPF_CHECKED;

		// No label?
		if (GetIconFlags( data->icon ) & ICONF_NO_LABEL)
		    data->menu_items[IIM_LABEL].flags &= ~POPUPF_CHECKED;
		else
		    data->menu_items[IIM_LABEL].flags |= POPUPF_CHECKED;
	    }

	    // Clear 'strip' flags
	    data->strip_newicon = 0;
	    data->strip_oldicon = 0;
	}

	// Delete temporary file
	DeleteDiskObject( data->tempname );

	data->tempname[0] = 0;
    }

    return result;
}


// Edit icon with custom editor
BOOL icon_edit_external( icon_data *data )
{
    char command[256];
    BOOL ok = FALSE;

    if (icon_save_temp( data, data->name ))
    {
	lsprintf( command, "%s %s", data->edit_command, data->tempname );

	if (WB_LaunchNotify(command,
		            data->screen,
		            1,		    // wait,
		            4096,	    // stack,
		            0,		    // default_tool,
		            0,		    // process
		            data->ipc,	    // notify_ipc,
		            0 ))	    // flags
	{
	    ok = TRUE;
	    data->busy = TRUE;
	    SetWindowBusy( data->window );
	}
    }

    if (!ok)
	    DisplayBeep( data->screen );

    return ok;
}


// Edit icon image
BOOL icon_edit( icon_data *data )
{
    ULONG           sigs;
    GalileoNotify  *nmsg;
    struct Screen  *screen;
    struct Window  *window;
    struct Process *process = 0;
    BOOL            ok = FALSE;

    if (!data->use_iconedit)
	    return icon_edit_external( data );

    // Flush notify port
    while (nmsg = (GalileoNotify *)GetMsg( data->notify_port ))
	ReplyFreeMsg( nmsg );

    // Launch IconEdit
    if (WB_LaunchNotify(
	    data->edit_command,
	    data->screen,
	    1,				    // wait,
	    4096,			    // stack,
	    0,				    // default_tool,
	    &process,
	    data->ipc,			    // notify_ipc,
	    LAUNCHF_OPEN_UNDER_MOUSE ))	    // flags
    {
	data->busy = TRUE;
	SetWindowBusy( data->window );

	// Wait for 5 seconds for appwindow to be added
	StartTimer( data->timer, 5, 0 );

	while (1)
	{
	    sigs = Wait( (1 << data->notify_port->mp_SigBit)
		    | (1 << data->timer->port->mp_SigBit) );

	    // Notify messages?
	    if (sigs & (1 << data->notify_port->mp_SigBit))
	    {
		while (nmsg = (GalileoNotify *)GetMsg( data->notify_port ))
		{
		    // AppWindow added?
		    if (nmsg->gn_Type == GN_APP_WINDOW_LIST)
		    {
			if (!nmsg->gn_Flags)
			    if (window = WB_AppWindowWindow( (struct AppWindow *)nmsg->gn_Data ))
				if (window->UserPort && window->UserPort->mp_SigTask == process)
				    goto breakout;
		    }
		    else
			kprintf( "** Icon module: Unexpected notify %lx\n", nmsg->gn_Type );

		    ReplyFreeMsg( nmsg );
		}
	    }

	    // Timer signal?
	    if (sigs & (1 << data->timer->port->mp_SigBit ))
		if (CheckTimer( data->timer ))
		    break;
	}

	breakout:

	StopTimer( data->timer );

	if (screen = LockPubScreen( 0 ))
	{
	    Forbid();

	    for (window = screen->FirstWindow; window; window = window->NextWindow)
		if (process == window->UserPort->mp_SigTask)
		    break;

	    if (window)
		if (icon_save_temp( data, data->name ))
		    if (icon_send_appmsg( data, window, data->tempname ))
			ok = TRUE;

	    Permit();

	    UnlockPubScreen( 0, screen );
	}
    }

    if (!ok)
	DisplayBeep( data->screen );

    return ok;
}


// Pick icon editor
void icon_pick_editor( icon_data *data )
{
    char  buffer[256];
    short use_iconedit = data->use_iconedit;
    long  res;

    stccpy( buffer, data->edit_command, 256 );

    data->busy = TRUE;
    SetWindowBusy( data->window );

    res = AsyncRequestTags(data->ipc,
	                   REQTYPE_SIMPLE,
	                   data->window,
	                   0,
	                   0,
	                   AR_Window,	   data->window,
	                   AR_Title,	   GetString(locale,MSG_PICK_EDITOR),
	                   AR_Message,	   GetString(locale,MSG_ENTER_FUNCTION),
	                   AR_Button,	   GetString(locale,MSG_ICON_SAVE),
	                   AR_Button,	   GetString(locale,MSG_USE),
	                   AR_Button,	   GetString(locale,MSG_ICON_CANCEL),
	                   AR_Buffer,	   buffer,
	                   AR_BufLen,	   256,
	                   AR_CheckMark,   GetString(locale,MSG_USE_ICONEDIT),
	                   AR_CheckPtr,	   &use_iconedit,
	                   AR_Flags,	   SRF_CHECKMARK,
	                   AR_Requester,   WINREQUESTER(data->window),
	                   TAG_DONE );

    ClearWindowBusy( data->window );
    data->busy = FALSE;

    if (res)
    {
	stccpy( data->edit_command, buffer, 256 );
	data->use_iconedit = use_iconedit;

	// Save?
	if (res == 1 || res == 65535)
	{
	    lsprintf( buffer, "%ld/%s", data->use_iconedit, data->edit_command );
	    SetEnv( "Galileo/Icon Editor", buffer, TRUE );
	}
    }
}

// Get icon filesize
void get_icon_fsize(icon_data *data)
{
    struct FileInfoBlock *fib;
    struct DiskObject *obj;
    BPTR lock;
    BOOL disk=FALSE;

    if(strlen(data->nameptr)==1 && data->nameptr[0]==':')
	disk=TRUE;

    if (obj=GetDiskObject((disk)?"disk":data->nameptr))
    {
	if (fib=AllocDosObject(DOS_FIB,NULL))
	{
	    if(!disk)
		lsprintf(data->buffer, "%s.info", data->nameptr);

	    if (lock = Lock((disk)?"disk.info":data->buffer, ACCESS_READ))
	    {
		Examine(lock,fib);

		data->icon_filesize = fib->fib_Size;

		UnLock(lock);
	    }
	    FreeDosObject(DOS_FIB,fib);
	}
	FreeDiskObject(obj);
    }
}

// Icon information
short icon_info( icon_data *data, char *name, struct Node *next )
{
    short a, ret = 1;
    BPTR  lock;

    // Copy name
    strcpy( data->name, name );

    // Try to lock file
    if (lock = Lock( name, ACCESS_READ ))
    {
	// Use WB info?
	if (data->wb_info)
	{
	    BPTR parent;

	    // Get parent dir
	    if (!(parent = ParentDir( lock )))
	    {
		// No parent; use main lock
		parent = lock;

		// Disk, presumably
		strcpy( data->name,"Disk" );
	    }

	    // Unlock main lock
	    else
		UnLock(lock);

	    // Strip .info suffix if applicable
	    if ((a = strlen(data->name)) > 5 && stricmp( data->name+a-5, ".info" ) == 0)
		data->name[a-5] = 0;

	    // Show info
	    WBInfo( parent, FilePart( data->name ), data->screen );

	    // Unlock dirs
	    UnLock( parent );
	    return 1;
	}

	// Examine file
	Examine( lock, &data->fib );

	// Get full path
	DevNameFromLock( lock, data->name, 256 );

	// Get path for location field
	NameFromLock( lock, data->path, 256 );

	// Unlock lock
	UnLock( lock );
    }

    // Special case for disk icons
    else if (stricmp( name, "disk" ) == 0)
    {
	// Get current dir
	if (lock = Lock( "", ACCESS_READ ))
	{
	    // Get path of current dir
	    DevNameFromLock( lock, data->name, 256 );
	    UnLock( lock );

	    // Add on "disk"
	    AddPart( data->name, "Disk", 256 );
	}
    }

    // Otherwise, no file
    else
    {
	// Copy name and add .info
	strcpy( data->path, name );
	strcat( data->path, ".info" );

	// Try to lock file
	if (lock = Lock( data->path, ACCESS_READ ))
	{
	    // Get path for location field
	    NameFromLock( lock, data->path, 256 );

	    // Store as full name
	    DevNameFromLock( lock, data->name, 256 );

	    // Unlock lock
	    UnLock( lock );
	}
	else
	    strcpy( data->path, "???" );
    }

    // Check if this really is a icon before stripping .info
    if (data->icon=GetDiskObject((strlen(name)==1 && name[0]==':')?"disk":name))
    {
	// Strip .info suffix if applicable
	if ((a = strlen(data->name)) > 5 && stricmp(data->name+a-5, ".info") == 0)
	    data->name[a-5] = 0;

	FreeDiskObject(data->icon);
    }

    data->icon=0;

    if (data->name[strlen(data->name)-1] == ':')
	strcat( data->name, "Disk" );

    data->nameptr = name;

    // Get icon from Galileo
    data->icon = (struct DiskObject *)IPC_Command( data->main_ipc, MAINCMD_GET_ICON, GCDOF_NOCACHE, data->name, 0, REPLY_NO_PORT );

    // Store icon name
    strcpy( data->icon_name, data->name );

    // Invalid icon?
    if (!data->icon)
    {
	icon_free( data );
	return 1;
    }

    // Store type
    data->icon_type = data->icon->do_Type;

    // Store program name
    strcpy( data->prog_name, data->name );

    // Default to newicon image
    data->icon_mode = 1;

    if (!openwindow( data, next ? 1 : 0 ))
    {
	icon_free( data );
	return 1;
    }

    // Clear 'strip' flags
    data->strip_newicon = 0;
    data->strip_oldicon = 0;

    // Event loop
    FOREVER
    {
	struct IntuiMessage *msg;
	struct AppMessage   *amsg;
	GalileoNotify         *nmsg;
	IPCMessage          *imsg;
	BOOL                 quit_flag=0;

	// AppMessages?
	if (data->app_window)
	{
	    // Get messages
	    while (amsg = (struct AppMessage *)GetMsg( data->app_port ))
	    {
		// Valid argument?
		if (amsg->am_NumArgs > 0 && !data->busy)
		{
		    // Get icon name
		    GetWBArgPath( &amsg->am_ArgList[0], data->name, 256 );

		    icon_drop( data, amsg->am_MouseX, amsg->am_MouseY );
		}

		// Reply message
		ReplyMsg( (struct Message *)amsg );
	    }
	}

	// Notify messages?
	if (data->notify_port)
	{
	    while (nmsg = (GalileoNotify *)GetMsg( data->notify_port ))
	    {
		if (nmsg->gn_Type != GN_APP_WINDOW_LIST)
		    kprintf( "** Icon module: Ignoring notify %lx\n", nmsg->gn_Type );

		ReplyFreeMsg( nmsg );
	    }
	}

	// IPC messages?
	if (data->ipc)
	{
	    while (imsg = (IPCMessage *)GetMsg(data->ipc->command_port))
	    {
		Point *p;

		// Abort?
		if ((imsg->command == IPC_ABORT || imsg->command == IPC_QUIT) && !data->busy)
		{
		    quit_flag = 1;
		    ret = 0;
		}

		else if (imsg->command == ICONINFO_TOOLTYPE && !data->busy)
		{
		    Att_Node  *node;
		    GL_Object *obj;
		    ULONG      drop_item;
		    Att_Node  *drop_node;

		    data->modified = TRUE;

		    // Remove list from lister
		    SetGadgetChoices(data->list,GAD_ICON_TOOLTYPES,(APTR)~0);

		    // Create a new node
		    node = tooltype_newnode( data->tool_list, imsg->data, 0, 0 );

		    obj = GetObject( data->list, GAD_ICON_TOOLTYPES );
		    p = imsg->data_free;

		    p->x -= data->window->LeftEdge;
		    p->y -= data->window->TopEdge;

		    // Dropped on listview?
		    if (CheckObjectArea( obj, p->x, p->y ))
		    {
			drop_item = (p->x << 16) | p->y;

			GetAttr( GLV_GetLine, GADGET(obj), &drop_item );

			// Find drop node
			if (drop_item != -1)
			{
			    drop_node = Att_FindNode( data->tool_list, drop_item );

			    Att_PosNode( data->tool_list, node, drop_node );
			}
		    }

		    // Attach list to lister
		    SetGadgetChoices( data->list, GAD_ICON_TOOLTYPES, data->tool_list );

		    // Edit node
		    icon_fix_toolgads(data,node);

		    // Set new flag
		    data->tool_new=1;
		}

		else if (imsg->command == ICONINFO_ICON && !data->busy)
		{
		    p = imsg->data_free;

		    strcpy( data->name, imsg->data );

		    icon_drop( data, p->x - data->window->LeftEdge, p->y - data->window->TopEdge );
		}

		else if (imsg->command == IPC_GOT_GOODBYE)
		{
		    icon_edit_reload( data );

		    ClearWindowBusy( data->window );
		    data->busy = FALSE;
		}
		else
		    kprintf( "** Icon module unknown IPC msg %lx\n", imsg->command );

		IPC_Reply(imsg);
	    }
	}

	// Intuition messages
	while (msg = GetWindowMsg( data->window->UserPort ))
	{
	    struct IntuiMessage msg_copy;

	    // Copy message and reply
	    msg_copy = *msg;
	    ReplyWindowMsg( msg );

	    switch (msg_copy.Class)
	    {
		case IDCMP_NEWSIZE:
			icon_draw_icon( data );
			break;

		case IDCMP_INTUITICKS:
			++data->tick_count;
			break;

		case IDCMP_MOUSEMOVE:
			if (data->draginfo)
			{
			    ShowDragImage(data->draginfo,
				          data->screen->MouseX + data->drag_x,
				          data->screen->MouseY + data->drag_y );
			}
			break;


		// Close window
		case IDCMP_CLOSEWINDOW:
			quit_flag=1;
			break;


		// Gadget
		case IDCMP_GADGETUP:
			switch (((struct Gadget *)msg_copy.IAddress)->GadgetID)
			{
			    // Skip/Cancel
			    case GAD_ICON_CANCEL:
				    ret = 0;
			    case GAD_ICON_NEXT:
				    quit_flag = 1;
				    icon_fix_toolgads( data, 0 );
				    break;

			    // Save
			    case GAD_ICON_SAVE:
				    SetWindowBusy(data->window);

				    if (data->icon_type == WBDISK)
				    {
				        // Get name
				        strcpy(data->buffer,(char *)GetGadgetValue(data->list,GAD_VOL_NAME));

				        // Has name changed?
				        if (strcmp(data->buffer,data->volume))
				        {
					    // Try to relabel
					    if (!(Relabel(data->path,data->buffer)))
						DisplayBeep(data->window->WScreen);
				        }
				    }
				    icon_fix_toolgads( data, 0 );
				    if (!icon_save( data, data->icon_name ,TRUE))
				    {
					ClearWindowBusy(data->window);
					break;
				    }
				    quit_flag = 1;
				    ret = 2;
				    break;


			    // Tooltype selected
			    case GAD_ICON_TOOLTYPES:
				    {
					Att_Node *node;

					// Get selected node
					if (node = Att_FindNode( data->tool_list, msg_copy.Code ))
					{
					    // Edit this node
					    icon_fix_toolgads( data, node );
					}
				    }
				    break;


			    // New
			    case GAD_ICON_TOOLTYPES_NEW:
				    {
					Att_Node *node;

					// Remove list from lister
					SetGadgetChoices( data->list, GAD_ICON_TOOLTYPES, (APTR)~0 );

					// Create a new node
					node = tooltype_newnode( data->tool_list, 0, 0, 0 );

					// If there's a currently selected node, position before that
					if (data->edit_tool && !data->tool_new)
						Att_PosNode( data->tool_list, node, data->edit_tool );

					// Attach list to lister
					SetGadgetChoices( data->list, GAD_ICON_TOOLTYPES, data->tool_list );

					// Edit node
					icon_fix_toolgads( data, node );

					// Set new flag
					data->tool_new = 1;
				    }
				    break;


			    // Delete tooltype
			    case GAD_ICON_TOOLTYPES_DELETE:

				    data->modified = TRUE;

				    // Clear edit field
				    SetGadgetValue( data->list, GAD_ICON_TOOLTYPES_EDIT, 0 );

				    // End editing
				    icon_fix_toolgads(
					    data,
					    (data->edit_tool && data->edit_tool->node.ln_Succ->ln_Succ) ? (Att_Node *)data->edit_tool->node.ln_Succ : 0 );
				    break;


			    // Sort tooltypes
			    case GAD_ICON_TOOLTYPES_SORT:
				    {
					Att_List *newlist;
					Att_Node *oldnode, *newnode;
					ULONG     sort_flags = ADDNODE_SORT;
					BOOL      ok = TRUE;

					// Shift key used?  Seperate enabled and disabled toolltypes
					if (msg_copy.Qualifier & (IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT))
					    sort_flags |= SORT_TOOLTYPE_SEPERATED;

					if (newlist = Att_NewList( LISTF_POOL ))
					{
					    data->modified = TRUE;

					    data->edit_tool = 0;

					    SetGadgetChoices( data->list, GAD_ICON_TOOLTYPES, (APTR)~0 );

					    for (oldnode = (Att_Node *)data->tool_list->list.lh_Head;
						    ok && oldnode->node.ln_Succ;
						    oldnode = (Att_Node *)oldnode->node.ln_Succ)
					    {
						if (!(newnode = tooltype_newnode(newlist,
							                         oldnode->node.ln_Name,
							                         oldnode->att_data,
							                         sort_flags )))
						{
						    ok = FALSE;
						}
					    }

					    if (ok)
					    {
						Att_RemList( data->tool_list, 0 );

						data->tool_list = newlist;
					    }
					    else
					    {
						Att_RemList( newlist, 0 );
					    }

					    SetGadgetChoices( data->list, GAD_ICON_TOOLTYPES, data->tool_list );

					    // End editing
					    icon_fix_toolgads( data, 0 );
					}
				    }
				    break;


			    // Return pressed on edit field
			    case GAD_ICON_TOOLTYPES_EDIT:

				    data->modified = TRUE;

				    // End editing
				    icon_fix_toolgads(data,0);
				    break;


			    // Default tool glass
			    case GAD_ICON_DEFAULT_TOOL_GLASS:
				    if (WINREQUESTER(data->window))
					    {
					    // Make window busy
					    SetWindowBusy(data->window);

					    strcpy( data->buffer, (char *)GetGadgetValue(data->list,GAD_ICON_DEFAULT_TOOL) );
					    *PathPart( data->buffer ) = 0;

					    // Put up file requester
					    if (!(AslRequestTags(
						    WINREQUESTER(data->window),
						    ASLFR_Window,data->window,
						    ASLFR_InitialDrawer,data->buffer,
						    ASLFR_InitialFile,FilePart((char*)GetGadgetValue(data->list,GAD_ICON_DEFAULT_TOOL)),
						    ASLFR_Flags1,FRF_PRIVATEIDCMP,
						    TAG_END)))
						    {
						    ClearWindowBusy(data->window);
						    break;
						    }

					    data->modified = TRUE;

					    // Build filename
					    strcpy(data->buffer,WINREQUESTER(data->window)->fr_Drawer);
					    AddPart(data->buffer,WINREQUESTER(data->window)->fr_File,256);

					    // Update gadgets
					    SetGadgetValue(data->list,GAD_ICON_DEFAULT_TOOL,(ULONG)data->buffer);

					    ClearWindowBusy(data->window);
					    }
				    break;


			    // More..
			    case GAD_INFO_FILE_MORE:

				    // Make window busy
				    SetWindowBusy(data->window);

				    ItoaU(data->icon_filesize, data->buffer+64, data->decimal_sep);

				    if (data->icon_filesize > 1024)
				    {
				        BytesToString(data->icon_filesize, data->buffer+192, 1, data->decimal_sep);
				        lsprintf(data->buffer+128, " (%s)", data->buffer+192);
				    }
				    else
				        data->buffer[128] = 0;

				    lsprintf(data->buffer, "%s %s%s%s", GetString(locale,MSG_ICON_FILE_SIZE), data->buffer+64, GetString(locale,MSG_BYTES_POSTFIX), data->buffer+128);
				    AsyncRequestTags(data->ipc,
						     REQTYPE_SIMPLE,
						     data->window,
						     0,
						     0,
						     AR_Window,	    data->window,
						     AR_Title,	    GetString(locale,MSG_INFO_FILE_MORE),
						     AR_Message,    data->buffer,
						     AR_Button,	    GetString(locale,MSG_OK),
						     TAG_DONE );
							
				    ClearWindowBusy(data->window);


				    break;
			}
			break;


		// Mouse buttons
		case IDCMP_MOUSEBUTTONS:

			if (data->draginfo)
			{
			    // Dropped?
			    if (msg_copy.Code == SELECTUP)
			    {
				if (msg_copy.Qualifier & (IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT))
				    drag_end( data, 2 );
				else
				    drag_end( data, 1 );
			    }

			    // Aborted?
			    else if (msg_copy.Code == MENUDOWN)
				    drag_end( data, 0 );

			    break;
			}

			// Click within icon area?
			if (msg_copy.Code == SELECTDOWN &&
				msg_copy.MouseX >= data->icon_area.MinX &&
				msg_copy.MouseX <= data->icon_area.MaxX &&
				msg_copy.MouseY >= data->icon_area.MinY &&
				msg_copy.MouseY <= data->icon_area.MaxY)
			{
			    // Switch to other image?
			    if (data->image[1])
				    data->image_num = 1 - data->image_num;

			    // Show icon
			    icon_draw_icon( data );

			    // Fill in new image info
			    icon_show_image_data( data );


			    drag_start_window(
				    data,
				    data->icon_area.MinX - msg_copy.MouseX,
				    data->icon_area.MinY - msg_copy.MouseY );
			}

			// Menu?
			else if (msg_copy.Code == MENUDOWN)
			{
			    short res;

			    // Do pop-up menu
			    if ((msg_copy.MouseX >= 0) &&
				    (msg_copy.MouseX <= (data->window->Width - 1)) &&
				    (msg_copy.MouseY >= 0) &&
				    (msg_copy.MouseY <= (data->window->Height - 1)))
			     if ((res = DoPopUpMenu( data->window, &data->menu, 0, MENUDOWN )) > -1)
			    {
				ULONG flags;

				// Get flags
				flags = GetIconFlags( data->icon );

				// Look at result
				switch (res)
				{
				    // Borderless?
				    case MENU_BORDERLESS:

					    data->modified = TRUE;

					    // Set flag?
					    if (data->menu_items[IIM_BORDERLESS].flags & POPUPF_CHECKED)
					    {
						flags &= ~ICONF_BORDER_OFF;
						if (GetLibraryFlags() & LIBDF_BORDERS_OFF)
						    flags |= ICONF_BORDER_ON;
						else
						    flags &= ~ICONF_BORDER_ON;
					    }
					    else
					    {
						flags |= ICONF_BORDER_OFF;
						flags &= ~ICONF_BORDER_ON;
					    }
					    break;


				    // No label
				    case MENU_NO_LABEL:

					    data->modified = TRUE;

					    // Set flag?
					    if (data->menu_items[IIM_LABEL].flags & POPUPF_CHECKED)
						    flags &= ~ICONF_NO_LABEL;
					    else
						    flags |= ICONF_NO_LABEL;
					    break;


				    // Strip NewIcons/OldIcons
				    case MENU_STRIP_NEWICONS:
				    case MENU_STRIP_OLD:

					    // Show warning
					    SetWindowBusy(data->window);

					    if (SimpleRequestTags(
						    data->window,
						    data->title,
						    GetString(locale,MSG_OK_CANCEL),
						    GetString(locale,(res==MENU_STRIP_NEWICONS)?MSG_STRIP_WARNING:MSG_STRIP_WARNING_OLD)))
					    {
						data->modified = TRUE;

						// Disable 'strip' items in menu
						data->menu_items[IIM_STRIP_NEWICONS].flags |= POPUPF_DISABLED;
						data->menu_items[IIM_STRIP_OLD].flags      |= POPUPF_DISABLED;
						data->menu_items[IIM_NEWICONS].flags       |= POPUPF_DISABLED;
						data->menu_items[IIM_OLDICONS].flags       |= POPUPF_DISABLED;

						// Strip original?
						if (res == MENU_STRIP_OLD)
						{
						    // Only newicon image left
						    data->which_images = 2;

						    // Set flag to strip original
						    data->strip_oldicon = 1;

						    // Switch to show NewIcon
						    icon_switch_image( data, 1 );
						}

						// Strip NewIcons
						else
						if (icon_switch_image( data, 0 ))
						{
						    // Only original image left
						    data->which_images = 0;

						    // Set flag to strip NewIcons
						    data->strip_newicon = 1;

						    data->newicon_path[0] = 0;
						}
					    }
					    ClearWindowBusy( data->window );
					    break;


				    // Show NewIcons image
				    case MENU_NEWICONS:

					    // Show image
					    icon_switch_image( data, 1 );
					    break;


				    // Show original image
				    case MENU_OLDICONS:

					    // Show image
					    icon_switch_image( data, 0 );
					    break;


				    // Edit
				    case MENU_ICON_EDIT:

					    icon_edit( data );
					    break;


				    // Pick editor
				    case MENU_ICON_PICK_EDITOR:

					    icon_pick_editor( data );
					    break;


				    // Type
				    case MENU_DISK:
				    case MENU_DRAWER:
				    case MENU_TOOL:
				    case MENU_PROJECT:
				    case MENU_GARBAGE:

					    {
						int new_type = WBPROJECT;

						switch (res)
						{
						    case MENU_DISK:
							    new_type = WBDISK;
							    break;

						    case MENU_DRAWER:
							    new_type = WBDRAWER;
							    break;

						    case MENU_TOOL:
							    new_type = WBTOOL;
							    break;

						    case MENU_PROJECT:
							    new_type = WBPROJECT;
							    break;

						    case MENU_GARBAGE:
							    new_type = WBGARBAGE;
							    break;
						}

						if (new_type != data->icon_type)
						{
						    data->modified = TRUE;

						    data->icon_type     = new_type;
						    data->icon->do_Type = data->icon_type;

						    closewindow( data );

						    openwindow( data, next ? 1 : 0 );
						}
					    }
					    break;
				}

				// Set flags
				SetIconFlags( data->icon, flags );
			    }
			}
			break;


		// Key press
		case IDCMP_RAWKEY:

			// Help?
			if (msg_copy.Code == 0x5f &&
				!(msg_copy.Qualifier & VALID_QUALIFIERS))
			{
			    // Valid main IPC?
			    if (data->main_ipc)
			    {
				// Set busy pointer
				SetWindowBusy( data->window );

				// Send help request
				IPC_Command( data->main_ipc, IPC_HELP, (1<<31), "IconInfo", 0, (struct MsgPort *)-1 );

				// Clear busy pointer
				ClearWindowBusy( data->window );
			    }
			}
			break;


		// IDCMP update
		case IDCMP_IDCMPUPDATE:
			{
			    struct TagItem *tags = (struct TagItem *)msg_copy.IAddress;
			    short item;

			    // Check ID
			    if (GetTagData( GA_ID, 0, tags) != GAD_ICON_TOOLTYPES)
				break;

			    // Get item
			    if ((item = GetTagData( GLV_DragNotify, -1, tags )) != -1)
			    {
				// Start the drag
				drag_start(data,
					   item,
					   GetTagData( GA_Width, 0, tags ),
					   GetTagData( GA_Height, 0, tags ),
					   -GetTagData( GA_RelRight, 0, tags ),
					   -GetTagData( GA_RelBottom, 0, tags ));
			    }
			}
			break;
	    }
	}

	if (quit_flag)
		break;

	// Check for drag and drop deadlock
	if (data->draginfo && CheckTimer( data->timer ))
	{
	    // Deadlocked?
	    if (data->tick_count == data->last_tick)
	    {
		//	kprintf( "** drag and drop deadlock!\n" );
		drag_end( data, 0 );
	    }

	    // Not deadlock, restart timer
	    else
	    {
		StartTimer( data->timer, 0, 500000 );
		data->last_tick = data->tick_count;
	    }
	}

	Wait(1 << data->window->UserPort->mp_SigBit |
	     ((data->app_port) ? 1 << data->app_port->mp_SigBit : 0)|
	     ((data->notify_port) ? 1 << data->notify_port->mp_SigBit : 0)|
	     ((data->ipc) ? 1 << data->ipc->command_port->mp_SigBit : 0));
    }

    // Free stuff
    icon_free( data );

    // Close window
    closewindow( data );

    return ret;
}

void icon_show_image_data( icon_data *data )
{
    struct Image  *image;

    if (!(image = data->image[data->image_num]))
	return;

    // Fill image size
    lsprintf(data->buffer, "%ld x %ld", image->Width, image->Height);
    SetGadgetValue(data->list, GAD_INFO_FILE_DIMS, (ULONG)data->buffer);

    // Fill icon type
    if (!data->icon_filesize)
	SetGadgetValue(data->list, GAD_INFO_ICON_TYPE, (ULONG)GetString(locale,data->icon_mode ? MSG_DEFICON_NEWICON : MSG_DEFICON_OLDICON));
    else
	SetGadgetValue(data->list, GAD_INFO_ICON_TYPE, (ULONG)GetString(locale,data->icon_mode ? MSG_ICON_NEWICON : MSG_ICON_OLDICON));

    // Fill icon color count
    lsprintf(data->buffer,"%ld", 1<<image->Depth);
    SetGadgetValue(data->list, GAD_INFO_FILE_COLORS, (ULONG)data->buffer);
}

// Display icon image
void icon_draw_icon( icon_data *data )
{
    struct Region *region, *old_region = 0;
    struct Image  *image;
    struct BitMap  bitmap;
    short          x, y;

    // Get image to display
    if (!(image = data->image[data->image_num]))
	return;

    // Create clip region
    if (region = NewRegion())
    {
	OrRectRegion( region, &data->icon_area );
	old_region = InstallClipRegion( data->window->WLayer, region );
    }

    // Get coordinates to display icon
    x = (1 + data->icon_area.MaxX-data->icon_area.MinX - image->Width)  >> 1;
    y = (1 + data->icon_area.MaxY-data->icon_area.MinY - image->Height) >> 1;

    // Erase region
    SetGadgetValue( data->list, GAD_ICON_IMAGE, 0 );

    // Get icon as bitmap
    image_to_bitmap( data, image, &bitmap, data->window->RPort->BitMap->Depth );

    // Draw icon
    BltBitMapRastPort(&bitmap, 0, 0,
	              data->window->RPort, x+data->icon_area.MinX, y+data->icon_area.MinY,
	              image->Width, image->Height,
	              0xc0 );

    // Free region
    if (region)
    {
	InstallClipRegion( data->window->WLayer, old_region );
	DisposeRegion( region );
    }
}


// Fix tooltype gadgets
void icon_fix_toolgads( icon_data *data, Att_Node *sel )
{
    unsigned long seconds, micros;

    // Get double-click time
    CurrentTime( &seconds, &micros );

    // Tooltype selected currently?
    if (data->edit_tool)
    {
	// Get edited tooltype
	strcpy( data->buffer, (char *)GetGadgetValue( data->list, GAD_ICON_TOOLTYPES_EDIT ) );

	// Same as new selection?
	if (sel == data->edit_tool)
	{
	    // Test double click
	    if (DoubleClick( data->seconds, data->micros, seconds, micros ))
	    {
		// Is tooltype currently disabled?
		if (data->buffer[0] == '(' &&
		    data->buffer[strlen(data->buffer)-1] == ')')
		{
		    // Enable it
		    strcpy( data->buffer, data->buffer+1 );
		    data->buffer[strlen(data->buffer)-1] = 0;
		}

		// It's enabled
		else
		{
		    // Disable it
		    data->buffer[0]='(';
		    strcpy(data->buffer+1,(char *)GetGadgetValue(data->list,GAD_ICON_TOOLTYPES_EDIT));
		    strcat(data->buffer,")");
		}

		// Clear double-click time
		seconds = 0;
	    }
	}

	// Remove list from lister
	SetGadgetChoices(data->list,GAD_ICON_TOOLTYPES,(APTR)~0);

	// Null string?
	if (!data->buffer[0])
	{
	    // Remove selected node
	    Att_RemNode( data->edit_tool );

	    // Was it the selected one?
	    if (sel == data->edit_tool)
		sel = 0;
	}

	// Otherwise, replace node name
	else
	{
	    data->modified = TRUE;
	    Att_ChangeNodeName( data->edit_tool, data->buffer );
	}

	// Attach list to lister
	SetGadgetChoices( data->list, GAD_ICON_TOOLTYPES, data->tool_list );
    }

    // Store double-click time
    data->seconds = seconds;
    data->micros = micros;

    // New tooltype selected?
    if (sel)
    {
	GL_Object *object;

	// Fill out edit field
	SetGadgetValue( data->list, GAD_ICON_TOOLTYPES_EDIT, (ULONG)sel->node.ln_Name );

	// Different to last selection?
	if (sel != data->edit_tool)
	{
	    // Enable gadgets
	    DisableObject(data->list,GAD_ICON_TOOLTYPES_EDIT,FALSE);
	    DisableObject(data->list,GAD_ICON_TOOLTYPES_DELETE,FALSE);

	    // Set selection
	    SetGadgetValue(data->list,GAD_ICON_TOOLTYPES,Att_FindNodeNumber(data->tool_list,sel));
	}

	// Activate edit field
	if (object=GetObject(data->list,GAD_ICON_TOOLTYPES_EDIT))
	    ActivateStrGad(GADGET(object),data->window);
    }

    // Otherwise
    else
    {
	// Clear edit field
	SetGadgetValue( data->list, GAD_ICON_TOOLTYPES_EDIT, 0 );

	// Disable gadgets
	DisableObject( data->list, GAD_ICON_TOOLTYPES_EDIT, TRUE );
	DisableObject( data->list, GAD_ICON_TOOLTYPES_DELETE, TRUE );

	// No selection
	SetGadgetValue( data->list, GAD_ICON_TOOLTYPES, (ULONG)-1 );
    }

    // Store edit node
    data->edit_tool = sel;

    // Clear new flag
    data->tool_new = 0;
}


// Convert an image to a bitmap
void image_to_bitmap( icon_data *data, struct Image *image, struct BitMap *bitmap, short depth )
{
    short plane=1;
    long planesize;

    // Initialise bitmap
    InitBitMap( bitmap, depth, image->Width, image->Height );

    // Get plane size
    planesize = ((image->Width+15)>>4) * image->Height;

    // First plane pointer
    bitmap->Planes[0] = (PLANEPTR)image->ImageData;

    // More than 1 plane?
    if (image->Depth > 1)
    {
	// Get second plane pointer
	bitmap->Planes[plane++] = (PLANEPTR)(image->ImageData+planesize);

	// If it's 3 planes, under 39, we can remap it
	if (image->Depth==3 && GfxBase->LibNode.lib_Version>=39 && data->remap)
	{
	    for (plane = 2; plane < depth; plane++)
		bitmap->Planes[plane] = (PLANEPTR)(image->ImageData + (planesize << 1));
	}

	// More than 2 planes?
	else
	if (image->Depth>2)
	{
	    for (plane = 2; plane<image->Depth; plane++)
		bitmap->Planes[plane] = (PLANEPTR)(image->ImageData + planesize * plane);
	}
    }

    // Clear extra planes
    for (; plane < depth; ++plane)
	bitmap->Planes[plane] = 0;
}


// Switch image
BOOL icon_switch_image( icon_data *data, short newicon )
{
    struct DiskObject *icon;
    BOOL               ok = 0;

    // Get icon to use
    if (newicon)
	icon = data->new_newicon;
    else
	icon = data->new_oldicon;

    if (!icon)
	icon = data->icon;

    // Show newicon?
    if (newicon)
	ok = 1;

    // Stripped old image?
    else if (data->strip_oldicon)
	ok = 0;

    // Get pointer to original icon
    else if (icon = GetOriginalIcon( icon ))
	ok = 1;

    // Ok?
    if (ok)
    {
	// Get original image pointers
	data->image[0] = (struct Image *)icon->do_Gadget.GadgetRender;
	data->image[1] = (struct Image *)icon->do_Gadget.SelectRender;

	data->icon_mode = newicon;
	data->image_num = 0;

	icon_draw_icon( data );

	// Fill  in newicon image data
	icon_show_image_data( data );

	// Fix flag
	if (newicon)
	{
	    data->menu_items[IIM_OLDICONS].flags &= ~POPUPF_CHECKED;
	    data->menu_items[IIM_NEWICONS].flags |= POPUPF_CHECKED;
	}
	else
	{
	    data->menu_items[IIM_OLDICONS].flags |= POPUPF_CHECKED;
	    data->menu_items[IIM_NEWICONS].flags &= ~POPUPF_CHECKED;
	}
    }
    else
	data->menu_items[IIM_OLDICONS].flags &= ~POPUPF_CHECKED;

    return ok;
}

// Save Protection bits
void save_protection(icon_data *data, char *save_name)
{
    Att_Node *node;
    ULONG     prot;
    WORD a;

    // Get all bits we don't change
    prot =  data->fib.fib_Protection;
    prot &= ~(FIBF_HOLD|FIBF_SCRIPT|FIBF_PURE|FIBF_ARCHIVE|FIBF_READ|FIBF_WRITE|FIBF_EXECUTE|FIBF_DELETE);

    // Go through 8 bits
    for (a = 0; a < 4; ++a)
    {
	// Go through protection nodes
	for (node = (Att_Node *)data->prot_list[a]->list.lh_Head;
	     node->node.ln_Succ;
	     node = (Att_Node *)node->node.ln_Succ)
	{
	    // Is this bit set?
	    if (node->node.lve_Flags & LVEF_SELECTED)
	    {
		if (node->att_data<4)
		    prot |= protect_lookup[node->att_data];
	    }
	    else if (node->att_data>3)
		prot |= protect_lookup[node->att_data];
	}

	// Set protection
	SetProtection( save_name, prot );
    }
}

// Save icon
BOOL icon_save( icon_data *data, char *save_name ,BOOL err)
{
    BOOL  ok = FALSE;
    char  **types = 0;
    short count = 0;
    struct DiskObject  *icon_pointer;

    if (IconBase->lib_Version>=44)
	return (icon_save_44(data,save_name ,err));

    // Backup icon information
    CopyMem( (char *)data->icon, (char *)&data->icon_copy, sizeof(struct DiskObject) );

    // If we're stripping a NewIcon, use copy
    icon_pointer = (data->strip_newicon) ? &data->icon_copy : data->icon;

    // Get stack
    icon_pointer->do_StackSize = GetGadgetValue( data->list, GAD_ICON_STACK );

    // Get default tool
    icon_pointer->do_DefaultTool = (char *)GetGadgetValue( data->list, GAD_ICON_DEFAULT_TOOL );

    // Any tooltypes?
    if ((count += Att_NodeCount( data->tool_list )) > 0)
    {
	// Allocate tooltype array
	if (types = AllocVec( sizeof(char *) * (count+1), MEMF_CLEAR ))
	{
	    short     num = 0;
	    Att_Node *node;


	    // Fill in tooltype pointers
	    for (node = (Att_Node *)data->tool_list->list.lh_Head;
		 node->node.ln_Succ;
		 node = (Att_Node *)node->node.ln_Succ)
	    {
		    types[num++] = node->node.ln_Name;
	    }
	}
    }
    icon_pointer->do_ToolTypes = types;

    if (NewIconBase)
    {
	struct NewDiskObject *dest;
	struct DiskObject    *oldobj;
	struct NewDiskObject *newobj = 0;

	// Try to get destination icon
	if (!(dest = GetNewDiskObject( data->icon_name )))
	    dest = GetDefNewDiskObject( data->icon_type );

	if (dest)
	{
	    char                *old_DefaultTool;
	    char               **old_ToolTypes;
	    APTR                 old_Render,  old_Select;
	    struct ChunkyImage  *old_Normal, *old_Selected;

	    // Save icon pointers
	    old_DefaultTool = dest->ndo_StdObject->do_DefaultTool;
	    old_ToolTypes   = dest->ndo_StdObject->do_ToolTypes;
	    old_Render      = dest->ndo_StdObject->do_Gadget.GadgetRender;
	    old_Select      = dest->ndo_StdObject->do_Gadget.SelectRender;
	    old_Normal      = dest->ndo_NormalImage;
	    old_Selected    = dest->ndo_SelectedImage;

	    // Set new icon pointers
	    dest->ndo_StdObject->do_StackSize   = icon_pointer->do_StackSize;
	    dest->ndo_StdObject->do_DefaultTool = icon_pointer->do_DefaultTool;
	    dest->ndo_StdObject->do_ToolTypes   = icon_pointer->do_ToolTypes;

	    // Standard images
	    if (data->strip_oldicon)
	    {
		dest->ndo_StdObject->do_Gadget.GadgetRender = &oldicon_image;
		dest->ndo_StdObject->do_Gadget.SelectRender = 0;
		dest->ndo_StdObject->do_Gadget.Width  = 1;
		dest->ndo_StdObject->do_Gadget.Height = 1;
		dest->ndo_StdObject->do_Gadget.Flags  = GFLG_GADGHCOMP|GFLG_GADGIMAGE;
	    }
	    else
	    {
		if (data->new_oldicon)
		    oldobj = GetOriginalIcon( data->new_oldicon );
		else
		    oldobj = GetOriginalIcon( data->icon );

		if (oldobj)
		{
		    dest->ndo_StdObject->do_Gadget.GadgetRender = oldobj->do_Gadget.GadgetRender;
		    dest->ndo_StdObject->do_Gadget.SelectRender = oldobj->do_Gadget.SelectRender;
		    dest->ndo_StdObject->do_Gadget.Width  = oldobj->do_Gadget.Width;
		    dest->ndo_StdObject->do_Gadget.Height = oldobj->do_Gadget.Height;
		    dest->ndo_StdObject->do_Gadget.Flags  = oldobj->do_Gadget.Flags;
		}
	    }

	    // Newicon images
	    if (data->strip_newicon)
	    {
		dest->ndo_NormalImage   = 0;
		dest->ndo_SelectedImage = 0;
	    }
	    else
	    {
		if (data->new_newicon)
		    newobj = ((struct NewIconDiskObject *)data->new_newicon)->nido_NewDiskObject;
		else if (GetIconType( data->icon ) == ICON_NEWICON)
		    newobj = ((struct NewIconDiskObject *)data->icon)->nido_NewDiskObject;

		if (newobj)
		{
		    dest->ndo_NormalImage   = newobj->ndo_NormalImage;
		    dest->ndo_SelectedImage = newobj->ndo_SelectedImage;
		}
	    }

	    dest->ndo_StdObject->do_Type = data->icon_type;

	    // Set border / label flags
	    SetIconFlags( dest->ndo_StdObject, GetIconFlags( data->icon ) );

	    // Save icon with new image
	    if (PutNewDiskObject( save_name, dest ))
	    {
		data->modified = FALSE;
		ok = TRUE;
	    }
	    else
	    if (err)
	    {
		char buf[200],name[115];
		lsprintf(name,"%s.info",FilePart(save_name));
		Fault(IoErr(),name,buf,sizeof(buf));
		AsyncRequestTags(data->ipc,
			         REQTYPE_SIMPLE,
			         data->window,
			         0,
			         0,
			         AR_Window, data->window,
			         AR_Message, buf,
			         AR_Button, GetString(locale,MSG_OK),
			         TAG_DONE );
	    }

	    // Restore icon pointers
	    dest->ndo_StdObject->do_DefaultTool         = old_DefaultTool;
	    dest->ndo_StdObject->do_ToolTypes           = old_ToolTypes;
	    dest->ndo_StdObject->do_Gadget.GadgetRender = old_Render;
	    dest->ndo_StdObject->do_Gadget.SelectRender = old_Select;
	    dest->ndo_NormalImage                       = old_Normal;
	    dest->ndo_SelectedImage                     = old_Selected;

	    // Free icon
	    FreeNewDiskObject( dest );
	}
    }
    else
    {
	struct DiskObject *obj;

	obj = data->new_oldicon ? data->new_oldicon : data->icon;

	// Set image pointers
	icon_pointer->do_Gadget.GadgetRender = (struct Image *)obj->do_Gadget.GadgetRender;
	icon_pointer->do_Gadget.SelectRender = (struct Image *)obj->do_Gadget.SelectRender;

	icon_pointer->do_Gadget.Flags  = obj->do_Gadget.Flags;
	icon_pointer->do_Gadget.Width  = obj->do_Gadget.Width;
	icon_pointer->do_Gadget.Height = obj->do_Gadget.Height;

	icon_pointer->do_Type = data->icon_type;

	// Set border / label flags
	SetIconFlags( icon_pointer, GetIconFlags( data->icon ) );

	// Save icon
	if (PutDiskObject( save_name, icon_pointer ))
	{
	    data->modified = FALSE;
	    ok = TRUE;
	}
	else
	if (err)
	{
	    char buf[200],name[115];
	    lsprintf(name,"%s.info",FilePart(save_name));
	    Fault(IoErr(),name,buf,sizeof(buf));
	    AsyncRequestTags(
		    data->ipc,
		    REQTYPE_SIMPLE,
		    data->window,
		    0,
		    0,
		    AR_Window, data->window,
		    AR_Message, buf,
		    AR_Button, GetString(locale,MSG_OK),
		    TAG_DONE );
	}
    }

    // Restore icon information
    if (!data->strip_newicon)
	CopyMem( (char *)&data->icon_copy, (char *)data->icon, sizeof(struct DiskObject) );

    // Comment field?
    if (GetObject( data->list, GAD_ICON_COMMENT ))
	SetComment( save_name, (char *)GetGadgetValue( data->list, GAD_ICON_COMMENT ) );

    // Protection bits?
    if (data->protection)
	save_protection(data,save_name);

    // Free tooltype array
    if (types)
	FreeVec( types );
    return ok;
}


// Special compare routine for tooltype strings
int tooltype_strcmp( char *name1, char *name2, ULONG flags )
{
    // Ignore euro quotes if both are authors
    if ((name1[0] == '�' || name1[0] == '�')
	&& (name2[0] == '�' || name2[0] == '�'))
    {
	while (*name1 == '�' || *name1 == '�')
	    ++name1;

	while (*name2 == '�' || *name2 == '�')
	    ++name2;
    }

    // Author comes before all else
    else if (name1[0] == '�' || name1[0] == '�')
	return -1;

    else if (name2[0] == '�' || name2[0] == '�')
	return 1;

    // Enabled tooltypes at top when seperated
    else if (flags & SORT_TOOLTYPE_SEPERATED)
    {
	if (name1[0] == '(')
	    return 1;

	else if (name2[0] == '(')
	    return -1;
    }

    // Ignore brackets when not seperated
    else
    {
	if (name1[0] == '(')
	    ++name1;

	if (name2[0] == '(')
	    ++name2;
    }

    return strcmpi( name1, name2 );
}


// Modified Att_NewNode for tooltype sorting
Att_Node *tooltype_newnode(Att_List *list,
	                   char *name,
	                   ULONG data,
	                   ULONG flags)
{
    Att_Node *node;
    BOOL added=0;

    // Valid list?
    if (!list) return 0;

    // Lock list
    LockAttList(list,TRUE);

    // Allocate node
    if (!(node=AllocMemH(list->memory,sizeof(Att_Node))))
    {
	UnlockAttList(list);
	return 0;
    }

    // If name supplied, create a copy of it
    if (name)
    {
	if (!(node->node.ln_Name=AllocMemH(list->memory,strlen(name)+1)))
	{
	    FreeMemH(node);
	    UnlockAttList(list);
	    return 0;
	}
	strcpy(node->node.ln_Name,name);
    }

    // Store data and list pointer
    node->att_data=data;
    node->list=list;

    // Alphabetical sort?
    if ((flags&ADDNODE_SORT) && name)
    {
	Att_Node *posnode,*lastnode=0;
	short match;

	// Go through existing nodes
	posnode=(Att_Node *)list->list.lh_Head;
	while (posnode->node.ln_Succ)
	{
	    // Compare new node name against existing name
	    if ((match=tooltype_strcmp(name,posnode->node.ln_Name,flags&SORT_TOOLTYPE_SEPERATED))<=0)
	    {
		// If exclusive flag is set and we matched exactly, reject this node
		if ((flags&ADDNODE_EXCLUSIVE) && match==0)
		{
		    FreeMemH(node->node.ln_Name);
		    FreeMemH(node);
		    UnlockAttList(list);
		    return 0;
		}

		// Insert into list
		Insert((struct List *)list,(struct Node *)node,(struct Node *)lastnode);
		added=1;
		break;
	    }
	    lastnode=posnode;
	    posnode=(Att_Node *)posnode->node.ln_Succ;
	}
    }

    // If not added by a sort, add to end
    if (!added) AddTail((struct List *)list,(struct Node *)node);

    // Unlock the list
    UnlockAttList(list);

    return node;
}


// Icon drop for V44

void icon_drop_44( icon_data *data, int x, int y )
{
    struct IntuiMessage *msg;
    struct DiskObject *icon;
    short   len;
    int	    failed = 0;

    // Disk icon?
    if (data->name[strlen(data->name)-1]==':')
	strcat(data->name,"Disk");

    // Already have a .info suffix? Strip it if so
    if ((len = strlen(data->name)) > 5 &&
	stricmp(data->name+len-5,".info") == 0)
    {
	data->name[len-5] = 0;
    }

    // Try and get icon
    if (icon=GetIconTags(data->name,
			 ICONGETA_FailIfUnavailable,TRUE,
			 ICONGETA_Screen,data->window->WScreen,
			 TAG_DONE))
    {
	int   area = 0;
	ULONG old_idcmp = data->window->IDCMPFlags;
	int   image_changed = 0;
	short res = -1;

	// Dropped in icon area?
	if (CheckObjectArea( GetObject( data->list, GAD_ICON_IMAGE ), x, y ))
	    area = AREA_TYPE_ICON_IMAGE;

	// Dropped in tooltypes area?
	else if (CheckObjectArea( GetObject( data->list, GAD_ICON_TOOLTYPES ), x, y ))
	    area = AREA_TYPE_ICON_TOOLTYPES;

	// Initialize poup menu options
	NewList((struct List *)&data->dropmenu);

	// Add icon options if not in tooltypes area
	if (area != AREA_TYPE_ICON_TOOLTYPES)
	{
	    data->dropmenu_items[IDM_OLDICONS].flags &= ~POPUPF_DISABLED;
	    AddTail((struct List *)&data->dropmenu,(struct Node *)&data->dropmenu_items[IDM_OLDICONS]);
	}

	// Add 'copy tooltypes' to dropmenu
	if (area == 0 && data->icon_type != WBDISK && icon->do_ToolTypes && icon->do_ToolTypes[0])
	{
	    AddTail((struct List *)&data->dropmenu,(struct Node *)&data->dropmenu_items[IDM_BAR]);
	    AddTail((struct List *)&data->dropmenu,(struct Node *)&data->dropmenu_items[IDM_TOOLTYPES]);
	}

	// make our window active again
	ActivateWindow(data->window);

	// Dropped in tooltypes area?
	if (area == AREA_TYPE_ICON_TOOLTYPES)
	{
	    if (icon->do_ToolTypes && icon->do_ToolTypes[0])
		res = MENU_COPY_TOOLTYPES;
	    else
		failed = 1;
	}

	else if (area == AREA_TYPE_ICON_IMAGE)
	{
	    res = MENU_COPY_OLDICON;
	}
	// Else show popup
	else
	{

	    // Disable intuiticks
	    ModifyIDCMP( data->window, data->window->IDCMPFlags & ~ IDCMP_INTUITICKS );

	    // Flush port
	    while (msg = GetWindowMsg( data->window->UserPort ))
		ReplyWindowMsg( msg );

	    // Re-enable intuiticks
	    ModifyIDCMP( data->window, old_idcmp );

	    // Popup menu
	    res = DoPopUpMenu( data->window, &data->dropmenu, 0, SELECTDOWN );
	}

	switch (res)
	{
	    // Popup aborted
	    case -1:

		    break;

	    case MENU_COPY_OLDICON:
		    {
			struct DiskObject *new;

			// create a new icon for V44 and and swap fields

			if (new=DupDiskObject(icon,TAG_DONE))
			{
			    struct DrawerData * do_DrawerData;
			    char * do_ToolWindow;

			    //preserve old pointer data

			    do_DrawerData      = new->do_DrawerData;
			    do_ToolWindow      = new->do_ToolWindow;

			    /* copy the relevant data from the original data into
			     * the copy; this will preserve the imagery associated
			     * with the copy
			     */

			    new->do_CurrentX    = icon->do_CurrentX;
			    new->do_CurrentY    = icon->do_CurrentY;
			    new->do_DrawerData  = icon->do_DrawerData;
			    new->do_ToolWindow  = icon->do_ToolWindow;

			    // make sure new icon has type we are editing
			    new->do_Type        = data->icon_type;

			    // Set border / label flags
			    SetIconFlags( new, GetIconFlags(data->icon) );

			    image_changed = 1;

			    // The old and new images are no longer stripped
			    // these are unused now

			    data->strip_oldicon = 0;
			    data->strip_newicon = 0;

			    // update image count and type
			    data->which_images = 1;


			    if (data->icon)
				    FreeDiskObject(data->icon);

			    data->icon = new;

			    // recover old data
			    icon->do_DrawerData  = do_DrawerData;
			    icon->do_ToolWindow  = do_ToolWindow;

			}
		    }

		    break;

	    case MENU_COPY_TOOLTYPES:
		    {
			int num;

			data->modified = TRUE;

			// Remove list from lister
			SetGadgetChoices( data->list, GAD_ICON_TOOLTYPES, (APTR)~0 );

			// Add tooltypes
			for (num = 0; icon->do_ToolTypes[num]; ++num)
			    tooltype_newnode( data->tool_list, icon->do_ToolTypes[num], 0, 0 );

			// Attach list to lister
			SetGadgetChoices( data->list, GAD_ICON_TOOLTYPES, data->tool_list );
		    }
		    break;
	}

	// Did we get a new icon image?
	if (image_changed)
	{
	    data->modified = TRUE;

	    // Get icon image pointers
	    icon_switch_image( data, data->icon_mode );
	}

	// Free icon
	FreeDiskObject( icon );
    }
    else
	failed = 1;

    // Failed
    if (failed)
	DisplayBeep(data->window->WScreen);
}


// Get icon full name
char *icon_fullname(char *name)
{
    char *full_name,*ptr;
    BPTR lock;
    short len;

    // Allocate buffer for full name
    if (!(full_name=AllocVec(300,0))) return 0;

    // Copy name
    strcpy(full_name,name);
    ptr=full_name+strlen(full_name);
    strcat(full_name,".info");


    // Lock icon
    if (!(lock=Lock(full_name,ACCESS_READ)))
    {
	// Strip .info
	*ptr=0;
	if (lock=Lock(full_name,ACCESS_READ))
	{
	    // Add .info back
	    strcpy(ptr,".info");
	}
    }

    // Got lock?
    if (lock)
    {
	// Expand path
	DevNameFromLock(lock,full_name,256);

	// Disk?
	if ((ptr=strchr(full_name,':')) && stricmp(ptr+1,"disk.info")==0)
	{
	    // Get real full name
	    NameFromLock(lock,full_name,256);

	    // Strip after colon
	    if (ptr=strchr(full_name,':')) *(ptr+1)=0;
	}

	// Unlock lock
	UnLock(lock);
    }


    // Strip .info from name
    if ((len=strlen(full_name))>5 &&
	stricmp(full_name+len-5,".info")==0)
    {
	// Strip it
	*(full_name+len-5)=0;
    }

    return full_name;
}


// Send icon notification
void icon_notify(char *name,ULONG flags,short delete)
{
    char *full_name;


    if (!(full_name=icon_fullname(name)))
	return;

    // Send notify message
    SendNotifyMsg(GN_WRITE_ICON,0,delete,FALSE,full_name,0);

    FreeVec(full_name);
}


// Save icon
BOOL icon_save_44( icon_data *data, char *save_name ,BOOL err)
{
    BOOL  ok = FALSE;
    char  **types = 0;
    short count = 0;


    if (data->icon)
    {
	// set icon type
	data->icon->do_Type = data->icon_type;

	// Get stack
	data->icon->do_StackSize = GetGadgetValue( data->list, GAD_ICON_STACK );

	// Get default tool
	data->icon->do_DefaultTool = (char *)GetGadgetValue( data->list, GAD_ICON_DEFAULT_TOOL );

	// Any tooltypes?
	if ((count += Att_NodeCount( data->tool_list )) > 0)
	{
	    // Allocate tooltype array
	    if (types = AllocVec( sizeof(char *) * (count+1), MEMF_CLEAR ))
	    {
		short     num = 0;
		Att_Node *node;

		// Fill in tooltype pointers
		for (node = (Att_Node *)data->tool_list->list.lh_Head;
		     node->node.ln_Succ;
		     node = (Att_Node *)node->node.ln_Succ)
		{
		    types[num++] = node->node.ln_Name;
		}
	    }
	}

	data->icon->do_ToolTypes = types;


	if (PutDiskObject(save_name,data->icon))
	{
	    data->modified = FALSE;
	    ok = TRUE;
	}
	else if (err)
	{
	    char buf[200],name[115];

	    lsprintf(name,"%s.info",FilePart(save_name));
	    Fault(IoErr(),name,buf,sizeof(buf));
	    AsyncRequestTags(data->ipc,
		             REQTYPE_SIMPLE,
		             data->window,
		             0,
		             0,
		             AR_Window, data->window,
		             AR_Message, buf,
		             AR_Button, GetString(locale,MSG_OK),
		             TAG_DONE );
	}
	

	// Comment field?
	if (GetObject( data->list, GAD_ICON_COMMENT ))
	    SetComment( save_name, (char *)GetGadgetValue( data->list, GAD_ICON_COMMENT ) );

	// Protection bits?
	if (data->protection)
	    save_protection(data,save_name);

	// Free tooltype array
	if (types)
	    FreeVec( types );
    }

return ok;
}



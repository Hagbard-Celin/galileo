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

#include "galileofm.h"

struct Window *lister_open_window(Lister *,struct Screen *);
void lister_close_window(Lister *,BOOL);

// Open a lister display
struct Window *lister_open(Lister *lister,struct Screen *screen)
{
	struct Window *window;

	// No screen?
	if (!screen && !(screen=GUI->screen_pointer))
		return 0;

	// Lock window
	GetSemaphore(&lister->backdrop_info->window_lock,SEMF_EXCLUSIVE,0);

	// Open window
	window=lister_open_window(lister,screen);

	// Unlock window
	FreeSemaphore(&lister->backdrop_info->window_lock);

	return window;
}

struct Window *lister_open_window(Lister *lister,struct Screen *screen)
{
	struct Gadget *gadget=0,*lock_gad,*iconify_gad;
	struct IBox *dims;
	ULONG mode;

	// If lister is already open, return
	if (!lister || lister->window) return lister->window;

	// Is lister meant to be iconified?
	if (lister->flags&LISTERF_ICONIFIED) return 0;

	// Is lister iconified?
	if (lister->appicon)
	{
		// Update icon position
		lister_update_icon(lister);

		// Remove AppIcon
		RemoveAppIcon(lister->appicon);
		lister->appicon=0;

		// Free icon
		if (lister->appicon_icon)
		{
			FreeDiskObjectCopy(lister->appicon_icon);
			lister->appicon_icon=0;
		}
	}

	// If lister was zoomed, swap dimensions around
	if ((lister->dimensions.wd_Flags&(WDF_VALID|WDF_ZOOMED))==(WDF_VALID|WDF_ZOOMED))
	{
		struct IBox temp;

		// Swap dimensions
		temp=lister->dimensions.wd_Zoomed;
		lister->dimensions.wd_Zoomed=lister->dimensions.wd_Normal;
		lister->dimensions.wd_Normal=temp;

		// Clear zoomed flag
		lister->dimensions.wd_Flags&=~WDF_ZOOMED;
	}

	// Get dimensions pointer
	dims=&lister->dimensions.wd_Normal;

	// Get screen font
	AskFont(&screen->RastPort,&lister->screen_font);

	// Calculate minimum size
	lister_calc_limits(lister,screen);

	// Check lister size against minimums
	if (dims->Width<lister->win_limits.Left) dims->Width=lister->win_limits.Left;
	if (dims->Height<lister->win_limits.Top) dims->Height=lister->win_limits.Top;

	// Initialise backfill pattern
	lister->pattern.hook.h_Entry=(ULONG (*)())PatternBackfill;
	lister->pattern.hook.h_Data=lister->backdrop_info;
	lister->pattern.pattern=&GUI->pattern[PATTERN_LISTER];

	// Is pattern initially enabled?
	if (lister->flags&LISTERF_VIEW_ICONS)
		lister->pattern.disabled=FALSE;
	else lister->pattern.disabled=TRUE;

	// Get refresh mode
	if (environment->env->lister_options&LISTEROPTF_SIMPLE) mode=WA_SimpleRefresh;
	else mode=WA_SmartRefresh;

	// Initialise boopsi list
	NewList(&lister->backdrop_info->boopsi_list);

	// Locked lister?
	// Create placeholder for size gadget to keep window borders consistent
	if (lister->flags&LISTERF_LOCK_POS)
	{
		struct Image *image=0;

		lister->drawinfo=GetScreenDrawInfo(screen);

		// Get size gadget image
		if (image=NewObject(
					0,
					"sysiclass",
					SYSIA_DrawInfo,lister->drawinfo,
					SYSIA_Which,SIZEIMAGE,
					TAG_END))
		{
			// Fill out gadget border border
			lister->size_gadget_border_coords[0]=0;
			lister->size_gadget_border_coords[1]=image->Height;
			lister->size_gadget_border_coords[2]=0;
			lister->size_gadget_border_coords[3]=0;
			lister->size_gadget_border_coords[4]=image->Width;
			lister->size_gadget_border_coords[5]=0;
			lister->size_gadget_border.LeftEdge=0;
			lister->size_gadget_border.TopEdge=0;
			lister->size_gadget_border.FrontPen=lister->drawinfo->dri_Pens[SHINEPEN];
			lister->size_gadget_border.BackPen=0;
			lister->size_gadget_border.DrawMode=JAM1;
			lister->size_gadget_border.Count=3;
			lister->size_gadget_border.XY=&lister->size_gadget_border_coords[0];
			lister->size_gadget_border.NextBorder=0;

			// Create size gadget placeholder
			lister->size_cover_gadget.NextGadget=0;
			lister->size_cover_gadget.Flags=GFLG_RELBOTTOM|GFLG_RELRIGHT|GFLG_DISABLED;
			lister->size_cover_gadget.Activation=GACT_RIGHTBORDER|GACT_BOTTOMBORDER;
			lister->size_cover_gadget.GadgetType=GTYP_BOOLGADGET;
			lister->size_cover_gadget.GadgetRender=&lister->size_gadget_border;
			lister->size_cover_gadget.SelectRender=0;
			lister->size_cover_gadget.Width=image->Width;
			lister->size_cover_gadget.Height=image->Height;
			lister->size_cover_gadget.LeftEdge=-image->Width+1;
			lister->size_cover_gadget.TopEdge=-image->Height+1;

			gadget=&lister->size_cover_gadget;

			// Free image
			DisposeObject(image);
		}

		FreeScreenDrawInfo(screen,lister->drawinfo);
		lister->drawinfo=0;
	}

	// Create iconify gadget
	if (iconify_gad=create_iconify_gadget(screen,
									 &lister->backdrop_info->boopsi_list,
									 (lister->flags&LISTERF_LOCK_POS)?TRUE:FALSE))
	{
		// Chain gadgets
		if (gadget) gadget->NextGadget=iconify_gad;
		else gadget=iconify_gad;
	}

	// Create lock gadget
	if (!(GUI->flags2&GUIF2_NO_PADLOCK) &&
			(lock_gad=
				CreateTitleGadget(
					screen,
					&lister->backdrop_info->boopsi_list,
					(lister->flags&LISTERF_LOCK_POS)?TRUE:FALSE,
					(iconify_gad)?-iconify_gad->Width:0,
					IM_LOCK,
					GAD_LOCK)))
	{
		// Chain gadgets
		if (iconify_gad) iconify_gad->NextGadget=lock_gad;
		else if (gadget) gadget->NextGadget=lock_gad;
		else gadget=lock_gad;

		// Fix selection flags
		lock_gad->Activation|=GACT_TOGGLESELECT;
		if (lister->more_flags&LISTERF_LOCK_FORMAT)
			lock_gad->Flags|=GFLG_SELECTED;
	}

	// Open lister window
	if (!(lister->window=OpenWindowTags(0,
		WA_Left,dims->Left,
		WA_Top,dims->Top,
		WA_Width,dims->Width,
		WA_Height,dims->Height,
		WA_MinWidth,lister->win_limits.Left,
		WA_MinHeight,lister->win_limits.Top,
		WA_MaxWidth,(ULONG)~0,
		WA_MaxHeight,(ULONG)~0,
		((lister->dimensions.wd_Flags&WDF_VALID) &&
		(!(lister->flags&LISTERF_LOCK_POS)))?WA_Zoom:TAG_IGNORE,(ULONG)&lister->dimensions.wd_Zoomed,
		WA_IDCMP,
				IDCMP_ACTIVEWINDOW|
				IDCMP_CHANGEWINDOW|
				IDCMP_CLOSEWINDOW|
				IDCMP_GADGETDOWN|
				IDCMP_GADGETUP|
				IDCMP_INACTIVEWINDOW|
				IDCMP_MENUHELP|
				IDCMP_MENUPICK|
				IDCMP_MENUVERIFY|
				IDCMP_MOUSEBUTTONS|
				IDCMP_MOUSEMOVE|
				IDCMP_NEWSIZE|
				IDCMP_REFRESHWINDOW|
				IDCMP_RAWKEY,
		WA_AutoAdjust,TRUE,
		WA_CloseGadget,!(lister->flags&LISTERF_LOCK_POS),
		WA_DragBar,!(lister->flags&LISTERF_LOCK_POS),
		WA_DepthGadget,TRUE,
		WA_MenuHelp,TRUE,
		WA_NewLookMenus,TRUE,
		WA_SizeGadget,!(lister->flags&LISTERF_LOCK_POS),
		(lister->flags&LISTERF_LOCK_POS)?TAG_IGNORE:WA_SizeBRight,TRUE,
		(lister->flags&LISTERF_LOCK_POS)?TAG_IGNORE:WA_SizeBBottom,TRUE,
		WA_CustomScreen,screen,
		WA_ScreenTitle,GUI->screen_title,
		WA_Gadgets,gadget,
		mode,TRUE,
		(lister->flags&LISTERF_LOCK_POS)?WA_Title:TAG_IGNORE,"",
		WA_BackFill,&lister->pattern,
		TAG_END))) return 0;

	// Get sizing gadget
	if (gadget=FindGadgetType(lister->window->FirstGadget,GTYP_SIZING))
	{
		// Remove gadget from the list
		RemoveGList(lister->window,gadget,1);

		// Add to end of list
		AddGList(lister->window,gadget,-1,1,0);
	}

	// Fix title gadgets
	FixTitleGadgets(lister->window);

	// Save left border
	lister->old_border_left=lister->window->BorderLeft;

	// Initialise ID
	SetWindowID(lister->window,&lister->id,WINDOW_LISTER,(struct MsgPort *)lister->ipc);

	// Initialise dimensions
	InitWindowDims(lister->window,&lister->dimensions);

	// Close existing font
	if (lister->font) CloseFont(lister->font);

	// Clear proportional font flag
	lister->more_flags&=~LISTERF_PROP_FONT;

	// Open font
	if (lister->font=OpenDiskFont(&lister->lister_font))
	{
		// Proportional	font?
		if (lister->font->tf_Flags&FPF_PROPORTIONAL)
		{
			lister->more_flags|=LISTERF_PROP_FONT;
		}
	}

	// Got a font?
	if (lister->font)
	{
		// Set font in window and text area
		SetFont(lister->window->RPort,lister->font);
		SetFont(&lister->text_area.rast,lister->font);
	}

	// Otherwise, get info on default font
	else
	{
		// Get font info
		AskFont(lister->window->RPort,&lister->lister_font);
	}

	// Get DrawInfo
	lister->drawinfo=GetScreenDrawInfo(lister->window->WScreen);

	// Set menus
	display_get_menu(lister->window);
	display_fix_menu(
		lister->window,
		(lister->flags&LISTERF_VIEW_ICONS)?WINDOW_LISTER_ICONS:WINDOW_LISTER,
		lister);
	lister_fix_menus(lister,0);

	// Initialise backdrop info
	backdrop_init_info(lister->backdrop_info,lister->window,0);

	// Turn on gauge if needed
	lister_set_gauge(lister,FALSE);

	// Fill out zoom covering gadget
	if ((gadget=FindGadgetType(lister->window->FirstGadget,GTYP_WZOOM)) &&
		(!(lister->flags&LISTERF_LOCK_POS)))
	{
		struct Image *image;
		struct Gadget *zgadget;

		// Get zoom gadget image
		if (image=NewObject(
					0,
					"sysiclass",
					SYSIA_DrawInfo,lister->drawinfo,
					SYSIA_Which,ZOOMIMAGE,
					IA_Height,screen->WBorTop+screen->Font->ta_YSize+1,
					TAG_END))
		{
			// Create gadget
			if (zgadget=NewObject(0,"galileobuttongclass",
				GA_ID,GAD_ZOOM,
				GA_RelRight,gadget->LeftEdge-1,
				GA_Top,gadget->TopEdge,
				GA_Width,gadget->Width,
				GA_Height,gadget->Height,
				GA_RelVerify,TRUE,
				GA_TopBorder,TRUE,
				GA_Image,image,
				TAG_END))
			{
				// Add to window
				AddGList(lister->window,zgadget,0,1,0);
				RefreshWindowFrame(lister->window);

				// Add to boopsi list
				DoMethod((Object *)image,OM_ADDTAIL,&lister->backdrop_info->boopsi_list);
				DoMethod((Object *)zgadget,OM_ADDTAIL,&lister->backdrop_info->boopsi_list);

			}
			else
			// Failed
			DisposeObject(image);
		}

	}

	// Create edit hook
	lister->path_edit_hook=
		GetEditHookTags(
			0,
			OBJECTF_NO_SELECT_NEXT,
			GTCustom_History,lister->path_history,
			TAG_END);

	// Create path field
	if (!(lister->path_field=
		NewObject(0,"galileostrgclass",
			GA_ID,GAD_PATH,
			GA_Left,lister->window->BorderLeft,
			GA_RelBottom,-(lister->window->BorderBottom+FIELD_FONT->tf_YSize+4),
			GA_RelWidth,-(lister->window->BorderLeft+lister->window->BorderRight),
			GA_Height,FIELD_FONT->tf_YSize+4,
			GA_RelVerify,TRUE,
			GA_Immediate,TRUE,
			GTCustom_ThinBorders,TRUE,
			GTCustom_NoGhost,TRUE,
			STRINGA_TextVal,(lister->cur_buffer)?lister->cur_buffer->buf_Path:"",
			STRINGA_MaxChars,511,
			STRINGA_Buffer,lister->path_buffer,
			STRINGA_UndoBuffer,GUI->global_undo_buffer,
			STRINGA_WorkBuffer,GUI->global_undo_buffer+512,
			STRINGA_Font,FIELD_FONT,
			STRINGA_EditHook,lister->path_edit_hook,
			TAG_END)))
	{
		lister_close(lister,0);
		return 0;
	}
	DoMethod((Object *)lister->path_field,OM_ADDTAIL,&lister->backdrop_info->boopsi_list);

	// Initialise side parent button
	lister->parent_button.NextGadget=lister->path_field;
	lister->parent_button.TopEdge=lister->window->BorderTop;
	lister->parent_button.Width=lister->window->BorderLeft;
	lister->parent_button.Height=-(lister->window->BorderTop+lister->window->BorderBottom);
	lister->parent_button.Flags=GFLG_GADGHCOMP|GFLG_RELHEIGHT;
	lister->parent_button.Activation=GACT_RELVERIFY|GACT_LEFTBORDER;
	lister->parent_button.GadgetType=GTYP_BOOLGADGET;
	lister->parent_button.GadgetID=GAD_PARENT;

	// Try to add AppWindow
	lister->appwindow=AddAppWindowA(
		WINDOW_LISTER,
		(ULONG)lister,
		lister->window,
		lister->app_port,0);

	// Is lister busy?
	if (lister->flags&LISTERF_BUSY || lister->old_flags&LISTERF_BUSY)
	{
		lister->flags&=~(LISTERF_BUSY|LISTERF_LOCK|LISTERF_LOCKED);
		lister_busy(lister,0);
	}
	else
	if (lister->flags&LISTERF_LOCKED || lister->old_flags&LISTERF_LOCKED)
	{
		lister->flags&=~(LISTERF_BUSY|LISTERF_LOCK|LISTERF_LOCKED);
		lister_busy(lister,1);
	}

	// Initialise lister flags
	lister->flags&=
		LISTERF_LOCK_POS|LISTERF_BUSY|
		LISTERF_LOCK|LISTERF_LOCKED|
		LISTERF_VIEW_ICONS|LISTERF_SHOW_ALL|LISTERF_PATH_FIELD|
		LISTERF_STORED_SOURCE|LISTERF_STORED_DEST|LISTERF_ICON_ACTION|
		LISTERF_FIRST_TIME;
	lister->more_flags&=~LISTERF_TITLEBARRED;

	// Update selection count if in icon mode
	if (lister->flags&LISTERF_ICON_ACTION)
		backdrop_fix_count(lister->backdrop_info,0);

	// Initialise cursor setting
	lister->cursor_line=-1;
	lister->edit_type=-1;

	// Setup lister display
	lister_init_display(lister);
	lister_refresh(lister,LREFRESH_FULL);
	lister_refresh_name(lister);

	// Clear old flags
	lister->old_flags=0;

	// Is this our first time open?
	if (lister->flags&LISTERF_FIRST_TIME)
	{
		char buf[16];

		// Build handle string	
		lsprintf(buf,"%ld",lister);

		// Launch script
		RunScript(SCRIPT_OPEN_LISTER,buf);
	}

	// If busy, show progress window
	if (lister->progress_window)
		ShowProgressWindow(lister->progress_window,0,lister->window);

	// Fix current directory
	lister_fix_cd(lister);

	// Initialise things
	lister->title_click=-1;

	// Return window pointer
	return lister->window;
}


// Close a lister display
void lister_close(Lister *lister,BOOL run_script)
{
	// Lock window
	GetSemaphore(&lister->backdrop_info->window_lock,SEMF_EXCLUSIVE,0);

	// Close window
	lister_close_window(lister,run_script);

	// Unlock window
	FreeSemaphore(&lister->backdrop_info->window_lock);
}

void lister_close_window(Lister *lister,BOOL run_script)
{
	short a;

	// If busy, hide progress window
	if (lister->progress_window)
		HideProgressWindow(lister->progress_window);

	// Fix current directory
	UnLock(CurrentDir(0));

	// Free icon remapping
	backdrop_free_remap(lister->backdrop_info,lister->window);

	// Close window
	if (lister->window)
	{
		// Cancel edit
		lister_end_edit(lister,EDITF_CANCEL|EDITF_NOREFRESH);

		// Remove 'hot name' requester
		lister_rem_hotname(lister);

		// Run script?
		if (run_script)
		{
			char buf[16];

			// Build handle string
			lsprintf(buf,"%ld",lister);

			// Run script
			RunScript(SCRIPT_CLOSE_LISTER,buf);
		}

		// Remove AppWindow
		RemoveAppWindow(lister->appwindow);
		lister->appwindow=0;

		// Remember window coordinates
		StoreWindowDims(lister->window,&lister->dimensions);
		if (lister->more_flags&LISTERF_TITLEBARRED)
		{
			lister->dimensions.wd_Normal.Width=lister->restore_dims.Width;
			lister->dimensions.wd_Normal.Height=lister->restore_dims.Height;
		}

		// If window was busy, abort timer
		if (lister->flags&LISTERF_BUSY)
			StopTimer(lister->busy_timer);

		// Clear menus
		display_free_menu(lister->window);

		// Free DrawInfo
		if (lister->drawinfo)
		{
			FreeScreenDrawInfo(lister->window->WScreen,lister->drawinfo);
			lister->drawinfo=0;
		}

		// Have we got a buffer?
		if (lister->cur_buffer)
		{
			// Store dimensions of window in buffer
			lister->cur_buffer->dimensions=*((struct IBox *)&lister->window->LeftEdge);
			lister->cur_buffer->dim_valid=1;
		}

		// Got Drag info?
		if (lister->drag_info)
		{
			// Free it
			FreeDragInfo(lister->drag_info);
			lister->drag_info=0;
		}

		// Close window
		CloseWindow(lister->window);
		lister->window=0;

		// Free edit hook
		FreeEditHook(lister->path_edit_hook);
		lister->path_edit_hook=0;
	}

	// Free render bitmap
	DisposeBitMap(lister->render_bitmap);
	lister->render_bitmap=0;

	// Free backdrop stuff
	backdrop_free_info(lister->backdrop_info);

	// Free gauge gadget 
	if (lister->gauge_gadget)
	{
		DisposeObject(lister->gauge_gadget);
		lister->gauge_gadget=0;
	}

	// Free pens
	for (a=0;a<ENVCOL_MAX;a++)
		lister_init_colour(lister,a,TRUE);

	// Remember flags
	lister->old_flags=lister->flags;

	// Clear flags
	lister->flags&=	LISTERF_LOCK_POS|
					LISTERF_ICONIFIED|
					LISTERF_BUSY|
					LISTERF_VIEW_ICONS|
					LISTERF_ICON_ACTION|
					LISTERF_SHOW_ALL;

	// Store source/destination status
	if (lister->old_flags&LISTERF_SOURCE) lister->flags|=LISTERF_STORED_SOURCE;
	else if (lister->old_flags&LISTERF_DEST) lister->flags|=LISTERF_STORED_DEST;
}


// Iconify a lister
BOOL lister_iconify(Lister *lister)
{
	struct TagItem tags[4];
	struct DiskObject *icon;

	// Check there's an icon to use
	if (!(icon=GUI->lister_icon)) return 0;

	// Have we got a distinct position?
	if (lister->icon_pos_x!=NO_ICON_POSITION)
	{
		// Try to copy icon
		if (lister->appicon_icon=CopyDiskObject(icon,DOCF_NOIMAGE))
		{
			// Use the copy
			icon=lister->appicon_icon;

			// Set position
			icon->do_CurrentX=lister->icon_pos_x-WBICONMAGIC_X;
			icon->do_CurrentY=lister->icon_pos_y-WBICONMAGIC_Y;
		}
	}

	// Make sure position is unset in icon
	else
	{
		icon->do_CurrentX=NO_ICON_POSITION;
		icon->do_CurrentY=NO_ICON_POSITION;
	}

	// Build icon name
	lister_build_icon_name(lister);

	// Tags
	tags[0].ti_Tag=GAE_Local;
	tags[0].ti_Data=1;
	tags[1].ti_Tag=GAE_Menu;
	tags[1].ti_Data=(ULONG)GetString(&locale,MSG_CLOSE);
	tags[2].ti_Tag=GAE_Special;
	tags[2].ti_Data=1;
	tags[3].ti_Tag=TAG_END;

	// Try to add AppIcon
	if (!(lister->appicon=AddAppIconA(
		WINDOW_LISTER,
		0,
		lister->icon_name,
		lister->app_port,
		0,
		icon,tags)))
	{
		// Failed; free icon if we had one
		if (lister->appicon_icon)
		{
			FreeDiskObjectCopy(lister->appicon_icon);
			lister->appicon_icon=0;
		}
		return 0;
	}

	// Set iconify flag
	lister->flags|=LISTERF_ICONIFIED;

	// Lister is busy?
	if (lister->flags&LISTERF_BUSY)
	{
		// Ghost image
		lister_set_busy_icon(lister);

		// Clear source/dest flags
		lister->flags&=~(LISTERF_STORED_SOURCE|LISTERF_STORED_DEST|LISTERF_SOURCEDEST_LOCK);
	}

	// Otherwise, clear source/dest flags
	else
	{
		lister->flags&=~(LISTERF_SOURCE|LISTERF_DEST|LISTERF_SOURCEDEST_LOCK);
	}
	return 1;
}


// Calculate size limits
void lister_calc_limits(Lister *lister,struct Screen *screen)
{
	short width=0,height;
	struct Gadget *gadget;

	// Get screen depth gadget
	if (gadget=FindGadgetType(screen->FirstGadget,GTYP_SDEPTH))
	{
		// Add room for title bar gadgets
		width=gadget->Width*((lister->flags&LISTERF_LOCK_POS)?4:5);
	}

	// In a mode with a toolbar?
	if (!(lister->flags&LISTERF_VIEW_ICONS) || lister->flags&LISTERF_ICON_ACTION)
	{
		// Valid toolbar?
		if (lister->toolbar)
		{
			// Check width is big enough for largest toolbar button plus arrow
			if (width<lister->toolbar->max_width+6)
				width=lister->toolbar->max_width+6;
		}
	}

	// Minimum width
	width+=screen->WBorLeft+screen->WBorRight+8;

	// Minimum initial height
	height=screen->WBorTop+screen->WBorBottom+screen->RastPort.TxHeight+1;

	// Status bar height
	height+=lister->name_area.box.Height;

	// Add toolbar height
	if (GUI->toolbar) height+=GUI->toolbar->height;

	// Add field titles height
	if (lister->more_flags&LISTERF_TITLE) height+=screen->RastPort.TxHeight+4;

	// Add enough for a couple of lines of text
	height+=screen->RastPort.TxHeight*2;

	// Path field height
	if (lister->flags&LISTERF_PATH_FIELD)
		height+=FIELD_FONT->tf_YSize+4;

	// Store in lister
	lister->win_limits.Left=width;
	lister->win_limits.Top=height;
}


// Set size limit for a lister
void lister_set_limits(Lister *lister)
{
	short width,height;

	// In zoomed mode?
	if (lister->more_flags&LISTERF_TITLEBARRED)
		return;

	// Calculate limits
	lister_calc_limits(lister,lister->window->WScreen);

	// Get current lister size
	width=lister->window->Width;
	height=lister->window->Height;

	// Bounds check
	if (width<lister->win_limits.Left)
		width=lister->win_limits.Left;
	if (height<lister->win_limits.Top)
		height=lister->win_limits.Top;

	// Need to resize?
	if (width!=lister->window->Width ||
		height!=lister->window->Height)
	{
		// Set to infinite limits
		WindowLimits(
			lister->window,
			1,1,
			(UWORD)~0,(UWORD)~0);

		// Change size
		ChangeWindowBox(
			lister->window,
			lister->window->LeftEdge,
			lister->window->TopEdge,
			width,
			height);
	}

	// Set window limits
	WindowLimits(
		lister->window,
		lister->win_limits.Left,
		lister->win_limits.Top,
		(UWORD)~0,
		(UWORD)~0);
}


// Build icon name
void lister_build_icon_name(Lister *lister)
{
	// Custom label?
	if (lister->cur_buffer->buf_CustomLabel[0])
		strcpy(lister->icon_name,lister->cur_buffer->buf_CustomLabel);

	// Otherwise
	else
	{
		char *name_ptr;

		// Is lister busy?
		if (lister->flags&LISTERF_BUSY)
		{
			// Start with bracket
			lister->icon_name[0]='(';
			name_ptr=lister->icon_name+1;
		}
		else name_ptr=lister->icon_name;

		// Valid path?
		if (lister->cur_buffer->buf_ExpandedPath[0])
		{
			char *ptr;

			// Copy name
			strcpy(lister->work_buffer,lister->cur_buffer->buf_ExpandedPath);

			// Strip trailing '/'
			if (*(ptr=lister->work_buffer+strlen(lister->work_buffer)-1)=='/')
				*ptr=0;

			// Get pointer to name
			if (!(ptr=FilePart(lister->work_buffer)) || !*ptr)
				ptr=lister->work_buffer;

			// Copy file part to icon name
			stccpy(name_ptr,ptr,32);
		}

		// Otherwise copy object name
		else stccpy(name_ptr,lister->cur_buffer->buf_ObjectName,32);

		// Get name pointer
		if (!*name_ptr)
		{
			char *ptr=0;

			// Cache list?
			if (lister->cur_buffer->more_flags&DWF_CACHE_LIST)
				ptr=GetString(&locale,MSG_BUFFER_LIST);

			// Device list?
			else
			if (lister->cur_buffer->more_flags&DWF_DEVICE_LIST)
				ptr=GetString(&locale,MSG_DEVICE_LIST);

			// Copy string
			if (ptr) strcpy(name_ptr,ptr);
		}

		// Is lister busy?
		if (lister->flags&LISTERF_BUSY)
		{
			// Add trailing bracket
			strcat(name_ptr,")");
		}
	}
}


// Add or remove fuel gauge
void lister_set_gauge(Lister *lister,BOOL refresh)
{
	BOOL want,change=0;

	// If lister is closed, problem is moot
	if (!lister->window ||
		(lister->more_flags&LISTERF_TITLEBARRED)) return;

	// See if lister wants a gauge
	want=lister_want_gauge(lister);

	// Does lister have gauge and we don't want it to?
	if (lister->gauge_gadget && !want)
	{
		// Remove the gadget
		RemoveGList(lister->window,lister->gauge_gadget,1);

		// Free gadget
		DisposeObject(lister->gauge_gadget);
		lister->gauge_gadget=0;

		// Fix border width
		lister->window->BorderLeft=lister->old_border_left;

		// Free pens
		lister_init_colour(lister,ENVCOL_GAUGE,TRUE);

		// Set change flag
		change=1;
	}

	// Or, do we not have it but want it?
	else
	if (!lister->gauge_gadget && want)
	{
		// Initialise pens
		lister_init_colour(lister,ENVCOL_GAUGE,FALSE);

		// Create gauge
		if (lister->gauge_gadget=
				NewObject(
					0,
					"galileogaugeclass",
					GA_ID,GAD_GAUGE,
					GA_Left,0,
					GA_Top,lister->window->BorderTop,
					GA_Width,lister->window->BorderRight,
					GA_RelHeight,-lister->window->BorderTop-lister->window->BorderBottom+3,
					GA_LeftBorder,TRUE,
					GGG_FillPen,lister->lst_Colours[ENVCOL_GAUGE].cr_Pen[0],
					GGG_FillPenAlert,lister->lst_Colours[ENVCOL_GAUGE].cr_Pen[1],
					TAG_END))
		{
			// Fix border width
			lister->window->BorderLeft=lister->window->BorderRight;

			// Add gauge to window after parent button
			AddGList(lister->window,lister->gauge_gadget,-1,1,0);
			RefreshGList(lister->gauge_gadget,lister->window,0,1);

			// Set change flag
			change=1;
		}

		// Failed
		else
		{
			// Free pens
			lister_init_colour(lister,ENVCOL_GAUGE,TRUE);
		}
	}

	// Changed?
	if (change)
	{
		// Fix left border related things
		lister->window->GZZWidth=lister->window->Width-lister->window->BorderLeft-lister->window->BorderRight;
		lister->parent_button.Width=lister->window->BorderLeft;
	}

	// Changed and need to refresh?
	if (change && refresh)
	{
		// Initialise and refresh
		lister_init_display(lister);
		lister_refresh(lister,LREFRESH_FULL|LREFRESH_FULL_ICON);

		// If the path field is added, refresh it
		if (lister->path_field && lister->flags&LISTERF_PATH_FIELD)
		{
			struct gpResize resize;

			// Fill out resize packet
			resize.MethodID=GM_RESIZE;
			resize.gpr_GInfo=0;
			resize.gpr_RPort=0;
			resize.gpr_Size.Left=lister->window->BorderLeft;
			resize.gpr_Size.Top=-(lister->window->BorderBottom+FIELD_FONT->tf_YSize+4);
			resize.gpr_Size.Width=-(lister->window->BorderLeft+lister->window->BorderRight);
			resize.gpr_Size.Height=FIELD_FONT->tf_YSize+4;
			resize.gpr_Redraw=0;
			resize.gpr_Window=lister->window;
			resize.gpr_Requester=0;

			// Tell string gadget to resize
			DoMethodA((Object *)lister->path_field,(Msg)&resize);

			// Erase behind path field
			SetAPen(lister->window->RPort,3);
			RectFill(
				lister->window->RPort,
				lister->window->BorderLeft,
				lister->window->Height+lister->path_field->TopEdge-2,
				lister->window->Width-lister->window->BorderRight-1,
				lister->window->Height+lister->path_field->TopEdge+lister->path_field->Height+1);

			// Refresh path field
			RefreshGList(lister->path_field,lister->window,0,1);
		}

		// Refresh the window frame
		RefreshWindowFrame(lister->window);

		// Do we have the gauge?
		if (lister->gauge_gadget)
		{
			// Refresh title and gauge
			lister_show_name(lister);
		}
	}
}


// Initialise or free a lister colour
void lister_init_colour(Lister *lister,short col,short free)
{
	// See if colour is allocated
	if (lister->lst_Colours[col].cr_Alloc&(1<<0))
		FreeCustomPen(col,0);
	if (lister->lst_Colours[col].cr_Alloc&(1<<1))
		FreeCustomPen(col,1);

	// Clear allocation flag
	lister->lst_Colours[col].cr_Alloc=0;

	// Want to get pen?
	if (!free)
	{
		short a;

		// Initalise allocation flag
		lister->lst_Colours[col].cr_Alloc=0;

		// Do two pens
		for (a=0;a<2;a++)
		{
			// See if there's a custom pen
			if ((lister->lst_Colours[col].cr_Pen[a]=GetCustomPen(col,a,0))>-1)
			{
				// Set allocation flag
				lister->lst_Colours[col].cr_Alloc|=(1<<a);
			}

			// No custom pen, use default
			else
			{
				// Different entries in the structure :-(
				switch (col)
				{
					case ENVCOL_FILES_UNSEL:
						lister->lst_Colours[col].cr_Pen[a]=environment->env->list_format.files_unsel[a];
						break;

					case ENVCOL_DIRS_UNSEL:
						lister->lst_Colours[col].cr_Pen[a]=environment->env->list_format.dirs_unsel[a];
						break;

					case ENVCOL_FILES_SEL:
						lister->lst_Colours[col].cr_Pen[a]=environment->env->list_format.files_sel[a];
						break;

					case ENVCOL_DIRS_SEL:
						lister->lst_Colours[col].cr_Pen[a]=environment->env->list_format.dirs_sel[a];
						break;

					case ENVCOL_SOURCE:
						lister->lst_Colours[col].cr_Pen[a]=environment->env->source_col[a];
						break;

					case ENVCOL_DEST:
						lister->lst_Colours[col].cr_Pen[a]=environment->env->dest_col[a];
						break;

					case ENVCOL_DEVICES:
						lister->lst_Colours[col].cr_Pen[a]=environment->env->devices_col[a];
						break;

					case ENVCOL_VOLUMES:
						lister->lst_Colours[col].cr_Pen[a]=environment->env->volumes_col[a];
						break;

					case ENVCOL_GAUGE:
						lister->lst_Colours[col].cr_Pen[a]=environment->env->gauge_col[a];
						break;

				}

				// Fix pen
				if (lister->lst_Colours[col].cr_Pen[a]>=4 && lister->lst_Colours[col].cr_Pen[a]<252)
					lister->lst_Colours[col].cr_Pen[a]=GUI->pens[lister->lst_Colours[col].cr_Pen[a]-4];
			}
		}
	}
}


// See if the lister wants a gauge
BOOL lister_want_gauge(Lister *lister)
{
	BOOL want=0;

	// Disabled by default for custom handlers
	if (lister->cur_buffer->buf_CustomHandler[0] && !(lister->cur_buffer->cust_flags&CUSTF_LEAVEGAUGE))
	{
		// Unless enabled by ARexx
		if (lister->cur_buffer->cust_flags&CUSTF_GAUGE)
			want=1;
	}

	// Gauge enabled in lister?
	else
	if (lister->format.flags&LFORMATF_GAUGE)
		want=1;

	return want;
}

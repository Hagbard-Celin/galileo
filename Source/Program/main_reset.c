/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 1993-2012 Jonathan Potter & GP Software
Copyright 2025 Hagbard Celine

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

#include "galileofm.h"
#include <gfm/semaphores_protos.h>
#include <gfm/config_return.h>
#include <gfm/config_display.h>
#include <gfm/button_protos.h>
#include <gfm/button_flags.h>
#include "lister_protos.h"
#include "toolbar_protos.h"
#include "misc_protos.h"
#include "environment.h"
#include "cx.h"
#include "buffers_protos.h"
#include "backdrop_protos.h"
#include "menu_data.h"
#include "scripts.h"

extern TimerHandle *filetype_timer;

// Handle reset command
void main_handle_reset(ULONG *flags,APTR data)
{
	BOOL done_drives=0;

	// Nothing happens here if startup is not completed
	if (!(GUI->flags&GUIF_DONE_STARTUP))
		return;

	// Set busy pointer
	if (GUI->window) SetBusyPointer(GUI->window);

	// Change NewIcons flags first in case we get full screen reset
	if (flags[1]&CONFIG_CHANGE_NEWICONS)
	    SetNewIconsFlags(environment->env->env_NewIconsFlags,environment->env->env_NewIconsPrecision);

	// Screen needs resetting?
	if (flags[0]&CONFIG_CHANGE_DISPLAY)
	{
		close_display(CLOSE_ALL|CLOSE_STOREPOS,FALSE);
		if (flags[0]&CONFIG_CHANGE_LIST_DISPLAY)
			listers_update(0,0);
		display_open(0);
	}

	// All windows need resetting?
	else
	if (flags[0]&CONFIG_CHANGE_PALETTE)
	{
		close_display(CLOSE_WINDOW|CLOSE_CHILDREN|CLOSE_STOREPOS,FALSE);
		if (flags[0]&CONFIG_CHANGE_LIST_DISPLAY)
			listers_update(0,0);
		display_open(0);
	}

	// Main window needs resetting?
	else
	if (flags[0]&CONFIG_CHANGE_DISPLAY_OPTIONS)
	{
		// Re-open display
		close_display(CLOSE_WINDOW|CLOSE_STOREPOS,FALSE);
		display_open(0);
	}

	else
	{
		BOOL done_listers=0;
		BOOL done_backdrop=0;


		if (flags[1]&CONFIG_CHANGE_DESKTOP_FOLDER)
		{
		    UnLock(GUI->desktop_dir_lock);
		    GUI->desktop_dir_lock = Lock(environment->env->desktop_location,ACCESS_READ);
		}

		// Desktop Icon font changed?
		if (flags[0]&CONFIG_CHANGE_ICON_FONT)
		{
			// Get new font
			backdrop_get_font(GUI->backdrop);

			// Redraw objects
			backdrop_show_objects(GUI->backdrop,flags[1]&CONFIG_CHANGE_NEWICONS?BDSF_CLEAR|BDSF_RESET|BDSF_RECALC:BDSF_CLEAR|BDSF_RESET);

			// Refresh any buttons (for the icon remap flag)
			IPC_ListCommand(&GUI->buttons_list,BUTTONEDIT_REDRAW,0,0,FALSE);

			// Set library flag for borderless icons
			SetLibraryFlags((environment->env->desktop_flags&DESKTOPF_NO_BORDERS)?LIBDF_BORDERS_OFF:0,LIBDF_BORDERS_OFF);

			done_backdrop=1;
		}

		if (!done_backdrop && flags[1]&CONFIG_CHANGE_DESKTOP_FOLDER)
		{
		    backdrop_show_objects(GUI->backdrop,BDSF_CLEAR|BDSF_RECALC);
		    done_backdrop=1;
		}
		// Window icon font changed?
		if (flags[0]&CONFIG_CHANGE_ICON_FONT_WINDOWS)
		{
			// Inform any groups and listers that are out there
			IPC_ListCommand(&GUI->lister_list,GROUP_NEW_FONT,0,0,FALSE);
			IPC_ListCommand(&GUI->group_list,GROUP_NEW_FONT,0,0,FALSE);
		}

		// Change pattern?
		if (flags[0]&CONFIG_CHANGE_BACKDROP)
		{
			// Launch process to read pattern
			misc_startup(NAME_PATTERNS,CMD_REMAP_PATTERNS,0,0,TRUE);
		}

		// Changed AppIcon status?
		if (flags[0]&CONFIG_CHANGE_APP_ICONS)
		{
			// Update AppIcons
			backdrop_read_appicons(GUI->backdrop,BDAF_SHOW|BDAF_LOCK);
		}

		// Update devices?
		if (flags[0]&CONFIG_CHANGE_BAD_DISKS)
		{
			// Update devices
			backdrop_refresh_drives(GUI->backdrop, NULL, BDEVF_SHOW|BDEVF_FORCE_LOCK);
			done_drives = 1;
		}

		// Listers need resetting?
		if (!done_listers && flags[0]&CONFIG_CHANGE_LIST_DISPLAY)
		{
			listers_update(
				(BOOL)(flags[0]&CONFIG_CHANGE_LIST_FONT),
				(BOOL)(flags[0]&CONFIG_CHANGE_LIST_FONT));
			done_listers=1;
		}

		// Lister toolbar changed?
		if (flags[0]&CONFIG_CHANGE_LIST_TOOLBAR)
		{
			ToolBarInfo *new_bar,*old;

			// Get new toolbar
			if (new_bar=OpenToolBar(data,environment->toolbar_path))
			{
				// Lock lister list
				lock_listlock(&GUI->lister_list,FALSE);

				// Clear toolbar from listers
				IPC_ListCommand(&GUI->lister_list,LISTER_TOOLBAR,0,0,TRUE);

				// Free old toolbar
				old=GUI->toolbar;
				GUI->toolbar=0;
				FreeToolBar(old);

				// Get cache for new toolbar
				GetToolBarCache(new_bar,TRUE);

				// Store as new system toolbar
				GUI->toolbar=new_bar;

				// Give to listers
				IPC_ListCommand(&GUI->lister_list,LISTER_TOOLBAR,0,(ULONG)new_bar,TRUE);

				// Unlock lister list
				unlock_listlock(&GUI->lister_list);
			}
		}

		// Lister menu bar changed?
		if (flags[0]&CONFIG_CHANGE_LIST_MENU)
		{
			// Get menu lock
			GetSemaphore(&GUI->lister_menu_lock,SEMF_EXCLUSIVE,0);

			// Free existing menu
			CloseButtonBank(GUI->lister_menu);

			// New one supplied?
			if (data) GUI->lister_menu=data;

			// Load new menu
			else
			{
			    GUI->lister_menu=OpenButtonBank(environment->menu_path, NULL);
			}
			// Got bank?
			if (GUI->lister_menu)
			{
				// Check if it needs conversion
				if (!(GUI->lister_menu->window.flags&BTNWF_FIX_MENU))
				{
					// Convert it to new format
					ConvertStartMenu(GUI->lister_menu);
					GUI->lister_menu->window.flags|=BTNWF_FIX_MENU;
				}
			}

			// Release menu lock
			FreeSemaphore(&GUI->lister_menu_lock);

			// Update listers
			listers_update(0,0);
		}

		// Filetypes changed?
		if (flags[0]&CONFIG_CHANGE_FILETYPES)
		{
			// Abort timer if it's going
			StopTimer(filetype_timer);

			// Launch process to read new filetypes
			misc_startup("galileo_filetypes",MENU_NEW_FILETYPES,0,0,1);
		}

		// Lister field titles?
		if (flags[0]&CONFIG_CHANGE_LISTER_TITLES && !done_listers)
		{
			// Refresh listers
			IPC_ListCommand(
				&GUI->lister_list,
				LISTER_REFRESH_WINDOW,
				REFRESHF_FULL,
				0,
				0);
		}

		// FIXME: Need to redraw groups and toolbars.
		// Color icon settings changed?
		if (flags[1]&CONFIG_CHANGE_NEWICONS)
		{
		    // Reload lister icons
		    IPC_ListCommand(&GUI->lister_list,LISTER_GET_ICONS,1,0,FALSE);

		    // Reload backdorp icons
		    if (!done_backdrop)
			backdrop_show_objects(GUI->backdrop,BDSF_CLEAR|BDSF_RECALC);

		}
	}

	// Locale settings changed
	if (flags[0]&CONFIG_CHANGE_LOCALE)
	{
		// Set library locale flags
		SetLocaleFlags(environment->env->settings.date_flags, environment->env->settings.date_format);

	        // Rescan listers
	        IPC_ListCommand(
		        &GUI->lister_list,
		        LISTER_RESCAN,
		        0,
		        0,
		        0);
	}

	// User menu changed?
	if (flags[0]&CONFIG_CHANGE_USER_MENU)
	{
		// Lock menu
		GetSemaphore(&GUI->user_menu_lock,SEMF_EXCLUSIVE,0);

		// Free existing menus
		CloseButtonBank(GUI->user_menu);

		// New one supplied?
		if (data) GUI->user_menu=(Cfg_ButtonBank *)data;

		// Load new menu							
		else
		{
		    GUI->user_menu=OpenButtonBank(environment->user_menu_path, NULL);
		}
		// Unlock menu
		FreeSemaphore(&GUI->user_menu_lock);

		// Rebuild user menu data
		display_build_user_menu();

		// Reset menus
		display_reset_menus(0,0);
	}

	// Changed Tools menu status?
	else
	if (flags[0]&CONFIG_CHANGE_TOOLS_MENU)
	{
		// Refresh menus
		display_build_user_menu();
		display_reset_menus(0,0);
	}

	// Hotkeys changed?
	if (flags[0]&CONFIG_CHANGE_HOTKEYS)
	{
		// Remove commodity
		cx_remove(GUI->cx);

		// Get hotkeys lock
		GetSemaphore(&GUI->hotkeys_lock,SEMF_EXCLUSIVE,0);

		// Free existing hotkeys
		CloseButtonBank(GUI->hotkeys);

		// New one supplied?
		if (data) GUI->hotkeys=data;

		// Load new menu							
		else
		{
			GUI->hotkeys=OpenButtonBank(environment->hotkeys_path, NULL);
		}
		// Release hotkeys lock
		FreeSemaphore(&GUI->hotkeys_lock);

		// Reinstall commodity
		cx_install(GUI->cx);
	}

	// Mouse hook changed?
	else
	if (flags[0]&CONFIG_CHANGE_MOUSEHOOK)
	{
		// Remove commodity
		cx_remove(GUI->cx);

		// Reinstall commodity
		cx_install(GUI->cx);
	}

	// Scripts changed?
	if (flags[0]&CONFIG_CHANGE_SCRIPTS)
	{
		// Get scripts lock
		GetSemaphore(&GUI->scripts_lock,SEMF_EXCLUSIVE,0);

		// Free existing hotkeys
		CloseButtonBank(GUI->scripts);

		// New one supplied?
		if (data) GUI->scripts=data;

		// Load new menu							
		else
		{
			GUI->scripts=OpenButtonBank(environment->scripts_path, NULL);
		}
		// Release hotkeys lock
		FreeSemaphore(&GUI->scripts_lock);
	}

	// Maximum buffers changed?
	if (flags[0]&CONFIG_CHANGE_MAX_BUFFERS)
	{
		buffers_clear(1);
	}

	// Hidden drives changed?
	if (flags[0]&CONFIG_CHANGE_HIDDEN_DRIVES && !done_drives)
	{
		// Refresh drives
		backdrop_refresh_drives(GUI->backdrop, NULL, BDEVF_FORCE_LOCK|BDEVF_SHOW);
	}

	// Shift AppIcons to tools?
	if (flags[1]&CONFIG_CHANGE_APP_TOOLS)
	{
		// Set library flag
		SetLibraryFlags((environment->env->display_options&DISPOPTF_SHIFT_APPICONS)?LIBDF_REDIRECT_TOOLS:0,LIBDF_REDIRECT_TOOLS);
	}

	// Custom drag?
	if (flags[1]&CONFIG_CHANGE_CUSTOM_DRAG)
	{
		// Set library flag
		SetLibraryFlags((environment->env->desktop_flags&DESKTOPF_NO_CUSTOMDRAG)?LIBDF_NO_CUSTOM_DRAG:0,LIBDF_NO_CUSTOM_DRAG);
	}

	// Thin borders?
	if (flags[1]&CONFIG_CHANGE_THIN_BORDERS)
	{
		// Set library flag
		SetLibraryFlags((environment->env->display_options&DISPOPTF_THIN_BORDERS)?LIBDF_THIN_BORDERS:0,LIBDF_THIN_BORDERS);
	}

	// Path list changed?
	if (flags[1]&CONFIG_CHANGE_PATHLIST)
	{
		// Update pathlist environment variable
		env_update_pathlist();
	}

	// MUFS flag changed?
	if (flags[1]&CONFIG_CHANGE_MUFS)
	{
		// Update pathlist environment variable
		env_fix_mufs();
	}

	// Themes path changed?
	if (flags[1]&CONFIG_CHANGE_THEMES)
	{
		// Update themes assignment
		env_fix_themes();
	}

	// Popup delay changed?
	if (flags[1]&CONFIG_CHANGE_POPUP_DELAY)
	{
		// Set popup delay
		SetPopUpDelay(environment->env->settings.popup_delay);
	}

	// Sound events changed
	if (flags[1]&CONFIG_CHANGE_SOUNDS)
	{
		// Initialise sounds	
		InitSoundEvents(FALSE);
	}

	// Clear busy pointer
	if (GUI->window) ClearPointer(GUI->window);
}




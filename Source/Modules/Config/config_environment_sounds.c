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
#include "config_environment.h"
#include "//Modules/modules.h"

void config_env_show_sound(config_env_data *data)
{
	short num;
	Cfg_SoundEntry *sound;

	// Get selection
	if ((num=GetGadgetValue(data->option_list,GAD_SETTINGS_SOUNDLIST))>-1 &&
		(sound=(Cfg_SoundEntry *)Att_FindNode((Att_List *)&data->sound_list,num)) && sound->dse_Sound[0])
	{
		SetGadgetValue(data->option_list,GAD_SETTINGS_SOUNDLIST_PATH,(ULONG)sound->dse_Sound);
		SetGadgetValue(data->option_list,GAD_SETTINGS_VOLUME,sound->dse_Volume);
		SetGadgetValue(data->option_list,GAD_SETTINGS_VOLUME_SLIDER,sound->dse_Volume);
	}
	else
	{
		SetGadgetValue(data->option_list,GAD_SETTINGS_SOUNDLIST_PATH,0);
		SetGadgetValue(data->option_list,GAD_SETTINGS_VOLUME,64);
		SetGadgetValue(data->option_list,GAD_SETTINGS_VOLUME_SLIDER,64);
	}
}

void config_env_store_sound(config_env_data *data)
{
	short num;
	Cfg_SoundEntry *sound;

	// Get selection
	if ((num=GetGadgetValue(data->option_list,GAD_SETTINGS_SOUNDLIST))>-1 &&
		(sound=(Cfg_SoundEntry *)Att_FindNode((Att_List *)&data->sound_list,num)))
	{
		short flags,old_volume;
		char old_sound[256];
		
		strcpy(old_sound,sound->dse_Sound);
		old_volume=sound->dse_Volume;

		strcpy(sound->dse_Sound,(char *)GetGadgetValue(data->option_list,GAD_SETTINGS_SOUNDLIST_PATH));
		UpdateGadgetValue(data->option_list,0,GAD_SETTINGS_VOLUME);
		if ((sound->dse_Volume=GetGadgetValue(data->option_list,GAD_SETTINGS_VOLUME))<0)
			sound->dse_Volume=0;
		else
		if (sound->dse_Volume>64)
			sound->dse_Volume=64;
		SetGadgetValue(data->option_list,GAD_SETTINGS_VOLUME,sound->dse_Volume);
		SetGadgetValue(data->option_list,GAD_SETTINGS_VOLUME_SLIDER,sound->dse_Volume);

		// Did sound change?
		if (strcmp(old_sound,sound->dse_Sound)!=0)
		{
			// Fix pen usage
			flags=sound->dse_Node.lve_Flags;
			if (sound->dse_Sound[0])
				sound->dse_Node.lve_Flags|=LVEF_USE_PEN;
			else
				sound->dse_Node.lve_Flags&=~LVEF_USE_PEN;
			if (flags!=sound->dse_Node.lve_Flags)
			{
				SetGadgetChoices(data->objlist,GAD_SETTINGS_SOUNDLIST,(APTR)~0);
				SetGadgetChoices(data->objlist,GAD_SETTINGS_SOUNDLIST,&data->sound_list);
			}
			sound->dse_Node.lve_Flags|=LVEF_TEMP;
		}

		// Or volume?
		else
		if (old_volume!=sound->dse_Volume)
			sound->dse_Node.lve_Flags|=LVEF_TEMP;
	}
}

void config_env_test_sound(config_env_data *data)
{
	struct Library *ModuleBase;
	BOOL ok=0;

	// Busy the window
	SetWindowBusy(data->window);

	// Open play.gfmmodule
	if (ModuleBase=OpenLibrary("PROGDIR:modules/play.gfmmodule",0))
	{
		short num;
		Cfg_SoundEntry *sound;

		// Get selection
		if ((num=GetGadgetValue(data->option_list,GAD_SETTINGS_SOUNDLIST))>-1 &&
			(sound=(Cfg_SoundEntry *)Att_FindNode((Att_List *)&data->sound_list,num)) &&
			sound->dse_Sound[0])
		{
			struct Node node;
			struct List list;

			// Build file list
			NewList(&list);
			node.ln_Name=sound->dse_Sound;
			AddTail(&list,&node);

			// Play the sound
			Module_Entry(
				&list,
				data->window->WScreen,
				data->ipc,
				data->main_ipc,
				0,
				(ULONG)data->window,
				sound->dse_Volume<<8);
			ok=1;
		}

		// Close module
		CloseLibrary(ModuleBase);
	}

	// Unbusy the window
	ClearWindowBusy(data->window);
	if (!ok) DisplayBeep(data->window->WScreen);
}

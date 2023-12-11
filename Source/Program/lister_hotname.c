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

#include "galileofm.h"

// Add 'hot name' requester
void lister_add_hotname(Lister *lister,char key)
{
	struct Gadget *gadget;
	struct Hook *hook;

	// Already added?
	if (lister->hot_name_req) return;

	// Must have path field gadget
	if (!(lister->flags&LISTERF_PATH_FIELD)) return;

	// Allocate requester
	if (!(lister->hot_name_req=AllocVec(sizeof(struct Requester),MEMF_CLEAR)))
		return;

	// Get edit hook
	if (!(hook=
		GetEditHookTags(
			0,
			OBJECTF_NO_SELECT_NEXT|OBJECTF_PATH_FILTER,
			GTCustom_ChangeSigTask,FindTask(0),
			GTCustom_ChangeSigBit,lister->hot_name_bit,
			TAG_END)))
	{
		FreeVec(lister->hot_name_req);
		lister->hot_name_req=0;
		return;
	}

	// Initialise buffer
	lister->hot_name[0]=key;
	lister->hot_name[1]=0;

	// Create gadget
	if (!(gadget=
		NewObject(0,"galileostrgclass",
			GA_ID,GAD_PATH,
			GA_RelWidth,0,
			GA_RelHeight,0,
			GA_RelVerify,TRUE,
			STRINGA_MaxChars,32,
			STRINGA_Buffer,lister->hot_name,
			STRINGA_Font,FIELD_FONT,
			STRINGA_EditHook,hook,
			TAG_END)))
	{
		// Free stuff
		FreeEditHook(hook);
		FreeVec(lister->hot_name_req);
		lister->hot_name_req=0;
		return;
	}

	// Set requester flag in gadget
	gadget->GadgetType|=GTYP_REQGADGET;

	// Initialise position to cover path field
	lister->hot_name_req->LeftEdge=lister->name_area.box.Left+(lister->name_area.box.Height>>1);
	lister->hot_name_req->TopEdge=lister->name_area.box.Top+(lister->name_area.box.Height>>1);
	lister->hot_name_req->Width=lister->name_area.box.Width;
	lister->hot_name_req->Height=FIELD_FONT->tf_YSize+4;

	// Initialise gadget and flags
	lister->hot_name_req->ReqGadget=gadget;
	lister->hot_name_req->Flags=NOISYREQ|SIMPLEREQ|NOREQBACKFILL;

	// Show requester
	if (!(Request(lister->hot_name_req,lister->window)))
	{
		// Free stuff
		lister_rem_hotname(lister);
		return;
	}

	// Activate gadget
	ActivateGadget(gadget,lister->window,lister->hot_name_req);

	// Save hook pointer
	lister->hot_name_hook=hook;

	// Handle initial key press
	lister_handle_hotname(lister);
	return;
}


// Remove 'hot name' requester
void lister_rem_hotname(Lister *lister)
{
	// Got requester?
	if (lister->hot_name_req)
	{
		// Remove it
		EndRequest(lister->hot_name_req,lister->window);

		// Free gadget
		DisposeObject(lister->hot_name_req->ReqGadget);

		// Free hook
		FreeEditHook(lister->hot_name_hook);
		lister->hot_name_hook=0;

		// Free structure
		FreeVec(lister->hot_name_req);
		lister->hot_name_req=0;
	}
}


// Handle 'hot name' input
void lister_handle_hotname(Lister *lister)
{
	// Valid name?
	if (lister->hot_name[0])
	{
		// Show entry
		lister_show_char(lister,lister->hot_name);
	}
}

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

#include "galileofmlib.h"
#include "layout_routines.h"

// Resize a window
void __asm __saveds L_LayoutResize(register __a0 struct Window *window)
{
	ObjectList *list;
	GL_Object *object;
	WindowData *wdata=(WindowData *)window->UserData;
	struct IBox box={0};

	// Clear window
	EraseRect(
		window->RPort,
		window->BorderLeft,
		window->BorderTop,
		window->BorderLeft+window->GZZWidth-1,
		window->BorderTop+window->GZZHeight-1);

	// Go through lists
	for (list=(ObjectList *)wdata->object_list.mlh_Head;list->node.mln_Succ;list=(ObjectList *)list->node.mln_Succ)
	{
		GL_Object *last_object;

		// Go through objects
		for (object=list->firstobject,last_object=0;object;last_object=object,object=object->next)
		{
			struct TagItem *rel_tag, *pos_tag;
			BOOL rel=FALSE,pos=FALSE;
			GL_Object *rel_obj=0, *pos_obj=0;

			// Get position object
			if (pos_tag=FindTagItem(GTCustom_LayoutPos,object->tags))
				if (pos_obj=L_GetObject((ObjectList *)wdata->object_list.mlh_Head,pos_tag->ti_Data))
					pos=TRUE;

			// Get relative object
			if (rel_tag=FindTagItem(GTCustom_LayoutRel,object->tags))
				if (rel_obj=L_GetObject((ObjectList *)wdata->object_list.mlh_Head,rel_tag->ti_Data))
					rel=TRUE;

			// Recalculate position
			L_CalcObjectDims(
				window,
				window->RPort->Font,
				&object->char_dims,
				&box,
				last_object,
				(rel_tag)?0:(wdata->flags&WINDOW_LAYOUT_ADJUST)?WINDOW_LAYOUT_ADJUST:0,
				object,
				(rel)?rel_obj:0,
				(pos)?pos_obj:0);

			// Store new dimensions in object
			object->dims=box;

			// Is object a gadget?
			if (object->type==OD_GADGET)
			{
				struct Gadget *gadget;
				struct gpResize resize;
				long pos;

				// Get gadget pointer (for gadtools or boopsi)
				gadget=
					(IS_GADTOOLS(object))?
						object->gl_info.gl_gadget.context:
						object->gl_info.gl_gadget.gadget;

				// Remove gadget from window
				pos=RemoveGList(window,gadget,object->gl_info.gl_gadget.components);

				// Fill out resize packet for boopsi gadgets
				resize.MethodID=GM_RESIZE;
				resize.gpr_GInfo=0;
				resize.gpr_RPort=0;
				resize.gpr_Size=box;
				resize.gpr_Redraw=0;
				resize.gpr_Window=window;
				resize.gpr_Requester=0;

				// Look at type
				switch (object->object_kind)
				{
					// Something that supports resizing?
					case MY_LISTVIEW_KIND:
					case STRING_KIND:
					case INTEGER_KIND:
					case BUTTON_KIND:
					case CHECKBOX_KIND:
					case NUMBER_KIND:
					case TEXT_KIND:
					case FILE_BUTTON_KIND:
					case DIR_BUTTON_KIND:
					case FONT_BUTTON_KIND:
					case FRAME_KIND:

						// Under 39 we have a neat call
						if (IntuitionBase->LibNode.lib_Version>=39)
						{
							// Tell it to resize
							DoGadgetMethodA(gadget,window,0,(Msg)&resize);
						}

						// Tell it the old way
						else
						{
							// Tell it to resize
							DoMethodA((Object *)gadget,(Msg)&resize);
						}
						break;


					// Unknown
					default:

						// Slider gadget?
						if (object->object_kind==SLIDER_KIND)
						{
							struct Gadget *gadget;

							// Get slider gadget
							gadget=object->gl_info.gl_gadget.context->NextGadget;

							// Slider gadget sits inside the border
							gadget->LeftEdge=box.Left+4;
							gadget->TopEdge=box.Top+2;
							gadget->Width=box.Width-8;
							gadget->Height=box.Height-4;

							// Get border gadget
							gadget=gadget->NextGadget;

							// This becomes the full size of the gadget
							*((struct IBox *)&gadget->LeftEdge)=box;
							break;
						}

						// Reposition and resize forcibly
						*((struct IBox *)&object->gl_info.gl_gadget.gadget->LeftEdge)=box;
						break;
				}

				// Add gadget back to window
				AddGList(window,gadget,pos,object->gl_info.gl_gadget.components,0);
			}

			// Or an area?
			else
			if (object->type==OD_AREA || object->type==OD_TEXT)
			{
				// Store area size
				object->gl_info.gl_area.area_pos=object->dims;
				object->flags&=~OBJECTFLAG_DRAWN;
			}
		}
	}

#ifndef BACKFILL
	// Need backfill?
	if (wdata->flags&WINDOW_REQ_FILL)
	{
		// Back-fill window
		_req_backfill(window);
	}
#endif

	// Refresh border
	RefreshWindowFrame(window);

	// Refresh objects
	L_RefreshObjectList(window,0);

	// Refresh gadgets
	RefreshGadgets(window->FirstGadget,window,0);
}


// Set window limits
void __asm __saveds L_SetConfigWindowLimits(
	register __a0 struct Window *window,
	register __a1 ConfigWindow *mindims,
	register __a2 ConfigWindow *maxdims)
{
	short minwidth=0,minheight=0,maxwidth=0,maxheight=0;

	// Calculate minimum size
	if (mindims)
	{
		minwidth=(mindims->char_dim.Width*window->WScreen->RastPort.TxWidth)+mindims->fine_dim.Width;
		minheight=(mindims->char_dim.Height*window->WScreen->RastPort.TxHeight)+mindims->fine_dim.Height;
	}

	// Calculate maximum size
	if (maxdims)
	{
		maxwidth=(maxdims->char_dim.Width*window->WScreen->RastPort.TxWidth)+maxdims->fine_dim.Width;
		maxheight=(maxdims->char_dim.Height*window->WScreen->RastPort.TxHeight)+maxdims->fine_dim.Height;
	}

	// Set size
	if (window)
		WindowLimits(window,
			(mindims)?minwidth:0,
			(mindims)?minheight:0,
			(maxdims)?maxwidth:0,
			(maxdims)?maxheight:0);
}

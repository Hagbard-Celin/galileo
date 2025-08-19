/*

Galileo Amiga File-Manager and Workbench Replacement
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

/// PropGadget dispatcher
ULONG __asm __saveds propgadget_dispatch(register __a0 Class *cl,
					 register __a2 Object *obj,
					 register __a1 Msg msg)

{
    struct Gadget *gadget = (struct Gadget *)obj;
    PropGadgetData *data = 0;
    ULONG ret = NULL;
    BOOL vert = 0;

    if (obj && msg->MethodID != OM_NEW)
    {
	data = INST_DATA(cl,obj);
	vert = (data->Flags & GPROPG_FREEVERT)?TRUE:FALSE;
    }

    switch (msg->MethodID)
    {
	case GM_RENDER:

	    {
		struct GadgetInfo *gi = (struct GadgetInfo *)((struct gpRender*)msg)->gpr_GInfo;

		propgadget_calc_coords(cl, data, gadget, gi, vert, TRUE);
		propgadget_render (cl, gadget, gi, (struct gpRender *)msg);
	    }
	    return TRUE;

	case GM_HITTEST:

	    return GMR_GADGETHIT;

	case GM_GOACTIVE:

	    if( ((struct gpInput *)msg)->gpi_IEvent && !(gadget->Flags & GFLG_DISABLED) )
	    {
		struct InputEvent *input = (struct InputEvent *)((struct gpInput *)msg)->gpi_IEvent;

		gadget->Flags |= GFLG_SELECTED;

		if ( input->ie_Class == IECLASS_RAWMOUSE )
		{
		    struct gpInput *gpi = (struct gpInput *)msg;

		    data->OrgKnobPos = data->OldKnobPos = (data->Flags & GPROPG_FREEVERT)?data->Knob.MinY:data->Knob.MinX;

		    // Click on knob?
		    if ((vert && (gpi->gpi_Mouse.Y >= (data->Knob.MinY - data->PropGadget.MinY)) &&
			(gpi->gpi_Mouse.Y <= (data->Knob.MaxY - data->PropGadget.MinY))) ||
			((!vert) && (gpi->gpi_Mouse.X >= (data->Knob.MinX - data->PropGadget.MinX)) &&
			(gpi->gpi_Mouse.X <= (data->Knob.MaxX - data->PropGadget.MinX))))
		    {
			data->OrgTop = data->Top;
			gadget->Flags |= GFLG_SELECTED;
			data->Scrolling = TRUE;

			if (vert)
			    data->MouseOffset = gpi->gpi_Mouse.Y - (data->Knob.MinY - data->PropGadget.MinY);
			else
			    data->MouseOffset = gpi->gpi_Mouse.X - (data->Knob.MinX - data->PropGadget.MinX);

			propgadget_render(cl, gadget, gpi->gpi_GInfo, (struct gpRender *)msg);

			return GMR_MEACTIVE;
		    }
		    else
		    {
			// Click outside of knob ...
			if ((vert && (gpi->gpi_Mouse.Y < (data->Knob.MinY - data->PropGadget.MinY))) ||
			    ((!vert) && (gpi->gpi_Mouse.X < (data->Knob.MinX - data->PropGadget.MinX))))
			{
			    // ... above/left
			    data->Top -= data->JumpSize;

			    if (data->Top < 0)
				data->Top = 0;
			}
			else
			{
			    // ... below/right
			    data->Top += data->JumpSize;

			    if (data->Top > (data->Total - data->Visible))
				data->Top = (data->Total - data->Visible);
			}

			// Move knob
			if (vert)
			{
			    if (data->Top == (data->Total - data->Visible))
			    {
				data->Knob.MaxY = data->PropGadget.MaxY;
				data->Knob.MinY = data->PropGadget.MaxY - data->KnobSize + 1;
			    }
			    else
			    {
				data->Knob.MinY = UDivMod32(UMult32(data->Top, (data->Height - data->KnobSize)), (data->Total- data->Visible)) + data->PropGadget.MinY;
				data->Knob.MaxY = data->KnobSize + data->Knob.MinY - 1;
			    }
			}
			else
			{
			    if (data->Top == (data->Total - data->Visible))
			    {
				data->Knob.MaxX = data->PropGadget.MaxX;
				data->Knob.MinX = data->PropGadget.MaxX - data->KnobSize + 1;
			    }
			    else
			    {
				data->Knob.MinX = UDivMod32(UMult32(data->Top, (data->Width - data->KnobSize)), (data->Total- data->Visible)) + data->PropGadget.MinX;
				data->Knob.MaxX = data->KnobSize + data->Knob.MinX -1;
			    }
			}
			propgadget_render(cl, gadget, gpi->gpi_GInfo, (struct gpRender *)msg);
			propgadget_notify(cl, obj, (struct opUpdate *)msg, data, gpi->gpi_GInfo, NULL);
		    }
		}
	    }
	    return  GMR_NOREUSE;

	case GM_HANDLEINPUT:

	    {
		struct gpInput *gpi = (struct gpInput *)msg;
		struct InputEvent	*ie = gpi->gpi_IEvent;
		BOOL render = FALSE;
		WORD *KnobPos, *KnobEnd, *Mouse, *GadPos, *GadEnd;

		// Default is active
		ret = GMR_MEACTIVE;

		if ( ie->ie_Class == IECLASS_RAWMOUSE )
		{
		    KnobPos = vert?&data->Knob.MinY:&data->Knob.MinX;
		    KnobEnd = vert?&data->Knob.MaxY:&data->Knob.MaxX;
		    Mouse = vert?&gpi->gpi_Mouse.Y:&gpi->gpi_Mouse.X;
		    GadPos = vert?&data->PropGadget.MinY:&data->PropGadget.MinX;
		    GadEnd = vert?&data->PropGadget.MaxY:&data->PropGadget.MaxX;


		    if (data->Scrolling)
		    {
			// Is pointer abowe/left-of gadget?
			if (*Mouse < data->MouseOffset)
			{
			    *KnobPos = *GadPos;
			    *KnobEnd = *GadPos + data->KnobSize -1;
			}
			else
			// Below/right-of gadget?
			if (*Mouse > ((*GadEnd - *GadPos - data->KnobSize + 1) + data->MouseOffset))
			{
			    *KnobEnd = *GadEnd;
			    *KnobPos = *GadEnd - data->KnobSize + 1;
			}
			else
			// Or inside gadget?
			{
			    *KnobPos = *GadPos + *Mouse - data->MouseOffset;
			    *KnobEnd = *KnobPos + data->KnobSize -1;
			}

			// Min Position
			if (*KnobPos <= *GadPos)
			{
			    *KnobPos = *GadPos;
			    *KnobEnd = *GadPos + data->KnobSize - 1;
			}

			// Max position
			if (*KnobEnd >= *GadEnd)
			{
			    *KnobEnd = *GadEnd;
			    *KnobPos = *GadEnd - data->KnobSize + 1;
			}

			// Only redraw if knob moved
			if (*KnobPos != data->OldKnobPos)
			{
			    ULONG CurrTop;
			    render = TRUE;
			    data->OldKnobPos = *KnobPos;

			    if (data->Top != (CurrTop = UDivMod32(UMult32((*KnobPos + data->HalfStep - *GadPos), (data->Total - data->Visible)), (data->RelSize - data->KnobSize))))
			    {
				if (CurrTop > (data->Total - data->Visible))
				{
				    data->Top =	    (data->Total - data->Visible);
				}
				else
				{
				    data->Top = CurrTop;
				}
				propgadget_notify(cl, (Object *)gadget, (struct opUpdate *)gpi, data, gpi->gpi_GInfo, OPUF_INTERIM);
			    }
			}
		    }

		    // Menu button: abort
		    if ( ie->ie_Code == IECODE_RBUTTON )
		    {
			if (KnobPos && (*KnobPos != data->OrgKnobPos))
			{
			    *KnobPos = data->OrgKnobPos;
			    *KnobEnd = data->KnobSize + *KnobPos - 1;
			    data->Top =	    data->OrgTop;
			    propgadget_notify(cl, (Object *)gadget, (struct opUpdate *)gpi, data, gpi->gpi_GInfo, NULL);

			    data->Scrolling = FALSE;
			}

			// Aborted by menu button
		        *gpi->gpi_Termination = 1;
			ret = GMR_NOREUSE | GMR_VERIFY;
		    }

		    // Select up: user is done, don't reuse input event
		    else
		    if ( ie->ie_Code == (IECODE_LBUTTON | IECODE_UP_PREFIX ) )
		    {
			if ((data->Flags & GPROPG_SPRINGLOADED) && (*KnobPos != data->OrgKnobPos))
			{
			    if (data->Top == (data->Total - data->Visible))
			    {
				*KnobEnd = *GadEnd;
				*KnobPos = *GadEnd - data->KnobSize +1;
			    }
			    else
			    {
				*KnobPos = UDivMod32(UMult32(data->Top, (data->RelSize - data->KnobSize)), (data->Total - data->Visible)) + *GadPos;
				*KnobEnd = data->KnobSize + *KnobPos - 1;
			    }
			}

			propgadget_notify(cl, (Object *)gadget, (struct opUpdate *)gpi, data, gpi->gpi_GInfo, NULL);

			data->Scrolling = FALSE;

			// Normal termination
		        *gpi->gpi_Termination = 0;
			ret = GMR_NOREUSE | GMR_VERIFY;
		    }
		    if (render)
			propgadget_render(cl, gadget, gpi->gpi_GInfo, (struct gpRender *)gpi);
		}
	    }
	    return (ret);


	case GM_GOINACTIVE:

	    gadget->Flags &= ~GFLG_SELECTED;
	    data->Scrolling = FALSE;
	    data->OrgTop = 0;
	    propgadget_render(cl, gadget, ((struct gpRender *)msg)->gpr_GInfo, (struct gpRender *)msg);
	    propgadget_notify(cl, obj, (struct opUpdate *)msg, data, ((struct gpRender *)msg)->gpr_GInfo, NULL);
	    return 0L;
#if 0
	case GM_LAYOUT:

	    // This method is only supported on V39 and above
	    layout_gpropg (cl, gadget, (struct gpLayout *)msg);
	    return TRUE;
#endif
	case OM_SET:

	case OM_UPDATE:

	    if (!data->Scrolling)
	    {
		ret = DoSuperMethodA(cl, obj, msg);
		{
		    struct TagItem *tag, *tags;
		    struct GadgetInfo *gi = (struct GadgetInfo *)((struct opSet*)msg)->ops_GInfo;

		    tags = ((struct opSet *)msg)->ops_AttrList;

		    if (tag = FindTagItem(PGA_Visible,tags))
		    {
			data->Visible = tag->ti_Data;
			ret = 1L;
		    }

		    if (tag = FindTagItem(PGA_Total,tags))
		    {
			if (tag->ti_Data <= 0x7FFF)
			    data->Total = tag->ti_Data;
			else
			    data->Total = 0x7FFF;

			propgadget_calc_coords(cl, data, gadget, gi, vert, FALSE);

			if (data->Top > (data->Total - data->Visible))
			{
				data->Top = data->Total - data->Visible;
				propgadget_notify(cl, obj, (struct opUpdate *)msg, data, gi, NULL);
			}
			ret = 1L;
		    }

		    if (tag = FindTagItem(PGA_Top,tags))
		    {
			if ((tag->ti_Data) > (data->Total - data->Visible))
				tag->ti_Data = data->Total - data->Visible;

			data->Top = tag->ti_Data;
			propgadget_notify(cl, obj, (struct opUpdate *)msg, data, gi, NULL);
			ret = 1L;
		    }

		    if (ret)
		    {
			propgadget_calc_coords(cl, data, gadget, gi, vert, FALSE);
			propgadget_render(cl, gadget, gi, (struct gpRender *)msg);
		    }
		}
	    }
	    return ret;

	case OM_GET:
	    {
	        struct opGet *og = (struct opGet *)msg;

		ret = TRUE;

		switch( og->opg_AttrID )
		{
		    case PGA_Top:
			*(og->opg_Storage)= (ULONG)data->Top;
			break;
		    case PGA_Freedom:
			*(og->opg_Storage)= (vert)?FREEVERT:FREEHORIZ;
			break;
		    default:
			ret = DoSuperMethodA(cl, obj, msg);
		}
	    }
	    return ret;

	case OM_NEW:
	    {
	        Object *object;

		if (object = (Object *)DoSuperMethodA(cl, obj, msg))
		{
		    struct TagItem *tags;
		    ULONG a;

		    data = INST_DATA(cl, object);

		    // Initialize instance data
		    tags = ((struct opSet *)msg)->ops_AttrList;
		    data->Scrolling = FALSE;

		    if ((a = GetTagData(PGA_Total, NULL, tags)) <= 0x7FFF)
			data->Total = (WORD)a;
		    else
			data->Total = 0x7FFF;

		    if ((a = GetTagData(PGA_Visible, NULL, tags)) <= 0x7FFF)
			data->Visible = (WORD)a;
		    else
			data->Visible = 0x7FFF;

		    if ((a = GetTagData(PGA_Top, NULL, tags)) <= data->Total - data->Visible)
			data->Top = (WORD)a;
		    else
			data->Top = data->Total - data->Visible;

		    data->Flags = (GetTagData(PGA_Freedom, FREEVERT, tags) & FREEVERT)?GPROPG_FREEVERT:GPROPG_FREEHORIZ;
		    data->Flags |= (GetTagData(GPGA_SpringLoaded, FALSE, tags))?GPROPG_SPRINGLOADED:NULL;

		    ret = (ULONG)object;
		}
	    }
	    return(ret);

	default:

	    return DoSuperMethodA (cl, obj, msg);
    }
}
///

void propgadget_calc_coords(Class *cl, PropGadgetData *data, struct Gadget *gadget, struct GadgetInfo *gi, BOOL vert, BOOL full)
{
    // Initial setup and in case the window-size changed
    if (full)
    {
	    // Gaget size/position
	    data->PropGadget.MinX = gadget->LeftEdge;
	    data->PropGadget.MinY = gadget->TopEdge;
	    data->Width = gadget->Width;
	    data->Height = gadget->Height;

	    if (gadget->Flags & GFLG_RELRIGHT)
		    data->PropGadget.MinX += gi->gi_Domain.Width - 1;

	    if (gadget->Flags & GFLG_RELBOTTOM)
		    data->PropGadget.MinY += gi->gi_Domain.Height - 1;

	    if (gadget->Flags & GFLG_RELWIDTH)
		    data->Width += gi->gi_Domain.Width;

	    if (gadget->Flags & GFLG_RELHEIGHT)
		    data->Height += gi->gi_Domain.Height;

	    data->PropGadget.MaxX = data->PropGadget.MinX + data->Width - 1;
	    data->PropGadget.MaxY = data->PropGadget.MinY + data->Height - 1;
    }

    // Size of each jump when clicking outside knob
    // Step only one line at a time if only one or two lines visible
    data->JumpSize = (data->Visible <= 2)?1:data->Visible - 1;

    // Get size of relevant axis
    data->RelSize = vert?data->Height:data->Width;

    // Halfstep is used to calculate when to change PGA_Top when dragging knob
    if (data->Total > data->Visible)
    {
	data->KnobSize = UDivMod32(UMult32(data->RelSize, data->Visible), (data->Total)?data->Total:1);
	data->HalfStep = UDivMod32(UDivMod32((data->RelSize - data->KnobSize), (data->Total - data->Visible)), 2);
    }
    else
    {
	data->KnobSize = data->RelSize;
	data->HalfStep = 0;
    }

    // Set minimum knob-size
    if (data->KnobSize < 6)
	data->KnobSize = 6;

    // Knob size/position
    if (data->Total > data->Visible)
    {
	data->Knob.MinX = vert?data->PropGadget.MinX:UDivMod32(UMult32(data->Top, (data->Width - data->KnobSize)), (data->Total- data->Visible)) + data->PropGadget.MinX;
	data->Knob.MinY = vert?UDivMod32(UMult32(data->Top, (data->Height - data->KnobSize)), (data->Total - data->Visible)) + data->PropGadget.MinY:data->PropGadget.MinY;
	data->Knob.MaxX = vert?data->PropGadget.MaxX:data->KnobSize + data->Knob.MinX - 1;
	data->Knob.MaxY = vert?data->KnobSize + data->Knob.MinY - 1:data->PropGadget.MaxY;
    }
    else
    {
	data->Knob.MinX = data->PropGadget.MinX;
	data->Knob.MinY = data->PropGadget.MinY;
	data->Knob.MaxX = data->PropGadget.MaxX;
	data->Knob.MaxY = data->PropGadget.MaxY;
    }
}


/// Render
ULONG propgadget_render(Class *cl, struct Gadget *gadget, struct GadgetInfo *GInfo, struct gpRender *msg)
{
    PropGadgetData *data = INST_DATA(cl, (Object *)gadget);
    struct RastPort *rp;
    ULONG retval = TRUE;
    UWORD *pens = GInfo->gi_DrInfo->dri_Pens;

    static const UWORD ditherData[] = {0xAAAA, 0x5555};

    // Use supplied rastport or get from gadgetinfo
    if (msg->MethodID == GM_RENDER)
	rp = msg->gpr_RPort;
    else
	rp = ObtainGIRPort(GInfo);

    if (rp)
    {
	UWORD fill, shine, shadow;

	// Get pens
	shine  = pens[SHINEPEN];
	shadow = pens[SHADOWPEN];
	if (gadget->Flags & GFLG_SELECTED)
	    fill   = pens[FILLPEN];
	else
	    fill   = pens[BACKGROUNDPEN];


	// Draw Background pattern
	SetDrMd(rp, JAM2);
	SetAfPt(rp, ditherData, 1);
	SetAPen(rp, pens[BACKGROUNDPEN]);
	SetBPen(rp, pens[SHADOWPEN]);
	RectFill(rp,data->PropGadget.MinX,
		 data->PropGadget.MinY,
		 data->PropGadget.MaxX,
		 data->PropGadget.MaxY);
	SetAfPt(rp, NULL, 0);

	// Draw knob
	SetDrMd(rp, JAM1);
	SetAPen(rp, fill);
	RectFill(rp, data->Knob.MinX,
		 data->Knob.MinY,
		 data->Knob.MaxX,
		 data->Knob.MaxY);

	// Knob shine border
	SetAPen(rp, shine);
	Move(rp, data->Knob.MinX, data->Knob.MaxY - 1);
	Draw(rp, data->Knob.MinX, data->Knob.MinY);
	Draw(rp, data->Knob.MaxX - 1, data->Knob.MinY);

	// Knob shadow border
	SetAPen(rp, shadow);
	Move(rp, data->Knob.MinX, data->Knob.MaxY);
	Draw(rp, data->Knob.MaxX, data->Knob.MaxY);
	Draw(rp, data->Knob.MaxX, data->Knob.MinY);

	// Free the rastport if we allocated it
	if (msg->MethodID != GM_RENDER)
	    ReleaseGIRPort(rp);
    }
    else retval = FALSE;
    return(retval);
}
///

void propgadget_notify(Class *cl, Object *obj, struct opUpdate *msg, PropGadgetData *data, struct GadgetInfo *GInfo, ULONG flags)
{
    struct TagItem tt[3];

    // Notify message tags
    tt[0].ti_Tag = GA_ID;
    tt[0].ti_Data = ((struct Gadget *)obj)->GadgetID;
    tt[1].ti_Tag = PGA_Top;
    tt[1].ti_Data = data->Top;
    if (msg->MethodID == OM_UPDATE)
    {
	tt[2].ti_Tag = TAG_MORE;
	tt[2].ti_Data = (ULONG) msg->opu_AttrList;
    }
    else
	tt[2].ti_Tag = TAG_DONE;

    // Send to superclass
    DoSuperMethod(cl, obj,
		  OM_NOTIFY,
		  tt,
		  GInfo,
		  ((msg->MethodID == OM_UPDATE) ? (msg->opu_Flags) : flags));
}



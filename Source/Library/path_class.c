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

/// Gadget dispatcher
// NOTE: This is a drop in replacement for the way the lister-window
//       used the OS string-gadget, and just that (for now).
//       Trying to use it as a replasement for the system gadet
//       elsewhere will probably not work.
ULONG __asm __saveds pathgadget_dispatch(register __a0 Class *cl,
					 register __a2 Object *obj,
					 register __a1 Msg msg)

{
    struct Gadget *gadget = (struct Gadget *)obj;
    PathGadgetData *data = 0;
    ULONG ret = NULL;
    Object *object;
    struct PathEditBuffer *editbuff;
#ifdef _DEBUG_PATHGADGET
    ULONG msgnr = 0;
#endif

    editbuff = &gfmlib_data.path_editbuff;

    //msgnr++;

    if (obj && msg->MethodID != OM_NEW)
    {
	data = INST_DATA(cl,obj);
	gadget=(struct Gadget *)obj;
    }

    switch (msg->MethodID)
    {
	    case GM_RENDER:
		    {
		        struct GadgetInfo *gi = ((struct gpRender*)msg)->gpr_GInfo;

			if (data->DomainWidth != gi->gi_Domain.Width ||
			    data->DomainHeight != gi->gi_Domain.Height ||
			    data->Flags&PATHF_RESIZE)
			{
			    pathgadget_calc_coords(data, gadget, gi);
			    if (!(gadget->Flags&GFLG_SELECTED))
			    {
			    }
			    if (!(gadget->Flags&GFLG_SELECTED) &&
				data->DisplayPos + data->DisplayLength == data->Length)
			    {
				data->Flags |= PATHF_FROM_END;
			    }

			}
		    }
		    pathgadget_render (cl, gadget, (struct gpRender *)msg);
		    return TRUE;

	    case GM_HITTEST:
		    return GMR_GADGETHIT;

	    case GM_GOACTIVE:

		    pathgadget_filleditbuffer(cl, data, NULL);
		    data->Flags &= ~PATHF_CANCEL;
		    data->Flags |= PATHF_EDITBUF_ACTIVE;
		    gadget->Flags |= GFLG_SELECTED;

	    case GM_HANDLEINPUT:
		    //KPrintF("%ld GM_HANDLEINPUT\n", msgnr);
		    {
		        struct GadgetInfo *gi = ((struct gpInput *)msg)->gpi_GInfo;
		        //KPrintF("%ld GM_HANDLEINPUT\n", msgnr);
		        if (data->DomainWidth != gi->gi_Domain.Width || data->DomainHeight != gi->gi_Domain.Height)
		            pathgadget_calc_coords(data, gadget, gi);
		    }
		    return pathgadget_handleinput (cl, gadget, (struct gpInput *)msg);

	    case GM_GOINACTIVE:
		    gadget->Flags &= ~GFLG_SELECTED;
		    if (data->Flags&PATHF_EDITBUF_ACTIVE)
		    {
		        data->Flags &= ~PATHF_EDITBUF_ACTIVE;

		        if (!(data->Flags&PATHF_CANCEL))
			    pathgadget_workbuffertostring(cl, data);

		        pathgadget_clearworkbuffer(cl, data);
		    }
		    pathgadget_render(cl, gadget, (struct gpRender *)msg);
		    //pathgadget_notify(cl, obj, (struct opUpdate *)msg, data);
		    return 0L;
#if 0
	    case GM_LAYOUT:
		    KPrintF("%ld \n", msgnr);
		    /* This method is only supported on V39 and above */
		    layout_gstringg (cl, gadget, (struct gpLayout *)msg);
		    return TRUE;
#endif
	    case OM_SET:
	    case OM_UPDATE:

		    ret = DoSuperMethodA(cl, obj, msg);
		    {
			struct TagItem *tag, *tags;
			struct GadgetInfo *gi = ((struct opSet*)msg)->ops_GInfo;

		        if (data->DomainWidth != gi->gi_Domain.Width || data->DomainHeight != gi->gi_Domain.Height)
			{
		            pathgadget_calc_coords(data, gadget, gi);
				ret = 1L;
			}

			tags = ((struct opSet *)msg)->ops_AttrList;

			if (tag = FindTagItem(STRINGA_TextVal,tags))
			{
			    pathgadget_setfromstring(data, (CONST_STRPTR)tag->ti_Data);

			    data->Flags |= PATHF_FROM_END;
			    ret = 1L;
			}

			if (ret)
			{
			    struct RastPort *rp;

			    // Get rastport
			    if (rp=ObtainGIRPort(((struct opSet *)msg)->ops_GInfo))
			    {
				// Send redraw
				DoMethod(
					obj,
					GM_RENDER,
					((struct opSet *)msg)->ops_GInfo,
					rp,
					GREDRAW_REDRAW);
				ReleaseGIRPort(rp);
			    }
			}
		    }
		    return ret;

	    // Resize
	    case GM_RESIZE:
		    data->Flags |= PATHF_RESIZE;
		    break;

/// if 0
#if 0
	    case OM_GET:
		    //KPrintF("%ld OM_GET\n", msgnr);
		    og= (struct opGet *)msg;
		    ret= TRUE;
		    switch( og->opg_AttrID )
		    {
			    case GSTRINGA_TextVal:
				    *(og->opg_Storage)= (ULONG)pathgadget_makestring(data);
				    break;
			    case PGA_Top:
				    *(og->opg_Storage)= (ULONG)data->Top;
				    break;
			    case PGA_Total:
				    *(og->opg_Storage)= (ULONG)data->Total;
				    break;
			    default:
				    ret = DoSuperMethodA(cl, obj, msg);
		    }
		    return ret;
#endif
///

	    case OM_NEW:
		    {
			struct TagItem *tags;
			STRPTR *stringp;

			tags=((struct opSet *)msg)->ops_AttrList;

			// Fail if stringpointer is not supplied
			if (!(stringp = (STRPTR *)GetTagData(PATHGA_StringPointer, NULL, tags)))
			    return 0;

			if (object = (Object *)DoSuperMethodA(cl, obj, msg))
			{
				STRPTR string;

				// Initial local instance data
				data = INST_DATA(cl, object);

				// Save pointer to stringpointer
				data->String = stringp;

				// Init editbuffer
				NewList((struct List *)&editbuff->peb_List);
				AddHead((struct List *)&editbuff->peb_List, (struct Node *)&editbuff->peb_FirstPart);

				data->Font = (struct TextFont *)GetTagData(STRINGA_Font, NULL, tags);

				data->History = (Att_List *)GetTagData(GTCustom_History, NULL, tags);

				if (string = (STRPTR)GetTagData(PATHGA_TextVal, NULL, tags))
				{
				    //FIXME
				    ;
#ifdef _DEBUG
				    KPrintF("GSTRINGA_TextVal Not implemented\n");
#endif
				}
				else
				if (string = (STRPTR)GetTagData(STRINGA_TextVal, NULL, tags))
				    pathgadget_setfromstring(data, string);
				else
				{
				    *data->String = AllocVec(1,MEMF_PUBLIC|MEMF_CLEAR);
				}

				data->DisplayPos = 0;
				data->Flags = 0;

				ret = (ULONG)object;
			}
		    }
		    break;

	    case OM_DISPOSE:
		    FreeVec(*data->String);
		    *data->String = 0;
		    return DoSuperMethodA (cl, obj, msg);

	    default:
		    //KPrintF("%ld default: msg->MethodID %08.lx\n", msgnr, msg->MethodID);
		    /* Unsupported method: let our superclass's
		     * dispatcher take a look at it.
		     */
		    return DoSuperMethodA (cl, obj, msg);
    }
    return(ret);
}
///

void pathgadget_clearworkbuffer(Class *cl, PathGadgetData *data)
{
    struct PathEditBufferPart *node;
    struct PathEditBuffer *editbuff;

    editbuff = &gfmlib_data.path_editbuff;

    node = (struct PathEditBufferPart *)editbuff->peb_List.mlh_Head;

    // Reset metadata
    editbuff->peb_CursorPart = editbuff->peb_DisplayPart = node;
    editbuff->peb_DPOffset = 0;
    editbuff->peb_Length = 0;
    editbuff->peb_CursorPos = 0;
    editbuff->peb_CPOffset = 0;

    // Clear first part
    node->pebp_Length = PEBPF_LAST_PART;
    node = (struct PathEditBufferPart *)node->pebp_Node.mln_Succ;


    // Free any following parts
    while (node->pebp_Node.mln_Succ)
    {
	struct PathEditBufferPart *delnode;

	delnode = node;
	node = (struct PathEditBufferPart *)node->pebp_Node.mln_Succ;
	Remove((struct Node *)delnode);
	FreeMem(delnode,sizeof(struct PathEditBufferPart));
    }

}


STRPTR pathgadget_setfromstring(PathGadgetData *data, CONST_STRPTR input_string)
{
    STRPTR string = 0, old;
    ULONG len;
    
    old = *data->String;

    if (input_string && (len = strlen(input_string)))
    {
	if (string = AllocVec(len + 1,MEMF_PUBLIC))
	{
	    strcpy(string,input_string);
	    *data->String = string;
	    data->Length = len;
	    data->DisplayPos = 0;
	    if (old)
		FreeVec(old);
	}
    }
    else
    {
	data->Length = 0;
	data->DisplayPos = 0;
	data->DisplayLength = 0;
	*data->String = 0;
	if (old)
	    FreeVec(old);
    }

    return string;
}

struct PathEditBufferPart *pathgadget_addbufpart(Class *cl, PathGadgetData *data, struct PathEditBufferPart *after)
{
    struct PathEditBufferPart *part;
    struct PathEditBuffer *editbuff;

    editbuff = &gfmlib_data.path_editbuff;

    if (part = AllocMem(sizeof(struct PathEditBufferPart),MEMF_ANY))
    {
	part->pebp_Length = 0;
	if (after)
	    Insert((struct List *)&editbuff->peb_List, (struct Node *)part, (struct Node *)after);
	else
	    AddTail((struct List *)&editbuff->peb_List, (struct Node *)part);
    }
    return part;
}

void pathgadget_filleditbuffer(Class *cl, PathGadgetData *data, STRPTR fromstring)
{
    struct PathEditBufferPart *part;
    ULONG len, offset, length = 0;
    BOOL got_dp = FALSE;
    STRPTR string;
    struct PathEditBuffer *editbuff;

    editbuff = &gfmlib_data.path_editbuff;

    // First part
    part = (struct PathEditBufferPart *)editbuff->peb_List.mlh_Head;

    if (!fromstring)
    {
	len = editbuff->peb_Length = data->Length;

	string = *data->String;
    }
    else
    {
	len = editbuff->peb_Length = strlen(fromstring);
	string = fromstring;
    }


    // Only need one buffer?
    if (len <= PWB_PART_LEN)
    {
	if (len)
	    memcpy(part->pebp_Buf, string, len);
	else
	    part->pebp_Buf[0] = 0;

	// Set length and mark as last
	part->pebp_Length = len | PEBPF_LAST_PART;
	editbuff->peb_DisplayPart = part;
	editbuff->peb_Length = len;
    }
    else
    {
	ULONG part_len;

	memcpy(part->pebp_Buf, string, PWB_PART_LEN);
	part->pebp_Length = PWB_PART_LEN;
	len -= PWB_PART_LEN;
	length = part_len = PWB_PART_LEN;
	if (data->DisplayPos < PWB_PART_LEN)
	{
	    editbuff->peb_DisplayPart = part;
	    editbuff->peb_DPOffset = 0;
	    got_dp = TRUE;
	}
	offset = PWB_PART_LEN;
	do
	{
	    struct PathEditBufferPart *tmp_part;
	    if (tmp_part = pathgadget_addbufpart(cl, data,NULL))
	    {
		if (len > PWB_PART_LEN)
		    part_len = PWB_PART_LEN;
		else
		    part_len = len;
		memcpy(tmp_part->pebp_Buf, string + offset, part_len);
		tmp_part->pebp_Length = part_len;
		len -= part_len;
		if (!got_dp && data->DisplayPos < part_len + offset)
		{
		    editbuff->peb_DisplayPart = tmp_part;
		    editbuff->peb_DPOffset = offset;

		    got_dp = TRUE;
		}
		offset += PWB_PART_LEN;
		length += part_len;
		part = tmp_part;
	    }
	} while (len);
	if (fromstring)
	    editbuff->peb_CPOffset = length - part_len;
	editbuff->peb_Length = length;
	part->pebp_Length |= PEBPF_LAST_PART;
    }
    if (fromstring)
    {
	editbuff->peb_CursorPart = part;
	editbuff->peb_CursorPos = length;
	data->Flags |= PATHF_FAKE_SPACE;
    }
}

void pathgadget_workbuffertostring(Class *cl, PathGadgetData *data)
{
    STRPTR string = 0, old;
    struct PathEditBuffer *editbuff;

    editbuff = &gfmlib_data.path_editbuff;
    
    old = *data->String;

    if (!editbuff->peb_Length)
    {
	data->Length = 0;
	data->DisplayPos = 0;
	data->DisplayLength = 0;

	*data->String = 0;
    }
    else
    if (string = AllocVec(editbuff->peb_Length + 1,MEMF_PUBLIC))
    {
	struct PathEditBufferPart *node;
	UBYTE *pos;

	pos = string;
	node = (struct PathEditBufferPart *)editbuff->peb_List.mlh_Head;
	while (node->pebp_Node.mln_Succ)
	{
	    memcpy(pos,node->pebp_Buf,node->pebp_Length&~PEBPF_LAST_PART);
	    pos += node->pebp_Length&~PEBPF_LAST_PART;
	    node = (struct PathEditBufferPart *)node->pebp_Node.mln_Succ;
	}
	*pos = 0;

	data->Length = editbuff->peb_Length;
	*data->String = string;
    }

    if (old && (string || !(*data->String)))
	FreeVec(old);
}

void pathgadget_calc_coords(PathGadgetData *data, struct Gadget *gadget, struct GadgetInfo *gi)
{
    data->Gadget.MinX   = gadget->LeftEdge;
    data->Gadget.MinY   = gadget->TopEdge;
    data->Width  = gadget->Width;
    data->Height = gadget->Height;

    if (gadget->Flags & GFLG_RELRIGHT)
	data->Gadget.MinX += gi->gi_Domain.Width - 1;

    if (gadget->Flags & GFLG_RELBOTTOM)
	data->Gadget.MinY += gi->gi_Domain.Height - 1;

    if (gadget->Flags & GFLG_RELWIDTH)
	data->Width += gi->gi_Domain.Width;

    if (gadget->Flags & GFLG_RELHEIGHT)
	data->Height += gi->gi_Domain.Height;

    data->Gadget.MaxX = data->Gadget.MinX + data->Width - 1;
    data->Gadget.MaxY = data->Gadget.MinY + data->Height - 1;

    // Store domain size
    data->DomainWidth = gi->gi_Domain.Width;
    data->DomainHeight = gi->gi_Domain.Height;

    if (data->Flags&PATHF_RESIZE)
	data->Flags &= ~PATHF_RESIZE;
}

/// Render
ULONG pathgadget_render(Class *cl, struct Gadget *gadget, struct gpRender *msg)
{
    PathGadgetData *data = INST_DATA(cl, (Object *)gadget);
    struct RastPort *rp;
    ULONG retval = TRUE;
    UWORD *pens;

    if (msg->MethodID == GM_RENDER)
    {
	rp = msg->gpr_RPort;
	pens = msg->gpr_GInfo->gi_DrInfo->dri_Pens;
    }
    else
    if (msg->MethodID <= GM_HANDLESCROLL)
    {
	rp = ObtainGIRPort(msg->gpr_GInfo);
	pens = msg->gpr_GInfo->gi_DrInfo->dri_Pens;
    }
    else
    {
	rp = ObtainGIRPort(((struct opSet*)msg)->ops_GInfo);
	pens = ((struct opSet*)msg)->ops_GInfo->gi_DrInfo->dri_Pens;
    }

    if (rp)
    {
	UWORD fill, text, highlight, cursor;
	WORD ypos;

	// Set font if supplied
	if (data->Font)
	    SetFont(rp, data->Font);

	fill   = pens[BACKGROUNDPEN];
	text  = pens[TEXTPEN];
	highlight = pens[HIGHLIGHTTEXTPEN];
	cursor = pens[FILLPEN];

	SetAPen(rp, fill);
	SetDrMd(rp, JAM1);
	SetAfPt(rp, NULL, 0);


	RectFill(rp,
		 data->Gadget.MinX,
		 data->Gadget.MinY,
		 data->Gadget.MaxX,
		 data->Gadget.MaxY);

	ypos = data->Gadget.MinY + ((gadget->Height - rp->TxHeight) >> 1);

	if (gadget->Flags&GFLG_SELECTED)
	{
	    struct PathEditBufferPart *part;
	    WORD len;
	    UBYTE part_len, pos, text_len;
	    BOOL done = FALSE;
	    struct PathEditBuffer *editbuff;

	    editbuff = &gfmlib_data.path_editbuff;

	    Move(rp, data->Gadget.MinX, ypos + rp->TxBaseline);
	    SetAPen(rp, text);

	    part = editbuff->peb_DisplayPart;
	    pos = data->DisplayPos - editbuff->peb_DPOffset;
	    part_len = (part->pebp_Length&~PEBPF_LAST_PART) - pos;

	    // Get pre-cursor string length
	    if (len = editbuff->peb_CursorPos - data->DisplayPos)
	    {
		// All in this part?
		if (part_len >= len)
	        {
		    text_len = len;
		    done = TRUE;
	        }
	        else
		    text_len = part_len;

	        Text(rp, part->pebp_Buf + pos, text_len);

		while (!done)
	        {
		    len -= text_len;
		    part = (struct PathEditBufferPart *)part->pebp_Node.mln_Succ;

		    if ((part_len = part->pebp_Length&~PEBPF_LAST_PART) >= len)
		    {
		        text_len = len;
		        done = TRUE;
		    }
		    else
		        text_len = part_len;

		    Text(rp, part->pebp_Buf, text_len);
	        }

	    }

	    SetAPen(rp, highlight);
	    SetBPen(rp, cursor);
	    SetDrMd(rp, JAM2);

	    // Cursor at end of string?
	    if (data->Flags&PATHF_FAKE_SPACE)
	    {
		Text(rp," ",1);
	    }
	    else
	    {
		// Render cursor
		part = editbuff->peb_CursorPart;
		pos = editbuff->peb_CursorPos - editbuff->peb_CPOffset;
		part_len = part->pebp_Length&~PEBPF_LAST_PART;
		Text(rp, part->pebp_Buf + pos, 1);
		pos++;
	    }

	    SetAPen(rp, text);

	    SetDrMd(rp, JAM1);
	    done = FALSE;

	    if (!(data->Flags&PATHF_FAKE_SPACE) && (len = data->DisplayLength - (editbuff->peb_CursorPos - data->DisplayPos) - 1))
	    {
		// End of part?
		if (pos == part_len)
		{
		    // Get next part
		    part = (struct PathEditBufferPart *)part->pebp_Node.mln_Succ;
		    part_len = part->pebp_Length&~PEBPF_LAST_PART;
		    pos = 0;
		}

		if (part_len - pos >= len)
	        {
		    text_len = len;
		    done = TRUE;
	        }
	        else
		    text_len = part_len - pos;

		Text(rp, part->pebp_Buf + pos, text_len);

	        while (!done)
	        {
		    len -= text_len;
		    part = (struct PathEditBufferPart *)part->pebp_Node.mln_Succ;

		    if ((part_len = part->pebp_Length&~PEBPF_LAST_PART) >= len)
		    {
		        text_len = len;
		        done = TRUE;
		    }
		    else
		        text_len = part_len;

		    Text(rp, part->pebp_Buf, text_len);
	        }
	    }
	}
	else
	if (*data->String)
	{
	    struct TextExtent extent;
	    STRPTR string;
	    ULONG len;
	    UWORD width;

	    width = data->Width;

	    if (GfxBase->LibNode.lib_Version<39 && rp->Font->tf_Flags&FPF_PROPORTIONAL)
		width++;

	    if (data->Flags&PATHF_FROM_END)
	    {
		string = *data->String;
		len = TextFit(rp, string, data->Length, &extent, 0, -1, width, rp->TxHeight);
		data->DisplayPos = data->Length - len;
		string += data->DisplayPos;
		data->Flags &= ~PATHF_FROM_END;
	    }
	    else
	    {
	        string = *data->String + data->DisplayPos;
	        len = TextFit(rp,string, data->Length - data->DisplayPos, &extent, NULL, 1, width, rp->TxHeight);
	    }
	    Move(rp, data->Gadget.MinX, ypos + rp->TxBaseline);
	    SetAPen(rp, text);
	    Text(rp, string, len);
	    data->DisplayLength = len;
	}
	// If we allocated a rastport, free it
	if (msg->MethodID != GM_RENDER)
	    ReleaseGIRPort(rp);
    }
    else retval = FALSE;

    return(retval);
}
///

ULONG pathgadget_textfit(Class *cl, PathGadgetData *data, struct gpInput *msg, UWORD length, BOOL from_curs, UWORD update, UWORD width, UWORD *text_width)
{
    struct RastPort *rp;
    struct TextExtent extent;
    struct PathEditBufferPart *tmp_part, *part;
    UWORD disp_chars, txtwidth;
    WORD numchars;
    ULONG totalchars = 0;
    UBYTE part_len;
    UBYTE pos;
    ULONG offset;
    struct PathEditBuffer *editbuff;

    editbuff = &gfmlib_data.path_editbuff;

    if (rp = ObtainGIRPort(msg->gpi_GInfo))
    {
	if (GfxBase->LibNode.lib_Version<39 && rp->Font->tf_Flags&FPF_PROPORTIONAL)
		    width++;

	// Set font if supplied
	if (data->Font)
	    SetFont(rp, data->Font);

	// Measure backwards from cursor position?
	if (from_curs)
	{

	    pos = editbuff->peb_CursorPos - editbuff->peb_CPOffset;

	    // At end of sting?
	    if (data->Flags&PATHF_FAKE_SPACE)
	    {
		width -= TextLength(rp," ",1);
		pos--;
	    }

	    totalchars = TextFit(rp, editbuff->peb_CursorPart->pebp_Buf + pos, pos + 1, &extent, 0, -1, width, rp->TxHeight);
	    txtwidth = extent.te_Width;
	    width -= txtwidth;
	    pos = pos + 1 - totalchars;
	    tmp_part = part = editbuff->peb_CursorPart;
	    offset = editbuff->peb_CPOffset;

	    while ((tmp_part = (struct PathEditBufferPart *)tmp_part->pebp_Node.mln_Pred) &&
	           tmp_part->pebp_Node.mln_Pred &&
	           (numchars = TextFit(rp, tmp_part->pebp_Buf + tmp_part->pebp_Length - 1, tmp_part->pebp_Length, &extent, 0, -1, width, rp->TxHeight)))
	    {
		width -= extent.te_Width;
	        txtwidth += extent.te_Width;
	        totalchars += numchars;
		part = tmp_part;
	        offset -= tmp_part->pebp_Length;
		pos = tmp_part->pebp_Length - numchars;
	    }

	    if (!(data->Flags&PATHF_FAKE_SPACE) && part != tmp_part && pos == 0)
	    {
		part = (struct PathEditBufferPart *)tmp_part->pebp_Node.mln_Succ;
	    }

	    if (update)
	    {
		editbuff->peb_DisplayPart = part;
		editbuff->peb_DPOffset = offset;
		data->DisplayPos = pos + offset;
	    }
	}
	// Measure forward from dispaly position?
	else
	{
	    pos = data->DisplayPos - editbuff->peb_DPOffset;
	    offset = editbuff->peb_DPOffset;
	    part = editbuff->peb_DisplayPart;

	    // Get length from dispaly position to end of curren buffer part
	    part_len = (part->pebp_Length&~PEBPF_LAST_PART) - pos;
	    numchars = TextFit(rp, part->pebp_Buf + pos, part_len, &extent, 0, 1, width, rp->TxHeight);
	    txtwidth = extent.te_Width;
	    disp_chars = length;
	    totalchars = numchars;

	    // Need more buffers?
	    if (!(part->pebp_Length&PEBPF_LAST_PART) && numchars == part_len && disp_chars > numchars)
	    {
		tmp_part = part;
		offset += part->pebp_Length;

		// Loop until string-end or dispalyh-area filled
		while (!(tmp_part->pebp_Length&PEBPF_LAST_PART) && numchars == part_len && disp_chars)
		{
		    tmp_part = (struct PathEditBufferPart *)tmp_part->pebp_Node.mln_Succ;

		    if (disp_chars >= numchars)
			disp_chars -= numchars;
		    else
			disp_chars = 0;

		    // Get length of this buffer part
		    part_len = tmp_part->pebp_Length&~PEBPF_LAST_PART;

		    numchars = TextFit(rp, tmp_part->pebp_Buf, part_len, &extent, 0, 1, width - txtwidth, rp->TxHeight);

		    if (numchars == tmp_part->pebp_Length && disp_chars > numchars)
		    {
			offset += part_len;
		        pos = 0;
		    }
		    else
		        pos = numchars;

	            totalchars += numchars;
	            txtwidth += extent.te_Width;

		    if (!(data->Flags&PATHF_FAKE_SPACE))
			part = tmp_part;
		}
	    }
	    else
		pos += numchars;
	    if (update)
	    {
		editbuff->peb_CursorPart = part;
		editbuff->peb_CPOffset = offset;
		editbuff->peb_CursorPos = pos + offset;
	    }
	}
	if (text_width)
	    *text_width = txtwidth;

	ReleaseGIRPort(rp);
    }
    return totalchars;
}

ULONG pathgadget_handleinput(Class *cl, struct Gadget *gadget, struct gpInput *msg)
{
    PathGadgetData *data= INST_DATA(cl, (Object *)gadget);
    ULONG	    retval = GMR_MEACTIVE;
    struct InputEvent	    *ie = msg->gpi_IEvent;
    BOOL render = FALSE;
    struct PathEditBuffer *editbuff;
    UWORD ieclass;

    editbuff = &gfmlib_data.path_editbuff;

    if (ie)
	ieclass = ie->ie_Class;
    else
	ieclass = PATH_NO_INPUTEVENT;

    if (ieclass == PATH_NO_INPUTEVENT || (ieclass == IECLASS_RAWMOUSE && ie->ie_Code == IECODE_LBUTTON))
    {
	if (msg->MethodID == GM_HANDLEINPUT &&
	    (msg->gpi_Mouse.X < 0 || msg->gpi_Mouse.Y < 0 ||
	     msg->gpi_Mouse.X > data->Width || msg->gpi_Mouse.Y > data->Height))
	{
	    render = FALSE;
	    retval = GMR_REUSE;
	}
	else
	{
	    WORD txtlen;

	    if (ieclass == PATH_NO_INPUTEVENT)
	    {
	        data->Flags |= PATHF_FAKE_SPACE;

	        editbuff->peb_CursorPart = (struct PathEditBufferPart *)editbuff->peb_List.mlh_TailPred;
	        editbuff->peb_CPOffset = editbuff->peb_Length - (editbuff->peb_CursorPart->pebp_Length&~PEBPF_LAST_PART);
	        editbuff->peb_CursorPos = editbuff->peb_Length;
	        data->DisplayLength = pathgadget_textfit(cl, data, msg, editbuff->peb_Length, TRUE, TRUE, data->Width, NULL);

		render = TRUE;

	    }
	    else
	    {
	        data->Flags &= ~PATHF_FAKE_SPACE;

	        txtlen = pathgadget_textfit(cl, data, msg, editbuff->peb_Length - data->DisplayPos, FALSE, TRUE, msg->gpi_Mouse.X, NULL);

		// Click after end of displayed string?
	        if (txtlen == data->DisplayLength)
		{
	            // End of string?
	            if (data->DisplayPos + txtlen == editbuff->peb_Length)
	            {
		        data->Flags |= PATHF_FAKE_SPACE;

		        if (editbuff->peb_Length)
		        {
		            data->DisplayLength = pathgadget_textfit(cl, data, msg, editbuff->peb_Length, TRUE, TRUE, data->Width, NULL);
		        }
		        else
		        {
		            data->DisplayLength = 0;
		        }
	            }
	            else
	            {
		        data->Flags &= ~PATHF_FAKE_SPACE;

		        data->DisplayLength = pathgadget_textfit(cl, data, msg, editbuff->peb_CursorPos + 1, TRUE, TRUE, data->Width, NULL);
	            }
		}
		else
		{
		    UBYTE pos;

	            data->Flags &= ~PATHF_FAKE_SPACE;
	            editbuff->peb_CursorPos = data->DisplayPos + txtlen;

		    pos = (editbuff->peb_DisplayPart->pebp_Length&~PEBPF_LAST_PART) - (data->DisplayPos - editbuff->peb_DPOffset);

	            // In this buffer part?
		    if (txtlen < pos)
	            {
	                editbuff->peb_CursorPart = editbuff->peb_DisplayPart;
	                editbuff->peb_CPOffset = editbuff->peb_DPOffset;
	            }
	            else
	            {
		        pos = txtlen - pos;
	                editbuff->peb_CursorPart = (struct PathEditBufferPart *)editbuff->peb_DisplayPart->pebp_Node.mln_Succ;
		        editbuff->peb_CPOffset = editbuff->peb_DPOffset + editbuff->peb_DisplayPart->pebp_Length&~PEBPF_LAST_PART;
		        while (!(editbuff->peb_CursorPart->pebp_Length&PEBPF_LAST_PART) && pos >= editbuff->peb_CursorPart->pebp_Length&~PEBPF_LAST_PART)
	                {
			    pos -= editbuff->peb_CursorPart->pebp_Length&~PEBPF_LAST_PART;
		            editbuff->peb_CPOffset += editbuff->peb_CursorPart->pebp_Length&~PEBPF_LAST_PART;
		            editbuff->peb_CursorPart = (struct PathEditBufferPart *)editbuff->peb_CursorPart->pebp_Node.mln_Succ;
	                }
	            }
		}
		render = TRUE;
	    }
	}

    }
    /* menu button: abort */
    else
    if (ieclass == IECLASS_RAWMOUSE && ie->ie_Code == IECODE_RBUTTON)
    {
	data->Flags |= PATHF_CANCEL;
	render = FALSE;
	retval = GMR_REUSE;
    }
    else
    if (ieclass == IECLASS_RAWKEY && !(ie->ie_Code&IECODE_UP_PREFIX))
    {
	TEXT keybuf[33] = {0};
	BOOL done = FALSE;
	UBYTE len;

	len = MapRawKey(ie, keybuf, 33, NULL);

	switch (ie->ie_Code)
	{
	    // Enter or Return
	    case 0x43:
	    case 0x44:
		    keybuf[0] = 0x0D;
		    len = 1;
		    break;

	    // Home
	    case 0x70:
		    keybuf[0] = 0x01;
		    len = 1;
		    break;

	    // End
	    case 0x71:
		    keybuf[0] = 0x1A;
		    len = 1;
	            break;

	    // Back space
	    case 0x41:
		    if (editbuff->peb_CursorPos > 0)
		    {
		        // Alt = path entry left
		        if (ie->ie_Qualifier&(IEQUALIFIER_LALT|IEQUALIFIER_RALT))
		        {
			    keybuf[0] = 0x17;
			    len = 1;
		        }
		        // Shift = full left
		        else
		        if (ie->ie_Qualifier&(IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT))
		        {
			    keybuf[0] = 0x15;
			    len = 1;
		        }
		        // Normal one character left
		        else
		        {
			    keybuf[0] = 0x08;
			    len = 1;
		        }
		    }
		    break;

	    // Del
	    case 0x46:
		    if (editbuff->peb_CursorPos < editbuff->peb_Length)
	            {
		        // Alt = path entry right
		        if (ie->ie_Qualifier&(IEQUALIFIER_LALT|IEQUALIFIER_RALT))
		        {
			    pathgadget_remove_fwd(cl, data, msg, editbuff->peb_Length - editbuff->peb_CursorPos, TRUE);
			    done = TRUE;
			    render = TRUE;
		        }
		        // Shift = full right
		        else
		        if (ie->ie_Qualifier&(IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT))
		        {
			    keybuf[0] = 0x0B;
			    len = 1;
		        }
		        // Normal one character left
		        else
		        {
			    pathgadget_remove_fwd(cl, data, msg, 1, FALSE);
			    done = TRUE;
			    render = TRUE;
		        }
		    }
		    break;
	}

	if (!done && len == 3)
	{
	    ULONG *key;

	    key = (ULONG *)keybuf;

	    switch (*key)
	    {
	        // Left arrow + Shift = full left
	        case 0x9B204100:
		        keybuf[0] = 0x01;
		        len = 1;
		        break;

		// Right arrow + Shift = full right
	        case 0x9B204000:
		        keybuf[0] = 0x1A;
		        len = 1;
		        break;
	    }
	}

	if (!done && len == 1)
	{
	    if (ie->ie_Qualifier&IEQUALIFIER_RCOMMAND)
	    {
	        switch (keybuf[0])
	        {
		    // Paste
		    case 'v':
			    pathgadget_insert(cl, data, msg, NULL, NULL, TRUE);
		            done = TRUE;
			    render = TRUE;
			    break;

		    // Paste-replace
		    case 'V':
			    pathgadget_clippastereplace(cl, data, msg);
		            done = TRUE;
			    render = TRUE;
			    break;

		    // Copy
		    case 'c':
		    case 'C':
			    if (!(pathgadget_clipcopy(cl, data)))
			        DisplayBeep(0);
		            done = TRUE;
			    break;

		    // Undo
		    case 'q':
		    case 'Q':
			    keybuf[0] = 0x11;
			    break;

		    // Clear
		    case 'x':
		    case 'X':
			    keybuf[0] = 0x18;
			    break;
	        }
	    }

	    //UWORD done = 0;
	    switch (keybuf[0])
	    {
	        // Control A (Home)
	        case 0x01:
		        if (editbuff->peb_CursorPos > 0)
		        {
		            data->Flags &= ~PATHF_FAKE_SPACE;
		            editbuff->peb_CursorPart = (struct PathEditBufferPart *)editbuff->peb_List.mlh_Head;
		            editbuff->peb_CPOffset = 0;
		            editbuff->peb_CursorPos = 0;
		            editbuff->peb_DisplayPart = editbuff->peb_CursorPart;
		            data->DisplayPos = 0;
		            editbuff->peb_DPOffset = 0;
		            data->DisplayLength = pathgadget_textfit(cl, data, msg, editbuff->peb_Length, FALSE, FALSE, data->Width, NULL);

	                    render = TRUE;
		        }
		        done = TRUE;
		        break;

	        // Control H (Basackspace)
	        case 0x08:
		        if (editbuff->peb_CursorPos > 0)
		        {
			    pathgadget_remove_rew(cl, data, msg, 1, FALSE);
			    render = TRUE;
		        }
		        done = TRUE;
		        break;

	        // Control K (Shift Del)
	        case 0x0B:
		        if (editbuff->peb_CursorPos < editbuff->peb_Length)
	                {
			    if (editbuff->peb_CursorPos == 0)
			    {
			        pathgadget_clearworkbuffer(cl, data);
			        data->Flags |= PATHF_FAKE_SPACE;
			    }
			    else
			        pathgadget_remove_fwd(cl, data, msg, editbuff->peb_Length - editbuff->peb_CursorPos, FALSE);

			    render = TRUE;
		        }
		        done = TRUE;
		        break;

	        // Control M (Enter)
	        case 0x0D:
		        if (editbuff->peb_Length && data->Flags&PATHF_FAKE_SPACE)
		        {
		            data->Flags &= ~PATHF_FAKE_SPACE;
		            editbuff->peb_CursorPos--;
		            data->DisplayLength = pathgadget_textfit(cl, data, msg, editbuff->peb_Length, TRUE, TRUE, data->Width, NULL);
		        }
		        *msg->gpi_Termination = 0;
		        retval = GMR_NOREUSE | GMR_VERIFY;
		        done = TRUE;
		        break;

	        // Control Q (undo)
	        case 0x11:
	                pathgadget_clearworkbuffer(cl, data);

		        pathgadget_filleditbuffer(cl, data, NULL);
		        data->Flags |= PATHF_FAKE_SPACE;
	                editbuff->peb_CursorPart = (struct PathEditBufferPart *)editbuff->peb_List.mlh_TailPred;
	                editbuff->peb_CPOffset = editbuff->peb_Length - (editbuff->peb_CursorPart->pebp_Length&~PEBPF_LAST_PART);
		        editbuff->peb_CursorPos = editbuff->peb_Length;

		        data->DisplayLength = pathgadget_textfit(cl, data, msg, editbuff->peb_Length, TRUE, TRUE, data->Width, NULL);

		        render = TRUE;
		        done = TRUE;
		        break;

	        // Control U (Shift BackSpace)
	        case 0x15:
		        if (editbuff->peb_CursorPos > 0)
		        {
			    if (editbuff->peb_CursorPos == editbuff->peb_Length)
			    {
			        pathgadget_clearworkbuffer(cl, data);
		                data->DisplayPos = 0;
		                data->DisplayLength = 0;
			        data->Flags |= PATHF_FAKE_SPACE;
			    }
			    else
			        pathgadget_remove_rew(cl, data, msg, editbuff->peb_CursorPos, FALSE);

			    render = TRUE;
		        }
		        done = TRUE;
		        break;

	        // Control W (Alt Backspace)
	        case 0x17:
		        if (editbuff->peb_CursorPos > 0)
		        {
			    pathgadget_remove_rew(cl, data, msg, editbuff->peb_CursorPos, TRUE);
			    render = TRUE;
		        }
		        done = TRUE;
		        break;

	        // Control X (clear)
	        case 0x18:
		        pathgadget_clearworkbuffer(cl, data);
	                data->DisplayPos = 0;
	                data->DisplayLength = 0;
		        data->Flags |= PATHF_FAKE_SPACE;
		        render = TRUE;
		        done = TRUE;
	                break;

	        // Control Z (End)
	        case 0x1A:
		        if (editbuff->peb_CursorPos < editbuff->peb_Length)
	                {
		            data->Flags |= PATHF_FAKE_SPACE;

		            editbuff->peb_CursorPart = (struct PathEditBufferPart *)editbuff->peb_List.mlh_TailPred;
		            editbuff->peb_CPOffset = editbuff->peb_Length - (editbuff->peb_CursorPart->pebp_Length&~PEBPF_LAST_PART);
		            editbuff->peb_CursorPos = editbuff->peb_Length;
		            data->DisplayLength = pathgadget_textfit(cl, data, msg, editbuff->peb_Length, TRUE, TRUE, data->Width, NULL);

	                    render = TRUE;
	                }
		        done = TRUE;
		        break;

	        // Escape deactivates gadget
	        case 0x1B:
			data->Flags |= PATHF_CANCEL;
			render = FALSE;
			retval = GMR_NOREUSE;
		        done = TRUE;
	                break;
	    }
	}

	if (!done && len == 2)
	{
	    UWORD *key;

	    key = (UWORD *)keybuf;

	    switch (*key)
	    {
	        // Left arrow
	        case 0x9B44:
		        if (editbuff->peb_CursorPos > 0)
		        {
		            data->Flags &= ~PATHF_FAKE_SPACE;

		            // Control = page left
		            if (ie->ie_Qualifier&IEQUALIFIER_CONTROL)
		            {
			        if (editbuff->peb_CursorPos > data->DisplayPos)
			        {
				    editbuff->peb_CursorPos = data->DisplayPos;
				    editbuff->peb_CursorPart = editbuff->peb_DisplayPart;
				    editbuff->peb_CPOffset = editbuff->peb_DPOffset;
			        }
			        else
			        {

				    if (!(editbuff->peb_CursorPos - editbuff->peb_DPOffset))
				    {
				        editbuff->peb_CursorPart = (struct PathEditBufferPart *)editbuff->peb_CursorPart->pebp_Node.mln_Pred;
				        editbuff->peb_CPOffset -= editbuff->peb_CursorPart->pebp_Length;
				    }
				    editbuff->peb_CursorPos--;
				    data->DisplayLength = pathgadget_textfit(cl, data, msg, editbuff->peb_Length - editbuff->peb_CursorPos, TRUE, TRUE, data->Width, NULL);
			            editbuff->peb_CursorPos = data->DisplayPos;
			            editbuff->peb_CursorPart = editbuff->peb_DisplayPart;
			            editbuff->peb_CPOffset = editbuff->peb_DPOffset;
				    if (!editbuff->peb_CursorPos)
				        data->DisplayLength = pathgadget_textfit(cl, data, msg, editbuff->peb_Length, FALSE, FALSE, data->Width, NULL);
			        }
		            }
		            // Alt = path entry left
		            else
		            if (ie->ie_Qualifier&(IEQUALIFIER_LALT|IEQUALIFIER_RALT))
			    {
			        struct PathEditBufferPart *part;
			        ULONG offset;
			        BOOL found = FALSE;
			        UBYTE pos = 0, tail = 2;

			        if (!(offset = editbuff->peb_CPOffset) &&
				    ((pos = editbuff->peb_CursorPos - editbuff->peb_CPOffset) < tail))
			        {
				    editbuff->peb_CursorPos = 0;
			        }
			        else
			        {
			            part = editbuff->peb_CursorPart;

				    if (tail >= pos)
				    {
				        tail -= pos;
				        part = (struct PathEditBufferPart *)part->pebp_Node.mln_Pred;
				        offset -= part->pebp_Length;
				        pos = part->pebp_Length - tail;
				    }
				    else
				    {
				        pos -= tail;
				    }

				    do
				    {
				        STRPTR find_char;

				        find_char = part->pebp_Buf + pos;

			                if (find_char[0] == '/' || find_char[0] == ':')
			                {
					    pos++;
				            found = TRUE;
			                }
				        if (!(offset + pos))
					    break;

				        if (!pos)
				        {
					    part = (struct PathEditBufferPart *)part->pebp_Node.mln_Pred;
					    pos = part->pebp_Length;
					    offset -= pos;
				        }
				    } while (!found && offset + pos--);

				    if (found && pos == part->pebp_Length)
				    {
				        offset += part->pebp_Length;
				        part = (struct PathEditBufferPart *)part->pebp_Node.mln_Succ;
				        pos = 0;
				    }

				    editbuff->peb_CPOffset = offset;
				    editbuff->peb_CursorPos = pos + offset;
				    editbuff->peb_CursorPart = part;

				    if (editbuff->peb_CursorPos < data->DisplayPos)
				    {
				        editbuff->peb_DisplayPart = editbuff->peb_CursorPart;
				        editbuff->peb_DPOffset = editbuff->peb_CPOffset;
				        data->DisplayPos = editbuff->peb_CursorPos;
				        data->DisplayLength = pathgadget_textfit(cl, data, msg, editbuff->peb_Length - data->DisplayPos, FALSE, FALSE, data->Width, NULL);
				    }
			        }
		            }
		            // Normal one character left
		            else
		            {
			        if (editbuff->peb_CursorPos - editbuff->peb_CPOffset)
			            editbuff->peb_CursorPos--;
			        else
			        {
			            editbuff->peb_CursorPart = (struct PathEditBufferPart *)editbuff->peb_CursorPart->pebp_Node.mln_Pred;
			            editbuff->peb_CPOffset -= editbuff->peb_CursorPart->pebp_Length;
			            editbuff->peb_CursorPos--;
			        }

		            }

		            if (editbuff->peb_CursorPos < data->DisplayPos)
		            {
			        editbuff->peb_DisplayPart = editbuff->peb_CursorPart;
			        data->DisplayLength = pathgadget_textfit(cl, data, msg, editbuff->peb_Length, FALSE, FALSE, data->Width, NULL);
			        editbuff->peb_DPOffset = editbuff->peb_CPOffset;
			        data->DisplayPos = editbuff->peb_CursorPos;
		            }
	                    render = TRUE;
		        }
		        done = TRUE;
		        break;

	        // Right arrow
	        case 0x9B43:
		        if (editbuff->peb_CursorPos < editbuff->peb_Length)
	                {
		            data->Flags &= ~PATHF_FAKE_SPACE;

			    // Control = page right
		            if (ie->ie_Qualifier&IEQUALIFIER_CONTROL)
		            {
			        if (editbuff->peb_CursorPos >= data->DisplayPos + data->DisplayLength - 1)
			        {
				    data->DisplayPos = editbuff->peb_CursorPos + 1;
				    editbuff->peb_DPOffset = editbuff->peb_CPOffset;
				    editbuff->peb_DisplayPart = editbuff->peb_CursorPart;
				    if (data->DisplayPos - editbuff->peb_DPOffset == PWB_PART_LEN)
				    {
				        editbuff->peb_DPOffset += editbuff->peb_DisplayPart->pebp_Length;
				        editbuff->peb_DisplayPart = (struct PathEditBufferPart *)editbuff->peb_DisplayPart->pebp_Node.mln_Succ;
				    }
			        }
			        data->DisplayLength = pathgadget_textfit(cl, data, msg, editbuff->peb_Length - data->DisplayPos, FALSE, TRUE, data->Width, NULL);
			        if (editbuff->peb_CursorPos == editbuff->peb_Length)
			        {
				    data->Flags |= PATHF_FAKE_SPACE;
				    data->DisplayLength = pathgadget_textfit(cl, data, msg, editbuff->peb_Length - data->DisplayPos, TRUE, TRUE, data->Width, NULL);
			        }
			        else
				    editbuff->peb_CursorPos--;
		            }
		            // Alt = path entry right
		            else
		            if (ie->ie_Qualifier&(IEQUALIFIER_LALT|IEQUALIFIER_RALT))
		            {
			        struct PathEditBufferPart *part;
			        ULONG offset;
			        BOOL found = FALSE;
			        UWORD pos;
			        UBYTE find_end;

			        part = editbuff->peb_CursorPart;  //1
			        pos = editbuff->peb_CursorPos - editbuff->peb_CPOffset;
			        find_end = part->pebp_Length&~PEBPF_LAST_PART;
			        offset = editbuff->peb_CPOffset;

			        do
			        {
				    STRPTR find_char;

				    find_char = part->pebp_Buf + pos;

			            if (find_char[0] == '/' || find_char[0] == ':')
			            {
				        found = TRUE;
			            }

				    pos++;

				    if (pos == find_end && !(part->pebp_Length&PEBPF_LAST_PART))
				    {
				        offset += part->pebp_Length;
				        part = (struct PathEditBufferPart *)part->pebp_Node.mln_Succ;
				        pos = 0;
				        find_end = part->pebp_Length&~PEBPF_LAST_PART;
				    }

			        } while (!found && pos < find_end);

			        if (pos == find_end)
				    data->Flags |= PATHF_FAKE_SPACE;

			        editbuff->peb_CursorPos = pos + offset;
			        editbuff->peb_CursorPart = part;
			        editbuff->peb_CPOffset = offset;

			        if (editbuff->peb_CursorPos >= data->DisplayPos + data->DisplayLength)
				    data->DisplayLength = pathgadget_textfit(cl, data, msg, editbuff->peb_Length, TRUE, TRUE, data->Width, NULL);

		            }
		            // Normal one character right
		            else
		            {
			        editbuff->peb_CursorPos++;
			        if (editbuff->peb_CursorPos - editbuff->peb_CPOffset >= editbuff->peb_CursorPart->pebp_Length)
			        {
				    editbuff->peb_CPOffset += editbuff->peb_CursorPart->pebp_Length;
				    editbuff->peb_CursorPart = (struct PathEditBufferPart *)editbuff->peb_CursorPart->pebp_Node.mln_Succ;

			        }

			            // Need to move first dispalyed char?
			            if (editbuff->peb_CursorPos - data->DisplayPos == data->DisplayLength)
			            {
				        // End of string?
				        if (editbuff->peb_CursorPos == editbuff->peb_Length)
				        {
				            data->Flags |= PATHF_FAKE_SPACE;
				        }
				        data->DisplayLength = pathgadget_textfit(cl, data, msg, editbuff->peb_Length, TRUE, TRUE, data->Width, NULL);

				    }
		            }

	                    render = TRUE;
	                }
		        done = TRUE;
	                break;

	        // Up arrow
	        case 0x9B41:
	        // Down arrow
	        case 0x9B42:
		        // Remember to do textfit
			{
			    Att_Node *node=0;

			    // No history?
			    if (data->History)
			    {
				// Lock history list
				L_LockAttList(data->History,FALSE);

				// Move up?
				if (*key == 0x9B41)
				{
					// If no current node, get last
					if (!data->History->current)
						node=(Att_Node *)data->History->list.lh_TailPred;

					// Otherwise, get previous
					else
					if (data->History->current->node.ln_Pred &&
						data->History->current->node.ln_Pred->ln_Pred)
						node=(Att_Node *)data->History->current->node.ln_Pred;
				}

				// Move down
				else
				{
					// If no current node, get first
					if (!data->History->current)
						node=(Att_Node *)data->History->list.lh_Head;

					// Otherwise, get next
					else
					if (data->History->current->node.ln_Succ &&
						data->History->current->node.ln_Succ->ln_Succ)
						node=(Att_Node *)data->History->current->node.ln_Succ;
				}

				// Valid node?
				if (node && node->node.ln_Succ && node->node.ln_Pred)
				{
					// Copy string
					pathgadget_clearworkbuffer(cl, data);
					pathgadget_filleditbuffer(cl, data, node->node.ln_Name);

					// Put cursor at end
				        data->Flags |= PATHF_FAKE_SPACE;
			                editbuff->peb_CursorPart = (struct PathEditBufferPart *)editbuff->peb_List.mlh_TailPred;
			                editbuff->peb_CPOffset = editbuff->peb_Length - (editbuff->peb_CursorPart->pebp_Length&~PEBPF_LAST_PART);
				        editbuff->peb_CursorPos = editbuff->peb_Length;

				        data->DisplayLength = pathgadget_textfit(cl, data, msg, editbuff->peb_Length, TRUE, TRUE, data->Width, NULL);

					// Store node
					data->History->current=node;

					// Set flag to render
					render = TRUE;
				}

				// Unlock history list
				L_UnlockAttList(data->History);
			    }
			}

		        done = TRUE;
		        break;
	    }
	}

	// Printable??
	if (!done)
	{
	    UBYTE a = 0;
	    BOOL printable = TRUE;

	    do
	    {
	        if (!(keybuf[a] >= 0x20 && keybuf[a] <= 0x7F) && !(keybuf[a] >= 0xA0 && keybuf[a] <= 0xFF))
		    printable = FALSE;
	    } while (++a < len);

	    if (printable && pathgadget_insert(cl, data, msg, keybuf, len, NULL))
	        render = TRUE;
	}
    }

    if (render)
	pathgadget_render(cl, gadget, (struct gpRender *)msg);

    return ( retval );
}

LONG pathgadget_clipcopy(Class *cl, PathGadgetData *data)
{
    struct MsgPort  *ClipMP;          /* pointer to message port*/
    struct IOClipReq *ClipIO;         /* pointer to IORequest */
    BOOL success = FALSE;
    struct PathEditBuffer *editbuff;

    editbuff = &gfmlib_data.path_editbuff;

    if (ClipMP=CreatePort(0L,0L) )
    {
	if (ClipIO=(struct IOClipReq *)
		    CreateExtIO(ClipMP,sizeof(struct IOClipReq)))
	{
	    if (OpenDevice("clipboard.device",0L,(struct IORequest *)ClipIO,0))
	    {
		;
	    }
	    else
	    {
		struct PathEditBufferPart *part;
		ULONG clipheader[5];
		ULONG length, slen, part_len;
		BOOL odd;

		slen = editbuff->peb_Length;
		odd = (slen & 1);               /* pad byte flag */

		length = (odd) ? slen+1 : slen;

		/* initial set-up for Offset, Error, and ClipID */

		ClipIO->io_Offset = 0;
		ClipIO->io_Error  = 0;
		ClipIO->io_ClipID = 0;

		clipheader[0]=ID_FORM;
		length+=12L;
		clipheader[1]=length;
		clipheader[2]=ID_FTXT;
		clipheader[3]=ID_CHRS;
		clipheader[4]=slen;

		ClipIO->io_Data    = (APTR)clipheader;
		ClipIO->io_Length  = 20L;
	        ClipIO->io_Command = CMD_WRITE;
	        DoIO( (struct IORequest *) ClipIO);

		part = (struct PathEditBufferPart *)editbuff;

		do
		{
		    part = (struct PathEditBufferPart *)part->pebp_Node.mln_Succ;
		    part_len = part->pebp_Length&~PEBPF_LAST_PART;

		    /* Write string */
		    ClipIO->io_Data    = (STRPTR)part->pebp_Buf;
		    ClipIO->io_Length  = part_len;
		    ClipIO->io_Command = CMD_WRITE;
		    DoIO( (struct IORequest *) ClipIO);

		    slen -= part_len;
		} while (slen);

		/* Pad if needed */
		if (odd)
	        {
		    ClipIO->io_Data   = (STRPTR)"";
		    ClipIO->io_Length = 1L;
		    DoIO( (struct IORequest *) ClipIO);
	        }

		/* Tell the clipboard we are done writing */

		ClipIO->io_Command=CMD_UPDATE;
		DoIO( (struct IORequest *) ClipIO);

		/* Check if io_Error was set by any of the preceding IO requests */
		success = ClipIO->io_Error ? FALSE : TRUE;

		CloseDevice((struct IORequest *)ClipIO);
	    }
	    DeleteExtIO((struct IORequest *)ClipIO);
	}

	DeletePort(ClipMP);
    }

    return success;
}

BOOL pathgadget_clippastereplace(Class *cl, PathGadgetData *data, struct gpInput *msg)
{
    struct MsgPort  *ClipMP;          /* pointer to message port*/
    struct IOClipReq *ClipIO;         /* pointer to IORequest */
    BOOL success = FALSE;
    struct PathEditBuffer *editbuff;

    editbuff = &gfmlib_data.path_editbuff;

    if (ClipMP=CreatePort(0L,0L) )
    {
	if (ClipIO=(struct IOClipReq *)
		    CreateExtIO(ClipMP,sizeof(struct IOClipReq)))
	{
	    if (OpenDevice("clipboard.device",0L,(struct IORequest *)ClipIO,0))
	    {
		;
	    }
	    else
	    {
		ULONG clipheader[5];

		/* initial set-up for Offset, Error, and ClipID */

		ClipIO->io_Offset = 0;
		ClipIO->io_Error  = 0;
		ClipIO->io_ClipID = 0;

		// Read header
		ClipIO->io_Data=(APTR)clipheader;
		ClipIO->io_Length=sizeof(ULONG)*5;
		ClipIO->io_Offset=0;
		ClipIO->io_Command=CMD_READ;
		if (DoIO((struct IORequest *)ClipIO)) return 0;

		// Valid text?
		if (clipheader[0]==ID_FORM && clipheader[2]==ID_FTXT)
		{
		    struct PathEditBufferPart *part;
		    ULONG length = 0, slen;

		    pathgadget_clearworkbuffer(cl, data);

		    slen = clipheader[4];

		    part = (struct PathEditBufferPart *)editbuff->peb_List.mlh_Head;

		    // Only need one buffer?
		    if (slen <= PWB_PART_LEN)
		    {
			// Read string
			ClipIO->io_Data=part->pebp_Buf;
			ClipIO->io_Length=slen;
			if (DoIO((struct IORequest *)ClipIO)) goto Fail;

			// Set length and mark as last
			part->pebp_Length = slen | PEBPF_LAST_PART;
			editbuff->peb_DisplayPart = part;
			editbuff->peb_CursorPos = slen;
			length = slen;
		    }
		    else
		    {
			ULONG part_len;

			ClipIO->io_Data=part->pebp_Buf;
			ClipIO->io_Length=PWB_PART_LEN;

			if (DoIO((struct IORequest *)ClipIO)) goto Fail;

			part->pebp_Length = PWB_PART_LEN;
			slen -= PWB_PART_LEN;
			length = part_len = PWB_PART_LEN;

			do
			{
			    struct PathEditBufferPart *tmp_part;

			    if (tmp_part = pathgadget_addbufpart(cl, data, NULL))
			    {
				if (slen > PWB_PART_LEN)
				    part_len = PWB_PART_LEN;
				else
				    part_len = slen;

				ClipIO->io_Data=tmp_part->pebp_Buf;
				ClipIO->io_Length=part_len;

				if (DoIO((struct IORequest *)ClipIO))
				{
				    break;
				}

				part = tmp_part;
				length += part_len;
				part->pebp_Length = part_len;
				slen -= part_len;
			    }
			} while (slen);

			editbuff->peb_CPOffset = length - part_len;
		    }
Fail:
		    editbuff->peb_Length = length;
		    part->pebp_Length |= PEBPF_LAST_PART;
		    editbuff->peb_CursorPos = length;
		    editbuff->peb_CursorPart = part;
		    data->Flags |= PATHF_FAKE_SPACE;


		    if (ClipIO->io_Actual)
		    {
			TEXT clip_buf[128];

			// Flush clipboard
			ClipIO->io_Data=clip_buf;
			ClipIO->io_Length=128;

			while (ClipIO->io_Actual)
			{
			    if (DoIO((struct IORequest *)ClipIO))
				break;
			}
		    }
		    data->DisplayLength = pathgadget_textfit(cl, data, msg, editbuff->peb_Length, TRUE, TRUE, data->Width, NULL);

		    /* Check if io_Error was set by any of the preceding IO requests */
		    success = ClipIO->io_Error ? FALSE : TRUE;
		}

		CloseDevice((struct IORequest *)ClipIO);
	    }

	    DeleteExtIO((struct IORequest *)ClipIO);
	}

	DeletePort(ClipMP);
    }

    return success;
}

BOOL pathgadget_deletepart(Class *cl, PathGadgetData *data, struct PathEditBufferPart *part, ULONG offset)
{
    struct PathEditBufferPart *del_part;
    BOOL ret = TRUE;
    struct PathEditBuffer *editbuff;

    editbuff = &gfmlib_data.path_editbuff;

    // First part?
    if (!offset)
    {
	// Not last part?
	if (!(part->pebp_Length&PEBPF_LAST_PART))
	{
	    // First buffer part is staticly allocated,
	    // so we set the following part for deletion...
	    del_part = (struct PathEditBufferPart *)part->pebp_Node.mln_Succ;

	    // ... and copy the following part into the first
	    part->pebp_Length = del_part->pebp_Length;
	    memcpy(part->pebp_Buf, del_part->pebp_Buf, del_part->pebp_Length&~PEBPF_LAST_PART);

	    // Was cursor in copied part?
	    if (editbuff->peb_CursorPart == del_part)
		editbuff->peb_CursorPart = part;

	    ret = FALSE;
	}
	else
	{
	    // Only need to clear the length
	    part->pebp_Length = PEBPF_LAST_PART;
	    return FALSE;
	}
    }
    else
    {
	// Delete this part
	del_part = part;

	// Is it the last part?
	if (part->pebp_Length&PEBPF_LAST_PART)
	{
	    // Set previous part as the last
	    ((struct PathEditBufferPart *)part->pebp_Node.mln_Pred)->pebp_Length |= PEBPF_LAST_PART;

	    // Move cursor if needed
	    if (editbuff->peb_CursorPart == del_part)
	    {
		editbuff->peb_CursorPart = (struct PathEditBufferPart *)del_part->pebp_Node.mln_Pred;
		editbuff->peb_CPOffset += editbuff->peb_CursorPart->pebp_Length;

	    }
	}
	// Not last part, and cursor in this part? Move cursor to next part
	else
	if (editbuff->peb_CursorPart == del_part)
	    editbuff->peb_CursorPart = (struct PathEditBufferPart *)del_part->pebp_Node.mln_Succ;
    }

    Remove((struct Node *)del_part);
    FreeMem(del_part,sizeof(struct PathEditBufferPart));

    return ret;
}

void pathgadget_remove_fwd(Class *cl, PathGadgetData *data, struct gpInput *msg, ULONG len, BOOL find)
{
    struct PathEditBufferPart *part, *del_part;
    ULONG del_left;
    UWORD pos;
    UBYTE part_len;
    struct PathEditBuffer *editbuff;

    editbuff = &gfmlib_data.path_editbuff;

    part = editbuff->peb_CursorPart;
    pos = editbuff->peb_CursorPos - editbuff->peb_CPOffset;
    part_len = (part->pebp_Length&~PEBPF_LAST_PART) - pos;

    if (find && part_len > 1)
    {
	STRPTR find_char;
	UBYTE find_pos, find_end;

	if (len > part_len)
	    find_end = part->pebp_Length&~PEBPF_LAST_PART;
	else
	    find_end = pos + len;

	find_pos = pos + 1;
	do
	{
	    find_char = part->pebp_Buf + find_pos;

	    if (find_char[0] == '/' || find_char[0] == ':')
	    {
		len = find_pos - pos;
		break;
	    }

	    find_pos++;
	} while (find_pos < find_end);
    }

    del_left = len;

    do
    {
	del_part = part;
	part = (struct PathEditBufferPart *)part->pebp_Node.mln_Succ;

	// Deleted whole part?
	if (!pos && del_left >= part_len)
	{

	    del_left -= del_part->pebp_Length&~PEBPF_LAST_PART;

	    pathgadget_deletepart(cl, data, del_part, editbuff->peb_CPOffset);

	}
	else
	{
	    if (del_left < part_len)
	    {
		movmem(del_part->pebp_Buf + pos + del_left, del_part->pebp_Buf + pos, part_len - del_left);
		del_part->pebp_Length -= del_left;
		editbuff->peb_CursorPart = del_part;
		del_left = 0;
	    }
	    else
	    {
		del_part->pebp_Length -= part_len;
		del_left -= part_len;

		if (!(del_part->pebp_Length&PEBPF_LAST_PART))
		{
		    editbuff->peb_CPOffset += del_part->pebp_Length;
		    editbuff->peb_CursorPart = part;
		}
	    }
	}

	pos = 0;
	part_len = part->pebp_Length&~PEBPF_LAST_PART;

	if (find && del_left)
	{
	    STRPTR find_char;
	    UBYTE find_pos, find_end;

	    if (del_left > part_len)
	        find_end = part_len;
	    else
	        find_end = del_left;

	    find_pos = 0;
	    do
	    {
	        find_char = part->pebp_Buf + find_pos;
	        if (find_char[0] == '/' || find_char[0] == ':')
		{
		    len = len - del_left + find_pos;
		    del_left = find_pos;
		    break;
		}
		find_pos++;
	    } while (find_pos < find_end);
	}
    } while (del_left);

    editbuff->peb_Length -= len;

    if (editbuff->peb_CursorPos == editbuff->peb_Length)
	data->Flags |= PATHF_FAKE_SPACE;

    data->DisplayLength = pathgadget_textfit(cl, data, msg, editbuff->peb_Length, FALSE, FALSE, data->Width, NULL);
}

void pathgadget_remove_rew(Class *cl, PathGadgetData *data, struct gpInput *msg, ULONG len, BOOL find)
{
    struct PathEditBufferPart *part;
    ULONG del_tmp, del_left;
    UWORD pos;
    struct PathEditBuffer *editbuff;

    editbuff = &gfmlib_data.path_editbuff;

    pos = editbuff->peb_CursorPos - editbuff->peb_CPOffset;
    part = editbuff->peb_CursorPart;
    if (find && pos > 0)
    {
	STRPTR find_char;
	UBYTE find_pos;

	find_pos = pos;

	while (find_pos--)
	{
	    find_char = part->pebp_Buf + find_pos;

	    if (find_char[0] == '/' || find_char[0] == ':')
	    {
		len = pos - find_pos - 1;
		break;
	    }
	}
    }

    del_left = len;

    if (len > pos)
	del_tmp = pos;
    else
	del_tmp = len;

    // Skip if at pos 0 of current buffer part
    if (del_tmp)
    {
	movmem(part->pebp_Buf + pos, part->pebp_Buf + pos - del_tmp, (part->pebp_Length&~PEBPF_LAST_PART) - pos);
	part->pebp_Length -= del_tmp;
	del_left -= del_tmp;
    }

    // More to delete?
    if (del_left)
    {
	part = (struct PathEditBufferPart *)editbuff->peb_CursorPart->pebp_Node.mln_Pred;

	do
	{
	    struct PathEditBufferPart *del_part;

	    del_part = part;
	    part = (struct PathEditBufferPart *)part->pebp_Node.mln_Pred;

	    // Deleted whole part?
	    if (del_left >= del_part->pebp_Length)
	    {
	        del_left -= del_part->pebp_Length;
		editbuff->peb_CPOffset -= del_part->pebp_Length;
		pathgadget_deletepart(cl, data, del_part, editbuff->peb_CPOffset);
	    }
	    else
	    {
	        editbuff->peb_CPOffset -= del_left;
		del_part->pebp_Length -= del_left;
	        del_left -= del_left;
	    }

	    if (find && del_left)
	    {
		STRPTR find_char;
		UBYTE find_pos;

		find_pos = part->pebp_Length;

		while (find_pos--)
		{
		    find_char = part->pebp_Buf + find_pos;

		    if (find_char[0] == '/' || find_char[0] == ':')
		    {
			len = len - del_left + part->pebp_Length - find_pos - 1;
			del_left = part->pebp_Length - find_pos - 1;
			break;
		    }
		}
	    }
	} while (del_left);
    }

    editbuff->peb_CursorPos -= len;
    editbuff->peb_Length -= len;

    if (editbuff->peb_CursorPos < data->DisplayPos)
    {
	data->DisplayPos = editbuff->peb_CursorPos;
	editbuff->peb_DisplayPart = editbuff->peb_CursorPart;
	editbuff->peb_DPOffset = editbuff->peb_CPOffset;
    }

    data->DisplayLength = pathgadget_textfit(cl, data, msg, editbuff->peb_Length, FALSE, FALSE, data->Width, NULL);
}

#ifdef RESOURCE_TRACKING
void pathgadget_close_clipboard(Class *cl, struct IOClipReq *ior)
#else
void pathgadget_close_clipboard(struct IOClipReq *ior)
#endif
{
    struct MsgPort *mp;

    mp = ior->io_Message.mn_ReplyPort;

    CloseDevice((struct IORequest *)ior);
    DeleteExtIO((struct IORequest *)ior);
    DeletePort(mp);
}

BOOL pathgadget_insert(Class *cl, PathGadgetData *data, struct gpInput *msg, STRPTR string, ULONG len, BOOL fromclipboard)
{
    struct PathEditBufferPart *next_part = 0, *curs_part;
    struct IOClipReq *ClipIO = 0;         /* pointer to IORequest */
    BOOL ret = TRUE;
    ULONG pos;
    ULONG tmp_len, next_len = 0, copy_len, need_len;
    BOOL ok = TRUE;
    struct PathEditBuffer *editbuff;

    editbuff = &gfmlib_data.path_editbuff;
    pos = editbuff->peb_CursorPos - editbuff->peb_CPOffset;

    if (fromclipboard)
    {
	struct MsgPort *ClipMP;          /* pointer to message port*/

	if (ClipMP=CreatePort(0L,0L) )
	{
	    if (ClipIO=(struct IOClipReq *)CreateExtIO(ClipMP,sizeof(struct IOClipReq)))
	    {
	        if (OpenDevice("clipboard.device",0L,(struct IORequest *)ClipIO,0))
		{
		    DeleteExtIO((struct IORequest *)ClipIO);
		    DeletePort(ClipMP);
		    return FALSE;
		}
	        else
	        {
		    ULONG clipheader[5];

		    /* initial set-up for Offset, Error, and ClipID */

		    ClipIO->io_Offset = 0;
		    ClipIO->io_Error  = 0;
		    ClipIO->io_ClipID = 0;

		    // Read header
		    ClipIO->io_Data=(APTR)clipheader;
		    ClipIO->io_Length=sizeof(ULONG)*5;
		    ClipIO->io_Offset=0;
		    ClipIO->io_Command=CMD_READ;
		    if (DoIO((struct IORequest *)ClipIO)) return 0;

		    // Valid text?
		    if (clipheader[0]==ID_FORM && clipheader[2]==ID_FTXT)
		    {
			len = clipheader[4];
		    }
		    else
		    {
		        if (ClipIO->io_Actual)
		        {
			    TEXT clip_buf[128];

			    // Flush clipboard
			    ClipIO->io_Data=clip_buf;
			    ClipIO->io_Length=128;

			    while (ClipIO->io_Actual)
			    {
				if (DoIO((struct IORequest *)ClipIO))
				    break;
			    }
		        }
#ifdef RESOURCE_TRACKING
			pathgadget_close_clipboard(cl, ClipIO);
#else
			pathgadget_close_clipboard(ClipIO);
#endif
			return FALSE;
		    }
	        }
	    }
	    else
	    {
		DeletePort(ClipMP);
		return FALSE;
	    }
	}
	else
	{
	    return FALSE;
	}
    }

    curs_part = editbuff->peb_CursorPart;

    // Fits this buffer?
    if (len + (curs_part->pebp_Length&~PEBPF_LAST_PART) <= PWB_PART_LEN)
    {
	if (!(data->Flags&PATHF_FAKE_SPACE))
	    movmem(curs_part->pebp_Buf + pos, curs_part->pebp_Buf + pos + len,(curs_part->pebp_Length&~PEBPF_LAST_PART) - pos);
	memcpy(curs_part->pebp_Buf + pos, string, len);
	curs_part->pebp_Length += len;
	editbuff->peb_CursorPos += len;
	editbuff->peb_Length += len;

    }
    else
    {
	if (data->Flags&PATHF_FAKE_SPACE)
	    copy_len = 0;
	else
	    copy_len = (curs_part->pebp_Length&~PEBPF_LAST_PART) - pos;

	// Calculate space needed in addtion to current buffer
	tmp_len = need_len = len - (PWB_PART_LEN - (curs_part->pebp_Length&~PEBPF_LAST_PART));

	// Check next buffer for free space and save its address
	if (!(curs_part->pebp_Length&PEBPF_LAST_PART))
	    next_len = PWB_PART_LEN - ((next_part = (struct PathEditBufferPart *)curs_part->pebp_Node.mln_Succ)->pebp_Length&~PEBPF_LAST_PART);

	// Need to allocate aditional buffers?
	if (next_len < tmp_len)
	{
	    BOOL done = FALSE;
	    struct PathEditBufferPart *new_part;

	    new_part = curs_part;

	    do
	    {
		if (new_part = pathgadget_addbufpart(cl, data, new_part))
	        {
		    if (curs_part->pebp_Length&PEBPF_LAST_PART && tmp_len < PWB_PART_LEN)
		        done = TRUE;
		    else
		    if (tmp_len > PWB_PART_LEN + next_len)
			tmp_len -= PWB_PART_LEN;
		    else
		        done = TRUE;
	        }
	        else
	        {
		    DisplayBeep(NULL);
		    ok = FALSE;
	        }
	    } while (!done);

	    if (ok)
	    {
		// Need to move end-of-string marker?
		if (curs_part->pebp_Length&PEBPF_LAST_PART)
		{
		    curs_part->pebp_Length &= ~PEBPF_LAST_PART;
		    new_part->pebp_Length |= PEBPF_LAST_PART;
		}

		if (tmp_len > PWB_PART_LEN)
		    tmp_len -= PWB_PART_LEN;
		else
		    next_part = new_part;
	    }
	}

	if (ok)
	{
	    ULONG copy_tmp, copy_left, copy_done;
	    struct PathEditBufferPart *tmp_part;

	    // At end of both string and current buffer-part?
	    if (data->Flags&PATHF_FAKE_SPACE && editbuff->peb_CursorPos - editbuff->peb_CPOffset >= PWB_PART_LEN)
	    {
		// Move cursor to next buffer part
		editbuff->peb_CPOffset += curs_part->pebp_Length;
		pos -= curs_part->pebp_Length;
		curs_part = (struct PathEditBufferPart *)curs_part->pebp_Node.mln_Succ;
	    }
	    // Need to move text out of current buffer to make room?
	    else
	    if (copy_len)
	    {
		// Need to move contents of buffer next_part that was originally the next next_part from cursor?
		if (next_part->pebp_Length&~PEBPF_LAST_PART)
		{
		    movmem(next_part->pebp_Buf, next_part->pebp_Buf + tmp_len, next_part->pebp_Length&~PEBPF_LAST_PART);
		}

		// Will what needs to be moved fit the next buffer next_part that was originally the next next_part from cursor
		if (tmp_len > copy_len)
		{
		    copy_tmp = copy_len;
		}
		else
		    copy_tmp = tmp_len;

		// Move text
		memcpy(next_part->pebp_Buf + tmp_len - copy_tmp, curs_part->pebp_Buf + (curs_part->pebp_Length&~PEBPF_LAST_PART) - copy_tmp, copy_tmp);
		curs_part->pebp_Length -= copy_tmp;
		next_part->pebp_Length += copy_tmp;

		// More left to move out of the way?
		if (copy_len > tmp_len)
		{
		    // Is original "next next_part from cursor" still the next next_part?
		    if ((tmp_part =  (struct PathEditBufferPart *)next_part->pebp_Node.mln_Pred) == curs_part)
		    {
			// Use movmem as it supports overlapping source/dest
			movmem(curs_part->pebp_Buf + pos, curs_part->pebp_Buf + pos + len, copy_len - tmp_len);
		    }
		    else
		    {
			memcpy(tmp_part->pebp_Buf + PWB_PART_LEN - (copy_len - tmp_len), curs_part->pebp_Buf + pos, copy_len - tmp_len);
			curs_part->pebp_Length -= copy_len - tmp_len;
			tmp_part->pebp_Length += copy_len - tmp_len;
		    }
		}
	    }

	    // Is free space in current part greater then required space
	    if ((copy_tmp = PWB_PART_LEN - (curs_part->pebp_Length&~PEBPF_LAST_PART)) > len)
		copy_tmp = len;

	    copy_left = len;

	    if (ClipIO)
	    {
	        // Read string
	        ClipIO->io_Data=curs_part->pebp_Buf + pos;
	        ClipIO->io_Length=copy_tmp;
	        if (DoIO((struct IORequest *)ClipIO)) goto ClipFail;
	    }
	    else
	        memcpy(curs_part->pebp_Buf + pos, string, copy_tmp);
	    curs_part->pebp_Length += copy_tmp;
	    editbuff->peb_CursorPos +=copy_tmp;
	    editbuff->peb_Length += copy_tmp;
	    copy_left -= copy_tmp;
	    copy_done = copy_tmp;
	    tmp_part = (struct PathEditBufferPart *)curs_part->pebp_Node.mln_Succ;

	    if (copy_left)
	    {
	        editbuff->peb_CPOffset += curs_part->pebp_Length;

	        do
	        {
		    copy_tmp = PWB_PART_LEN - (tmp_part->pebp_Length&~PEBPF_LAST_PART);
		    if (copy_left < copy_tmp)
		        copy_tmp = copy_left;

		    if (ClipIO)
		    {
		        // Read string
		        ClipIO->io_Data=tmp_part->pebp_Buf;
		        ClipIO->io_Length=copy_tmp;
		        if (DoIO((struct IORequest *)ClipIO)) goto ClipFail;
		    }
		    else
		        memcpy(tmp_part->pebp_Buf, string + copy_done, copy_tmp);
		    tmp_part->pebp_Length += copy_tmp;
	            editbuff->peb_CursorPos +=copy_tmp;
	            editbuff->peb_Length += copy_tmp;
		    copy_done += copy_tmp;
		    if (copy_left -= copy_tmp)
		    {
		        editbuff->peb_CPOffset += tmp_part->pebp_Length&~PEBPF_LAST_PART;
		        tmp_part = (struct PathEditBufferPart *)tmp_part->pebp_Node.mln_Succ;
		    }
		    curs_part = tmp_part;
	        } while (copy_left);
	    }
	}
    }

    // At end of string?
    if (data->Flags&PATHF_FAKE_SPACE)
    {
	data->DisplayLength = pathgadget_textfit(cl, data, msg, editbuff->peb_Length, TRUE, TRUE, data->Width, NULL);
    }
    else
    {
	// Need to move the cursor?
	if (editbuff->peb_CursorPos - editbuff->peb_CPOffset >= PWB_PART_LEN)
	{
	    editbuff->peb_CPOffset += curs_part->pebp_Length;
	    curs_part = (struct PathEditBufferPart *)curs_part->pebp_Node.mln_Succ;
	}
	editbuff->peb_CursorPart = curs_part;
	data->DisplayLength = pathgadget_textfit(cl, data, msg, editbuff->peb_Length, FALSE, FALSE, data->Width, NULL);

	// Need to scroll the displayed text?
	if (editbuff->peb_CursorPos - data->DisplayPos >= data->DisplayLength)
	{
	    data->DisplayLength = pathgadget_textfit(cl, data, msg, editbuff->peb_Length, TRUE, TRUE, data->Width, NULL);
	}
    }

ClipFail:
    if (ClipIO)
    {
	if (ClipIO->io_Actual)
	{
	    TEXT clip_buf[128];
	    // Flush clipboard
	    ClipIO->io_Data=clip_buf;
	    ClipIO->io_Length=128;
	    while (ClipIO->io_Actual)
	    {
		if (DoIO((struct IORequest *)ClipIO))
		    break;
	    }
	}
#ifdef RESOURCE_TRACKING
	pathgadget_close_clipboard(cl, ClipIO);
#else
	pathgadget_close_clipboard(ClipIO);
#endif
    }
    return ret;
}


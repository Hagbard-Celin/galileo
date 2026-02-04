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

#ifndef GALILEOFM_OBJECTLIST_H
#define GALILEOFM_OBJECTLIST_H

#ifndef INTUITION_INTUITION_H
#include <intuition/intuition.h>
#endif

#define GADGET(obj) (obj->gl_info.gl_gadget.gadget)
#define AREA(obj) obj->gl_info.gl_area
#define OBJLIST(win) ((ObjectList *)(DATA(win)->object_list.mlh_Head))

typedef struct _GL_Object
{
    struct _GL_Object	    *next;	    // Next object
    short		    type;	    // Type of object
    unsigned char	    key;	    // Key equivalent

    unsigned char	    flags2;	    // Additional flags

    unsigned short	    id;		    // Object ID
    unsigned short	    control_id;	    // Object that this controls
    struct IBox		    dims;	    // Object dimensions
    ULONG		    flags;	    // Object flags
    char		    *text;	    // Text
    UWORD		    object_kind;    // Object kind

    union
    {
	struct _gl_gadget
	{
	    struct Gadget *context; // Context data for the gadget
	    struct Gadget *gadget;  // The gadget itself
	    int components;	    // Number of component gadgets
	    LONG data;		    // Some data for the gadget
	    short choice_max;	    // Number of choices
	    short choice_min;	    // Minimum choice
	    struct Image *image;    // Gadget image
	} gl_gadget;

	struct _gl_text
	{
	    struct IBox text_pos;   // Text position
	    struct IBox area_pos;   // Area position
	    WORD base_pos;	    // Baseline position
	    short uscore_pos;	    // Underscore position
	} gl_text;

	struct _gl_area
	{
	    struct IBox text_pos;   // Text position within area
	    struct IBox area_pos;   // Area position
	    int frametype;	    // Frame type
	} gl_area;

	struct _gl_image
	{
	    struct IBox image_pos;  // Image position
	    struct Image *image;    // Image
	} gl_image;
    } gl_info;

    APTR memory;			    // Any other memory

    char *original_text;		    // Original text string
    char fg,bg;				    // Current pen colours

    ULONG   data_ptr;			    // Pointer to other data

    struct TagItem  *tags;		    // Copy of tags

    struct IBox char_dims;		    // Original dimensions
    struct IBox fine_dims;
} GL_Object;

typedef struct _ObjectList {
    struct MinNode	    node;
    GL_Object		    *firstobject;   // First object
    struct TextAttr	    attr;	    // Font used
    struct Window	    *window;	    // Window used
} ObjectList;

GL_Object *GetObject(ObjectList *,int);
CheckObjectArea(GL_Object *,int,int);
void SetGadgetChoices(ObjectList *list,ULONG id,APTR choices);

#endif

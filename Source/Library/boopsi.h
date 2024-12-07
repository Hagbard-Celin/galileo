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

#define THICK	0
#define THIN	1

#define CLASS_STRINGGAD		0x1234
#define CLASS_CHECKGAD		0x1235
#define CLASS_VIEW		0x1236
#define CLASS_FRAME		0x1237
#define CLASS_GAUGE		0x1238

typedef struct
{
    struct Library	    *IntuitionBase;
    struct GfxBase	    *GfxBase;
    struct Library	    *UtilityBase;
    struct Library	    *LayersBase;
#ifdef RESOURCE_TRACKING
    struct Library	    *ResTrackBase;
#endif
    struct LibData	    *data;
} BoopsiLibs;

Class *init_class(struct LibData *,char *,char *,unsigned long (*)(),long);
void class_free(Class *);

typedef struct
{
    struct IBox	    dims;		    // Gadget dimensions (total)
    struct IBox	    list_dims;		    // List dimensions
    struct IBox	    scroller_dims;	    // Scroller dimensions
    struct IBox	    text_dims;		    // Dimensions to render text within

    struct Gadget   *scroller;		    // Scroller gadget
    struct Image    *check;		    // Checkmark
    struct List	    boopsi_list;	    // List of BOOPSI gadgets

    short	    arrow_height;	    // Height of arrows

    unsigned short  text_width;		    // Text display width
    unsigned short  text_height;	    // Text display height
    unsigned short  text_offset;	    // Text display offset


    struct TextFont *list_font;		    // Font to use in lister

    short	    top;		    // Top item
    short	    count;		    // Item count
    short	    lines;		    // Number of visible lines
    short	    sel;		    // Selected item
    short	    last_sel;		    // Previously selected item
    short	    last_top;		    // Previously selected top

    struct List	    *labels;		    // Items
    struct Node	    *top_item;		    // Current top item

    ULONG	    flags;		    // Flags
    ULONG	    layout_flags;	    // Layout flags

    struct TagItem  scroll_map[2];	    // Scroller mapping tags
    struct TagItem  arrow_up_map[2];	    // Arrow mapping tags
    struct TagItem  arrow_down_map[2];	    // Arrow mapping tags

    long	    last_selection;	    // Return value for handle input

    char	    title[80];		    // Listview title
    short	    title_uscore;	    // Underscore position

    unsigned short  scroll_count;	    // Number of scroll events for arrows

    struct Gadget   *arrows[2];		    // Arrow gadgets

    struct TagItem  notify_tags[11];	    // Tags for OM_NOTIFY

    struct DrawInfo *draw_info;		    // Cached DrawInfo

    ULONG	    seconds;		    // For double-click
    ULONG	    micros;
    short	    click_sel;

    short	    drag_x_position;	    // Positions for reporting drag
    short	    drag_y_position;
    short	    drag_sel;		    // Drag-selected item

    short 	    scroller_width;
} ListViewData;

#define LVF_SHOW_SELECTED		(1<<0)		// Show selected
#define LVF_SELECTED_CHECK		(1<<1)		// Check selected
#define LVF_MULTI_SELECT		(1<<2)		// Multiple selections
#define LVF_DETACHED			(1<<3)		// List has been detached
#define LVF_GOT_FONT			(1<<4)		// Got our own font
#define LVF_READ_ONLY			(1<<5)		// Read-only listview
#define LVF_SELECTED_HIGH		(1<<6)		// Highlight selected
#define LVF_DONE_FIRST			(1<<7)		// Done first draw
#define LVF_SHOW_CHECKS			(1<<8)		// Show checkmarks
#define LVF_NO_SCROLLER			(1<<9)		// No scroller necessary
#define LVF_TOP_JUSTIFY			(1<<10)		// Top-justify items
#define LVF_RIGHT_JUSTIFY		(1<<11)		// Right-justify items
#define LVF_DRAG_NOTIFY			(1<<12)		// Notify of drags
#define LVF_DRAG_FLAG			(1<<13)
#define LVF_NO_VERT_SCROLL		(1<<14)
#define LVF_CANCEL			(1<<15)
#define LVF_NO_HIGHLIGHT		(1<<16)		// No highlighting with checkmarks
#define LVF_SHIFT_ARROWS		(1<<17)
#define LVF_SHOW_FILENAMES		(1<<18)		// Only show filenames
#define LVF_SHOW_SEPARATORS		(1<<19)		// Show --- as separator
#define LVF_SCROLLER_LEFT		(1<<20)		// Scroller on left
#define LVF_DISABLE_CHANGE		(1<<21)		// Disable state changed
#define LVF_THIN_BORDER			(1<<22)		// Thin borders

#define LVF_NO_SCROLLING        (1<<27)
#define LVF_SCROLL_FLAG			(1<<28)

typedef struct
{
    WORD	        Top;
    WORD	        Total;
    WORD	        Visible;
    WORD	        OrgTop;
    WORD	        RelSize;
    WORD	        Height;
    WORD	        Width;
    WORD	        KnobSize;
    struct Rectangle    PropGadget;
    struct Rectangle    Knob;
    BOOL	        Scrolling;
    WORD                JumpSize;
    WORD                HalfStep;
    WORD                OrgKnobPos;
    WORD                OldKnobPos;
    WORD                MouseOffset;
    UWORD 	        Flags;
} PropGadgetData;

#define GPROPG_FREEVERT     (1<<0)
#define GPROPG_FREEHORIZ    (1<<1)
#define GPROPG_SPRINGLOADED (1<<2)

typedef struct
{
    short		    type;
    short		    fpen;
    struct LibData	    *data;
    unsigned short	    flags;
} BoopsiImageData;

#define BIF_THIN_BORDERS	(1<<0)

#define LVGID_SCROLLER		65535
#define LVGID_ARROW		65534

typedef struct
{
    char	    title[80];	    // Button text
    short	    title_uscore;   // Underscore position
    struct TextFont *font;	    // Font to use
    unsigned long   flags;	    // Flags
    short	    place;	    // Text placement
    long	    value;
    char	    *data;
    short	    justify;	    // Justification
} ButtonData;

typedef struct
{
    ButtonData	    data;
    struct IBox	    border;
    struct TextFont *string_font;
    char	    *buffer;
    struct Task	    *change_task;
    short	    change_bit;
} StringData;

typedef struct
{
    ButtonData	    data;
    struct Image    *check_image;
} CheckData;

typedef struct
{
    ButtonData	    data;
    long	    total;
    long	    free;
    short	    fillpen;
    short	    fillpen_alert;
} GaugeData;

#define BUTTONF_THIN		(1<<0)	// Thin border
#define BUTTONF_TEXT_INSIDE	(1<<1)	// Text inside
#define BUTTONF_STRING		(1<<2)	// String gadget
#define BUTTONF_INSIDE		(1<<3)	// Inside gadget
#define BUTTONF_BOLD		(1<<4)	// Bold text
#define BUTTONF_CHECK		(1<<5)	// Checkbox
#define BUTTONF_SEL		(1<<6)	// Button was selected
#define BUTTONF_SHRUNK		(1<<7)	// Done shrink (strings)
#define BUTTONF_NO_GHOST	(1<<8)	// No ghosting on disable
#define BUTTONF_DISABLED	(1<<9)	// Button is disabled
#define BUTTONF_VIEW		(1<<10)	// Text view
#define BUTTONF_VIEW_NUM	(1<<11)	// Is a number
#define BUTTONF_ITALICS		(1<<12)	// Italics
#define BUTTONF_FRAME		(1<<13)	// Frame
#define BUTTONF_NOBORDER	(1<<14)	// No border
#define BUTTONF_RECESSED	(1<<15)	// Recessed
#define BUTTONF_GAUGE		(1<<16)	// Gauge

typedef struct
{
    struct IBox		    box;	    // Internal display box
    short		    pen_width;	    // Width of each pen selector
    short		    pen_height;	    // Height of each pen selector
    short		    pen_cols;	    // Number of columns
    short		    pen_rows;	    // Number of rows
    short		    pen_count;	    // Number of pens
    short		    *pen_array;	    // Pen array
    short		    pen_sel;	    // Selected pen

    short		    *def_pen_array;
    short		    old_sel;
    BOOL		    done_first;
    short		    last_row,last_col;
    short		    last_sel;
    struct Rectangle	    selection_box;

    char		    title[80];	    // Title
    short		    title_uscore;   // Underscore position
    struct TextFont	    *font;	    // Font to use

    short		    flags;	    // Flags

    struct TagItem	    notify_tags[3]; // Tags for OM_NOTIFY
} PaletteData;

ULONG __asm image_dispatch(register __a0 Class *cl,register __a2 Object *obj,register __a1 Msg msg);
ULONG __asm button_dispatch(register __a0 Class *cl,register __a2 Object *obj,register __a1 Msg msg);
ULONG __asm propgadget_dispatch(register __a0 Class *cl, register __a2 Object *obj, register __a1 Msg msg);
ULONG __asm listview_dispatch(register __a0 Class *cl,register __a2 Object *obj,register __a1 Msg msg);
ULONG __asm palette_dispatch(register __a0 Class *cl,register __a2 Object *obj,register __a1 Msg msg);


void image_draw(Class *,struct Image *,BoopsiImageData *,struct impDraw *);
void button_render(Class *,struct Gadget *,ButtonData *,struct gpRender *);
void propgadget_calc_coords(Class *cl, PropGadgetData *data, struct Gadget *gadget, struct GadgetInfo *gi, BOOL vert, BOOL full);
ULONG propgadget_render(Class *cl, struct Gadget *gadget, struct GadgetInfo *GInfo, struct gpRender *msg);
void propgadget_notify(Class *cl, Object *obj, struct opUpdate *msg, PropGadgetData *data, struct GadgetInfo *GInfo, ULONG flags);
void listview_render(Class *,struct Gadget *,ListViewData *,struct gpRender *);
void listview_border(Class *,struct RastPort *,UWORD *,struct IBox *,ULONG,short);
void listview_draw_items(Class *,struct Gadget *,struct RastPort *,struct DrawInfo *,ListViewData *,short);
void listview_draw_item(Class *,struct Gadget *,struct RastPort *,struct DrawInfo *,ListViewData *,struct Node *,unsigned short,struct IBox *);
short listview_get_sel(Class *,struct Gadget *,ListViewData *,struct gpInput *,short);
void listview_get_dims(Class *,ListViewData *);
void listview_resize(Class *,struct Gadget *,ListViewData *,struct gpResize *);

void button_resize(Class *,struct Gadget *,ButtonData *,struct gpResize *);
void button_field_shrink(StringData *,struct Gadget *,struct Window *,struct Requester *);

void palette_render(Class *,struct Gadget *,PaletteData *,struct gpRender *);


#define GOTBUF_MAIN	(1<<0)
#define GOTBUF_UNDO	(1<<1)
#define GOTBUF_WORK	(1<<2)
#define GOTBUF_ALL	(GOTBUF_MAIN|GOTBUF_UNDO|GOTBUF_WORK)

#ifdef BOOPSI_LIBS
#define IntuitionBase	((BoopsiLibs *)cl->cl_Dispatcher.h_Data)->IntuitionBase
#define GfxBase		((BoopsiLibs *)cl->cl_Dispatcher.h_Data)->GfxBase
#define UtilityBase	((BoopsiLibs *)cl->cl_Dispatcher.h_Data)->UtilityBase
#define LayersBase	((BoopsiLibs *)cl->cl_Dispatcher.h_Data)->LayersBase
#ifdef RESOURCE_TRACKING
#define ResTrackBase	((BoopsiLibs *)cl->cl_Dispatcher.h_Data)->ResTrackBase
#endif
#define DOSBase		((BoopsiLibs *)cl->cl_Dispatcher.h_Data)->data->dos_base
#endif

struct gpRenderExt
{
    struct gpRender render;
    long	    value;
};

#define GREDRAW_EXTRA	(0x380)

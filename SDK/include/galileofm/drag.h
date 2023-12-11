#ifndef _GALILEOFM_DRAG
#define _GALILEOFM_DRAG

/*****************************************************************************

 Drag routines

 *****************************************************************************/

#ifndef GRAPHICS_GELS_H
#include <graphics/gels.h>
#endif

typedef struct _DragInfo
{
	struct RastPort	*rastport;	// Stores RastPort this bob belongs to
	struct ViewPort	*viewport;	// Stores ViewPort

	WORD		width;		// Bob width
	WORD		height;		// Bob height

	struct VSprite	sprite;		// VSprite structure
	struct Bob	bob;		// BOB structure

	unsigned long	flags;		// Flags

	struct RastPort	drag_rp;	// RastPort we can draw into
	struct BitMap	drag_bm;	// BitMap we can draw into

	struct Window	*window;	// Window pointer
} DragInfo;

typedef struct
{
	struct VSprite	head;		// GEL list head sprite
	struct VSprite	tail;		// GEL list tail sprite
	struct GelsInfo	info;		// GEL info
} DragInfoExtra;

#define DRAGF_VALID		(1<<0)	// Bob is valid
#define DRAGF_OPAQUE		(1<<1)	// Bob should be opaque
#define DRAGF_DONE_GELS		(1<<2)	// Installed GelsInfo
#define DRAGF_NO_LOCK		(1<<3)	// Don't lock layers
#define DRAGF_TRANSPARENT	(1<<4)	// Bob should be transparent (use with opaque)

void FreeDragInfo(DragInfo *);
void GetDragImage(DragInfo *,long,long);
DragInfo *GetDragInfo(struct Window *,struct RastPort *,long,long,long);
void GetDragMask(DragInfo *);
void HideDragImage(DragInfo *);
void ShowDragImage(DragInfo *,long,long);
void StampDragImage(DragInfo *,long,long);


#endif

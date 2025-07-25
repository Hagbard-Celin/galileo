#ifndef _GALILEOFM_GUI
#define _GALILEOFM_GUI

/*****************************************************************************

 GUI support

 *****************************************************************************/

// Screen Info

#define SCRI_LORES	(1<<0)

ULONG ScreenInfo(struct Screen *);


// Drawing
void DrawBox(struct RastPort *,struct Rectangle *,struct DrawInfo *,BOOL);
void DrawFieldBox(struct RastPort *,struct Rectangle *,struct DrawInfo *);


// Palette
void LoadPalette32(struct ViewPort *,unsigned long *);
void GetPalette32(struct ViewPort *,unsigned long *,unsigned short,short);


// BOOPSI
struct Gadget *FindBOOPSIGadget(struct List *,UWORD);
void BOOPSIFree(struct List *);


// Gadgets
struct Gadget *AddScrollBars(struct Window *,struct List *,struct DrawInfo *,short);
struct Gadget *CreateTitleGadget(struct Screen *,struct List *,BOOL,short,short,unsigned short);
struct Gadget *FindGadgetType(struct Gadget *,UWORD);
void FixTitleGadgets(struct Window *);
void ActivateStrGad(struct Gadget *,struct Window *);

#define SCROLL_NOIDCMP	(1<<0)		// Don't send IDCMPUPDATE messages
#define SCROLL_VERT	(1<<1)		// Vertical scroller
#define SCROLL_HORIZ	(1<<2)		// Horizontal scroller

enum
{
	GAD_VERT_SCROLLER=2,	// Vertical scroller
	GAD_VERT_ARROW_UP,
	GAD_VERT_ARROW_DOWN,

	GAD_HORIZ_SCROLLER,	// Horizontal scroller
	GAD_HORIZ_ARROW_LEFT,
	GAD_HORIZ_ARROW_RIGHT,
};


// Bitmap routines
struct BitMap *NewBitMap(ULONG,ULONG,ULONG,ULONG,struct BitMap *);
void DisposeBitMap(struct BitMap *);


// Find public screen node
struct PubScreenNode *FindPubScreen(struct Screen *,BOOL);


// Set busy pointer in a window
void SetBusyPointer(struct Window *);

#endif

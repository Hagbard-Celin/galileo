#ifndef _GALILEOFM_POPUP
#define _GALILEOFM_POPUP

/*****************************************************************************

 Popup menus

 *****************************************************************************/

// Defines an item in a menu
typedef struct {
	struct MinNode	node;
	char		*item_name;		// Menu item name
	UWORD		id;			// Menu ID
	UWORD		flags;			// Menu item flags
	APTR		data;			// Menu item data
} PopUpItem;

#define POPUPF_LOCALE			(1<<0)	// Item name is a locale ID
#define POPUPF_CHECKIT			(1<<1)	// Item can be checked
#define POPUPF_CHECKED			(1<<2)	// Item is checked
#define POPUPF_SUB			(1<<3)	// Item has sub-items
#define POPUPF_DISABLED			(1<<4)	// Item is disabled

#define POPUP_BARLABEL			(char *)-1

#define POPUP_HELPFLAG			(1<<15)	// Set if help key pressed

// Used to declare a callback for a menu
#ifndef REF_CALLBACK
#define REF_CALLBACK(name)	void __asm (*name)(register __d0 ULONG,register __a0 APTR,register __a1 APTR)
#endif

// Defines a popup menu
typedef struct {
	struct MinList		item_list;	// List of menu items
	struct GalileoLocale	*locale;	// Locale data
	ULONG			flags;		// Flags
	ULONG			userdata;	// User data
	REF_CALLBACK		(callback);	// Refresh callback
} PopUpMenu;

#define POPUPMF_HELP		(1<<0)		// Supports help
#define POPUPMF_REFRESH		(1<<1)		// Use refresh callback
#define POPUPMF_ABOVE		(1<<2)		// Open above parent window

UWORD DoPopUpMenu(struct Window *,PopUpMenu *,PopUpItem **,UWORD);
PopUpItem *GetPopUpItem(PopUpMenu *,UWORD);

#endif

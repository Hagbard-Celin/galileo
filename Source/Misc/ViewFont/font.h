#define CATCOMP_NUMBERS
#include "font.strings"
#include "//Library/galileofmbase.h"
#include "//Library/galileofmpragmas.h"

#ifdef RESOURCE_TRACKING
#include <restrack_protos.h>
#include <g_restrack_pragmas.h>
#endif

enum
{
	ARG_FONT,
	ARG_SIZE,
	ARG_BOLD,
	ARG_ITALIC,
	ARG_ULINE,
	ARG_SCREEN,
	ARG_COUNT
};

typedef struct
{
	struct Screen		*screen;
	struct Window		*window;
	struct GalileoLocale	locale;

	ConfigWindow		win_dims;
	NewConfigWindow		new_win;
	ObjectList		*list;

	struct TextAttr		attr;
	struct TextFont		*font;

	char			font_name[256];
	short			font_size;
	short			font_style;

	char			title[80];

	ULONG 			arg_array[ARG_COUNT];

	char			font_text[256];

	struct MsgPort		*appport;
	struct AppWindow	*appwindow;

	struct RDArgs		*args;
	BOOL			resized;
	BOOL			first;

	struct Hook		refresh_hook;

	char			last_font_name[256];
	char			**size_labels;

	struct Window		*about;

    BPTR 			lock;
} font_data;

BOOL font_open(font_data *data);
void font_close(font_data *data);
void font_free(font_data *data);
void font_get_font(font_data *data);
void font_show_font(font_data *data,BOOL refresh);
void font_get_name(font_data *data,char *name);
void font_ask_name(font_data *data);

extern ConfigWindow font_window;
extern ObjectDef font_objects[];

enum
{
	GAD_FONT_LAYOUT,

	GAD_FONT_FONT,
	GAD_FONT_SIZE,
	GAD_FONT_PRINT,
	GAD_FONT_UP,
	GAD_FONT_DOWN,
	GAD_FONT_BOLD,
	GAD_FONT_ITALIC,
	GAD_FONT_ULINE,
	GAD_FONT_DISPLAY,
	GAD_FONT_FONT_POPUP,
	GAD_FONT_CYCLE,

	MENU_OPEN_FONT,
	MENU_SAVE_SETTINGS,
	MENU_QUIT,
	MENU_ABOUT,
};


extern struct Library *GalileoFMBase;
extern struct LocaleBase *LocaleBase;
extern struct Library *LayersBase;
extern struct Library *AslBase;
extern struct Library *DiskfontBase;
extern struct LocaleBase *LocaleBase;
extern struct Library *WorkbenchBase;

#ifdef RESOURCE_TRACKING
extern struct Library *ResTrackBase;
#endif

extern MenuData font_menus[];

void init_locale_data(struct GalileoLocale *locale);
ULONG __asm font_refresh(register __a0 struct Hook *hook,register __a1 struct IntuiMessage *msg);
short font_get_size(font_data *data,short dir);
void font_build_labels(font_data *data,struct FontContentsHeader *fch);
void font_show_about(font_data *data);
void font_save_settings(font_data *data);
void font_read_settings(font_data *data);
void read_parse_set(char **ptr,unsigned short *val);

void lsprintf __ARGS((char *,...));

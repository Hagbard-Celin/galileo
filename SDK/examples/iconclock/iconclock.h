#define CATCOMP_NUMBERS
#include "iconclock.strings"

#define _GALILEOFM_MODULE_DEF
#include <galileofm/modules.h>

#include <exec/memory.h>
#include <exec/execbase.h>
#include <workbench/workbench.h>
#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/utility_protos.h>
#include <clib/wb_protos.h>
#include <pragmas/exec_pragmas.h>
#include <pragmas/dos_pragmas.h>
#include <pragmas/graphics_pragmas.h>
#include <pragmas/intuition_pragmas.h>
#include <pragmas/utility_pragmas.h>
#include <pragmas/wb_pragmas.h>
#include <string.h>
#include <stdio.h>
#include <dos.h>
#include <ctype.h>
#include <stdlib.h>

#define ENV_ICONCLOCK		"Galileo/Icon Clock"
#define NAME_ICONCLOCK		"Icon Clock"

#define CLOCK_WIDTH	48
#define CLOCK_HEIGHT	17

#define NUMERAL_WIDTH	8
#define NUMERAL_HEIGHT	13
#define COLON_WIDTH	6
#define COLON_HEIGHT	13

// Semaphore used to find the icon clock process in memory
typedef struct
{
	struct SignalSemaphore	sem;
	IPCData			*ipc;
} IconClockSemaphore;

// Data used by this program
typedef struct
{
	ULONG			a4;		// Keep A4 pointer to reference global data
	struct Library		*library;	// Pointer to the galileofm.library
	struct Library		*module;	// Pointer to this module

	IPCData			*ipc;		// Our IPC pointer
	struct MsgPort		*notify_port;	// Notify message port
	APTR			notify_req;	// Active notify request

	struct DiskObject	clock_icon;	// Clock icon
	struct Image		clock_image;	// Clock image

	struct RastPort		clock_rp;	// Rastport for drawing
	struct BitMap		clock_bm;	// Bitmap to draw into

	struct AppIcon		*appicon;	// AppIcon pointer
	struct MsgPort		*app_port;	// AppIcon message port

	long			icon_x;		// Current position
	long			icon_y;
	char			buffer[256];	// Work buffer

	IconClockSemaphore	sem;		// Semaphore to identify ourselves

	TimerHandle		*timer;		// Timer stuff
	long			day;
	char			date_buffer[20];

	struct Screen		*screen;	// Screen we're on
	short			pen;		// The pen we've allocated
	long			colour;

	IPCData			*main_ipc;	// Galileo main IPC pointer

	struct Window		*about;		// Pointer to about window
} iconclock_data;

void icon_clock(void);
ULONG __asm iconclock_startup(
	register __a0 IPCData *,
	register __a1 iconclock_data *);
void iconclock_cleanup(iconclock_data *);
void iconclock_show(iconclock_data *);
void read_parse_set(char **,long *);
void iconclock_init(iconclock_data *,struct Screen *);

extern USHORT __chip numeral_data[10][13],colon_data[3][13];
void iconclock_info(iconclock_data *);

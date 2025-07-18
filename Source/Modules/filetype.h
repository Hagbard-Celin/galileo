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

Opus� and Directory Opus� and associated images are registered trademarks of GPSoftware.
DOpus(TM), DirOpus(TM) and Opus File Manager(TM) are trademarked 1991 by GPSoftware.
Opus Magellan (TM) is trademarked 1996 GPSoftware.
All other trademarks are the property of their respective owners.

The release of Directory Opus 5 under the GPL in NO WAY affects
the existing commercial status of Directory Opus for Windows.

For more information on Directory Opus for Windows please see:

		 http://www.gpsoft.com.au

*/

#define CATCOMP_NUMBERS
#include "filetype.strings"
#include "modules_lib.h"
#include "module.h"

#include "Config/galileoconfig.h"
#include "Config/galileoconfig_pragmas.h"

/* mod_ids */
enum
{
    MODID_FIND_FT,
    MODID_CREATE_FT
};

#define FILENAME_MAXLEN	(30)
#define FILETYPE_MAXLEN	(31)
#define ID_MAXLEN	(7)
#define PRI_MAXLEN	(4)

/* How many bytes to look at at start of file */
#define BYTECOUNT	16

typedef struct
{
    struct Screen	    *screen;
    IPCData		    *ipc;
    IPCData		    *main_ipc;
    CONST GalileoCallbackInfo     *gci;

    NewConfigWindow	    new_win;
    struct Window	    *window;

    struct MsgPort	    *app_port;
    struct AppWindow	    *app_window;

    ObjectList		    *list;

    struct pointer_packet   pointer_packet;
    Att_List		    *filetype_cache;

    Att_List		    *filetypes_list;
    Att_List		    *storage_list;

    Att_List		    *listview_list;

    char		    current_entry_path[260];

    int			    count_filetypes;
    int			    best_filetypes;
    int			    count_storage;
    int			    best_storage;

    Cfg_FiletypeList	    *filetype_list;
    Cfg_Filetype	    *best_installed_ft;
    Cfg_Filetype	    *best_stored_ft;
    Cfg_Filetype	    *edited_filetype;

    IPCData		    *editor_ipc;
    IPCData		    *creator_ipc;
    ULONG		    a4;

    char		    path[256];
} finder_data;

struct filetype_info
{
    char  fti_path[256 + 1];
    char  fti_filename[30 + 1];		    /* Null terminated. Max 30 chars used for now */
    LONG  fti_iff_type;			    /* IFF FORM type */
    ULONG fti_datatype_group;		    /* Group that the DataType is in */
    ULONG fti_datatype_ID;		    /* ID for DataType (same as IFF FORM type) */
    UBYTE fti_filebytes[BYTECOUNT];	    /* First BYTECOUNT bytes of the file */
    UBYTE fti_bytemask[BYTECOUNT];	    /* Mask for matching bytes */
    int   fti_bytecount;		    /* 0 to BYTECOUNT */
    UBYTE fti_filebytesc[BYTECOUNT];	    /* First BYTECOUNT bytes of the file (case insensitive) */
    UBYTE fti_bytemaskc[BYTECOUNT];	    /* Mask for matching bytes (case insensitive) */
    int   fti_bytecountc;		    /* 0 to BYTECOUNT (case insensitive) */
    ULONG fti_flags;			    /* See below */
};

typedef struct
{
    struct Screen	    *screen;
    IPCData		    *ipc;
    IPCData		    *main_ipc;
    IPCData		    *galileo_ipc;

    APTR		    memh;

    NewConfigWindow	    new_win;
    struct Window	    *window;
    struct MsgPort	    *app_port;
    struct AppWindow	    *app_window;

    ObjectList		    *list;

    Att_List		    *file_list;

    char		    filetype_name[FILETYPE_MAXLEN + 1];

    int			    caseflag;

    struct filetype_info    master_fti;
    char		    *match_name;
    char		    *match_group;
    char		    *match_id;
    char		    *match_iff;
    char		    *match_bytes;
    char		    *match_bytesc;

    Cfg_FiletypeList	    *filetype_list;
    Cfg_Filetype	    *filetype;

    int			    edited;

    struct FileRequester    *filereq;
    char		    req_dir[256 + 1];

    IPCData		    *editor_ipc;
    ULONG		    a4;
} creator_data;

extern ConfigWindow _finder_window;
extern ObjectDef _finder_objects[];

extern ConfigWindow _creator_window;
extern ObjectDef _creator_objects[];

enum
{
    GAD_FIND_LAYOUT,
    GAD_FIND_LISTVIEW,
    GAD_FIND_TEXT1,
    GAD_FIND_TEXT2,
    GAD_FIND_TEXT3,
    GAD_FIND_TEXT4,
    GAD_FIND_TEXT5,
    GAD_FIND_TEXT6,
    GAD_FIND_TEXT7,
    GAD_FIND_TEXT8,
    GAD_FIND_USE,
    GAD_FIND_INSTALL,
    GAD_FIND_CREATE,
    GAD_FIND_EDIT,
    GAD_FIND_CANCEL,

    GAD_CREATE_LAYOUT,
    GAD_CREATE_LISTVIEW,
    GAD_CREATE_ADD,
    GAD_CREATE_DELETE,
    GAD_CREATE_CLEAR,
    GAD_CREATE_FILETYPE,
    GAD_CREATE_NAME,
    GAD_CREATE_NAME_FIELD,
    GAD_CREATE_IFF,
    GAD_CREATE_IFF_FIELD,
    GAD_CREATE_GROUP,
    GAD_CREATE_GROUP_FIELD,
    GAD_CREATE_ID,
    GAD_CREATE_ID_FIELD,
    GAD_CREATE_BYTES,
    GAD_CREATE_BYTES_FIELD,
    GAD_CREATE_CYCLE,
    GAD_CREATE_EDIT,
    GAD_CREATE_SAVE,
    GAD_CREATE_CANCEL,
};

#define FTIF_USED		(1 << 0)
#define FTIF_FILENAME		(1 << 1)
#define FTIF_FILENAME_END	(1 << 2)
#define FTIF_GROUP		(1 << 3)
#define FTIF_ID			(1 << 4)
#define FTIF_IFF		(1 << 5)
#define FTIF_BYTES		(1 << 6)
#define FTIF_BYTESC		(1 << 7)

// Filetype matching commands
enum
{
FTOP_NOOP,		// No operation (0)
FTOP_MATCH,		// Match text (1)
FTOP_MATCHNAME,		// Match filename (2)
FTOP_MATCHBITS,		// Match protection bits (3)
FTOP_MATCHCOMMENT,	// Match comment (4)
FTOP_MATCHSIZE,		// Match size (5)
FTOP_MATCHDATE,		// Match date (6)
FTOP_MOVETO,		// Move to absolute location (7)
FTOP_MOVE,		// Move to relative location (8)
FTOP_SEARCHFOR,		// Search for text (9)
FTOP_MATCHFORM,		// Match an IFF FORM (10)
FTOP_FINDCHUNK,		// Find an IFF chunk (11)
FTOP_MATCHDTGROUP,	// Match datatypes group (12)
FTOP_MATCHDTID,		// Match datatypes ID (13)
FTOP_MATCHNOCASE,	// Match text case insensitive (14)
FTOP_DIRECTORY,		// Match directory (15)

FTOP_LAST,		// Last valid command

FTOP_SPECIAL=252,	// Start of special instructions

FTOP_OR,		// Or (253)
FTOP_AND,		// And (254)
FTOP_ENDSECTION		// End of a section (255)
};

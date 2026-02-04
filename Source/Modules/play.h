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

#include <ctype.h>
#include <stdlib.h>
#include <clib/alib_protos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/icon.h>
#include <proto/datatypes.h>
#include <proto/wb.h>
#include <datatypes/soundclass.h>
#include <devices/audio.h>

#include <gfm/version.h>
#include "/Library/galileofmbase.h"
#include "modules.h"
#include "modules_lib.h"

#define ENV_PLAY	"Galileo/Player"

typedef struct VHeader {
	ULONG oneShotHiSamples,repeatHiSamples,samplesPerHiCycle;
	UWORD samplesPerSec;
	UBYTE ctOctave,sCompression;
	long volume;
} Voice8Header;

#define ID_CHAN MAKE_ID('C','H','A','N')

typedef struct
{
	IPCData         *ipc;
	char            *file;

	struct MsgPort      *audio_port;
	struct IOAudio      audio_req1[2];
	struct IOAudio      audio_req2[2];

	unsigned char       *audio_data;
	unsigned long       audio_size;
	unsigned long       stereo_offset;
	unsigned char       *play_buffer[2];

	struct Window       *window;
	ObjectList      *objlist;

	NewConfigWindow     new_win;

	Voice8Header        header;
	unsigned long       channels;
	BOOL            svx_flag;

	unsigned long       play_speed;
	unsigned long       clock;

	APTR            dt_object;
	char            dt_signal;
	struct Library      *dt_base;

	struct Library      *music_base;
	short           module_type;

	char            *type_string;

	struct AppIcon      *app_icon;
	struct MsgPort      *app_port;
	struct DiskObject   *icon;

	char		play_name[40];
	char		play_length[40];
	char		play_type[40];
	char		play_status[40];

	struct Screen	*screen;

	ConfigWindow	dims;
	struct IBox	pos;
	long		icon_x;
	long		icon_y;
	char		buf[50];

	ULONG		flags;
} play_data;

#define PLAYF_QUIET	(1<<0)
#define PLAYF_ICON	(1<<1)

#define DataTypesBase   (data->dt_base)
#define MUSICBase   (data->music_base)


#define PLAY_DOS_ERROR      0
#define PLAY_OK             1
#define PLAY_ABORT          -1
#define PLAY_MEMORY_ERROR   -2
#define PLAY_INIT_ERROR     -3

#define PLAY_MAX_SIZE       25600

void play_free(play_data *);
BOOL play_init(play_data *);
BOOL play_file(play_data *);
void play_cleanup(play_data *);
void unpack_delta(char *,unsigned long,char *,unsigned char);
void play_update_length(play_data *);
BOOL wait_for_play(play_data *data,BOOL dt);
void play_iconify(play_data *data);
void play_open_window(play_data *data,struct Screen *screen,struct Window *parent);
void play_update_info(play_data *data,char *,char *,char *,char *);
void play_reveal(play_data *data,struct Screen *scr);
void read_parse_set(char **ptr,long *val);
void play_snapshot(play_data *data,struct AppSnapshotMsg *msg);

extern ConfigWindow play_window;
extern ObjectDef play_objects[];

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

#include "about.h"
#include "modules.h"

#define DO_MESSAGE

#define SPIN_TIME	50000
#define TEXT_TIME	40000
#define ROLL_TICKS	500

char *version="$VER: about.gfmmodule 0.2 "__AMIGADATE__" ";

static const struct TextAttr topaz_attr={"topaz.font",8,0,0};

void remap_logo(struct Screen *scr);
void free_logo_remap(void);
void about_show_image(struct Window *,GL_Object *,image_data *);
void about_scroll_text(about_data *data);
void free_about_data(about_data *data);
void about_next_line(about_data *data);
void get_message(about_data *data);

short count=0,first=1;

int __asm __saveds L_Module_Entry_Internal(
	register __a0 struct List *files,
	register __a1 struct Screen *screen,
	register __a2 IPCData *ipc,
	register __a3 IPCData *main_ipc,
	register __d0 ULONG mod_id,
	register __d1 ULONG mod_data)
{
	ConfigWindow dims;
	NewConfigWindow newwin;
	ObjectList *objlist;
	TimerHandle *timer;
	short frame=1;
	GL_Object *object;
	short left=0,top=0,a;
	struct Node *node;
	short show_jon=0,need_open=1,scroll_text=0;
	short ret=0;
	struct Image logo;
	about_data data;
	long timer_val=SPIN_TIME,timer_count=0;
	struct TextFont *topaz_font;

#ifdef MAKE_MESSAGE
	{
		APTR file;

		if (file=OpenBuf("ram:foo",MODE_NEWFILE,4096))
		{
			short count=0;
			unsigned char *buf,foo[10];

			WriteBuf(file,"	{",-1);
			buf=about_message;
			while (*buf)
			{
				unsigned char ch;

				ch=*buf;
				ch=(~ch)+1;
				lsprintf(foo,"0x%02lx,",ch);
				WriteBuf(file,foo,-1);
				if (++count==16)
				{
					WriteBuf(file,"\n	",-1);
					count=0;
				}
				++buf;
			}
			WriteBuf(file,"0x00};\n",-1);
			CloseBuf(file);
		}
		return 0;
	}
#endif

	// Only run once
	if (count>0) return 0;
	count=1;

	if (!(topaz_font=OpenFont(&topaz_attr)))
	    return ret;

	// Get datatypes library
	DataTypesBase=OpenLibrary("datatypes.library",0);

	// Initialise
	for (a=0;a<logo_frames;a++)
		logo_obj[a]=0;
	data.window=0;
	remap.ir_PenArray=0;
	data.message=0;
	data.trans_info=0;

	// Get dimensions
	dims=about_window;

	// Count text lines
	for (node=files->lh_Head,a=0;node->ln_Succ;a++,node=node->ln_Succ);
	dims.char_dim.Height=a-4;

	// Open timer
	if (timer=AllocTimer(UNIT_MICROHZ,0))
	{
		// Send time request
		StartTimer(timer,0,timer_val);
	}

	// Fill out Image structure
	logo.NextImage=0;
	logo.Width=logo_width;
	logo.Height=logo_height;
	logo.Depth=2;
	logo.PlanePick=3;
	logo.PlaneOnOff=0;

	// Event loop
	FOREVER
	{
		struct IntuiMessage *msg;
		IPCMessage *imsg;
		BOOL break_flag=0;

		// IPC messages?
		while (imsg=(IPCMessage *)GetMsg(ipc->command_port))
		{
			// Activate?
			if (imsg->command==IPC_ACTIVATE && data.window)
			{
				WindowToFront(data.window);
				ActivateWindow(data.window);
			}

			// Quit?
			else
			if (imsg->command==IPC_ABORT ||
				imsg->command==IPC_QUIT)
			{
				break_flag=1;
			}

			// Hide?
			else
			if (imsg->command==IPC_HIDE && data.window)
			{
				// Free remapped images
				free_logo_remap();

				// Close stuff
				free_about_data(&data);
			}

			// Show?
			else
			if (imsg->command==IPC_SHOW && !data.window)
			{
				// Need to re-open
				need_open=1;
				screen=(struct Screen *)imsg->data;
			}

			IPC_Reply(imsg);
		}

		// Check break flag
		if (break_flag) break;

		// Open window?
		if (need_open)
		{
			need_open=0;

			// Fill out new window
			newwin.parent=screen;
			newwin.dims=&dims;
			newwin.title="About Galileo";
			newwin.locale=0;
			newwin.port=0;
			newwin.flags=WINDOW_VISITOR|WINDOW_AUTO_KEYS|WINDOW_REQ_FILL|WINDOW_SCREEN_PARENT;
			newwin.font=topaz_font;

			// Remap image
			remap_logo(screen);

			// Open window
			if (!(data.window=OpenConfigWindow(&newwin)) ||
				!(objlist=AddObjectList(data.window,about_objects))) break;

			ret=1;

			// Initialise data
			data.text_area=GetObject(objlist,GAD_ABOUT_COPYRIGHT);
			data.text_off=0;
			data.text_disp=0;

			// Clone rastport, set mask
			data.text_rp=*data.window->RPort;
			SetWrMsk(&data.text_rp,DRAWINFO(data.window)->dri_Pens[TEXTPEN]);
			SetAPen(&data.text_rp,DRAWINFO(data.window)->dri_Pens[TEXTPEN]);
			SetDrMd(&data.text_rp,JAM1);

			// Get text area
			data.text_dims=AREA(data.text_area).text_pos;
			data.text_dims.Left+=data.text_rp.TxWidth;
			data.text_dims.Top++;
			data.text_dims.Width-=data.text_rp.TxWidth<<1;
			data.text_dims.Height-=2;

			// Get region
			if (data.text_reg=NewRegion())
			{
				struct Rectangle rect;

				// Set region rectangle
				rect.MinX=data.text_dims.Left;
				rect.MinY=data.text_dims.Top;
				rect.MaxX=rect.MinX+data.text_dims.Width-1;
				rect.MaxY=rect.MinY+data.text_dims.Height-1;
				OrRectRegion(data.text_reg,&rect);
			}

			// Render copyright text
			SetAPen(data.window->RPort,DRAWINFO(data.window)->dri_Pens[TEXTPEN]);
			node=files->lh_Head;
			for (a=0;a<5 && node->ln_Succ;a++,node=node->ln_Succ)
				SetGadgetValue(objlist,GAD_COPYRIGHT_1+a,(ULONG)node->ln_Name);

			// Get translator information
			if (node->ln_Succ)
			{
				data.trans_info=node->ln_Name;
				node=node->ln_Succ;
			}

			// Render license text
			for (a=GAD_LICENSE_1;a<=GAD_LICENSE_12 && node->ln_Succ;node=node->ln_Succ,a++)
				SetGadgetValue(objlist,a,(ULONG)node->ln_Name);

			// Get message
			get_message(&data);

			// Get anim area
			object=GetObject(objlist,GAD_ABOUT_LOGO);
			left=AREA(object).area_pos.Left+((AREA(object).area_pos.Width-logo_width)>>1);
			top=AREA(object).area_pos.Top+((AREA(object).area_pos.Height-logo_height)>>1);

			// Got image?
			if (logo_obj[0])
			{
				// Draw image
				RenderImage(
					data.window->RPort,
					logo_obj[0],
					left,top,
					0);
			}

			// Using image
			else
			{
				// Fill out image structure
				logo.LeftEdge=left;
				logo.TopEdge=top;

				// Show first frame
				logo.ImageData=logo_image[0];
				DrawImage(data.window->RPort,&logo,0,0);
			}
		}

		// Timer return?
		if (data.window && CheckTimer(timer))
		{
#ifdef DO_MESSAGE
			// Increment count
			if (++timer_count==ROLL_TICKS)
			{
				// Start scroll, if we have a message
				if (data.message)
				{
					scroll_text=1;
					timer_val=TEXT_TIME;
				}
			}
#endif

			// Scroll text
			if (scroll_text) about_scroll_text(&data);

			// Ok to show hedgehog?
			if (!show_jon)
			{
				// Got image?
				if (logo_obj[frame])
				{
					// Draw image
					RenderImage(
						data.window->RPort,
						logo_obj[frame],
						left,top,
						0);
				}

				// Using image
				else
				{
					// Show next frame
					logo.ImageData=logo_image[frame];
					DrawImage(data.window->RPort,&logo,0,0);
				}

				// Move to next frame
				if (++frame==logo_frames) frame=0;
			}

			// Send timer request again
			StartTimer(timer,0,timer_val);
		}

		// Any Intuition messages?
		if (data.window)
		{
			while (msg=GetWindowMsg(data.window->UserPort))
			{
				struct IntuiMessage copy_msg;

				// Copy message and reply
				copy_msg=*msg;
				ReplyWindowMsg(msg);

				// Close window?
				if (copy_msg.Class==IDCMP_CLOSEWINDOW ||
					(copy_msg.Class==IDCMP_VANILLAKEY && copy_msg.Code==0x1b))
				{
					break_flag=1;
					break;
				}
			}
		}

		// Check break flag
		if (break_flag) break;

		// Wait for an event
		Wait(
			1<<ipc->command_port->mp_SigBit|
			((data.window)?(1<<data.window->UserPort->mp_SigBit):0)|
			((timer)?(1<<timer->port->mp_SigBit):0));
	}

	// Free remapped images
	free_logo_remap();

	// Close stuff
	free_about_data(&data);

	// Free timer
	FreeTimer(timer);
	CloseLibrary(DataTypesBase);
	CloseFont(topaz_font);

	count=0;
	return ret;
}

void free_about_data(about_data *data)
{
	// Close window
	CloseConfigWindow(data->window);
	data->window=0;

	// Free region
	if (data->text_reg)
	{
		DisposeRegion(data->text_reg);	
		data->text_reg=0;
	}

	// Free message
	FreeVec(data->message);
	data->message=0;
}


void init_locale_data(struct GalileoLocale *locale)
{
	locale->li_BuiltIn=0;
}


void remap_logo(struct Screen *scr)
{
	short frame;
	ULONG palette[13];
	OpenImageInfo info;
	APTR im;

	// Can't remap under 37
	if (GfxBase->LibNode.lib_Version<39) return;

	// Fill out info
	info.oi_Palette=palette;
	info.oi_Width=logo_width;
	info.oi_Height=logo_height;
	info.oi_Depth=2;

	// Set palette
	palette[0]=0x00040000;
	palette[4]=119<<24;
	palette[5]=119<<24;
	palette[6]=119<<24;
	palette[7]=0;
	palette[8]=0;
	palette[9]=0;
	palette[10]=68<<24;
	palette[11]=68<<24;
	palette[12]=68<<24;

	// Clear remap flags
	remap.ir_Flags=0;

	// Do ten images
	for (frame=0;frame<logo_frames;frame++)
	{
		// Get data
		info.oi_ImageData=logo_image[frame];

		// Get image
		if (!(im=OpenImage(0,&info)))
			break;

		// Remap image
		RemapImage(im,scr,&remap);

		// Save pointer
		logo_obj[frame]=im;
	}

	// Failed to get all?
	if (frame<logo_frames) free_logo_remap();
}

void free_logo_remap(void)
{
	short a;

	// Go through frames
	for (a=0;a<logo_frames;a++)
	{
		if (logo_obj[a])
		{
			// Close image
			CloseImage(logo_obj[a]);
			logo_obj[a]=0;
		}
	}

	// Free pens
	FreeImageRemap(&remap);
}

void about_scroll_text(about_data *data)
{
	// Increment count
	++data->text_off;

	// Ok to scroll?
	if (!(data->text_off&1)) return;

	// Increment position
	++data->text_pos;

	// Scroll area upwards
	ScrollRaster(
		&data->text_rp,
		0,1,
		data->text_dims.Left,
		data->text_dims.Top,
		data->text_dims.Left+data->text_dims.Width-1,
		data->text_dims.Top+data->text_dims.Height-1);

	// Not displaying text yet?
	if (!data->text_disp)
	{
		// Have we scrolled the whole lot away?
		if (data->text_pos>data->text_dims.Height)
		{
			// Displaying text now
			data->text_disp=1;
			data->text_off=0;
			data->text_pos=0;
			data->text_line=0;

			// Get first text line
			about_next_line(data);
		}
	}

	// We are displaying text
	else
	{
		// Got text?
		if (data->text_len>0)
		{
			// Install clip region
			InstallClipRegion(data->window->WLayer,data->text_reg);

			// Position for text
			Move(
				&data->text_rp,
				data->text_dims.Left+((data->text_dims.Width-data->text_size)>>1),
				data->text_rp.TxBaseline+data->text_dims.Top+data->text_dims.Height-data->text_pos-1);

			// Draw text
			Text(
				&data->text_rp,
				data->text_line,
				data->text_len);

			// Remove clipping
			InstallClipRegion(data->window->WLayer,0);
		}

		// Have we scrolled a full line?
		if (data->text_pos>data->text_rp.TxHeight+1)
		{
			// Reset offset
			data->text_pos=0;

			// Get the next line
			about_next_line(data);
		}
	}
}


// Get next line of about text
void about_next_line(about_data *data)
{
	short want_len,max_len;
	struct TextExtent extent;

	// No current line?
	if (!data->text_line) data->text_line=data->message;

	// Move on from current line
	else
	{
		// Were we on a newline?
		if (data->text_len==0) ++data->text_line;

		// No
		else
		{
			// Bump pointer
			data->text_line+=data->text_len;

			// If this leaves us on a newline or space, skip over it
			if (*data->text_line=='\n' || *data->text_line=='\t') ++data->text_line;
		}
	}

	// End of text?
	if (!*data->text_line) data->text_line=data->message;

	// If we're on a space, skip over it
	if (*data->text_line==' ') ++data->text_line;

	// Calculate desired length of the line
	for (want_len=0;data->text_line[want_len] && data->text_line[want_len]!='\n';++want_len);

	// Blank line?
	if (want_len==0)
	{
		data->text_len=0;
		data->text_size=0;
		return;
	}

	// Get maximum length that will actually fit
	max_len=TextFit(
		&data->text_rp,
		data->text_line,
		want_len,
		&extent,
		0,1,
		data->text_dims.Width,data->text_rp.TxHeight);

	// Go from here to end of current word
	want_len=max_len;
	while ( data->text_line[want_len] &&
			data->text_line[want_len]!='\n' && 
			data->text_line[want_len]!=' ') ++want_len;

	// Loop until successful
	do
	{
		// Get text size
		TextExtent(&data->text_rp,data->text_line,want_len,&extent);

		// Will it fit?
		if (extent.te_Width<=data->text_dims.Width)
		{
			// Save size
			data->text_len=want_len;
			data->text_size=extent.te_Width;
			return;
		}

		// Come backwards to word break
		for (--want_len;want_len>0 && data->text_line[want_len]!=' ';--want_len);

		// Didn't find one?
		if (want_len<1)
		{
			// Get maximum length
			want_len=max_len;
		}
	} while(1);
}


// Build scroll message
void get_message(about_data *data)
{
	short size;

	// Get size we need
	size=strlen(about_message);
	if (data->trans_info) size+=strlen(data->trans_info);
	size+=4;

	// Allocate buffer
	if (data->message=AllocVec(size,0))
	{
		// Build message
		lsprintf(data->message,about_message,data->trans_info);
	}
}

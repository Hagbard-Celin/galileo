/*
  XADBrowser.gfmmodule 1.22 - Galileo plugin to browse through XAD archives
  Copyright (C) 1999,2000 Mladen Milinkovic <mladen.milinkovic@ri.tel.hr>
  Copyright 2023 Hagbard Celine

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "XADbrowser.h"
//#include "compiler.h"
#include <stdio.h>

#include <libraries/xadmaster.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/xadmaster.h>

ULONG __saveds __asm ProgressHook(register __a0 struct Hook *hook, register __a1 struct xadProgressInfo *xpi)
{
	struct ModuleData *data = hook->h_Data;
	struct Library *GalileoFMBase;
	struct DosLibrary *DOSBase;
	struct Library *UtilityBase;
    struct xadMasterBase *xadMasterBase;
	struct TagItem tags[] = {PW_FileDone, 0, TAG_DONE};
/*	struct TagItem reqtags[]={AR_Window,0,AR_Message,0, AR_Button,0,
					AR_Button,0, AR_Button,0, AR_Button,0,
					AR_Button,0, AR_Button,0, TAG_DONE};*/
	ULONG ret = XADPIF_OK;
	LONG rc;
	char mess[200];

	tags[0].ti_Data = xpi->xpi_CurrentSize;

	if(!(GalileoFMBase=data->GalileoFMBase)) return(NULL);
	if(!(DOSBase=data->DOSBase)) return(NULL);
	if(!(UtilityBase=data->UtilityBase)) return(NULL);
    if(!(xadMasterBase=data->xadMasterBase)) return(NULL);

	SetProgressWindow(data->ptr, tags);

	switch(xpi->xpi_Mode)
	{
		case XADPMODE_ERROR:
		{
			sprintf(data->buf,"lister request %s \"%s\" %s|%s",
				data->lists, xadGetErrorText(xpi->xpi_Error),
				GetString(data->locale, MSG_OK),
				GetString(data->locale, MSG_ABORT));
            if (!(data->hook.gc_RexxCommand(data->buf, NULL, NULL, NULL, NULL)))
            {
                data->over=TRUE;
				ret &= ~XADPIF_OK;
            }

			break;
		}
		
		case XADPMODE_ASK:
		{
			if(xpi->xpi_Status & XADPIF_OVERWRITE)
			{
				if(data->All == TRUE)
				{
					ret |= XADPIF_OVERWRITE;
					break;
				}
				else if(data->All == FALSE) break;

				sprintf(mess, GetString(data->locale, MSG_EXISTS_FORM), FilePart(xpi->xpi_FileName));
/*				reqtags[0].ti_Data = (ULONG) data->listw;
				reqtags[1].ti_Data = (ULONG) mess;
				reqtags[2].ti_Data = (ULONG) GalileoGetString(data->locale,MSG_REPLACE);
				reqtags[3].ti_Data = (ULONG) GalileoGetString(data->locale,MSG_REPLACE_ALL);
				reqtags[4].ti_Data = (ULONG) GalileoGetString(data->locale,MSG_SKIP);
				reqtags[5].ti_Data = (ULONG) GalileoGetString(data->locale,MSG_SKIP_ALL);
				reqtags[6].ti_Data = (ULONG) GalileoGetString(data->locale,MSG_ABORT);
				AsyncRequest(IPCDATA(data->ipc),REQTYPE_SIMPLE,NULL,NULL,NULL,reqtags);
*/
				sprintf(data->buf, "lister request %s \"%s\" %s|%s|%s|%s|%s",
					data->lists, mess, GetString(data->locale, MSG_REPLACE),
					GetString(data->locale, MSG_REPLACE_ALL), GetString(data->locale, MSG_SKIP),
					GetString(data->locale, MSG_SKIP_ALL), GetString(data->locale, MSG_ABORT));
				
				rc = data->hook.gc_RexxCommand(data->buf, NULL, NULL, NULL, NULL);

				if(!rc) ret = NULL;
				else
				{
					if(rc <= 2) ret |= XADPIF_OVERWRITE;
					if(rc == 2) data->All = TRUE;
					else if(rc == 4) data->All = FALSE;
				}
			}
			break;
		}
	}

	return ret;
}

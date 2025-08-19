/*
  XADBrowser.gfmmodule 1.22 - Galileo plugin to browse through XAD archives
  Copyright (C) 1999,2000 Mladen Milinkovic <mladen.milinkovic@ri.tel.hr>
  Copyright 2023,2025 Hagbard Celine

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

ULONG __asm __saveds ProgressHook(register __a0 struct Hook *hook, register __a1 struct xadProgressInfo *xpi)
{
	struct XADbrowserData *data = hook->h_Data;
	struct xadMasterBase *xadMasterBase;
	struct TagItem tags[] = {PW_FileDone, 0, TAG_DONE};
	ULONG ret = XADPIF_OK;
	LONG rc;

	tags[0].ti_Data = xpi->xpi_CurrentSize;

	if (!(xadMasterBase=data->xadMasterBase)) return(NULL);

	SetProgressWindow(data->progress, tags);

	switch(xpi->xpi_Mode)
	{
		case XADPMODE_ERROR:
		{
			if (!data->over && xpi->xpi_Error != XADERR_SKIP)
			{
			    if (!(AsyncRequestTags(data->ipc, REQTYPE_SIMPLE, NULL, NULL, NULL,
						   AR_Window, data->source_window,
					           AR_Message, xadGetErrorText(xpi->xpi_Error),
						   AR_Button, GetString(locale, MSG_RETRY),
					           AR_Button, GetString(locale, MSG_ABORT),
						   TAG_END)))
		            {
				data->over=TRUE;
		            }
			    else
				data->retry = TRUE;
			}
			break;
		}
		
		case XADPMODE_ASK:
		{
			if (xpi->xpi_Status & XADPIF_OVERWRITE)
			{
				if (data->All == TRUE)
				{
				    ret |= XADPIF_OVERWRITE;
				    break;
				}
				else if (data->All == FALSE)
				{
				    ret |= XADPIF_SKIP;
				    break;
				}

				sprintf(data->buf, GetString(locale, MSG_EXISTS_FORM), FilePart(xpi->xpi_FileName));


				if (!(rc = AsyncRequestTags(data->ipc, REQTYPE_SIMPLE, NULL, NULL, NULL,
							    AR_Window, data->source_window,
							    AR_Message, data->buf,
							    AR_Button, GetString(locale, MSG_REPLACE),
							    AR_Button, GetString(locale, MSG_REPLACE_ALL),
							    AR_Button, GetString(locale, MSG_SKIP),
							    AR_Button, GetString(locale, MSG_SKIP_ALL),
							    AR_Button, GetString(locale, MSG_ABORT),
							    TAG_END)))
				{
				    data->over = TRUE;
				}
				else
				{
					if (rc <= 2)
					{
					    ret |= XADPIF_OVERWRITE;
					    if (rc == 2) data->All = TRUE;
					}
					else
					if (rc >= 3)
					{
					    ret |= XADPIF_SKIP;
					    if (rc == 4) data->All = FALSE;
					}
				}
			}
			if (xpi->xpi_Status & XADPIF_ISDIRECTORY)
			{
				sprintf(data->buf, GetString(locale, MSG_ERROR_DEST_IS_DIRECTORY), FilePart(xpi->xpi_FileName));

				if (!(AsyncRequestTags(data->ipc, REQTYPE_SIMPLE, NULL, NULL, NULL,
						       AR_Window, data->source_window,
						       AR_Message, data->buf,
						       AR_Button, GetString(locale, MSG_ERROR_DEST_IS_DIRECTORY),
						       AR_Button, GetString(locale, MSG_SKIP),
						       AR_Button, GetString(locale, MSG_ABORT),
						       TAG_END)))
				{
				    data->over = TRUE;
				}
				else
				    ret |= XADPIF_SKIP;
			}
			break;
		}
	}

	return ret;
}

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

#define CATCOMP_BLOCK
#include "galileo.strings"
#include "/Library/galileofmbase.h"

#define LocaleBase locale->li_LocaleBase

void init_locale_data(struct GalileoLocale *locale)
{
	// Initialise, get pointer to built-in strings
	locale->li_Catalog=0;
	locale->li_BuiltIn=(char *)CatCompBlock;

	// Try to open locale library
	if (locale->li_LocaleBase=OpenLibrary("locale.library",38))
	{
		// Try for a catalog
		locale->li_Catalog=OpenCatalogA(0,"galileo.catalog",0);

		// Get default locale
		locale->li_Locale=OpenLocale(0);
	}
}

void free_locale_data(struct GalileoLocale *locale)
{
	if (locale)
	{
		if (locale->li_LocaleBase)
		{
			CloseLocale(locale->li_Locale);
			CloseCatalog(locale->li_Catalog);
			CloseLibrary(locale->li_LocaleBase);
		}
	}
}

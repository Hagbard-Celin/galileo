/*
  XADbrowser.gfmmodule 1.22 - Galileo plugin to browse through XAD archives
  Copyright (C) 1999,2000 Mladen Milinkovic <mladen.milinkovic@ri.tel.hr>

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

#define CATCOMP_BLOCK
#include "XADbrowser.strings"
#include "//Library/galileofmbase.h"
//#include <galileofm/locale.h>

// This module includes all the locale strings,
// and initialises a pointer to them.

void init_locale_data(struct GalileoLocale *locale)
{
	locale->li_BuiltIn = (char *)CatCompBlock;
}


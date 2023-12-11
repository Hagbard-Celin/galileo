#define CATCOMP_BLOCK
#include "font.strings"
#include "//Library/galileofmbase.h"

void init_locale_data(struct GalileoLocale *locale)
{
	locale->li_BuiltIn=(char *)CatCompBlock;
}

#define CATCOMP_BLOCK
#include "viewfont.strings"
#include <galileofm/locale.h>

void init_locale_data(struct GalileoLocale *locale)
{
	locale->li_BuiltIn=(char *)CatCompBlock;
}

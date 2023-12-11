#define CATCOMP_BLOCK
#include "example.strings"
#include <galileofm/locale.h>

// This module includes all the locale strings,
// and initialises a pointer to them.
void init_locale_data(struct GalileoLocale *locale)
{
	locale->li_BuiltIn=(char *)CatCompBlock;
}

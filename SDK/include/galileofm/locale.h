#ifndef _GALILEOFM_LOCALE
#define _GALILEOFM_LOCALE

/*****************************************************************************

 Locale

 *****************************************************************************/

// Locale marker
struct GalileoLocale
{
	APTR		li_LocaleBase;
	APTR		li_Catalog;
	char		*li_BuiltIn;
	struct Locale	*li_Locale;
};

STRPTR GalileoGetString(struct GalileoLocale *,LONG);

#endif

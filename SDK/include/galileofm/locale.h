#ifndef _GALILEOFM_LOCALE
#define _GALILEOFM_LOCALE

/*****************************************************************************

 Locale

 *****************************************************************************/

#define	    LOCALE_DATE_SUBST	(1<<0)
#define	    LOCALE_CLOCK_12HOUR	(1<<1)
#define	    LOCALE_1000SEP	(1<<2)

// Locale marker
struct GalileoLocale
{
	APTR		li_LocaleBase;
	APTR		li_Catalog;
	char		*li_BuiltIn;
	struct Locale	*li_Locale;
};

STRPTR GalileoGetString(struct GalileoLocale *,LONG);
UWORD GetLocaleSettings(UBYTE);

#endif

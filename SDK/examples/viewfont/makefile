# Makefile for the icon clock

# Standard modules and libraries
#DEBUGLIB    =   lib:debug.lib
DEBUGLIB    =
STDOBJS     =   


# Files for this program
VIEWFONTOBJS   =   viewfont.o viewfont_data.o viewfont_strings.o


# Compiler options
CCOPTS = 
#CCOPTS      =   DEBUG=LINE

#########################################################################

all: 		viewfont_strings.o ViewFont

#########################################################################

# This will create the string file
viewfont_strings.o : viewfont.strings
	setdate viewfont_strings.c
	sc viewfont_strings.c

#########################################################################

ViewFont: $(STDOBJS) $(VIEWFONTOBJS)
	slink with <<
from lib:c.o $(STDOBJS) $(VIEWFONTOBJS)
to $@
lib lib:sc.lib lib:amiga.lib $(DEBUGLIB) lib:galileofm.lib
noicons
sc sd 
nd
<

#########################################################################

.c.o:
	sc $(CCOPTS) $*.c
.asm.o:
	sc:c/asm -iASMINC: $*.asm
.cd.strings:
	catcomp descriptor=$*.cd cfile=$*.strings

#########################################################################

clean:
	delete ~(\#?_strings).o quiet
	setdate \#?.cd

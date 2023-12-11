# Makefile for a sample Galileo module

# This is based on the structure of the real Galileo makefile, but may be
# adapted as you see fit

# Standard modules and libraries
#DEBUGLIB    =   lib:debug.lib
DEBUGLIB    =
STDOBJS     =   


# Files for this module
EXAMPLEOBJS   =   example.o example_data.o example_strings.o
EXAMPLEVER    =   1
EXAMPLEREV    =   0


# Compiler options
CCOPTS = 
#CCOPTS      =   DEBUG=LINE

#########################################################################

all: 		example.strings example.module

#########################################################################

# This will create the string file
example_strings.o : example.strings
	setdate example_strings.c
	sc example_strings.c

#########################################################################

example.module: $(STDOBJS) $(EXAMPLEOBJS)
	slink with <<
libprefix _L_
libfd modules.fd
from lib:libent.o lib:libinit.o lib:modinit.o $(STDOBJS) $(EXAMPLEOBJS)
to $@
lib lib:sc.lib lib:amiga.lib $(DEBUGLIB) lib:galileofmlib.lib
libversion $(EXAMPLEVER)
librevision $(EXAMPLEREV)
noicons
sc sd 
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

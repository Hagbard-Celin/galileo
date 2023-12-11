# Makefile for a sample Galileo module

# This is based on the structure of the real Galileo makefile, but may be
# adapted as you see fit

# Standard modules and libraries
#DEBUGLIB    =   lib:debug.lib
DEBUGLIB    =
STDOBJS     =   


# Files for this module
ICONCLOCKOBJS   =   iconclock.o iconclock_data.o iconclock_strings.o
ICONCLOCKVER    =   1
ICONCLOCKREV    =   0


# Compiler options
CCOPTS = 
#CCOPTS      =   DEBUG=LINE

#########################################################################

all: 		iconclock.strings iconclock.module

#########################################################################

# This will create the string file
iconclock_strings.o : iconclock.strings
	setdate iconclock_strings.c
	sc iconclock_strings.c

#########################################################################

iconclock.module: $(STDOBJS) $(ICONCLOCKOBJS)
	slink with <<
libprefix _L_
libfd modules.fd
from lib:libent.o lib:libinit.o lib:modinit.o $(STDOBJS) $(ICONCLOCKOBJS)
to $@
lib lib:sc.lib lib:amiga.lib $(DEBUGLIB) lib:galileofmlib.lib
libversion $(ICONCLOCKVER)
librevision $(ICONCLOCKREV)
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

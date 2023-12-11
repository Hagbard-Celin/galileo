# Makefile for a sample Galileo module

# This is based on the structure of the real Galileo makefile, but may be
# adapted as you see fit

# Standard modules and libraries
#DEBUGLIB    =   lib:debug.lib
DEBUGLIB    =
STDOBJS     =   


# Files for this module
ENVOYOBJS   =   envoy.o envoy_data.o envoy_strings.o
ENVOYVER    =   1
ENVOYREV    =   0


# Compiler options
CCOPTS = 
#CCOPTS      =   DEBUG=LINE


#########################################################################

all: 		envoy.strings envoy.module

#########################################################################

# This will create the string file
envoy_strings.o : envoy.strings
	setdate envoy_strings.c
	sc envoy_strings.c

#########################################################################

envoy.module: $(STDOBJS) $(ENVOYOBJS)
	slink with <<
libprefix _L_
libfd modules.fd
from lib:libent.o lib:libinit.o lib:modinit.o $(STDOBJS) $(ENVOYOBJS)
to $@
lib lib:sc.lib lib:amiga.lib $(DEBUGLIB) lib:galileofmlib.lib
libversion $(ENVOYVER)
librevision $(ENVOYREV)
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

##########################################################################
#   File:       SAudioTool.make
#   Target:     SAudioTool
#   Sources:    SAudioTool.c
#
#	Copyright � 1993-4 The 3DO Company
#
# 	All rights reserved. This material constitutes confidential and proprietary 
#	information of the 3DO Company and shall not be used by any Person or for any 
#	purpose except as expressly authorized in writing by the 3DO Company.
#
#
#	History:
#	05/31/94	DLD		Fixed some dependency problems
#						for the ToolKit directory structure.
##########################################################################

#  If you wish to build this tool into it's own directory, then
#		you should comment out the next two lines, instead
#		of the following two lines.

Destination	= :::
COPYTODESTINATION	= Move -y {PROGRAM} "{Destination}"{PROGRAM}
#Destination		= :
#COPYTODESTINATION	= 

PROGRAM				= SAudioTool

OBJECTS = SAudioTool.c.o
#DEBUG_FLAGS = -sym full
DEBUG_FLAGS = 


{PROGRAM}		� "{Destination}"{PROGRAM}
"{Destination}"{PROGRAM} � {PROGRAM}.make {PROGRAM}.r {OBJECTS}
	Rez  -o {PROGRAM} {PROGRAM}.r -a -ov
	Link -d -c 'MPS ' -t MPST �
		{DEBUG_FLAGS} �
		{OBJECTS} �
		#"{CLibraries}"CSANELib.o �
		#"{CLibraries}"Math.o �
		#"{CLibraries}"Complex.o �
		"{CLibraries}"StdClib.o �
		"{Libraries}"Stubs.o �
		"{Libraries}"Runtime.o �
		"{Libraries}"Interface.o �
		"{Libraries}"ToolLibs.o �
		-o {PROGRAM}
	{COPYTODESTINATION}

{PROGRAM}.c.o � {PROGRAM}.make {PROGRAM}.c {PROGRAM}.h
	 C {DEBUG_FLAGS} -r  {PROGRAM}.c

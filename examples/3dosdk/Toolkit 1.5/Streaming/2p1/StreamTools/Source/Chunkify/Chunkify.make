##########################################################################
#   File:       Chunkify.make
#   Target:     Chunkify
#   Sources:    Chunkify.c
#   Created:    June 16, 1993
#
#	Copyright � 1993-4 The 3DO Company
#
# 	All rights reserved. This material constitutes confidential and proprietary 
#	information of the 3DO Company and shall not be used by any Person or for any 
#	purpose except as expressly authorized in writing by the 3DO Company.
#
#
#	History:
#	08/17/94	dtc		Linked with a new set of objectfiles
#						to make it work with MPW 3.4a4.
#	05/27/94	DLD		Fixed some dependency problems
#						for the ToolKit directory structure.
##########################################################################

#  If you wish to build this tool into it's own directory, then
#		you should comment out the next two lines, instead
#		of the following two lines.

Destination	= :::
COPYTODESTINATION	= Move -y {PROGRAM} "{Destination}"{PROGRAM}
#Destination		= :
#COPYTODESTINATION	= 

PROGRAM			= Chunkify
OBJECTS			= {PROGRAM}.c.o
#DEBUGOPTIONS	= -sym full 
DEBUGOPTIONS	=

{PROGRAM}		� "{Destination}"{PROGRAM}
"{Destination}"{PROGRAM} � {PROGRAM}.make {PROGRAM}.r {OBJECTS}
	Rez  -o {PROGRAM} {PROGRAM}.r -a -ov
	Link {DEBUGOPTIONS} -d -c 'MPS ' -t MPST �
		{OBJECTS} �
		"{CLibraries}StdCLib.o" �
		"{Libraries}Interface.o" �
		"{Libraries}ToolLibs.o" �
		"{Libraries}MacRuntime.o" �
		"{Libraries}"IntEnv.o �
		-o {PROGRAM}
	{COPYTODESTINATION}

{PROGRAM}.c.o � {PROGRAM}.make {PROGRAM}.c
	 C {DEBUGOPTIONS} -r {PROGRAM}.c

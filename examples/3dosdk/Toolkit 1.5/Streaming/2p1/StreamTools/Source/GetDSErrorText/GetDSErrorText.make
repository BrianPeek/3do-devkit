##########################################################################
#   File:       GetDSErrorText.make
#   Target:     GetDSErrorText
#   Sources:    GetDSErrorText.c DataStreamDebug.c
#   Created:    Monday, November 15, 1993 6:54:31 PM
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

PROGRAM				= GetDSErrorText

OBJECTS = {PROGRAM}.c.o

{PROGRAM}		� "{Destination}"{PROGRAM}
"{Destination}"{PROGRAM} � {PROGRAM}.make {PROGRAM}.r {OBJECTS}
	Rez  -o {PROGRAM} {PROGRAM}.r -a -ov
	Link -d -c 'MPS ' -t MPST �
		{OBJECTS} �
		"{CLibraries}StdCLib.o" �
		"{Libraries}Interface.o" �
		"{Libraries}MacRuntime.o" �
		"{Libraries}"IntEnv.o �
		-o {PROGRAM}
	{COPYTODESTINATION}

{PROGRAM}.c.o � {PROGRAM}.make {PROGRAM}.c
	 C -r  {PROGRAM}.c

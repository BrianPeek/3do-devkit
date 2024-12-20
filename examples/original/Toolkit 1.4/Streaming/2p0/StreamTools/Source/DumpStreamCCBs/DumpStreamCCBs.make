##########################################################################
#   File:       DumpStreamCCBs.make
#   Target:     DumpStreamCCBs
#   Sources:    DumpStreamCCBs.c
#   Created:    9/14/93
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

PROGRAM				= DumpStreamCCBs

OBJECTS			= {PROGRAM}.c.o
DEBUGOPTIONS	= 
#DEBUGOPTIONS	= -sym full


{PROGRAM}		� "{Destination}"{PROGRAM}
"{Destination}"{PROGRAM} � {PROGRAM}.make {PROGRAM}.r {OBJECTS}
	Rez  -o {PROGRAM} {PROGRAM}.r -a -ov
	Link {DEBUGOPTIONS} -d -c 'MPS ' -t MPST �
		{OBJECTS} �
		"{CLibraries}"StdClib.o �
		"{Libraries}"Stubs.o �
		"{Libraries}"Runtime.o �
		"{Libraries}"Interface.o �
		"{Libraries}"ToolLibs.o �
		-o {PROGRAM}
	{COPYTODESTINATION}

{PROGRAM}.c.o � {PROGRAM}.make {PROGRAM}.c �
			PortableSANMDefs.h �
			WeaveStream.h
	 C {DEBUGOPTIONS} -r  {PROGRAM}.c

#
#	File:		UFO.make
#
#	Contains:	make file for UFO example
#
#	Written by:	Joe Buczek
#
#	Copyright:	� 1993 by The 3DO Company. All rights reserved.
#				This material constitutes confidential and proprietary
#				information of the 3DO Company and shall not be used by
#				any Person or for any purpose except as expressly
#				authorized in writing by the 3DO Company.
#
#	Change History (most recent first):
#
#		 <6>	 8/15/93	JAY		add modbin and duplicate commands to build
#		 <5>	 6/23/93	JAY		add check stack (4B1) option to COptions. Needed for Dragon
#									Release (4B1)
#		 <4>	  4/7/93	JAY		make the make consistant
#		 <3>	 3/31/93	JAY		adding input.lib to link line for 3B1
#		 <2>	 3/18/93	JAY		change Program to Application to avoid error message during make
#		 <1>	 3/18/93	JAY		first checked in
#
#	To Do:
#

#####################################
#		Symbol definitions
#####################################
Application		=	UFO
DebugFlag		=	0
ObjectDir		=	:Objects:
CC				=	armcc
ASM				=	armasm
LINK			=	armlink


#####################################
#	Default compiler options
#####################################
COptions		= -Wan -fa -zps0 -za1 -i "{3DOIncludes}" -d DEBUG={DebugFlag}

SOptions		= -bi -g -i "{3DOIncludes}"

LOptions		= -aif -r -b 0x00 -workspace 0x4000 


#####################################
#		Object files
#####################################
LIBS			=	"{3DOLibs}Lib3DO.lib"		�
					"{3DOLibs}operamath.lib"	�
					"{3DOLibs}filesystem.lib"	�
					"{3DOLibs}graphics.lib"		�
					"{3DOLibs}audio.lib"		�
					"{3DOLibs}music.lib"		�
					"{3DOLibs}input.lib"			�
					"{3DOLibs}clib.lib"			�
					"{3DOLibs}swi.lib"

# NOTE: Add object files here...
OBJECTS			=	"{ObjectDir}{Application}.c.o"


#####################################
#	Default build rules
#####################################
All				�	{Application}

{ObjectDir}		�	:

.c.o			�	.c
	{CC} {COptions} -o {TargDir}{Default}.c.o {DepDir}{Default}.c

.s.o			�	.s
	{ASM} {SOptions} -o {TargDir}{Default}.s.o {DepDir}{Default}.s


#####################################
#	Target build rules
#####################################
{Application}		��	{Application}.make {LIBS} {OBJECTS}
	{LINK}	{LOptions}					�
			-o {Application}				�
			"{3DOLibs}cstartup.o"		�
			{OBJECTS}					�
			{LIBS}
	SetFile {Application} -c 'EaDJ' -t 'PROJ'
	modbin "{Application}" -stack 0x4000
	stripaif {Application} -o {Application}
	duplicate "{Application}" "{Application}".sym "{3DORemote}" {3DOAutodup}


#####################################
#	Include file dependencies
#####################################
{Application}.c		�	"{3DOLibs}"Lib3DO.lib UFO.make
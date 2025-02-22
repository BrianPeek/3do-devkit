#
#	File:		PlayBgndMusic.make
#
#	Contains:	make file for JumpStart example
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
#		 <6>	 8/15/93	JAY		add modbin to build
#		 <5>	 7/30/93	JAY		add operamath.lib to link line for Dragon Tail release.
#		 <4>	 6/25/93	dsm		Increased stack from 0x600 to 0x1000
#		 <3>	 6/23/93	JAY		add stack checking (zps0) for Dragon (4B1)
#		 <2>	 3/18/93	JAY		change Program to Application to avoid error messages in Make
#		 <1>	 3/18/93	JAY		first checked in
#
#	To Do:
#

#####################################
#		Symbol definitions
#####################################
Application		=	PlayBgndMusic
DebugFlag		=	1
3DORelease		=	3B1
ObjectDir		=	:Objects:
CC				=	armcc
ASM				=	armasm
LINK			=	armlink


#####################################
#	Default compiler options
#####################################
CDebugOptions	= -g
COptions		= {CDebugOptions} -fa -zps0 -za1 -i "{3DOIncludes}" -d DEBUG={DebugFlag}

SOptions		= -bi -g -i "{3DOIncludes}"

LOptions		= -aif -r -b 0x00 -workspace 0x4000 


#####################################
#		Object files
#####################################
LIBS			=	"{3DOLibs}music.lib"		�
					"{3DOLibs}operamath.lib"	�
					"{3DOLibs}filesystem.lib"	�
					"{3DOLibs}audio.lib"		�
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
{Application}		��	{Application}.make {OBJECTS}
	{LINK}	{LOptions}					�
			-o {Application}				�
			"{3DOLibs}cstartup.o"		�
			{OBJECTS}					�
			{LIBS}
	SetFile {Application} -c 'EaDJ' -t 'PROJ'
	modbin {Application} -stack 0x4000
	Duplicate {Application} {Application}.task {3DOAutodup}

#####################################
#	Include file dependencies
#####################################
#{Application}.c		�
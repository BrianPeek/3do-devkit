#
#	File:		BounceSound.make
#
#	Contains:	make file for building Bounce with Sound
#
#	Written by:	Gregg Gorsiski and Darren Gibbs
#
#	Copyright:	� 1993 by The 3DO Company. All rights reserved.
#				This material constitutes confidential and proprietary
#				information of the 3DO Company and shall not be used by
#				any Person or for any purpose except as expressly
#				authorized in writing by the 3DO Company.
#
#	Change History (most recent first):
#
#		 <3>	 8/15/93	JAY		add modbin to build
#		 <2>	 6/14/93	JAY		added stack checking option (zps0) to COptions. Needed for
#									Dragon Release (4B1)
#		 <1>	  4/7/93	JAY		Use to be called Bounce.make.  Renamed to bring in line with
#									other examples.
#		 <2>	  4/3/93	JAY		change target in duplicate to something more generic and fooled
#									around with Application
#		 <1>	  4/3/93	JAY		first checked in
#
#	To Do:
#

#####################################
#		Symbol definitions
#####################################
Application		=	PALBounceSound
DebugFlag		=	1
ObjectDir		=	:Objects:
CC				=	armcc
ASM				=	armasm
LINK			=	armlink

CDebugOptions	= -g		# turn on symbolic information
#CDebugOptions	=			# turn off symbolic information
COptions		= {CDebugOptions} -zps0 -za1 -i "{3DOIncludes}" -d DEBUG={DebugFlag}

SOptions		= -bi -g -i "{3DOIncludes}"

LDebugOptions	= -d		# turn on symbolic information
#LDebugOptions	=			# turn off symbolic information
LOptions		= -aif -r -b 0x00 {LDebugOptions} -workspace 40000


#####################################
#		Object files
#####################################
LIBS			=	"{3DOLibs}Lib3DO.lib"		�
					"{3DOLibs}operamath.lib"	�
					"{3DOLibs}graphics.lib"	�
					"{3DOLibs}audio.lib"		�
					"{3DOLibs}filesystem.lib"		�
					"{3DOLibs}input.lib"		�
					"{3DOLibs}clib.lib"		�
					"{3DOLibs}swi.lib"

# NOTE: Add object files here...
OBJECTS			=	"{ObjectDir}PALbounce.c.o" �
					"{ObjectDir}PALbounce_sound.c.o"

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
			-o "{Application}"				�
			"{3DOLibs}cstartup.o"		�
			{OBJECTS}					�
			{LIBS}
	SetFile "{Application}" -c 'EaDJ' -t 'PROJ'
	modbin "{Application}" -stack 40000 -debug
	stripaif "{Application}" -o "{Application}"
	duplicate "{Application}" "{Application}".sym "{3DORemote}" {3DOAutodup}
	
#####################################
#	Include file dependencies
#####################################
PALbounce.c		�	PALbounce.h 
PALbounce_sound.c	� 	PALbounce_sound.h 

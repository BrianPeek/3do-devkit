##########################################################################
#   File:       NuPlayer.make
#
#	Contains:	make file for building NuPlayer
#
#	Copyright � 1993 The 3DO Company
#
# 	All rights reserved. This material constitutes confidential and proprietary 
#	information of the 3DO Company and shall not be used by any Person or for any 
#	purpose except as expressly authorized in writing by the 3DO Company.
#
#
#	History:
#	11/12/93	jb		Add JoyPad.c
#	10/20/93	jb		Move codec.lib above clib.lib so it can get to
#						malloc() and free()
#	9/20/93		jb		New today
##########################################################################

#####################################
#		Symbol definitions
#####################################
Program			=	NuPlayer
DebugFlag		=	1
#DebugFlag		=	0
ObjectDir		=	:Objects:
StreamDir		=	:::DataStream:
SubscriberDir	=	{StreamDir}Subscribers:

CC				=	armcc
ASM				=	armasm
LINK			=	armlink
				
#####################################
#	Default compiler options
#####################################
#COptions		= -g -zps0 -za1 -i "{3DOIncludes}" -i "{StreamDir}" -i "{SubscriberDir}" -d DEBUG={DebugFlag}
COptions		=    -zps0 -za1 -i "{3DOIncludes}" -i "{StreamDir}" -i "{SubscriberDir}" -d DEBUG={DebugFlag}

#LOptions		= -aif -r -b 0x00 -d
LOptions		= -aif -r -b 0x00

SOptions		= -bi -g -i "{3DOIncludes}"

#####################################
#		Object files
#####################################

LIBS			=	�
					"{SubscriberDir}subscriber.lib"		�
					"{StreamDir}dataacq.lib"	�
					"{StreamDir}ds.lib"			�
					"{3DOLibs}codec.lib" �
					"{3DOLibs}Lib3DO.lib"		�
					"{3DOLibs}operamath.lib"	�
					"{3DOLibs}filesystem.lib"	�
					"{3DOLibs}graphics.lib"		�
					"{3DOLibs}audio.lib"		�
					"{3DOLibs}music.lib"		�
					"{3DOLibs}input.lib"			�
					"{3DOLibs}clib.lib"			�
					"{3DOLibs}swi.lib"

OBJECTS			=	"{ObjectDir}{Program}.c.o"	�
					"{ObjectDir}PrepareStream.c.o"	�
					"{ObjectDir}PlayCPakStream.c.o"	�
					"{ObjectDir}JoyPad.c.o"

#####################################
#	Default build rules
#####################################
All				�	{Program}

{ObjectDir}		�	:

.c.o			�	.c
	{CC} {DepDir}{Default}.c -o {TargDir}{Default}.c.o  {COptions}

.s.o			�	.s
	{ASM} {SOptions} -o {TargDir}{Default}.s.o {DepDir}{Default}.s


#####################################
#	Target build rules
#####################################
{Program}		�	{Program}.make {OBJECTS} {LIBS}
	{LINK}	{LOptions}					�
			-o {Program}				�
			"{3DOLibs}cstartup.o"		�
			{OBJECTS}					�
			{LIBS}
	SetFile {Program} -c 'EaDJ' -t 'PROJ'
	modbin {program} -stack 0x4000 -debug

#####################################
#	Include file dependencies
#####################################
{Program}.c.o		�	{Program}.c  "{Program}.h"

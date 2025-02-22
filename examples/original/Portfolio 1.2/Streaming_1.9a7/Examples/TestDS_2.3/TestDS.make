##########################################################################
#   File:       TestDS.make
#
#	Contains:	make file for building TestDS
#
#	Copyright � 1993 The 3DO Company
#
# 	All rights reserved. This material constitutes confidential and proprietary 
#	information of the 3DO Company and shall not be used by any Person or for any 
#	purpose except as expressly authorized in writing by the 3DO Company.
#
#
#	History:
#	8/25/93		jb		Switch to using modbin tool to set stack & debug
#	8/10/93		jb		Add Extend_AIF command as required in Dragontail4
#	8/9/93		jb		Add SetDebugInit command as required in Dragontail3
#	7/24/93		jb		Get rid of 'H' in 'DSH'
#	6/18/93		jb		Move 'subscriber.lib' to first library in list
#						so silly linker will load references to Lib3DO.lib
#	6/15/93		jb		Update for release directory structure
#	4/17/93		jb		New today.
##########################################################################

#####################################
#		Symbol definitions
#####################################
Program			=	TestDS
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
COptions		=    -zps0 -za1 -i "{3DOIncludes}" -i "{StreamDir}" -i "{SubscriberDir}" -d DEBUG={DebugFlag}
#COptions		= -g -zps0 -za1 -i "{3DOIncludes}" -i "{StreamDir}" -i "{SubscriberDir}" -d DEBUG={DebugFlag}

LOptions		= -aif -r -b 0x00
#LOptions		= -aif -r -b 0x00 -d

SOptions		= -bi -g -i "{3DOIncludes}"

#####################################
#		Object files
#####################################

LIBS			=	�
					"{SubscriberDir}subscriber.lib"		�
					"{3DOLibs}Lib3DO.lib"		�
					"{3DOLibs}input.lib"		�
					"{3DOLibs}graphics.lib"		�
					"{3DOLibs}audio.lib"		�
					"{3DOLibs}filesystem.lib"	�
					"{3DOLibs}music.lib"		�
					"{3DOLibs}operamath.lib"	�
					"{3DOLibs}clib.lib"			�
					"{3DOLibs}swi.lib" 			�
					"{StreamDir}dataacq.lib"	�
					"{StreamDir}ds.lib"			�
					"{SubscriberDir}codec.lib"

OBJECTS			=	"{ObjectDir}{Program}.c.o"

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

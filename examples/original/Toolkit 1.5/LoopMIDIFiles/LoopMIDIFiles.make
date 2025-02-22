##########################################################################
#   File:       LoopMIDIFiles.make
#
#	Contains:	make file for building LoopMIDIFiles
#
#	Copyright � 1994 The 3DO Company
#
# 	All rights reserved. This material constitutes confidential and proprietary 
#	information of the 3DO Company and shall not be used by any Person or for any 
#	purpose except as expressly authorized in writing by the 3DO Company.
#
#
#	History:
#	10/17/94	rdg		fix AppsDir to move executables to proper place
#	3/18/94		rdg		Convert from JuggleTest.make
#
##########################################################################

#####################################
#		Symbol definitions
#####################################
Program			=	LoopMIDIFiles
DebugFlag		=	1
#DebugFlag		=	0
ObjectDir		=	:Objects:
AppsDir			=	::Apps & Data:LoopMIDIFiles:

CC				=	armcc
ASM				=	armasm
LINK			=	armlink
				
#####################################
#	Default compiler options
#####################################
#COptions		=    -zps0 -za1 -i "{3DOIncludes}" -d DEBUG={DebugFlag}
COptions		= -g -zps0 -za1 -i "{3DOIncludes}" -d DEBUG={DebugFlag}

#LOptions		= -aif -r -b 0x00
LOptions		= -aif -r -b 0x00 -d

SOptions		= -bi -g -i "{3DOIncludes}"

#####################################
#		Object files
#####################################

LIBS			=	�
					"{3DOLibs}Lib3DO.lib"		�
					"{3DOLibs}input.lib"		�
					"{3DOLibs}graphics.lib"		�
					"{3DOLibs}audio.lib"		�
					"{3DOLibs}music.lib"		�
					"{3DOLibs}filesystem.lib"	�
					"{3DOLibs}operamath.lib"	�
					"{3DOLibs}clib.lib"			�
					"{3DOLibs}swi.lib" 			�

OBJECTS			=	"{ObjectDir}{Program}.c.o"	�
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
	modbin {Program} -stack 0x4000 -debug
	stripaif {Program} -o {Program}
	duplicate -y {Program} 		"{3DORemote}"
	if `exists "{Program}.sym"`
		duplicate -y {Program}.sym 	"{3DORemote}"
	end
	if not `exists "{AppsDir}"`
		newFolder "{AppsDir}"
	end
	move -y {Program} "{AppsDir}"{Program}
	if `exists "{Program}.sym"`
		move -y {Program}.sym "{AppsDir}"{Program}.sym
	end

#####################################
#	Include file dependencies
#####################################
{Program}.c.o		�	{Program}.make {Program}.c
JoyPad.c.o			�	{Program}.make JoyPad.c JoyPad.h
#####################################
##
## Copyright (C) 1995, an unpublished work by The 3DO Company. All rights reserved.
## This material contains confidential information that is the property of The 3DO Company.
## Any unauthorized duplication, disclosure or use is prohibited.
## $Id: perftest.make,v 1.6 1995/01/19 02:04:37 mattm Exp $
##
#####################################
#   File:       perftest.make
#   Target:     perftest
#   Sources:    perftest.c sprite.c
#
#   Copyright (c) 1995, The 3DO Company
#   All rights reserved.
#

#####################################
#	Symbol definitions
#####################################

App				= perftest
DebugFlag		= 1
SourceDir		= {3DOFolder}:Examples:Graphics:Performance_Test:
ObjectDir		= :Objects:
Apps_Data		= :Apps_Data:
CC				= armcc
LINK			= armlink
WorkingDisk		=
3DOAutoDup		= -y

#####################################
#	Default compiler options
#####################################

COptions			= -fa -zps0 -za1
CDebugOptions		= -g -d DEBUG={DebugFlag}
LOptions			= -aif -r -b 0x00
LStackSize			= 4096
LDebugOptions		= -d
ModbinDebugOptions	= -debug

#####################################
#	Object files
#####################################

OBJECTS			=	 {ObjectDir}perftest.c.o {ObjectDir}sprite.c.o "{3DOLibs}"cstartup.o

LIBS			=	"{3DOFolder}Examples:ExamplesLib:exampleslib.lib" �
					"{3DOLibs}lib3do.lib" 		�
					"{3DOLibs}audio.lib" 		�
					"{3DOLibs}operamath.lib" 	�
					"{3DOLibs}graphics.lib" 	�
					"{3DOLibs}input.lib" 		�
					"{3DOLibs}clib.lib"

#####################################
#	Default build rules
#####################################

All				�	{App}

{ObjectDir}		�	:

.c.o	�	.c
	{CC} -i "{3DOIncludes}" -i "{3DOFolder}Examples:ExamplesLib:"  {COptions} {CDebugOptions} -o {TargDir}{Default}.c.o {DepDir}{Default}.c

#####################################
#	Target build rules
#####################################

{App} � {App}.make {OBJECTS}
	{LINK} {LOptions} {LDebugOptions} �
		{OBJECTS} �
		{LIBS} �
		-o "{WorkingDisk}"{Targ}
	SetFile "{WorkingDisk}"{Targ} -c 'EaDJ' -t 'PROJ'
	modbin "{WorkingDisk}"{Targ} -stack {LStackSize} {ModbinDebugOptions}
	stripaif "{WorkingDisk}"{Targ} -o {Targ} -s {Targ}.sym
	move {3DOAutodup} {Targ} "{Apps_Data}"
	move {3DOAutodup} {Targ}.sym "{Apps_Data}"

#####################################
#	Additional Target Dependencies
#####################################

{ObjectDir}perftest.c.o			�	{App}.make
{ObjectDir}sprite.c.o			�	{App}.make

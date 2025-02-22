#####################################
##
## Copyright (C) 1995, an unpublished work by The 3DO Company. All rights reserved.
## This material contains confidential information that is the property of The 3DO Company.
## Any unauthorized duplication, disclosure or use is prohibited.
## $Id: eventbroker_tests.make,v 1.5 1995/01/19 01:40:18 mattm Exp $
##
#####################################
#   File:       EventBroker_Tests.make
#   Target:     cpdump focus lookie luckie maus
#   Sources:    cpdump.c focus.c lookie.c luckie.c maus.c
#
#   Copyright (c) 1995, The 3DO Company
#   All rights reserved.
#

#####################################
#	Symbol definitions
#####################################

App				= EventBroker_Tests
DebugFlag		= 1

SourceDir		= {3DOFolder}Examples:EventBroker:EventBroker_Tests:
ObjectDir		= :Objects:
ExecutableDir	= {SourceDir}Apps_Data:
TempDir			= :

CC				= armcc
LINK			= armlink

#####################################
#	Default compiler options
#####################################

COptions			= -fa -zps0 -za1
CDebugOptions		= -g -d DEBUG={DebugFlag}
LOptions			= -aif -r -b 0x00 -workspace 0x10000
LStackSize			= 6000
LDebugOptions		= -d
ModbinDebugOptions	= -debug

#####################################
#	Object files
#####################################

OBJECTS			=	{ObjectDir}cpdump.c.o		�
					{ObjectDir}focus.c.o		�
					{ObjectDir}lookie.c.o		�
					{ObjectDir}luckie.c.o		�
					{ObjectDir}maus.c.o
					
LIBS			=	"{3DOLibs}Lib3DO.lib" �
					"{3DOLibs}audio.lib" �
					"{3DOLibs}music.lib" �
					"{3DOLibs}operamath.lib" �
					"{3DOLibs}filesystem.lib" �
					"{3DOLibs}graphics.lib" �
					"{3DOLibs}input.lib" �
					"{3DOLibs}clib.lib"
					
#####################################
#	Default build rules
#####################################

All				�	{App}

{ObjectDir}		�	:

.c.o	�	.c
	{CC} -i "{3DOIncludes}" {COptions} {CDebugOptions} -o {TargDir}{Default}.c.o {DepDir}{Default}.c
	{LINK} {LOptions} {LDebugOptions} �
		{TargDir}{Default}.c.o �
		"{3DOLibs}"cstartup.o �
		{LIBS} �
		-o {TempDir}{Default}.nostrip
	SetFile {TempDir}{Default}.nostrip -c 'EaDJ' -t 'PROJ'
	modbin {TempDir}{Default}.nostrip -stack {LStackSize} {ModbinDebugOptions}
	stripaif {TempDir}{Default}.nostrip -o {ExecutableDir}{Default} -s {ExecutableDir}{Default}.sym
	delete {TempDir}{Default}.nostrip

{App} � {App}.make {OBJECTS}


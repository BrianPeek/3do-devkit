#####################################
##
## Copyright (C) 1995, an unpublished work by The 3DO Company. All rights reserved.
## This material contains confidential information that is the property of The 3DO Company.
## Any unauthorized duplication, disclosure or use is prohibited.
## $Id: compression.make,v 1.5 1995/01/19 02:40:34 mattm Exp $
##
#####################################
#   File:       compressexample.make
#   Target:     compressexample
#   Sources:    compressexample.c
#
#   Copyright (c) 1995, The 3DO Company
#   All rights reserved.
#

#####################################
#	Symbol definitions
#####################################

App				= compression
DebugFlag		= 1
SourceDir		= {3DOFolder}Examples:Miscellaneous:Compression:
ObjectDir		= :Objects:
ExecutableDir	= {SourceDir}Apps_Data:
CC				= armcc
LINK			= armlink
TempDir			= :
WorkingDisk		=

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

OBJECTS			=	 {ObjectDir}compression.c.o "{3DOLibs}"cstartup.o

LIBS			=	 �
					"{3DOLibs}compression.lib" �
					"{3DOLibs}filesystem.lib" �
					"{3DOLibs}clib.lib"

#####################################
#	Default build rules
#####################################

All				�	{App}

{ObjectDir}		�	:

.c.o	�	.c
	{CC} -i "{3DOIncludes}" {COptions} {CDebugOptions} -o {TargDir}{Default}.c.o {DepDir}{Default}.c

#####################################
#	Target build rules
#####################################

{App} � {App}.make {OBJECTS}
	{LINK} {LOptions} �
		{OBJECTS} �
		{LIBS} �
		-o "{WorkingDisk}"{Targ}.nostrip
	SetFile "{WorkingDisk}"{Targ}.nostrip -c 'EaDJ' -t 'PROJ'
	modbin "{WorkingDisk}"{Targ}.nostrip -stack {LStackSize} {ModbinDebugOptions}
	stripaif "{WorkingDisk}"{Targ}.nostrip -o {ExecutableDir}{Targ} -s {ExecutableDir}{Targ}.sym
	delete "{WorkingDisk}"{Targ}.nostrip

#####################################
#	Additional Target Dependencies
#####################################

{ObjectDir}compression.c.o			�	{App}.make

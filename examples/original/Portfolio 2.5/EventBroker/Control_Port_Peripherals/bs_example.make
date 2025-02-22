#####################################
##
## Copyright (C) 1995, an unpublished work by The 3DO Company. All rights reserved.
## This material contains confidential information that is the property of The 3DO Company.
## Any unauthorized duplication, disclosure or use is prohibited.
## $Id: bs_example.make,v 1.5 1995/01/19 01:24:29 mattm Exp $
##
#####################################
#   File:       bs_example.make
#   Target:     bs_example
#   Sources:    broker_shell.c bs_cpad.c bs_joystick.c bs_lgun.c bs_mouse.c main.c
#
#   Copyright (c) 1995, The 3DO Company
#   All rights reserved.
#

#####################################
#	Symbol definitions
#####################################

App				= bs_example
DebugFlag		= 1
SourceDir		= {3DOFolder}Examples:EventBroker:Control_Port_Peripherals:
ObjectDir		= :Objects:
Apps_Data		= :Apps_Data:
CC				= armcc
LINK			= armlink
WorkingDisk		=
3DOAutodup		= -y

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

OBJECTS			=	 �
					{ObjectDir}broker_shell.c.o �
					{ObjectDir}bs_cpad.c.o �
					{ObjectDir}bs_joystick.c.o �
					{ObjectDir}bs_mouse.c.o �
					{ObjectDir}bs_example.c.o �
					"{3DOLibs}"cstartup.o

LIBS			=	 �
					"{3DOLibs}Lib3DO.lib" �
					"{3DOLibs}filesystem.lib" �
					"{3DOLibs}graphics.lib" �
					"{3DOLibs}input.lib" �
					"{3DOLibs}clib.lib" �

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

{ObjectDir}broker_shell.c.o		�	{App}.make
{ObjectDir}bs_cpad.c.o			�	{App}.make
{ObjectDir}bs_joystick.c.o		�	{App}.make
{ObjectDir}bs_lgun.c.o			�	{App}.make
{ObjectDir}bs_mouse.c.o			�	{App}.make
{ObjectDir}bs_example.c.o		�	{App}.make


#
#	File:		memtest.make
#
#	Contains:	make for building memtest
#
#	Written by:	Dave Maynard
#
#	Copyright:	� 1993 by The 3DO Company. All rights reserved.
#				This material constitutes confidential and proprietary
#				information of the 3DO Company and shall not be used by
#				any Person or for any purpose except as expressly
#				authorized in writing by the 3DO Company.
#
#	Change History (most recent first):
#
#		 <1>	 8/20/93	JAY		first checked in
#
#	To Do:
#

#####################################
#		Symbol definitions
#####################################
Application		=	memtest
DebugFlag		=	1
ObjectDir		=	:Objects:
CC				=	armcc
ASM				=	armasm
LINK			=	armlink


#####################################
#	Default compiler options
#####################################
# USE THE FOLLOWING LINE FOR SYMBOLIC DEBUGGING
CDebugOptions	= -g
# USE THE FOLLOWING LINE FOR OPTIMIZED CODE
#CDebugOptions	=

COptions		= {CDebugOptions} -zps0 -za1 -i "{3DOIncludes}" -d DEBUG={DebugFlag}

SOptions		= -bi -g -i "{3DOIncludes}"

# USE THE FOLLOWING LINE FOR SYMBOLIC DEBUGGING
LDebugOptions	= -d		# turn on symbolic information
# USE THE FOLLOWING LINE FOR OPTIMIZED CODE
#LDebugOptions	=			# turn off symbolic information

LOptions		= {LDebugOptions} -aif -r -b 0x00 -workspace 4096 

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
OBJECTS			=	"{ObjectDir}greentest.c.o"�
					"{ObjectDir}gmemtest.s.o"	

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
	modbin {Application} -stack 0x400 -debug
	stripaif {Application} -o {Application}
	duplicate {Application} "{3DORemote}" {3DOAutodup}
	duplicate {Application}.sym "{3DORemote}" {3DOAutodup}
	
#####################################
#	Include file dependencies
#####################################
#{Application}.c		�	{Application}.h 

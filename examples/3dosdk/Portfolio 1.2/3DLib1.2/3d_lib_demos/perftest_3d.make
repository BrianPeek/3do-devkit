##########################################################################
#   File:       perftest_3d.make
#
#	Contains:	
#
#	History:
##########################################################################

# Remember to set 3DOLibs and 3DOIncludes in your MPW "Startup" file

#####################################
#		Symbol definitions
#####################################
Program			=	perftest_3d
DebugFlag		=	0
ObjectDir		=	:objects:
CC				=	armcc
ASM				=	armasm
LINK			=	armlink


#####################################
#	Default compiler options
#####################################
COptions		= -fa -zps0 -za1 -i "{3DOIncludes}" -d DEBUG={DebugFlag} 

SOptions		= -bi -g -i "{3DOIncludes}"

LOptions		=  -d -aif -r -b 0x00 -workspace 0x400 


#####################################
#		Object files
#####################################
LIBS			=	"{3DOLibs}3dlib.lib"		�
					"{3DOLibs}graphics.lib"		�
					"{3DOLibs}Lib3DO.lib"		�
					"{3DOLibs}filesystem.lib"	�
					"{3DOLibs}input.lib"		�
					"{3DOLibs}operamath.lib"	�
					"{3DOLibs}swi.lib"			�
					"{3DOLibs}clib.lib"			�
					"{3DOLibs}audio.lib"

# NOTE: Add object files here...
OBJECTS			=	"{ObjectDir}perftest_core_3d.c.o"	�
					"{ObjectDir}perftest_stuff_3d.c.o" �
					"{ObjectDir}framecounthack.c.o"

#####################################
#	Default build rules
#####################################
All				�	{Program}

{ObjectDir}		�	:

.c.o			�	.c
	{CC} {COptions} -o {TargDir}{Default}.c.o {DepDir}{Default}.c

.s.o			�	.s
	{ASM} {SOptions} -o {TargDir}{Default}.s.o {DepDir}{Default}.s


#####################################
#	Target build rules
#####################################
{Program}		��	{Program}.make {OBJECTS}
	{LINK}	{LOptions}					�
			-o {Program}				�
			"{3DOLibs}cstartup.o"		�
			{OBJECTS}					�
			{LIBS}
	SetFile {Program} -c 'EaDJ' -t 'PROJ'
	echo modbinning {Program}
	modbin {Program} -stack 10000 -debug


#####################################
#	Include file dependencies
#####################################
{Program}.c		�  perftest_3d.make
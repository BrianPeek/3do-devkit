##########################################################################
#   File:       TestSA.make
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
#	06/17/94	dtc		Removed link dependency on AppsDir and created
#						'Apps & Data' folder if it doesn't exists.
#						Added stripaif.
#	06/09/94	dtc		Defined RelativeBranchSwitch compile switch
#	05/27/94	DLD		Fixed some dependency problems
#						for the ToolKit directory structure.
#	05/16/94	dtc		Move application to 'Apps & Data:'
#	3/15/94		MPH		Changed location of codec.lib.  Corrected strange link order.
#	9/14/93		rdg		converted from TestDS
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
Program			=	TestSA
DebugFlag		=	1
#DebugFlag		=	0
AppsDir			=	:Apps & Data:
ObjectDir		=	:Objects:
StreamDir		=	:::DataStream:
SubscriberDir	=	{StreamDir}Subscribers:
AudioSubsDir	= 	{SubscriberDir}SAudioSubscriber:

CC				=	armcc
ASM				=	armasm
LINK			=	armlink
				
#####################################
#	Default compiler options
#####################################
RelativeBranchSwitch = -dRELATIVE_BRANCHING=0

#COptions		=    -zps0 -za1 {RelativeBranchSwitch} -i "{3DOIncludes}" -i "{StreamDir}" -i "{SubscriberDir}" -i "{AudioSubsDir}" -d DEBUG={DebugFlag}
COptions		= -g -zps0 -za1 {RelativeBranchSwitch} -i "{3DOIncludes}" -i "{StreamDir}" -i "{SubscriberDir}" -i "{AudioSubsDir}" -d DEBUG={DebugFlag}

#LOptions		= -aif -r -b 0x00
LOptions		= -aif -r -b 0x00 -d

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
					"{3DOLibs}input.lib"		�
					"{3DOLibs}clib.lib"			�
					"{3DOLibs}swi.lib"

OBJECTS			=	"{ObjectDir}{Program}.c.o"

#####################################
#	Default build rules
#####################################
All				�	{Program}

{ObjectDir}		�	:

.c.o			�	.c
	{CC} {DepDir}{Default}.c -o {ObjectDir}{Default}.c.o  {COptions}

.s.o			�	.s
	{ASM} {SOptions} -o {ObjectDir}{Default}.s.o {DepDir}{Default}.s


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
	stripaif {Program} -o {Program}
	if not `exists "{AppsDir}"`
		newFolder "{AppsDir}"
	end
	move -y {Program} "{AppsDir}"{Program}

#####################################
#	Include file dependencies
#####################################

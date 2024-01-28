##########################################################################
#   File:       ShuttlePlayer.make
#
#	Contains:	make file for building ShuttlePlayer
#
#	Copyright � 1993 The 3DO Company
#
# 	All rights reserved. This material constitutes confidential and proprietary 
#	information of the 3DO Company and shall not be used by any Person or for any 
#	purpose except as expressly authorized in writing by the 3DO Company.
#
#
#	History:
#
#	08/22/94	dtc		Re-generated dependencies list.
#	08/18/94	dtc		Add auto-generation of .c.o dependencies
#	08/17/94	dtc		Fixed compile option to look at Includes folder
#						in SubscriberDir for Subscribers header files.
#	 6/30/94	dtc		Turned DebugFlag off.
#	06/23/94	dtc		Removed multiply defined RelativeBranchSwitch
#	06/17/94	dtc		Removed link dependency on AppsDir and created
#						'Apps & Data' folder if it doesn't exists.
#						Added stripaif.
#	06/09/94	dtc		Defined RelativeBranchSwitch compile switch.
#						Fixed to look for codec.lib in the {3DOLibs}
#						instead of {SubscriberDir}.
#	11/12/93	jb		Add JoyPad.c
#	10/20/93	jb		Move codec.lib above clib.lib so it can get to
#						malloc() and free()
#	9/20/93		jb		New today
#
#	To regenerate the .c.o -> .h file dependencies, get write access to this
#	make file and execute the following MPW code:
#	make Depends -f ShuttlePlayer.make  �� Dev:Null > temp.makeout; temp.makeout �� Dev:Null; delete -i temp.makeout
#
##########################################################################

#####################################
#		Symbol definitions
#####################################
Program			=	ShuttlePlayer
#DebugFlag		=	1
DebugFlag		=	0
AppsDir			=	:Apps & Data:
ObjectDir		=	:Objects:
StreamDir		=	:::DataStream:
SubscriberDir	=	{StreamDir}Subscribers:

CC				=	armcc
ASM				=	armasm
LINK			=	armlink

MakeFileName		=	{Program}.make
RelativeBranchSwitch = -dRELATIVE_BRANCHING=1
				
#####################################
#	Default compiler options
#####################################

#COptions		= -g -zps0 -za1 {RelativeBranchSwitch} -i "{3DOIncludes}" -i "{StreamDir}" -i "{SubscriberDir}Includes:" -d DEBUG={DebugFlag}
COptions		=    -zps0 -za1 {RelativeBranchSwitch} -i "{3DOIncludes}" -i "{StreamDir}" -i "{SubscriberDir}Includes:" -d DEBUG={DebugFlag}

#LOptions		= -aif -r -b 0x00 -d
LOptions		= -aif -r -b 0x00

SOptions		= -bi -g -i "{3DOIncludes}"

#####################################
#		Object files
#####################################

LIBS			=	�
					"{SubscriberDir}subscriberShuttle.lib"		�
					"{StreamDir}dataacqShuttle.lib"	�
					"{StreamDir}dsShuttle.lib"	�
					"{3DOLibs}Lib3DO.lib"		�
					"{3DOLibs}input.lib"		�
					"{3DOLibs}graphics.lib"		�
					"{3DOLibs}audio.lib"		�
					"{3DOLibs}filesystem.lib"	�
					"{3DOLibs}music.lib"		�
					"{3DOLibs}operamath.lib"	�
					"{3DOLibs}codec.lib"		�
					"{3DOLibs}clib.lib"			�
					"{3DOLibs}swi.lib" 			

OBJECTS			=	"{ObjectDir}{Program}.c.o"		�
					"{ObjectDir}PlayCPakStream.c.o"	�
					"{ObjectDir}PrepareStream.c.o"	�
					"{ObjectDir}PlaySSNDStream.c.o"	�
					"{ObjectDir}JoyPad.c.o"			�
					"{ObjectDir}UserControls.c.o"	�
					"{ObjectDir}Screens.c.o"		�
					"{ObjectDir}elkabong.c.o"		�
					"{ObjectDir}Menus.c.o"

OBJECTDEPENDS		=	"{ObjectDir}{Program}.c.depends"	�
					"{ObjectDir}PlayCPakStream.c.depends"	�
					"{ObjectDir}PrepareStream.c.depends"	�
					"{ObjectDir}PlaySSNDStream.c.depends"	�
					"{ObjectDir}JoyPad.c.depends"			�
					"{ObjectDir}UserControls.c.depends"		�
					"{ObjectDir}Screens.c.depends"			�
					"{ObjectDir}elkabong.c.depends"			�
					"{ObjectDir}Menus.c.depends"

#####################################
#	Default build rules
#####################################
All				�	{Program}

{ObjectDir}		�	:

.c.o			�	.c
	{CC} {DepDir}{Default}.c -o {TargDir}{Default}.c.o  {COptions}

.c.depends		�	.c
	{CC} {COptions} -o {TargDir}{Default}.c.o {DepDir}{Default}.c -M -c �
		| search -q -r "{3DOIncludes}" �
		| StreamEdit -e "1 delete; 1,$ Replace /{ObjectDir}/ '�"�{ObjectDir�}�"'; Replace /�/ '	�'" �
		>> "{MakeFileName}"

.s.o			�	.s
	{ASM} {SOptions} -o {TargDir}{Default}.s.o {DepDir}{Default}.s

#####################################
#	Dependency re-building rules
#	The .c.depends rule asks the compiler to generate source file dependencies, then
#	removes the first line (.c.o dependency on .c), substitutes a symbolic reference
#	to "{ObjectDir}", puts in a tab before the �s, and appends the result to this make
#	file. The following rules setup and sequence the work.
#
#	HOW TO USE IT: Get write access to this make file then make "depends".
#	This will replace the include file dependencies lines at the end of this makefile.
#####################################
Depends					�	DeleteOldDependencies {ObjectDepends} SaveNewMakefile

DeleteOldDependencies	�
	# This is a workaround to make it work with the latest version of Make Tool (MPW V3.4a4).
	# Without the next line, find /.../ will break (MakeFileName) isn't resolved.
	set MakeFileName "{MakeFileName}"
	Open "{MakeFileName}"
	Find � "{MakeFileName}"
	Find /�#�tInclude file dependencies �(Don�t change this line or put anything after this section.�)�/ "{MakeFileName}"
	Find /�[�#]/  "{MakeFileName}"
	Replace Ƥ:� "�n" "{MakeFileName}"

SaveNewMakefile			�
	Save "{MakeFileName}"

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
#	Include file dependencies (Don�t change this line or put anything after this section.)
#####################################

"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:Subscribers:Includes:CPakSubscriberS.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:DataStreamLib.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:DataStream.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:MsgUtils.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:MemPool.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:SubsChunkCommon.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:HaltChunk.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:DSStreamHeader.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:SubsChunkCommon.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:Subscribers:Includes:codec.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:PlayCPakStream.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:Subscribers:Includes:CPakSubscriberS.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:PrepareStream.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:Subscribers:Includes:CPakSubscriberS.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:DataStreamLib.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:DataAcq.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:ItemPool.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:MarkerChunk.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:Subscribers:Includes:SAudioSubscriber.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:DataStream.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:Subscribers:Includes:SAStreamChunks.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:Subscribers:Includes:SAChannel.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:MemPool.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:Subscribers:Includes:SATemplates.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:Subscribers:Includes:SAStreamChunks.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:Subscribers:Includes:SAControlMsgs.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:Subscribers:Includes:ControlSubscriber.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:ItemPool.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:DataStreamDebug.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:PlayCPakStream.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:Subscribers:Includes:CPakSubscriberS.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:::DataStream:DataStreamDebug.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:ShuttlePlayer.h
"{ObjectDir}"ShuttlePlayer.c.o	�	:joypad.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:Subscribers:Includes:CPakSubscriberS.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:DataStreamLib.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:DataStream.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:MsgUtils.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:MemPool.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:SubsChunkCommon.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:HaltChunk.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:DSStreamHeader.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:SubsChunkCommon.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:Subscribers:Includes:codec.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:DataStreamDebug.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:DataStreamLib.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:DataAcq.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:ItemPool.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:MarkerChunk.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:Subscribers:Includes:SAudioSubscriber.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:DataStream.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:Subscribers:Includes:SAStreamChunks.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:Subscribers:Includes:SAChannel.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:MemPool.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:Subscribers:Includes:SATemplates.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:Subscribers:Includes:SAStreamChunks.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:Subscribers:Includes:SAControlMsgs.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:Subscribers:Includes:ControlSubscriber.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:ItemPool.h
"{ObjectDir}"PlayCPakStream.c.o	�	:ShuttlePlayer.h
"{ObjectDir}"PlayCPakStream.c.o	�	:PlayCPakStream.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:Subscribers:Includes:CPakSubscriberS.h
"{ObjectDir}"PlayCPakStream.c.o	�	:PrepareStream.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:Subscribers:Includes:CPakSubscriberS.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:DataStreamLib.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:DataAcq.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:Subscribers:Includes:SAudioSubscriber.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:Subscribers:Includes:ControlSubscriber.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:DataStreamDebug.h
"{ObjectDir}"PlayCPakStream.c.o	�	:PlayCPakStream.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:Subscribers:Includes:CPakSubscriberS.h
"{ObjectDir}"PlayCPakStream.c.o	�	:::DataStream:DSStreamHeader.h
"{ObjectDir}"PrepareStream.c.o	�	:PrepareStream.h
"{ObjectDir}"PrepareStream.c.o	�	:::DataStream:Subscribers:Includes:CPakSubscriberS.h
"{ObjectDir}"PrepareStream.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"PrepareStream.c.o	�	:::DataStream:DataStreamLib.h
"{ObjectDir}"PrepareStream.c.o	�	:::DataStream:DataStream.h
"{ObjectDir}"PrepareStream.c.o	�	:::DataStream:MsgUtils.h
"{ObjectDir}"PrepareStream.c.o	�	:::DataStream:MemPool.h
"{ObjectDir}"PrepareStream.c.o	�	:::DataStream:SubsChunkCommon.h
"{ObjectDir}"PrepareStream.c.o	�	:::DataStream:HaltChunk.h
"{ObjectDir}"PrepareStream.c.o	�	:::DataStream:DSStreamHeader.h
"{ObjectDir}"PrepareStream.c.o	�	:::DataStream:SubsChunkCommon.h
"{ObjectDir}"PrepareStream.c.o	�	:::DataStream:Subscribers:Includes:codec.h
"{ObjectDir}"PrepareStream.c.o	�	:::DataStream:DataStreamLib.h
"{ObjectDir}"PrepareStream.c.o	�	:::DataStream:DataStreamDebug.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:DataStreamDebug.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:DataStream.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:MsgUtils.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:MemPool.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:SubsChunkCommon.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:HaltChunk.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:DSStreamHeader.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:DataStreamLib.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:DataStream.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:DataAcq.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:ItemPool.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:MarkerChunk.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:DataStreamLib.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:SubsChunkCommon.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:Subscribers:Includes:SAudioSubscriber.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:DataStream.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:Subscribers:Includes:SAStreamChunks.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:Subscribers:Includes:SAChannel.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:MemPool.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:Subscribers:Includes:SATemplates.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:Subscribers:Includes:SAStreamChunks.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:Subscribers:Includes:SAControlMsgs.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:Subscribers:Includes:ControlSubscriber.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:ItemPool.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:PlaySSNDStream.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:PrepareStream.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:Subscribers:Includes:CPakSubscriberS.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:Subscribers:Includes:codec.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:DataStreamLib.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:DataAcq.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:Subscribers:Includes:SAudioSubscriber.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:Subscribers:Includes:ControlSubscriber.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:DataStreamDebug.h
"{ObjectDir}"PlaySSNDStream.c.o	�	:::DataStream:DSStreamHeader.h
"{ObjectDir}"JoyPad.c.o	�	:JoyPad.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:DataStreamDebug.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:DataStream.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:MsgUtils.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:MemPool.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:SubsChunkCommon.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:HaltChunk.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:DSStreamHeader.h
"{ObjectDir}"UserControls.c.o	�	:PlayCPakStream.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:Subscribers:Includes:CPakSubscriberS.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:DataStreamLib.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:DataStream.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:SubsChunkCommon.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:Subscribers:Includes:codec.h
"{ObjectDir}"UserControls.c.o	�	:PrepareStream.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:Subscribers:Includes:CPakSubscriberS.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:DataStreamLib.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:DataAcq.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:ItemPool.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:MarkerChunk.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:Subscribers:Includes:SAudioSubscriber.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:DataStream.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:Subscribers:Includes:SAStreamChunks.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:Subscribers:Includes:SAChannel.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:MemPool.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:Subscribers:Includes:SATemplates.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:Subscribers:Includes:SAStreamChunks.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:Subscribers:Includes:SAControlMsgs.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:Subscribers:Includes:ControlSubscriber.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:ItemPool.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:DataStreamDebug.h
"{ObjectDir}"UserControls.c.o	�	:PlayCPakStream.h
"{ObjectDir}"UserControls.c.o	�	:::DataStream:Subscribers:Includes:CPakSubscriberS.h
"{ObjectDir}"UserControls.c.o	�	:ShuttlePlayer.h
"{ObjectDir}"UserControls.c.o	�	:joypad.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:DataStreamDebug.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:DataStream.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:MsgUtils.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:MemPool.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:SubsChunkCommon.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:HaltChunk.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:DSStreamHeader.h
"{ObjectDir}"Screens.c.o	�	:PlayCPakStream.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:Subscribers:Includes:CPakSubscriberS.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:DataStreamLib.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:DataStream.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:SubsChunkCommon.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:Subscribers:Includes:codec.h
"{ObjectDir}"Screens.c.o	�	:PrepareStream.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:Subscribers:Includes:CPakSubscriberS.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:DataStreamLib.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:DataAcq.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:ItemPool.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:MarkerChunk.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:Subscribers:Includes:SAudioSubscriber.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:DataStream.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:Subscribers:Includes:SAStreamChunks.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:Subscribers:Includes:SAChannel.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:MemPool.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:Subscribers:Includes:SATemplates.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:Subscribers:Includes:SAStreamChunks.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:Subscribers:Includes:SAControlMsgs.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:Subscribers:Includes:ControlSubscriber.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:ItemPool.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:DataStreamDebug.h
"{ObjectDir}"Screens.c.o	�	:PlayCPakStream.h
"{ObjectDir}"Screens.c.o	�	:::DataStream:Subscribers:Includes:CPakSubscriberS.h
"{ObjectDir}"Screens.c.o	�	:ShuttlePlayer.h
"{ObjectDir}"Screens.c.o	�	:joypad.h
"{ObjectDir}"Menus.c.o	�	:ShuttlePlayer.h
"{ObjectDir}"Menus.c.o	�	:joypad.h

##########################################################################
#   File:       SlideStream.make
#
#	Contains:	make file for building SlideStream
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
#	08/18/94	dtc		Add auto-generation of .c.o dependencies
#	08/17/94	dtc		Fixed compile option to look at Includes folder
#						in SubscriberDir for Subscribers header files.
#	 6/30/94	dtc		Turned DebugFlag off.
#	06/17/94	dtc		Removed link dependency on AppsDir and created
#						'Apps & Data' folder if it doesn't exists.
#						Added stripaif.
#	06/09/94	dtc		Defined RelativeBranchSwitch compile switch
#	05/27/94	DLD		Fixed some dependency problems
#						for the ToolKit directory structure.
#	05/16/94	dtc		Move application to 'Apps & Data:'
#	3/15/94		MPH 	Corrected link order to standard.  Updated (removed) reference to codec.lib
#	10/20/93	jb		Move codec.lib above clib.lib so it can get to
#						malloc() and free()
#	9/20/93		jb		New today
#
#	To regenerate the .c.o -> .h file dependencies, get write access to this
#	make file and execute the following MPW code:
#	make Depends -f SlideStream.make  �� Dev:Null > temp.makeout; temp.makeout �� Dev:Null; delete -i temp.makeout
#
##########################################################################

#####################################
#		Symbol definitions
#####################################
Program			=	SlideStream
#DebugFlag		=	1
DebugFlag		=	0
AppsDir			=	:Apps & Data:
ObjectDir		=	:Objects:
StreamDir		=	:::DataStream:
SubscriberDir	=	{StreamDir}Subscribers:

CC				=	armcc
ASM				=	armasm
LINK			=	armlink
MakeFileName	=	{Program}.make
				
#####################################
#	Default compiler options
#####################################
RelativeBranchSwitch = -dRELATIVE_BRANCHING=0

COptions		=    -zps0 -za1 {RelativeBranchSwitch} -i "{3DOIncludes}" -i "{StreamDir}" -i "{SubscriberDir}Includes:" -d DEBUG={DebugFlag}
#COptions		= -g -zps0 -za1 {RelativeBranchSwitch} -i "{3DOIncludes}" -i "{StreamDir}" -i "{SubscriberDir}Includes:" -d DEBUG={DebugFlag}

LOptions		= -aif -r -b 0x00
#LOptions		= -aif -r -b 0x00 -d

SOptions		= -bi -g -i "{3DOIncludes}"

#####################################
#		Object files
#####################################

LIBS			=	�
					"{SubscriberDir}subscriber.lib"		�
					"{StreamDir}dataacq.lib"	�
					"{StreamDir}ds.lib"			�
##					"{3DOLibs}codec.lib" �
					"{3DOLibs}Lib3DO.lib"		�
					"{3DOLibs}operamath.lib"	�
					"{3DOLibs}filesystem.lib"	�
					"{3DOLibs}graphics.lib"		�
					"{3DOLibs}audio.lib"		�
					"{3DOLibs}music.lib"		�
					"{3DOLibs}input.lib"			�
					"{3DOLibs}clib.lib"			�
					"{3DOLibs}swi.lib"

OBJECTS			=	"{ObjectDir}{Program}.c.o"	�
					"{ObjectDir}PrepareStream.c.o"	�
					"{ObjectDir}PlayImageStream.c.o"

OBJECTDEPENDS		=	"{ObjectDir}{Program}.c.depends"	�
					"{ObjectDir}PrepareStream.c.depends"	�
					"{ObjectDir}PlayImageStream.c.depends"

#####################################
#	Default build rules
#####################################
All				�	{Program}

{ObjectDir}		�	:

.c.o			�	.c
	{CC} {DepDir}{Default}.c -o {ObjectDir}{Default}.c.o  {COptions}

.c.depends		�	.c
	{CC} {COptions} -o {TargDir}{Default}.c.o {DepDir}{Default}.c -M -c �
		| search -q -r "{3DOIncludes}" �
		| StreamEdit -e "1 delete; 1,$ Replace /{ObjectDir}/ '�"�{ObjectDir�}�"'; Replace /�/ '	�'" �
		>> "{MakeFileName}"

.s.o			�	.s
	{ASM} {SOptions} -o {ObjectDir}{Default}.s.o {DepDir}{Default}.s

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

"{ObjectDir}"SlideStream.c.o	�	:PlayImageStream.h
"{ObjectDir}"SlideStream.c.o	�	:PrepareStream.h
"{ObjectDir}"SlideStream.c.o	�	:::DataStream:DataStreamLib.h
"{ObjectDir}"SlideStream.c.o	�	:::DataStream:DataStream.h
"{ObjectDir}"SlideStream.c.o	�	:::DataStream:MsgUtils.h
"{ObjectDir}"SlideStream.c.o	�	:::DataStream:MemPool.h
"{ObjectDir}"SlideStream.c.o	�	:::DataStream:SubsChunkCommon.h
"{ObjectDir}"SlideStream.c.o	�	:::DataStream:HaltChunk.h
"{ObjectDir}"SlideStream.c.o	�	:::DataStream:DSStreamHeader.h
"{ObjectDir}"SlideStream.c.o	�	:::DataStream:DataAcq.h
"{ObjectDir}"SlideStream.c.o	�	:::DataStream:ItemPool.h
"{ObjectDir}"SlideStream.c.o	�	:::DataStream:MarkerChunk.h
"{ObjectDir}"SlideStream.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"SlideStream.c.o	�	:::DataStream:DataStreamLib.h
"{ObjectDir}"SlideStream.c.o	�	:::DataStream:SubsChunkCommon.h
"{ObjectDir}"SlideStream.c.o	�	:::DataStream:Subscribers:Includes:SAudioSubscriber.h
"{ObjectDir}"SlideStream.c.o	�	:::DataStream:DataStream.h
"{ObjectDir}"SlideStream.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"SlideStream.c.o	�	:::DataStream:Subscribers:Includes:SAStreamChunks.h
"{ObjectDir}"SlideStream.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"SlideStream.c.o	�	:::DataStream:Subscribers:Includes:SAChannel.h
"{ObjectDir}"SlideStream.c.o	�	:::DataStream:MemPool.h
"{ObjectDir}"SlideStream.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"SlideStream.c.o	�	:::DataStream:Subscribers:Includes:SATemplates.h
"{ObjectDir}"SlideStream.c.o	�	:::DataStream:Subscribers:Includes:SAStreamChunks.h
"{ObjectDir}"SlideStream.c.o	�	:::DataStream:Subscribers:Includes:SAControlMsgs.h
"{ObjectDir}"SlideStream.c.o	�	:::DataStream:Subscribers:Includes:ControlSubscriber.h
"{ObjectDir}"SlideStream.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"SlideStream.c.o	�	:::DataStream:ItemPool.h
"{ObjectDir}"SlideStream.c.o	�	:::DataStream:Subscribers:Includes:JoinSubscriber.h
"{ObjectDir}"SlideStream.c.o	�	:::DataStream:DataStreamLib.h
"{ObjectDir}"SlideStream.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"SlideStream.c.o	�	:::DataStream:DataStreamDebug.h
"{ObjectDir}"PrepareStream.c.o	�	:PrepareStream.h
"{ObjectDir}"PrepareStream.c.o	�	:::DataStream:DataStreamLib.h
"{ObjectDir}"PrepareStream.c.o	�	:::DataStream:DataStream.h
"{ObjectDir}"PrepareStream.c.o	�	:::DataStream:MsgUtils.h
"{ObjectDir}"PrepareStream.c.o	�	:::DataStream:MemPool.h
"{ObjectDir}"PrepareStream.c.o	�	:::DataStream:SubsChunkCommon.h
"{ObjectDir}"PrepareStream.c.o	�	:::DataStream:HaltChunk.h
"{ObjectDir}"PrepareStream.c.o	�	:::DataStream:DSStreamHeader.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:DataStreamDebug.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:DataStream.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:MsgUtils.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:MemPool.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:SubsChunkCommon.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:HaltChunk.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:DSStreamHeader.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:DataStreamLib.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:DataStream.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:DataAcq.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:ItemPool.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:MarkerChunk.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:DataStreamLib.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:SubsChunkCommon.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:Subscribers:Includes:SAudioSubscriber.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:DataStream.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:Subscribers:Includes:SAStreamChunks.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:Subscribers:Includes:SAChannel.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:MemPool.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:Subscribers:Includes:SATemplates.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:Subscribers:Includes:SAStreamChunks.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:Subscribers:Includes:SAControlMsgs.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:Subscribers:Includes:ControlSubscriber.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:ItemPool.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:Subscribers:Includes:JoinSubscriber.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:DataStreamLib.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:Subscribers:Includes:SubscriberUtils.h
"{ObjectDir}"PlayImageStream.c.o	�	:PlayImageStream.h
"{ObjectDir}"PlayImageStream.c.o	�	:PrepareStream.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:DataStreamLib.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:DataAcq.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:Subscribers:Includes:SAudioSubscriber.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:Subscribers:Includes:ControlSubscriber.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:Subscribers:Includes:JoinSubscriber.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:DataStreamDebug.h
"{ObjectDir}"PlayImageStream.c.o	�	:::DataStream:DSStreamHeader.h

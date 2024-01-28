##########################################################################
#   File:       SAudioSubscriber.make
#
#	Contains:	makefile for building the SAudioSubscriber
#
#	Copyright � 1994 The 3DO Company
#
# 	All rights reserved. This material constitutes confidential and proprietary 
#	information of the 3DO Company and shall not be used by any Person or for any 
#	purpose except as expressly authorized in writing by the 3DO Company.
#
#
#	History:
#	 8/15/94	dtc		Add auto-generation of .c.o dependencies
#	6/24/94		rdg		Let each subscriber have its own makefile...
#	6/23/94		rdg		Rename audio trace switches, they must be unique so
#						that other subscriber trace switches can be defined here
#	6/9/94		fyp		Define RelativeBranch switch here instead of at source.
#	6/8/94		fyp		Define SAudio trace switches here instead of in source.
#	4/04/94		rdg		Add ProtoSubscriber and SubscriberTraceUtils 
#	10/19/93	jb		Change to new audio subscriber file organization 
#						with all ".c" source in a separate directory.
#	6/23/93		jb		Turn on stack checking for Dragon O/S release
#	6/17/93		jb		Add JoinSubscriber.c
#	09/06/93	akm		New today.
#
#	To regenerate the .c.o -> .h file dependencies, get write access to this
#	make file and execute the following MPW code:
#	make Depends -f SAudioSubscriber.make �� Dev:Null > temp.makeout; temp.makeout �� Dev:Null; delete -i temp.makeout
#
##########################################################################

#####################################
#		Symbol definitions
#####################################
Program			=	SAudioSubscriber
DebugFlag		=	0
#DebugFlag		=	1
SubscriberDir	=	::
StreamDir		=	:::
CC				=	armcc
ASM				=	armasm

ObjectDir		=	{SubscriberDir}Objects:

#Objects			=	SAMain.c.o SAChannel.c.o SAFolioInterface.c.o SATemplates.c.o
#Includes		=	SAChannel.h SAControlMsgs.h SAErrors.h SAFolioInterface.h SAStreamChunks.h �
#						SATemplates.h SAudioSubscriber.h SAudioTraceCodes.h

MakeFileName	=	{Program}.make

#####################################
# Trace switches
#
# XXXXX_TRACE_MAIN causes the given subscriber to leave timestamped trace data in a 
# circular buffer which can be examined using the debugger, or dumped with
# XXXXX_DUMP_TRACE_ON_STREAM_CLOSE - which dumps the buffer whenever a StreamClosing
# message is received.
#
# Don't forget link with the trace code utilities if you want to use them..
#
#####################################
 
AudioTraceSwitches  = -dSAUDIO_TRACE_MAIN=0  -dSAUDIO_TRACE_BUFFERS=0 -dSAUDIO_TRACE_CHANNELS=0 �
					-dSAUDIO_TRACE_TEMPLATES=0 -dSAUDIO_DUMP_TRACE_ON_STREAM_CLOSE=0 �
					-dSAUDIO_DUMP_TRACE_ON_STREAM_ABORT=0 -dSAUDIO_DUMP_TRACE_ON_BUFFER_COMP_ERR=0

#####################################
#	Default compiler options
#####################################
COptions		=	{GlobalCOptions}  -dDEBUG={DebugFlag} -i ":" -i "{StreamDir}" �
					-i "{SubscriberDir}SubscriberUtilities:"

#####################################
#	Object files
#	Be sure to keep these two definitions in synch!
#####################################

OBJECTS			=	"{ObjectDir}SAMain.c.o"				�
					"{ObjectDir}SAChannel.c.o"			�
					"{ObjectDir}SAFolioInterface.c.o"	�
					"{ObjectDir}SATemplates.c.o"

OBJECTDEPENDS	=	"{ObjectDir}SAMain.c.depends"			�
					"{ObjectDir}SAChannel.c.depends"		�
					"{ObjectDir}SAFolioInterface.c.depends"	�
					"{ObjectDir}SATemplates.c.depends"

#####################################
#	Default build rules
#####################################
All				�	{OBJECTS}

{ObjectDir}		�	:

.c.o			�	.c
	echo "	compiling {Default}.c with {GlobalCOptions} {RelBranchSwitch}"
	{CC} {COptions} -o {ObjectDir}{Default}.c.o  {Default}.c {RelBranchSwitch} {AudioTraceSwitches}
	
.c.depends		�	.c
	{CC} {COptions} -o {TargDir}{Default}.c.o {DepDir}{Default}.c -M -c �
		| search -q -r "{3DOIncludes}" �
		| StreamEdit -e "1 delete; 1,$ Replace /{ObjectDir}/ '�"�{ObjectDir�}�"'; Replace /�/ '	�'" �
		>> "{MakeFileName}"

#####################################
#	Dependency re-building rules
#	The .c.depends rule asks the compiler to generate source file dependencies, then
#	removes the first line (.c.o dependency on .c), substitutes a symbolic reference
#	to "{ObjectDir}", puts in a tab before the �s, and appends the result to this make
#	file. The following rules setup and sequence the work.
#
#	HOW TO USE IT: Get write access to this make file then make "Depends".
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
	Find /�[�#]/ "{MakeFileName}"
	Replace Ƥ:� "�n" "{MakeFileName}"

SaveNewMakefile			�
	Save "{MakeFileName}"
	
#####################################
#	make or build script Dependancies
#####################################
# Artifical target to force build of all subscriber object files
{Program}		�	{Objects}

# Target dependancy to rebuild when makefile or build script changes
#{ObjectDir}SAMain.c.o				�	{Program}.make {SubscriberDir}BuildSubscriberLib

#{ObjectDir}SAChannel.c.o			�	{Program}.make {SubscriberDir}BuildSubscriberLib

#{ObjectDir}SAFolioInterface.c.o	�	{Program}.make {SubscriberDir}BuildSubscriberLib
#{ObjectDir}SATemplates.c.o			�	 {Program}.make {SubscriberDir}BuildSubscriberLib
{Objects}					�	{Program}.make {SubscriberDir}BuildSubscriberLib

#####################################
#	Include file dependencies (Don�t change this line or put anything after this section.)
#####################################

"{ObjectDir}"SAMain.c.o	�	:::DataStreamLib.h
"{ObjectDir}"SAMain.c.o	�	:::DataStream.h
"{ObjectDir}"SAMain.c.o	�	:::MsgUtils.h
"{ObjectDir}"SAMain.c.o	�	:::MemPool.h
"{ObjectDir}"SAMain.c.o	�	:::SubsChunkCommon.h
"{ObjectDir}"SAMain.c.o	�	:::HaltChunk.h
"{ObjectDir}"SAMain.c.o	�	:::DSStreamHeader.h
"{ObjectDir}"SAMain.c.o	�	:::MsgUtils.h
"{ObjectDir}"SAMain.c.o	�	:::MemPool.h
"{ObjectDir}"SAMain.c.o	�	:::ThreadHelper.h
"{ObjectDir}"SAMain.c.o	�	:::MakeName.h
"{ObjectDir}"SAMain.c.o	�	:SAErrors.h
"{ObjectDir}"SAMain.c.o	�	:SATemplates.h
"{ObjectDir}"SAMain.c.o	�	:SAStreamChunks.h
"{ObjectDir}"SAMain.c.o	�	::SubscriberUtilities:SubscriberUtils.h
"{ObjectDir}"SAMain.c.o	�	:::DataStreamLib.h
"{ObjectDir}"SAMain.c.o	�	:::SubsChunkCommon.h
"{ObjectDir}"SAMain.c.o	�	:SAControlMsgs.h
"{ObjectDir}"SAMain.c.o	�	:SAFolioInterface.h
"{ObjectDir}"SAMain.c.o	�	::SubscriberUtilities:SubscriberUtils.h
"{ObjectDir}"SAMain.c.o	�	:SAChannel.h
"{ObjectDir}"SAMain.c.o	�	:::MemPool.h
"{ObjectDir}"SAMain.c.o	�	::SubscriberUtilities:SubscriberUtils.h
"{ObjectDir}"SAMain.c.o	�	:SAudioSubscriber.h
"{ObjectDir}"SAMain.c.o	�	:::DataStream.h
"{ObjectDir}"SAMain.c.o	�	::SubscriberUtilities:SubscriberUtils.h
"{ObjectDir}"SAMain.c.o	�	:SAStreamChunks.h
"{ObjectDir}"SAMain.c.o	�	:SAChannel.h
"{ObjectDir}"SAMain.c.o	�	:SATemplates.h
"{ObjectDir}"SAMain.c.o	�	:SAControlMsgs.h
"{ObjectDir}"SAMain.c.o	�	:SAudioSubscriber.h
"{ObjectDir}"SAMain.c.o	�	::SubscriberUtilities:SubscriberTraceUtils.h
"{ObjectDir}"SAMain.c.o	�	:SAudioTraceCodes.h
"{ObjectDir}"SAChannel.c.o	�	:SAErrors.h
"{ObjectDir}"SAChannel.c.o	�	:SAChannel.h
"{ObjectDir}"SAChannel.c.o	�	:::MemPool.h
"{ObjectDir}"SAChannel.c.o	�	::SubscriberUtilities:SubscriberUtils.h
"{ObjectDir}"SAChannel.c.o	�	:::DataStreamLib.h
"{ObjectDir}"SAChannel.c.o	�	:::DataStream.h
"{ObjectDir}"SAChannel.c.o	�	:::MsgUtils.h
"{ObjectDir}"SAChannel.c.o	�	:::SubsChunkCommon.h
"{ObjectDir}"SAChannel.c.o	�	:::HaltChunk.h
"{ObjectDir}"SAChannel.c.o	�	:::DSStreamHeader.h
"{ObjectDir}"SAChannel.c.o	�	:::SubsChunkCommon.h
"{ObjectDir}"SAChannel.c.o	�	:SAudioSubscriber.h
"{ObjectDir}"SAChannel.c.o	�	:::DataStream.h
"{ObjectDir}"SAChannel.c.o	�	::SubscriberUtilities:SubscriberUtils.h
"{ObjectDir}"SAChannel.c.o	�	:SAStreamChunks.h
"{ObjectDir}"SAChannel.c.o	�	::SubscriberUtilities:SubscriberUtils.h
"{ObjectDir}"SAChannel.c.o	�	:SAChannel.h
"{ObjectDir}"SAChannel.c.o	�	:SATemplates.h
"{ObjectDir}"SAChannel.c.o	�	:SAStreamChunks.h
"{ObjectDir}"SAChannel.c.o	�	:SAControlMsgs.h
"{ObjectDir}"SAChannel.c.o	�	:SAStreamChunks.h
"{ObjectDir}"SAChannel.c.o	�	:SAFolioInterface.h
"{ObjectDir}"SAChannel.c.o	�	::SubscriberUtilities:SubscriberUtils.h
"{ObjectDir}"SAChannel.c.o	�	:SAChannel.h
"{ObjectDir}"SAChannel.c.o	�	:SAudioSubscriber.h
"{ObjectDir}"SAChannel.c.o	�	::SubscriberUtilities:SubscriberTraceUtils.h
"{ObjectDir}"SAChannel.c.o	�	:SAudioTraceCodes.h
"{ObjectDir}"SAFolioInterface.c.o	�	:SAErrors.h
"{ObjectDir}"SAFolioInterface.c.o	�	:SAFolioInterface.h
"{ObjectDir}"SAFolioInterface.c.o	�	::SubscriberUtilities:SubscriberUtils.h
"{ObjectDir}"SAFolioInterface.c.o	�	:::DataStreamLib.h
"{ObjectDir}"SAFolioInterface.c.o	�	:::DataStream.h
"{ObjectDir}"SAFolioInterface.c.o	�	:::MsgUtils.h
"{ObjectDir}"SAFolioInterface.c.o	�	:::MemPool.h
"{ObjectDir}"SAFolioInterface.c.o	�	:::SubsChunkCommon.h
"{ObjectDir}"SAFolioInterface.c.o	�	:::HaltChunk.h
"{ObjectDir}"SAFolioInterface.c.o	�	:::DSStreamHeader.h
"{ObjectDir}"SAFolioInterface.c.o	�	:::SubsChunkCommon.h
"{ObjectDir}"SAFolioInterface.c.o	�	:SAChannel.h
"{ObjectDir}"SAFolioInterface.c.o	�	:::MemPool.h
"{ObjectDir}"SAFolioInterface.c.o	�	::SubscriberUtilities:SubscriberUtils.h
"{ObjectDir}"SAFolioInterface.c.o	�	:SAudioSubscriber.h
"{ObjectDir}"SAFolioInterface.c.o	�	:::DataStream.h
"{ObjectDir}"SAFolioInterface.c.o	�	::SubscriberUtilities:SubscriberUtils.h
"{ObjectDir}"SAFolioInterface.c.o	�	:SAStreamChunks.h
"{ObjectDir}"SAFolioInterface.c.o	�	::SubscriberUtilities:SubscriberUtils.h
"{ObjectDir}"SAFolioInterface.c.o	�	:SAChannel.h
"{ObjectDir}"SAFolioInterface.c.o	�	:SATemplates.h
"{ObjectDir}"SAFolioInterface.c.o	�	:SAStreamChunks.h
"{ObjectDir}"SAFolioInterface.c.o	�	:SAControlMsgs.h
"{ObjectDir}"SAFolioInterface.c.o	�	:SAudioSubscriber.h
"{ObjectDir}"SAFolioInterface.c.o	�	::SubscriberUtilities:SubscriberTraceUtils.h
"{ObjectDir}"SAFolioInterface.c.o	�	:SAudioTraceCodes.h
"{ObjectDir}"SATemplates.c.o	�	:SAErrors.h
"{ObjectDir}"SATemplates.c.o	�	:SATemplates.h
"{ObjectDir}"SATemplates.c.o	�	:SAStreamChunks.h
"{ObjectDir}"SATemplates.c.o	�	::SubscriberUtilities:SubscriberUtils.h
"{ObjectDir}"SATemplates.c.o	�	:::DataStreamLib.h
"{ObjectDir}"SATemplates.c.o	�	:::DataStream.h
"{ObjectDir}"SATemplates.c.o	�	:::MsgUtils.h
"{ObjectDir}"SATemplates.c.o	�	:::MemPool.h
"{ObjectDir}"SATemplates.c.o	�	:::SubsChunkCommon.h
"{ObjectDir}"SATemplates.c.o	�	:::HaltChunk.h
"{ObjectDir}"SATemplates.c.o	�	:::DSStreamHeader.h
"{ObjectDir}"SATemplates.c.o	�	:::SubsChunkCommon.h
"{ObjectDir}"SATemplates.c.o	�	:SAStreamChunks.h
"{ObjectDir}"SATemplates.c.o	�	:SAudioSubscriber.h
"{ObjectDir}"SATemplates.c.o	�	:::DataStream.h
"{ObjectDir}"SATemplates.c.o	�	::SubscriberUtilities:SubscriberUtils.h
"{ObjectDir}"SATemplates.c.o	�	:SAStreamChunks.h
"{ObjectDir}"SATemplates.c.o	�	:SAChannel.h
"{ObjectDir}"SATemplates.c.o	�	:::MemPool.h
"{ObjectDir}"SATemplates.c.o	�	::SubscriberUtilities:SubscriberUtils.h
"{ObjectDir}"SATemplates.c.o	�	:SATemplates.h
"{ObjectDir}"SATemplates.c.o	�	:SAControlMsgs.h
"{ObjectDir}"SATemplates.c.o	�	::SubscriberUtilities:SubscriberTraceUtils.h
"{ObjectDir}"SATemplates.c.o	�	:SAudioTraceCodes.h

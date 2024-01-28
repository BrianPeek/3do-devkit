##########################################################################
#   File:       Subscriber.lib.make
#
#	Contains:	make file for building Subscriber
#
#	Copyright � 1992 The 3DO Company
#
# 	All rights reserved. This material constitutes confidential and proprietary 
#	information of the 3DO Company and shall not be used by any Person or for any 
#	purpose except as expressly authorized in writing by the 3DO Company.
#
#
#	History:
#	6/9/94		fyp		Define RelativeBranch switch here instead of at source.
#	6/8/94		fyp		Define SAudio trace switches here instead of in source.
#	4/04/94		rdg		Add ProtoSubscriber and SubscriberTraceUtils 
#	10/19/93	jb		Change to new audio subscriber file organization 
#						with all ".c" source in a separate directory.
#	6/23/93		jb		Turn on stack checking for Dragon O/S release
#	6/17/93		jb		Add JoinSubscriber.c
#	09/06/93	akm		New today.
##########################################################################

#####################################
#		Symbol definitions
#####################################
Library			=	Subscriber
DebugFlag		=	1
StreamDir		=	::
ObjectDir		=	:Objects:
AudioDir		=	:SAudioSubscriber:
CC				=	armcc
ASM				=	armasm
LIBRARIAN		=	armlib
AudioTraceSwitch  = -dTRACE_MAIN=0  -dTRACE_BUFFERS=0 -dTRACE_CHANNELS=0 -dTRACE_TEMPLATES=0 �
					-dDUMP_TRACE_ON_STREAM_CLOSE=0 -dDUMP_TRACE_ON_STREAM_ABORT=0 -dDUMP_TRACE_ON_BUFFER_COMP_ERR=0
RelativeBranchSwitch = -dRELATIVE_BRANCHING=0

#####################################
#	Default compiler options
#####################################
COptions		= -g -zps0 -za1 -i "{3DOIncludes}" -i "{StreamDir}" -i ":" -dDEBUG={DebugFlag} {AudioTraceSwitch} {RelativeBranchSwitch}
#COptions		=    -zps0 -za1 -i "{3DOIncludes}" -i "{StreamDir}" -i ":" -dDEBUG={DebugFlag}

SOptions		= -bi -g -i "{3DOIncludes}"

LOptions		= -o

#####################################
#		Object files
#####################################
OBJECTS			=	"{ObjectDir}ControlSubscriber.c.o"		�
					"{ObjectDir}SubscriberUtils.c.o"		�
					"{ObjectDir}JoinSubscriber.c.o"			�
					"{ObjectDir}SAnimSubscriber.c.o"		�
					"{ObjectDir}CPakSubscriber.c.o"			�
					"{ObjectDir}SAMain.c.o"					�
					"{ObjectDir}SAChannel.c.o"				�
					"{ObjectDir}SAFolioInterface.c.o"		�
					"{ObjectDir}SATemplates.c.o"			�
					"{ObjectDir}SubscriberTraceUtils.c.o"	�
					"{ObjectDir}SubscriberTraceUtils.c.o"		

#####################################
#	Default build rules
#####################################
All				�	{Library}.lib

{ObjectDir}		�	: {AudioDir}

.c.o			�	.c
	{CC} {COptions} -o {ObjectDir}{Default}.c.o {DepDir}{Default}.c

.s.o			�	.s
	{ASM} {SOptions} -o {ObjectDir}{Default}.s.o {DepDir}{Default}.s

#####################################
#	Target build rules
#####################################
{Library}.lib		��	{Library}.lib.make {OBJECTS}
	{LIBRARIAN}	{LOptions}			�
				-c {Library}.lib	�
				{OBJECTS}

#####################################
#	Include file dependencies
#####################################
{ObjectDir}SubscriberUtils.c.o		�	SubscriberUtils.h
{ObjectDir}ControlSubscriber.c.o	�	ControlSubscriber.h
{ObjectDir}JoinSubscriber.c.o		�	JoinSubscriber.h
{ObjectDir}SAnimSubscriber.c.o		�	SAnimSubscriber.h
{ObjectDir}CPakSubscriber.c.o		�	CPakSubscriber.h
{ObjectDir}SubscriberTraceUtils.c.o	�	SubscriberTraceUtils.h
{AudioDir}SAMain.c.o				�	SAudioSubscriber.h SAErrors.h SATemplates.h �
										SAControlMsgs.h SAFolioInterface.h
{AudioDir}SAChannel.c.o				�	SAChannel.h SAErrors.h SAudioSubscriber.h �
										SAStreamChunks.h SAFolioInterface.h
{AudioDir}SAFolioInterface.c.o		�	SAFolioInterface.h �
										SAudioSubscriber.h SAErrors.h
{AudioDir}SATemplates.c.o			�	SATemplates.h SAErrors.h �
										SAStreamChunks.h
{AudioDir}SATrace.c.o				�	SATrace.h

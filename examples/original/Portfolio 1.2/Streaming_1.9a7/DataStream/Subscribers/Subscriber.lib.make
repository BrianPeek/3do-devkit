##########################################################################
#   File:       Subscriber.make
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

#####################################
#	Default compiler options
#####################################
#COptions		= -g -zps0 -za1 -i "{3DOIncludes}" -i "{StreamDir}" -i ":" -dDEBUG={DebugFlag}
COptions		=    -zps0 -za1 -i "{3DOIncludes}" -i "{StreamDir}" -i ":" -dDEBUG={DebugFlag}

SOptions		= -bi -g -i "{3DOIncludes}"

LOptions		= -o

#####################################
#		Object files
#####################################
OBJECTS			=	"{ObjectDir}ControlSubscriber.c.o"		�
					"{ObjectDir}CPakSubscriber.c.o"			�
					"{ObjectDir}JoinSubscriber.c.o"			�
					"{ObjectDir}SAnimSubscriber.c.o"		�
					"{ObjectDir}SAMain.c.o"					�
					"{ObjectDir}SAChannel.c.o"				�
					"{ObjectDir}SAFolioInterface.c.o"		�
					"{ObjectDir}SATemplates.c.o"			�
					"{ObjectDir}SATrace.c.o"				�
					"{ObjectDir}SubscriberUtils.c.o"

#####################################
#	Default build rules
#####################################
All				�	{Library}.lib

{ObjectDir}		�	: {AudioDir}

.c.o			�	.c
	{CC} {COptions} -o {TargDir}{Default}.c.o {DepDir}{Default}.c

.s.o			�	.s
	{ASM} {SOptions} -o {TargDir}{Default}.s.o {DepDir}{Default}.s

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
ControlSubscriber.c				�	ControlSubscriber.h
CPakSubscriber.c				�	CPakSubscriber.h
JoinSubscriber.c				�	JoinSubscriber.h
SAnimSubscriber.c				�	SAnimSubscriber.h
SubscriberUtils.c				�	SubscriberUtils.h
{AudioDir}SAMain.c				�	SAudioSubscriber.h SAErrors.h SATemplates.h �
									SAControlMsgs.h SAFolioInterface.h
{AudioDir}SAChannel.c			�	SAChannel.h SAErrors.h SAudioSubscriber.h �
									SAStreamChunks.h SAFolioInterface.h
{AudioDir}SAFolioInterface.c	�	SAFolioInterface.h �
									SAudioSubscriber.h SAErrors.h
{AudioDir}SATemplates.c			�	SATemplates.h SAErrors.h �
									SAStreamChunks.h
{AudioDir}SATrace.c				�	SATrace.h

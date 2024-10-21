#
#	File:		Lib3DO.lib.make
#
#	Contains:	Make file for Lib3DOLib
#
#	Written by:	Joe Buczek
#
#	Copyright:	� 1992 by The 3DO Company. All rights reserved.
#				This material constitutes confidential and proprietary
#				information of the 3DO Company and shall not be used by
#				any Person or for any purpose except as expressly
#				authorized in writing by the 3DO Company.
#
#	Change History (most recent first):
#
#		 <3>	  4/7/93	JAY		make the make consistant
#		 <2>	 3/18/93	JAY		turning debug on
#		 <1>	 3/17/93	JAY		This is the 2B1 release being checked in for the Cardinal build
#		<10>	 1/28/93	dsm		removed the d{threeDORelease}=1 define in the make file.
#		 <9>	  1/2/93	pgb		Modify Make to compile without symbols. Also switched include
#									order to look in local headers first.
#		 <8>	12/27/92	dsm		Modify Make to compile without symbols.
#		 <6>	12/14/92	JAY		remove -W compiler switch (suppress warnings)
#		 <5>	12/14/92	JAY		adding compiler switch threeDORelease to aid in
#									upgrading/downgrading between releases
#		 <4>	12/10/92	JML		Update for dependency on Parse3DO.h
#		 <3>	12/10/92	JML		Update for dependency on Portfolio.h
#		 <2>	12/10/92	JML		Updated to find interfaces and copy library to new location
#				 12/9/92	JML		For projector
#
#	To Do:
#

#####################################
#		Symbol definitions
#####################################
DebugFlag		=	1
ObjectDir		=	:Objects:
CC				=	armcc
ASM				=	armasm
LIBRARIAN		=	armlib

#####################################
#	Default compiler options
#####################################
CDebugOptions	= -g		# turn on symbolic information
COptions		= {CDebugOptions} -zps0 -za1 -W -i "{3DOIncludes}" -d RED_HARDWARE=0 -d DEBUG={DebugFlag}

SOptions		= -bi -g -i "{3DOIncludes}"
#SOptions		= -bi  -i "{3DOIncludes}"

LOptions		= -c -o 

#####################################
#		Object files
#####################################
OBJECTS			=	"{ObjectDir}KTextBox.c.o" �


#####################################
#	Default build rules
#####################################
All				�	KTextBox.lib

{ObjectDir}		�	:

.c.o			�	.c
	{CC} {COptions} -o {TargDir}{Default}.c.o {DepDir}{Default}.c

.s.o			�	.s
	{ASM} {SOptions} -o {TargDir}{Default}.s.o {DepDir}{Default}.s

#####################################
#	Target build rules
#####################################
KTextBox.lib		��	KTextBox.lib.make {OBJECTS}
	{LIBRARIAN}	{LOptions}		�
				KTextBox.lib		�
				{OBJECTS}
#	Duplicate -y KTextBox.lib {3DOLibs}KTextBox.lib

#####################################
#	Include file dependencies
#####################################


KTextBox.lib.c		� types.h 
					� debug.h
					� nodes.h
					� kernelnodes.h
					� list.h
					� folio.h
					� task.h
					� kernel.h
					� mem.h
					� operamath.h
					� audio.h
					� semaphore.h
					� io.h
					� strings.h
					� stdlib.h
					� graphics.h
					� filefunctions.h
					� Form3DO.h
					� Init3DO.h
					� Parse3DO.h
					� Utils3DO.h
					� KTextBox.h

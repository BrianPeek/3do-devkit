#####################################
##
## Copyright (C) 1995, an unpublished work by The 3DO Company. All rights reserved.
## This material contains confidential information that is the property of The 3DO Company.
## Any unauthorized duplication, disclosure or use is prohibited.
## $Id: Lib3DO.lib.make,v 1.4 1995/02/20 23:42:10 stan Exp $
##
#####################################
#   File:       Lib3DO.lib.make
#   Target:     Lib3DO.lib
#   Sources:
#
#   Copyright (c) 1994, The 3DO Company
#   All rights reserved.
#

#####################################
#		Symbol definitions
#####################################

Library			= 	Lib3DO.lib
DebugFlag		=	1
Version			=	1.5a12

#####################################
#	Default compiler options
#####################################

CDebugOptions	= -d DEBUG={DebugFlag} # -g

COptions		= {CDebugOptions} -zps0 -zpv1 -za1 -fa -fh -fk -i ":Includes:" -i "{3DOIncludes}"

SOptions		= -bi -g -i "{3DOIncludes}"

LOptions		= -c -o

#####################################
#		Object files
#####################################

ObjectDir		=	:Objects:

"{ObjectDir}"	�	�
					:AnimUtils:								�
					:CelUtils:								�
					:DisplayUtils:							�
					:IOUtils:								�
					:MiscUtils:								�
					:Objects:								�
					:TimerUtils:							�
					:TextLib:								�

AnimUtils =			�
					"{ObjectDir}DrawAnimCel.c.o"			�
					"{ObjectDir}GetAnimCel.c.o"				�
					"{ObjectDir}LoadAnim.c.o"				�
					"{ObjectDir}ParseAnim.c.o"				�


CelUtils =			�
					"{ObjectDir}CenterPoint.c.o"			�
					"{ObjectDir}CenterRect.c.o"				�
					"{ObjectDir}CenterRectCelIPoint.c.o"	�
					"{ObjectDir}CenterRectCelFPoint.c.o"	�
					"{ObjectDir}CenterRectCelRect.c.o"		�
					"{ObjectDir}ChainCels.c.o" 				�
					"{ObjectDir}CloneCel.c.o" 				�
					"{ObjectDir}CreateBackdropCel.c.o"	 	�
					"{ObjectDir}CreateCel.c.o" 				�
					"{ObjectDir}CreateLRFormCel.c.o" 		�
					"{ObjectDir}CreateSubrectCel.c.o" 		�
					"{ObjectDir}CrossFadeCels.c.o"			�
					"{ObjectDir}DeleteCelMagic.c.o" 		�
					"{ObjectDir}GetCelBufferSize.c.o"		�
					"{ObjectDir}InitCel.c.o"				�
					"{ObjectDir}InterUnionRect.c.o"			�
					"{ObjectDir}LinkCel.c.o"				�
					"{ObjectDir}LoadCel.c.o" 				�
					"{ObjectDir}MapCelToPoint.c.o"			�
					"{ObjectDir}MapCelToRect.c.o" 			�
					"{ObjectDir}MapCelToQuad.c.o" 			�
					"{ObjectDir}OffsetInsetRect.c.o"		�
					"{ObjectDir}OffsetCel.c.o"	 			�
					"{ObjectDir}ParseCel.c.o" 				�
					"{ObjectDir}PointConversions.c.o"		�
					"{ObjectDir}RectConversions.c.o" 		�
					"{ObjectDir}RectFromCel.c.o"			�
					"{ObjectDir}RenderCelPixel.c.o"			�


DisplayUtils =		�
					"{ObjectDir}ClearBitmap.c.o"			�
					"{ObjectDir}DrawImage.c.o"				�
					"{ObjectDir}LoadImage.c.o"				�
					"{ObjectDir}FadeFromBlack.c.o"			�
					"{ObjectDir}FadeToBlack.c.o"			�
					"{ObjectDir}CreateBasicDisplay.c.o"			�


IOUtils =			�
					"{ObjectDir}AsyncLoadFile.c.o"			�
					"{ObjectDir}BlockFile.c.o"				�
					"{ObjectDir}GetFileSize.c.o"			�
					"{ObjectDir}LoadFile.c.o"				�
					"{ObjectDir}ReadFile.c.o"				�
					"{ObjectDir}SaveFile.c.o"				�
					"{ObjectDir}WriteMacFile.c.o"			�


MiscUtils =			�
					"{ObjectDir}Debug3DO.c.o"				�
					"{ObjectDir}GetChunk.c.o"				�
					"{ObjectDir}UMemory.c.o"				�
					"{ObjectDir}MSEvents.c.o"				�


TimerUtils =			�
					"{ObjectDir}TimerServicesAPI.c.o"		�
					"{ObjectDir}TimerServicesAPIU.c.o"		�
					"{ObjectDir}TimerServicesAPIV.c.o"		�
					"{ObjectDir}TimerServicesThread.c.o"	�
					"{ObjectDir}TimerUtilsGetIOReq.c.o"		�
					"{ObjectDir}TimerUtilsGetTime.c.o"		�
					"{ObjectDir}TimerUtilsSleep.c.o"		�


TextLib =			�
					"{ObjectDir}taTextLib.c.o"				�
					"{ObjectDir}FontBlit3To8_.s.o"			�
					"{ObjectDir}FontBlit5To8_.s.o"			�
					"{ObjectDir}FontLib.c.o"				�
					"{ObjectDir}TextLib.c.o"				�


OBJECTS			=	�
					{AnimUtils}								�
					{CelUtils}								�
					{DisplayUtils}							�
					{IOUtils}								�
					{MiscUtils}								�
					{TimerUtils}							�
					{TextLib}								�


#####################################
#	Default build rules
#####################################

CC				=	armcc
ASM				=	armasm
LIBRARIAN		=	armlib

.c.o			�	.c
	{CC}  {DepDir}{Default}.c {COptions} -o {TargDir}{Default}.c.o

.s.o			�	.s
	{ASM} {DepDir}{Default}.s {SOptions} -o {TargDir}{Default}.s.o

#####################################
#	Primary target build rules
#####################################

"{Library}"		� {OBJECTS}
	{LIBRARIAN}	{LOptions} "{Library}" {OBJECTS}
	SetVersion  "{Library}" -t 'vers' -version {Version}
	Duplicate -y "{Library}" "{3DOLibs}{Library}"

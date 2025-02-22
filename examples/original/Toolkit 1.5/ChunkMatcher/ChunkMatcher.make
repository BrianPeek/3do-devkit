#	ChunkMatcher.make
#
#	Copyright � 1994 The 3DO Company. All Rights Reserved.
#
#	10/25/94	ec		move tool to 'apps & data:chunkmatcher:' when built.
#	10/11/94	ec		move tool to 'apps & data' when built.
#	9/28/94		crm		Link against ToolLib.o to get SpinCursor function
#


#####################################
#		Symbol definitions
#####################################
Program			=	ChunkMatcher
ObjectDir		=	:Objects:
CC				=	CPlus
ASM				=	
LINK			=	Link
Common			=	:
AppsDir			=	'::Apps & Data':{Program}:
StackSize		=	
StackSizeBytes	=	
DebugOption		=	-d DEBUG=1


#####################################
#	Default compiler options
#####################################
NormCOptions	= {DebugOption} -model far
COptions		= {NormCOptions}

SOptions		= 

NormLOptions	= -c "MPS " -t "MPST" -d -model far
LOptions		= {NormLOptions}


#####################################
#		Object files
#####################################

LIBS		=												�
				"{MPW}Libraries:Libraries:MacRuntime.o"		�
	 			"{MPW}Libraries:Libraries:IntEnv.o"			�
	 			"{MPW}Libraries:CLibraries:CPluslib.o"		�
				"{MPW}Libraries:CLibraries:StdCLib.o"		�
	 			"{MPW}Libraries:Libraries:Interface.o"		�
				"{MPW}Libraries:Libraries:ToolLibs.o"


OBJECTS		=												�
				"{ObjectDir}Options.cp.o"					�
				"{ObjectDir}Main.cp.o"						�
				"{ObjectDir}TypeID.cp.o"					�
				"{ObjectDir}AnimExtractor.cp.o"				�
				"{ObjectDir}Chunk.cp.o"						�
				"{ObjectDir}ChunkFile.cp.o"					�
				"{ObjectDir}ChunkQueue.cp.o"				�
				"{ObjectDir}ChunkSequence.cp.o"				�
				"{ObjectDir}ChunkSequenceHandler.cp.o"		�
				"{ObjectDir}Form3DOChunks.cp.o"				�
				"{ObjectDir}MultiPartFileName.cp.o"			�
				"{ObjectDir}SAAAnimExtractor.cp.o"			�
				"{ObjectDir}SAnimExtractor.cp.o"			�
				"{ObjectDir}SCelExtractor.cp.o"				�
				"{ObjectDir}StreamChunks.cp.o"				�
				"{ObjectDir}StreamAnalyzer.cp.o"


#####################################
#	Default build rules
#####################################
All				�	{Program}

{ObjectDir}		�	:

.cp.o			�	.cp
	{CC} {COptions} -o {TargDir}{Default}.cp.o {DepDir}{Default}.cp

.s.o			�	.s
	{ASM} {SOptions} -o {TargDir}{Default}.s.o {DepDir}{Default}.s


#####################################
#	Target build rules
#####################################

Options.cp				�	 Options.h
TypeID.cp				�	 TypeID.h
AnimExtractor.cp		�	 AnimExtractor.h
Chunk.cp				�	 Chunk.h
ChunkFile.cp			�	 ChunkFile.h
ChunkQueue.cp			�	 ChunkQueue.h
ChunkSequence.cp		�	 ChunkSequence.h
ChunkSequenceHandler.cp	�	 ChunkSequenceHandler.h
Form3DOChunks.cp		�	 Form3DOChunks.h
MultiPartFileName.cp	�	 MultiPartFileName.h
SAAAnimExtractor.cp		�	 SAAAnimExtractor.h
SAnimExtractor.cp		�	 SAnimExtractor.h
SCelExtractor.cp		�	 SCelExtractor.h
StreamChunks.cp			�	 StreamChunks.h
StreamAnalyzer.cp		�	 StreamAnalyzer.h


{Program}		��	{Program}.make {Program}.r {OBJECTS}
	Rez  -o {Program} {Program}.r -a -ov
	{LINK}	{LOptions}						�
			-o {Program}					�
			{OBJECTS}						�
			{LIBS}
	if not `exists {AppsDir}`
		newFolder {AppsDir}
	end
	move -y {Program} {AppsDir}{Program}
	if `exists "{Program}.sym"`
		move -y {Program}.sym {AppsDir}{Program}.sym
	end


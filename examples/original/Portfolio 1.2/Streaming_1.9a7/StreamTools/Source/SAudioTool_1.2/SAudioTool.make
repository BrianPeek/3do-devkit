#   File:       SAudioTool.make
#   Target:     SAudioTool
#   Sources:    SAudioTool.c


OBJECTS = SAudioTool.c.o
#DEBUG_FLAGS = -sym full
DEBUG_FLAGS = 


SAudioTool �� SAudioTool.make {OBJECTS}
	Rez  -o SAudioTool SAudioTool.r -a -ov
	Link -d -c 'MPS ' -t MPST �
		{DEBUG_FLAGS} �
		{OBJECTS} �
		#"{CLibraries}"CSANELib.o �
		#"{CLibraries}"Math.o �
		#"{CLibraries}"Complex.o �
		"{CLibraries}"StdClib.o �
		"{Libraries}"Stubs.o �
		"{Libraries}"Runtime.o �
		"{Libraries}"Interface.o �
		"{Libraries}"ToolLibs.o �
		-o SAudioTool
SAudioTool.c.o � SAudioTool.make SAudioTool.c
	 C {DEBUG_FLAGS} -r  SAudioTool.c

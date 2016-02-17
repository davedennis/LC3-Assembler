# List of files
C_HEADERS = assembler.h field.h lc3.h        symbol.h tokens.h util.h
C_SRCS	  = assembler.c               main.c                         
C_OBJS	  = assembler.o               main.o
EXE       = mylc3as
LIB       = lc3as.a
STD_LIB   =
DEFINES   =

# Compiler and loader commands and flags
GCC		= gcc
GCC_FLAGS	= -g -std=c99 -Wall -c
LD_FLAGS	= -g -std=c99 -Wall

# Compile .c files to .o files
.c.o:
	$(GCC) $(GCC_FLAGS) $(DEFINES) $<

# Target is the executable
$(EXE): $(C_OBJS) $(LIB)
	$(GCC) $(LD_FLAGS) -o $(EXE) $(C_OBJS) $(LIB) $(STD_LIB)

testTokens: $(LIB) testTokens.o
	$(GCC) $(LD_FLAGS) testTokens.o $(LIB) -o testTokens

seeLC3: seeLC3.o $(LIB)
	$(GCC) $(LD_FLAGS) seeLC3.c $(LIB) -o seeLC3

# Recompile C objects if headers change
${C_OBJS}: ${C_HEADERS}

# Clean up the directory
clean:
	rm -f *.o *~ $(EXE) testTokens seeLC3

CC = gcc
DEBUG_FLAGS = -g
OPT_FLAGS = -fPIC -O3

#FLAGS = $(WARN_FLAGS)
#FLAGS = $(DEBUG_FLAGS)
#I found that in Ubuntu12.04-x86_64 and gcc4,6, the P&R result was same whether or not added -std=c99,
#but they were different in Fedora17-i386 system, gcc4.7.3
#FLAGS = $(OPT_FLAGS)
FLAGS = $(DEBUG_FLAGS) $(OPT_FLAGS)

#Uncomment line below if X Windows isn't installed on your system.
#FLAGS = $(OPT_FLAGS) -DNO_GRAPHICS

#Useful flags on HP machines
#DEBUG_FLAGS = -Aa -g
#OPT_FLAGS = -Aa +O3

EXE = path

#in obj file, it didn't had test_h.o and graphics.o, it should compile seperately
OBJ = main.o resolve_file.o
#attention, the test_h.c and graphics.c should delete from SRC, it should compile seperately.
SRC = main.c resolve_file.c

Header = resolve_file.h 
# I haven't been able to make -static work under Solaris.  Use shared
# libraries all the time.

# Add purify before $(CC) in the link line below to run purify on VPR.

#link all needed object files to binary file.
#FIXME: the statement must define at all object files, graphics.o and test_h.o,
#otherwise, it will not compile all the needed object files.

#then I should set most object files. Attention, I should use %.o, %.c, not *.o, *.c.
obj/%.o : $(SRC_DIR)/%.c
	$(CC) $(FLAGS) -c -o $@ $<

$(EXE):$(OBJ)
	$(CC) $(FLAGS) $(OBJ) -o $(EXE)
clean:
	rm *.o $(EXE)

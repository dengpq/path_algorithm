CC = gcc
DEBUG_FLAGS = -g
OPT_FLAGS = -fPIC -O3

FLAGS = $(DEBUG_FLAGS) $(OPT_FLAGS)

EXE = read

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
	$(CC) $(FLAGS) -c $< -o $@

$(EXE):$(OBJ)
	$(CC) $(OBJ) -lm -o $(EXE)
clean:
	rm *.o $(EXE)

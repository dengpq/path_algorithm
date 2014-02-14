CC = gcc
DEBUG_FLAGS = -g -DDEBUG
OPT_FLAGS = -fPIC -O3

FLAGS = $(DEBUG_FLAGS) $(OPT_FLAGS)
LFLAGS = $(FLAGS) -lm

EXE = path

#in obj file, it didn't had test_h.o and graphics.o, it should compile seperately
OBJ = resolve_file.o stack.o graph.o path.o main.o
#attention, the test_h.c and graphics.c should delete from SRC, it should compile seperately.
SRC = resolve_file.c stack.c graph.c path.c main.c

Header = resolve_file.h stack.h vertex.h edge.h graph.h path.h

#link all needed object files to binary file.
#FIXME: the statement must define at all object files, graphics.o and test_h.o,
#otherwise, it will not compile all the needed object files.

#then I should set most object files. Attention, I should use %.o, %.c, not *.o, *.c.
obj/%.o:$(SRC_DIR)/%.c
	$(CC) -g -pg $(OPT_FLAGS) -c -o $@ $<

$(EXE):$(OBJ)
	$(CC) $(LFLAGS) $(OBJ) -o $(EXE)

clean:
	rm $(OBJ) $(EXE)

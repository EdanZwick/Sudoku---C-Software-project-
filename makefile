CC = gcc
OBJS = main.o mainAux.o files.o game.o history.o ILP.o solver.o parser.o map.o generator.o recStack.o dispatcher.o
EXEC = sudoku-console
COMP_FLAG = -ansi -Wall -Wextra -Werror -pedantic-errors -g
GUROBI_COMP = -I/usr/local/lib/gurobi563/include
GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56

all 	: $(EXEC)
$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(GUROBI_LIB) -o $@
main.o: main.c parser.h game.h dispatcher.h mode.h sizes.h
	$(CC) $(COMP_FLAG) -c $*.c
mainAux.o: mainAux.c generator.h mode.h files.h solver.h game.h ILP.h sizes.h
	$(CC) $(COMP_FLAG) -c $*.c
files.o: files.c mode.h
	$(CC) $(COMP_FLAG) -c $*.c
history.o: history.c history.h  
	$(CC) $(COMP_FLAG) -c $*.c
game.o: game.c game.h history.h mode.h solver.h
	$(CC) $(COMP_FLAG) -c $*.c
Solver.o: Solver.c recStack.h
	$(CC) $(COMP_FLAG) -c $*.c
ILP.o: ILP.c solver.h map.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
parser.o: parser.c mode.h
	$(CC) $(COMP_FLAG) -c $*.c
map.o: map.c map.h solver.h
	$(CC) $(COMP_FLAG) -c $*.c
generator.o: generator.c solver.h ILP.h sizes.h
	$(CC) $(COMP_FLAG) -c $*.c
recStack.o: recStack.c recStack.h
	$(CC) $(COMP_FLAG) -c $*.c
dispatcher.o: dispatcher.c mainAux.h mode.h game.h
	$(CC) $(COMP_FLAG) -c $*.c
clean:
	rm -f $(OBJS) $(EXEC)

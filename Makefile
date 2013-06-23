CC = gcc
CCFLAGS = -Wall
SRC = tema2.c
PROGRAM = tema2


all:
	$(CC) -o $(PROGRAM) $(SRC) $(CCFLAGS)


.PHONY : clean
clean :
	rm -f $(PROGRAM) core *~



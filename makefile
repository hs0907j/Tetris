all	: tetris.o
	gcc -g -lncurses tetris.c

clean:
	rm a.out *.o

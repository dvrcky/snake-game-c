CFLAGS = -lm -lSDL2
CC = clang

snake: main.c
	$(CC) $(CFLAGS) main.c -o snake


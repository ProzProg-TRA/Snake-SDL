all:
	gcc -o bin/snake snake.c -lSDL2 
debug:
	gcc -o bin/snake snake.c -lSDL2 -g
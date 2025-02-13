build:
	mkdir -p ./bin
	gcc main.c -o ./bin/chess-timer `sdl2-config --cflags --libs`

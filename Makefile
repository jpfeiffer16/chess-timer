build:
	mkdir -p ./bin
	gcc main.c -Wall -Werror -Wpedantic -o ./bin/chess-timer `sdl2-config --cflags --libs` -lSDL2_ttf -lm
build-debug:
	mkdir -p ./bin
	gcc main.c -Wall -Werror -Wpedantic -o ./bin/chess-timer `sdl2-config --cflags --libs` -lSDL2_ttf -lm -ggdb
build-release:
	mkdir -p ./bin
	gcc main.c -Wall -Werror -Wpedantic -o ./bin/chess-timer `sdl2-config --cflags --libs` -lSDL2_ttf -lm -O3
install: build-release
	sudo cp ./bin/chess-timer /usr/bin/

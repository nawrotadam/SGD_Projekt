say_hello:
	@echo "Hello"

generate:
	g++ `pkg-config --cflags sdl2` -o game.out main.cpp `pkg-config --libs sdl2` -std=c++11

generate_squares:
	g++ `pkg-config --cflags sdl2` -o squares.out simple_squares.cpp `pkg-config --libs sdl2` -std=c++11

generate_cannon:
	g++ `pkg-config --cflags sdl2` -o cannon.out simple_cannon.cpp `pkg-config --libs sdl2` -std=c++11

install_sdl2:
	sudo apt-get install libsdl2-dev
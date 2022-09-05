all:
	g++ -c zmath.cpp 
	g++ -c object.cpp
	g++ sdl_main.cpp -o main object.o zmath.o -lSDL2 -lGL
run:
	./main

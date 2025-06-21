#OBJS= object.o console.o zmath.o Shader.o font.o
OBJS=

all: $(OBJS) test_main

%.o : %.cpp %.h
	g++ -c -g $<

font.o : font.cpp font.h
	g++ -c -g $< -I/usr/include/freetype2

test_main: test_main.cpp  $(OBJS)
	g++ -g test_main.cpp -o main $(OBJS) 

main: sdl_main.cpp  $(OBJS)
	g++ -g sdl_main.cpp -o main $(OBJS) -lSDL2 -lGL -lGLU -lGLEW -lfreetype 
	#./main
run: 
	./main

clean:
	rm *.o main
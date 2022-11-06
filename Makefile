OBJS= object.o console.o zmath.o Shader.o font.o 

all: $(OBJS) main

%.o : %.cpp %.h
	g++ -c -g $<

font.o : font.cpp font.h
	g++ -c -g $< -I/usr/include/freetype2

main: sdl_main.cpp  $(OBJS)
	g++ -g sdl_main.cpp -o main $(OBJS) -lSDL2 -lGL -lGLU -lGLEW -lfreetype 
	#./main
run: 
	./main

clean:
	rm *.o main
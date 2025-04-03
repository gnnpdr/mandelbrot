all:
	@g++ -c -o obj/main.o main.cpp
	@g++ -c -mavx2 -o obj/draw.o draw/draw.cpp
	@g++ obj/main.o obj/draw.o -o mandelbrot -O3 -lsfml-graphics -lsfml-window -lsfml-system

clean:
	rm -rf obj/*
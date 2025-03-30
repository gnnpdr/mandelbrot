all:
	@g++ -c -o obj/ver1.o ver1.cpp
	@g++ obj/ver1.o -o ver1 -lsfml-graphics -lsfml-window -lsfml-system
	@./ver1

clean:
	rm -rf obj/*
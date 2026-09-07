SDL_LIBS = `sdl2-config --libs`
SDL_FLAGS = `sdl2-config --cflags`

main.out: main.o termigine.o soli.o
	g++ -std=c++23 main.o termigine.o soli.o -lsqlite3 -o main.out ${SDL_LIBS}

main.o: main.cpp termigine.h soli.h
	g++ -std=c++23 ${SDL_FLAGS} -c main.cpp -o main.o

termigine.o: termigine.cpp termigine.h
	g++ -std=c++23 -c termigine.cpp -o termigine.o

soli.o: soli.cpp soli.h
	g++ -std=c++23 ${SDL_FLAGS} -c soli.cpp -o soli.o

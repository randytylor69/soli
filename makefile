main.out: main.o termigine.o
	g++ -std=c++23 main.o termigine.o -o main.out

main.o: main.cpp termigine.h
	g++ -std=c++23 -c main.cpp -o main.o

termigine.o: termigine.cpp termigine.h
	g++ -std=c++23 -c termigine.cpp -o termigine.o

main.out: main.o termigine.o soli.o
	g++ -std=c++23 main.o termigine.o soli.o -o main.out

main.o: main.cpp termigine.h soli.h
	g++ -std=c++23 -c main.cpp -o main.o

termigine.o: termigine.cpp termigine.h
	g++ -std=c++23 -c termigine.cpp -o termigine.o

soli.o: soli.cpp soli.h
	g++ -std=c++23 -c soli.cpp -o soli.o

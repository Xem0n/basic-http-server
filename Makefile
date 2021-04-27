server: build/main.o build/connection.o build/response.o build/Socket.o
	g++ build/main.o build/connection.o build/Socket.o build/response.o -lwsock32 -o server

build/main.o: src/main.cpp
	g++ -c src/main.cpp -o build/main.o -Wall

build/connection.o: src/connection.cpp src/connection.h
	g++ -c src/connection.cpp -o build/connection.o 

build/response.o: src/response.cpp src/response.h
	g++ -c src/response.cpp -o build/response.o

build/Socket.o: src/libs/Socket.cpp src/libs/Socket.h
	g++ -c src/libs/Socket.cpp -o build/Socket.o

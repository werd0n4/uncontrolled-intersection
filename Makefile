all: main

main:
	g++ -std=c++17 main.cpp -lncurses -lpthread -o out.o  

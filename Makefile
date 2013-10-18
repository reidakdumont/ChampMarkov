all:
	g++ -O2 -Wall -g -std=c++0x -o champ Recuit.cpp main.cpp `pkg-config --cflags --libs opencv`
clean:
	rm -rf champ

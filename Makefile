.DEFAULT_GOAL := all

all:
	clang++ -std=c++17 -O3 -lSDL3 chris24/src/main_other.cpp -o out/demo.app

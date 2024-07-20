all:
	g++-13 src/operations.cpp src/parser.cpp src/main.cpp src/commands.cpp -o main -I./include -std=c++17
	./main
all:
	g++-13 src/parser.cpp src/operations.cpp src/lexer.cpp src/main.cpp src/commands.cpp -o main -I./include -std=c++17
	./main
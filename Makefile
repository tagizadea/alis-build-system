src = src/operations.cpp src/lexer.cpp src/main.cpp src/commands.cpp
src += src/ast.cpp src/parser.cpp src/eval.cpp

all:
	g++-14 ${src} -o main -I./include -std=c++17
	./main
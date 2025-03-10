src = src/ast.cpp src/parser.cpp src/eval.cpp src/env.cpp
src += src/operations.cpp src/lexer.cpp src/main.cpp src/commands.cpp

all:
	g++-14 -g ${src} -o main -I./include -std=c++17
	./main
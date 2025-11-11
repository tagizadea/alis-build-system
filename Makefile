src = src/ast.cpp src/parser.cpp src/eval.cpp src/env.cpp
src += src/operations.cpp src/lexer.cpp src/main.cpp src/manager.cpp

all:
	g++-15 ${src} -o main -I./include -std=c++17 -O2
	./main
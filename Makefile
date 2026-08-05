src = src/ast.cpp src/parser.cpp src/eval.cpp src/env.cpp src/xxhash64.cpp
src += src/operations.cpp src/lexer.cpp src/main.cpp src/manager.cpp src/debug.cpp

# Default language for debug messages: EN or AZ
# Override with: make ABS_LANG=AZ
ABS_LANG ?= EN

# Performance profiling (off by default)
# Override with: make ABS_PROFILE=1
ABS_PROFILE ?= 0

all:
	g++-15 ${src} -o main -I./include -std=c++20 -g -DLANG=\"${ABS_LANG}\" -DABS_PROFILE=${ABS_PROFILE}
	./main

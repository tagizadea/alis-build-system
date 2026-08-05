src = src/ast.cpp src/parser.cpp src/eval.cpp src/env.cpp src/xxhash64.cpp
src += src/operations.cpp src/lexer.cpp src/main.cpp src/manager.cpp src/debug.cpp

# Default language for debug messages: EN or AZ
# Override with: make ABS_LANG=AZ
ABS_LANG ?= EN

# Performance profiling (off by default)
# Override with: make ABS_PROFILE=1
ABS_PROFILE ?= 0

# Export symbols so compiled plugins (.so/.dylib) can call back into ABS
# (Make_NFunc, Make_String, Env::declareVar, etc.)
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    EXPORT_FLAG = -Wl,-export_dynamic
else
    EXPORT_FLAG = -rdynamic
endif

all:
	g++-15 ${src} -o main -I./include -std=c++20 -g ${EXPORT_FLAG} -DLANG=\"${ABS_LANG}\" -DABS_PROFILE=${ABS_PROFILE}
	./main

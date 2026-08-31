# ============================================================
# ABS Makefile
# ============================================================

TARGET := main

CXX := g++-15

SRC := \
	src/ast.cpp \
	src/parser.cpp \
	src/eval.cpp \
	src/env.cpp \
	src/xxhash64.cpp \
	src/operations.cpp \
	src/lexer.cpp \
	src/main.cpp \
	src/manager.cpp \
	src/debug.cpp

OBJ := $(patsubst src/%.cpp,bin/%.o,$(SRC))
DEP := $(OBJ:.o=.d)

# ------------------------------------------------------------
# Configuration
# ------------------------------------------------------------

# Default language for debug messages: EN or AZ
# Override with: make ABS_LANG=AZ
ABS_LANG ?= EN

# Performance profiling (off by default)
# Override with: make ABS_PROFILE=1
ABS_PROFILE ?= 0

# Compiler warnings (off by default)
# Override with: make WARNINGS=1
WARNINGS ?= 0

# ------------------------------------------------------------
# Platform
# ------------------------------------------------------------

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
    EXPORT_FLAG = -Wl,-export_dynamic
else ifeq ($(UNAME_S),Linux)
    EXPORT_FLAG = -rdynamic
else
    EXPORT_FLAG =
endif

# ------------------------------------------------------------
# Compiler flags
# ------------------------------------------------------------

CPPFLAGS := \
	-I./include \
	-DLANG=\"$(ABS_LANG)\" \
	-DABS_PROFILE=$(ABS_PROFILE)

CXXFLAGS := \
	-std=c++20 \
	-g \
	-MMD \
	-MP

ifeq ($(WARNINGS),1)
	CXXFLAGS += -Wall -Wextra -Wpedantic
endif

# ------------------------------------------------------------
# Linker flags
# ------------------------------------------------------------

LDFLAGS := $(EXPORT_FLAG)

# ------------------------------------------------------------
# Build
# ------------------------------------------------------------

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(@D)
	$(CXX) $(OBJ) $(LDFLAGS) -o $@

bin/%.o: src/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# Automatically generated header dependencies
-include $(DEP)

# ------------------------------------------------------------
# Run
# ------------------------------------------------------------

.PHONY: run
run: $(TARGET)
	./$(TARGET)

# Preserve original "build + run" behavior
.PHONY: start
start: all
	./$(TARGET)

# ------------------------------------------------------------
# Cleanup
# ------------------------------------------------------------

.PHONY: clean
clean:
	$(RM) -r bin
	$(RM) -r $(TARGET)

.PHONY: rebuild
rebuild: clean all

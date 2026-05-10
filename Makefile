MAKEFLAGS := --jobs=6

CC = gcc
CXX = g++

CXXFLAGS = -std=c++20 -Iinclude -Igenerated -I/usr/include/antlr4-runtime
CFLAGS = -Iinclude -Wall -Wextra -Werror -O2

ANTLR_RUNTIME =-lantlr4-runtime

CPP_SOURCES := $(shell find generated src -name '*.cpp')
C_SOURCES := $(shell find src -name '*.c')

OBJECTS := $(CPP_SOURCES:.cpp=.o) $(C_SOURCES:.c=.o)

all: compiler

compiler: $(OBJECTS)
	$(CXX) $(OBJECTS) $(ANTLR_RUNTIME) -o compiler

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) compiler

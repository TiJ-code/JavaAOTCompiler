CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -Werror -O2

SRC = src/main.c
OUT = compiler

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

run: all
	./$(OUT)
	./a.out
	echo $$?

clean:
	rm -f $(OUT) a.out

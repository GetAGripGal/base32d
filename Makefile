CC=gcc
BIN_NAME=base32d

all:
	$(CC) main.c base32.c -o $(BIN_NAME)
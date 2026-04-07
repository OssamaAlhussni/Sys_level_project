
CC = gcc
CFLAGS = -std=c11
TARGET = monitor
SRC = src/main.c src/scanner.c src/backup.c src/ipc.c src/signals.c

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
	rm -rf backup
	rm -rf logs

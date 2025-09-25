CC = gcc
CFLAGS = -Wall -g
TARGET = dragonshell

all: $(TARGET)

$(TARGET): dragonshell.o
	$(CC) $(CFLAGS) -o $(TARGET) dragonshell.o

compile: dragonshell.o

dragonshell.o: dragonshell.c
	$(CC) $(CFLAGS) -c dragonshell.c

clean:
	rm -f $(TARGET) *.o

CC = gcc
CFLAGS = -Wall -Wextra -o2

TARGET = diamond_square

&(TARGET): main.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c -lm

clean:
	rm -f $(TARGET)
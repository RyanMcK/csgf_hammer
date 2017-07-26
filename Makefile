CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -O3 -lm
EXEC=mandelbrot
OBJ=mandelbrot.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY = clean

clean:
	rm -f *.o

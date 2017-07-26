CC=pgcc
CFLAGS=-c99 -acc -ta=tesla,cc60,pinned -Minfo=accel -fast -Minline
EXEC=mandelbrot
OBJ=mandelbrot.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY = clean

clean:
	rm -f *.o

CC=pgcc
CFLAGS=-c99 -acc -ta=tesla,cc60,pinned -Minfo=accel -fast -Minline
EXEC=mandelbrot
OBJ=mandelbrot.o

STABLE_EXEC=mandelbrot_stable
STABLE_OBJ=mandelbrot_stable.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

$(STABLE_EXEC): $(STABLE_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY = clean

clean:
	rm -f *.o

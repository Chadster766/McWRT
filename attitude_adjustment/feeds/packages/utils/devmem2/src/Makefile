OBJ = devmem2.o

all: compile

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

compile: $(OBJ)
	$(CC) -o devmem2 $(LDFLAGS) $(OBJ)

clean:
	rm -f *.o *.so devmem2


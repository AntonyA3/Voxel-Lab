CC=gcc
CFLAGS = -I ./include -Wall
SRCDIR = ./src/
DEPSDIR= ./include/
_OBJDIR= obj/
OBJDIR = ./objects/


%.o: $(SRCDIR)%.c
	$(CC) -c $< $(CFLAGS)

voxellab: *.o
	$(CC) *.o -o voxellab $(CFLAGS)

clean:
	-rm *o voxellab

run:
	./voxellab
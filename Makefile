CC=gcc
CFLAGS = -I ./include -Wall -g
SRCDIR = ./src/
DEPSDIR= ./include/
_OBJDIR= obj/
OBJDIR = ./objects/
LIBS = -lGL -lGLU -lglfw -lX11 -lXxf86vm -lXrandr -lpthread -lXi -lm 
GLLIBS =  -lGL -lGLU
main.o: $(SRCDIR)main.c
	$(CC) $< -c $(CFLAGS)

camera.o: $(SRCDIR)camera.c
	$(CC) $< -c $(CFLAGS)

grid.o: $(SRCDIR)grid.c
	$(CC) $< -c $(CFLAGS)


voxellab: main.o camera.o grid.o
	$(CC) -o bin/voxellab main.o camera.o grid.o $(CFLAGS) $(LIBS)

clean:
	-rm *o bin/voxellab

run:
	./bin/voxellab
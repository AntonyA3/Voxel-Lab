CC=gcc
CFLAGS = -I ./include -Wall -g
SRCDIR = ./src/
TESTDIR= ./test/
DEPSDIR= ./include/
_OBJDIR= obj/
OBJDIR = ./objects/
IMGUIDIR=./imgui
LIBS = -lGL -lGLU -lGLEW -lglut -lglfw -lX11 -lXxf86vm -lXrandr -lpthread -lXi -lm
main.o: $(SRCDIR)main.c 
	$(CC) $< -c $(CFLAGS)

%.o: $(SRCDIR)%.c
	$(CC) $< -c $(CFLAGS)

voxel_test.o: $(TESTDIR)voxel_test.c
	$(CC) $< -c $(CFLAGS)

voxellab: main.o camera.o grid.o voxel.o mouse.o
	$(CC) -o bin/voxellab main.o camera.o grid.o voxel.o mouse.o $(CFLAGS) $(LIBS)

buildtest: voxel_test.o voxel.o
	$(CC)  -o bin/test voxel_test.o voxel.o mouse.o $(CFLAGS) $(LIBS) -ggdb

runtest:
	./bin/test

clean:
	-rm *o bin/*

run:
	./bin/voxellab
CC=gcc
CFLAGS = -I ./include ./linmath/linmath.h ./nuklear/nuklear.h -Wall -g
SRCDIR = ./src/

LIBS = -lGL -lGLU -lGLEW -lglut -lglfw -lX11 -lXxf86vm -lXrandr -lpthread -lXi -lm

%.o: $(SRCDIR)%.c
	$(CC) $< -c $(CFLAGS)

install: main.o cursor.o color.o grid.o orbit_camera.o app_viewport.o voxel.o ray.o
	$(CC) -o bin/voxellab $(CFLAGS) main.o cursor.o color.o grid.o orbit_camera.o app_viewport.o voxel.o ray.o $(LIBS) 
clean:
	-rm *o bin/voxellab

run:
	./bin/voxellab
debug:
	./bin/voxellab debug


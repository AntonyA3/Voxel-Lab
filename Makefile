CC=gcc
CFLAGS = -I ./include ./linmath/linmath.h ./nuklear/nuklear.h -Wall -g
SRCDIR = ./src/
TESTDIR = ./test/
LIBS = -lGL -lGLU -lGLEW -lglut -lglfw -lX11 -lXxf86vm -lXrandr -lpthread -lXi -lm

OBJ = $(patsubst $(SRCDIR)%.c,%.o,$(wildcard ./src/*.c))
showobj:
	echo $(OBJ)
	
%.o: $(SRCDIR)%.c
	$(CC) $< -c $(CFLAGS)

%.o: $(TESTDIR)%.c
	$(CC) $< -c $(CFLAGS)

install: $(OBJ)
	$(CC) -o bin/voxellab $(CFLAGS) $(OBJ) $(LIBS) 
clean:
	-rm *o bin/voxellab

run:
	./bin/voxellab
	
debug:
	./bin/voxellab debug


buildtest: test.o
	$(CC) -o bin/test $(CFLAGS) test.o aabb.o voxel_model.o voxel.o box_generator.o sphere.o $(LIBS) 

runtest:
	./bin/test


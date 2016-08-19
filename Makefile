# Makefile for OpenGL engine program
# ------------------------------------

# Object files list
OBJS = bmath.o font.o loader.o collisions.o main.o

# Executable name
EXENAME = engine

# Ensure graphe.txt exists
$(shell test -f txt/polygones4.txt || cp txt/polygones4.txt.dist txt/polygones4.txt)
$(shell test -f txt/textures.txt || cp txt/textures.txt.dist txt/textures.txt)

# Testing for OS X v Linux
OS = $(shell uname -s)

ifeq ($(OS), Darwin)
    LIBS = -framework GLUT -framework OpenGL -framework Cocoa
else
    LIBS = -lGL -lGLU -lglut
endif

all : $(EXENAME)

$(EXENAME) : $(OBJS)
	gcc -fexpensive-optimizations -funroll-loops -finline-functions -ffast-math -Wall -g -O2 -o bin/$(EXENAME) $(OBJS) -lm $(LIBS)
bmath.o: bmath.c bmath.h collisions.h
	gcc -Wno-deprecated -c bmath.c
font.o: font.c font.h
	gcc -Wno-deprecated -c font.c
loader.o: loader.c loader.h
	gcc -Wno-deprecated -c loader.c
collisions.o: collisions.c collisions.h
	gcc -Wno-deprecated -c collisions.c
main.o: main.c loader.h font.h collisions.h
	gcc -Wno-deprecated -c main.c

clean : rm -f src/*.o bin/$(EXENAME) 2>/dev/null

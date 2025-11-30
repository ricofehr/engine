# Makefile for OpenGL engine program
# ------------------------------------

# Object files list
OBJS = bmath.o font.o loader.o polygon.o vector.o main.o

# Executable name
EXENAME = engine

# Ensure graphe.txt exists
$(shell test -f txt/universe.txt || cp txt/universe.txt.dist txt/universe.txt)
$(shell test -f txt/textures.txt || cp txt/textures.txt.dist txt/textures.txt)

# Testing for OS X v Linux
OS = $(shell uname -s)

ifeq ($(OS), Darwin)
    LIBS = -framework GLUT -framework OpenGL
else
    LIBS = -lGL -lGLU -lglut
endif

all : $(EXENAME)

$(EXENAME) : $(OBJS)
	gcc -fexpensive-optimizations -funroll-loops -finline-functions -ffast-math -Wall -g -O2 -o bin/$(EXENAME) $(OBJS) -lm $(LIBS)
bmath.o: bmath.c bmath.h
	gcc -Wno-deprecated -c bmath.c
font.o: font.c font.h
	gcc -Wno-deprecated -c font.c
loader.o: loader.c loader.h
	gcc -Wno-deprecated -c loader.c
vector.o: vector.c vector.h
	gcc -Wno-deprecated -c vector.c
polygon.o: polygon.c polygon.h vector.h
	gcc -Wno-deprecated -c polygon.c
main.o: main.c loader.h polygon.h
	gcc -Wno-deprecated -c main.c

clean :
	rm -f *.o bin/$(EXENAME) 2>/dev/null

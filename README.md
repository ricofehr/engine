# engine

A 3d engine at very early stage and in working progress. 
The program generates cubes with randomly coordinates and translations,
and manages collisions with room and between theirs.

## Compile

Requirements
- In Mac os, Opengl and GLUT Framework. This ones are bundled with Xcode.
- In Linux, libglut-dev or freeglut3-dev package.

Once this library are available, just execute "make" for compile binary program
```
$ make
gcc -Wno-deprecated -c bmath.c
gcc -Wno-deprecated -c font.c
gcc -Wno-deprecated -c loader.c
gcc -Wno-deprecated -c polygon.c
gcc -Wno-deprecated -c vector.c
gcc -Wno-deprecated -c main.c
gcc -fexpensive-optimizations -funroll-loops -finline-functions -ffast-math -Wall -g -O2 -o bin/engine bmath.o font.o loader.o polygon.o vector.o main.o -lm -framework GLUT -framework OpenGL
```

## Folders
```
/		Sources are on the root of Repository
+--bin/		Binary folder where engine executable is written
+--assets/      Textures bmp files
+--demos/	Gif for demos execution
+--txt/      	Data text files for textures list and initial universe coordinates.
		Default files are present (the .dist.txt) but can be overrided
```

## Run

2 optional parameters are available for the binary bin/./engine
- First parameter (int) is the count of cubes into the universe. Default is 64 and max is 256
- Second parameter ("g") enables the grid mode on opengl.

Execution, with 128 cubes and grid mode disabled
```
bin/./engine 128
```
![Engine](demos/engine.gif?raw=true)


## Todo

- Improve collision algorithm
- Manage shadows and lights
- Manage camera move and interactions
- Manage drawing of spheres and his transforms

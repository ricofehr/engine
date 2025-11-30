obj = fonctionMath.o font.o loader.o collisions.o main.o

moteur_3d : $(obj) 
	g++ -Wno-deprecated -fexpensive-optimizations -funroll-loops -finline-functions  -ffast-math -Wall -g -O2  -o bin/moteur_3d -export-dynamic $(obj) -lX11 -lglut -lGL -lGLU
fonctionMath.o:fonctionMath.cpp fonctionMath.h collisions.h
	g++ -Wno-deprecated -c fonctionMath.cpp
font.o:font.cpp font.h
	g++ -Wno-deprecated -c font.cpp
loader.o:loader.cpp loader.h
	g++ -Wno-deprecated -c loader.cpp
collisions.o:collisions.cpp collisions.h
	g++ -Wno-deprecated -c collisions.cpp
main.o:main.cpp loader.h font.h collisions.h
	g++ -Wno-deprecated -c main.cpp


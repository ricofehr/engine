#ifndef LOADER_H
#define LOADER_H

#include <stdio.h>
#include "bmath.h"

#if defined __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#define EXIT {fclose(bmp);return;}
#define CTOI(C) (*(int*)&C)	//Char* to int

/* Global variables used by loader */
extern unsigned int *texturenames;
extern struct poly4 *tabpoly4;
extern struct poly4 *polinit;
extern int cntpoly4;
extern int cntload;
extern struct move col;

void loadBMP(char *filename, int indtex);
void loadtextures(const char filename[]);
void loadpoly4();
void displaypoly4();

#endif

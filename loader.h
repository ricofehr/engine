/*
* Header file for loader.c
* @author Eric Fehr (ricofehr@nextdeploy.io, @github: ricofehr)
*/

#ifndef LOADER_H
#define LOADER_H

#include <stdio.h>
#include "bmath.h"

#if defined __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#define EXIT { fclose(bmp); return; } /* close file and return */
#define CTOI(C) (*(int*)&C)	/* char* to int */

/* Gl Texture array */
extern unsigned int *texturenames;
/* debug mode */
extern int debug;

void load_textures(const char filename[]);
void load_universe();
void display_poly4();

#endif

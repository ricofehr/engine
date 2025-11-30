#ifndef FONT_H
#define FONT_H

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "bmath.h"

#if defined __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

/* Global variables used by font.cpp */
extern float camx,camy,camz,vuex,vuey,vuez;

void drawString (char *s);
void drawStringBig (char *s);
void move2dText(int x, int y);
void glText2d(int x, int y, const char *chaine, ...);
void drawFPS();

#endif

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

void draw_fps();

#endif

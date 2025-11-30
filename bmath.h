/*
* Header file for bmath.c
* @author Eric Fehr (ricofehr@nextdeploy.io, @github: ricofehr)
*/

#ifndef _BMATH_H
#define _BMATH_H

#if defined __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#define PI (float) 3.14159265358979323846
#include <math.h>
void matrix_mult(float *a, float b[4][4]);
float *matrix_inverse(const float *matr);
int fps();
void gl_matrix_mult(GLfloat *a, GLfloat *b);
GLfloat *inverse_matr(GLfloat *mat);

/**
*  absolute - compute absolute
*  @x: number to compute
*
*  Return absolute value of x
*/

static inline double absolute(double x)
{
	return x >= 0 ? x : -x;
}
#endif

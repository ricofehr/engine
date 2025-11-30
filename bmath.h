#ifndef BMATH_H
#define BMATH_H

#define PI (float) 3.14159265358979323846
#include <math.h>

#if defined __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

struct vector {
	float x, y, z;
};

struct point {
	float x, y, z;
};

struct line {
	struct point orig;
	struct vector dir;
};

struct poly4 {
	GLuint texture;
	GLfloat *pt;
	GLfloat k;
	struct vector norm;
	struct line *equa;
	int cntsides;
	int col;
	int move;
	int polcol;
	int cntfaces;
	int nb;
	int indpol;
	int indangle;
	int indtrans;
	float ray;
	float rayfc;
	struct point center;
	struct point centerfc;
};

struct move {
	GLfloat dt[4];
	struct point inter[4];
};

struct vector initvector(float xx, float yy, float zz);
struct vector vectorproduct(struct vector v1, struct vector v2);
struct vector setvector(struct point p1, struct point p2);
double magnitude(struct vector normal);
struct vector normalize(struct vector v);
struct vector rotatepoint(struct vector p, struct point center, struct point rotate);
void matrixmult(float *a, float b[4][4]);
float dot(struct vector v1, struct vector v2);
double anglevectors(struct vector v1, struct vector v2);
GLfloat computedist(struct point p1, struct point p2);
double absolute(double x);
int fps();
/*
void view(int ix, int iy);
void deplacement(int direction,int type);
void souris();
void test();
*/
#endif

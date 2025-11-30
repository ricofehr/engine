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

struct vector vector_init(float xx, float yy, float zz);
struct vector vector_product(struct vector v1, struct vector v2);
struct vector vector_set(struct point p1, struct point p2);
double vector_magnitude(struct vector normal);
struct vector vector_normalize(struct vector v);
struct vector point_rotate(struct vector p, struct point center, struct point rotate);
float vector_dot(struct vector v1, struct vector v2);
double vector_angle(struct vector v1, struct vector v2);
GLfloat segment_dist(struct point p1, struct point p2);
double absolute(double x);
int fps();
#endif

/*
* Header file for vector.c: define vector, line, and point objects
* @author Eric Fehr (ricofehr@nextdeploy.io, @github: ricofehr)
*/

#ifndef _VECTOR_H
#define _VECTOR_H

#if defined __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

struct point {
	float x, y, z;
};

struct vector {
	float x, y, z;

	int (*vector_is_same_dir)(struct vector v1, struct vector v2);
	struct vector (*vector_product)(struct vector v1, struct vector v2);
	double (*vector_magnitude)(struct vector normal);
	struct vector (*vector_normalize)(struct vector v);
	float (*vector_dot)(struct vector v1, struct vector v2);
	double (*vector_get_angle)(struct vector v1, struct vector v2);
	struct vector (*vector_rotate)(struct vector p, struct point center, struct point rotate);
	GLfloat * (*vector_to_matrix)(struct vector u, float fact);
	struct point (*vector_translate)(struct vector v, struct point p, float fact);
};

struct line {
	struct point orig, orig2;
	struct vector dir;

	struct point (*segment_center)(struct line s);
	float (*segment_magnitude)(struct line s);
	struct point (*segment_intersec)(struct line u, struct line v);
	struct point (*segment_intersec_line)(struct line d, struct line e);
	int (*segment_contains)(struct line d, struct point p);
	struct point (*line_compute_point)(struct line d, float fact);
};

typedef struct line segment;

struct vector vector_init(float x, float y, float z);
struct vector vector_set(struct point p1, struct point p2);
struct line line_init(struct point orig, struct vector dir);
segment segment_init(struct point orig, struct point orig2);
#endif

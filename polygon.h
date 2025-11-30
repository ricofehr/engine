/*
* Header for polygonc.c: define polygon object
* @author Eric Fehr (ricofehr@nextdeploy.io, @github: ricofehr)
*/

#ifndef _POLYGON_H
#define _POLYGON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmath.h"
#include "vector.h"

#if defined __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

struct polygon {
	struct polygon *next;
	struct polygon *prev;
	struct polygon *col;
	struct polygon *oldcol;
	double distcol;
	GLuint texture;
	GLfloat *pts;
	struct vector trans;
	struct vector vartrans;
	struct line *equa;
	int cntsides;
	int move;
	int cntfaces;
	int indfaces;
	int indpol;
	int counter;
	float rayon;
	struct point center;

	void (*poly_compute_coord)(struct polygon *poly);
	struct point (*poly_intersec_quad)(const struct polygon *poly, struct line e);
	int (*poly_contains)(struct polygon *poly, struct point inter);
	struct point (*poly_intersec)(struct polygon *poly, struct line d);
	void (*poly_pivot_collision)(struct polygon *poly);
	void (*poly_detect_collision)(struct polygon *poly1, struct polygon *poly2);
};

/* head of linked list */
extern struct polygon *poly_head;
/* count of cubes */
extern int cnt_cubes;
/* grid mod enabled */
extern int mode_grid;

void init_transforms();
void free_transforms();
void init_cube(int move);
struct polygon *polygon_init(int counter, int cntsides, int move, int indpol,
                             int cntfaces, int indfaces, int texture, GLfloat pts[]);
void next_hop();
void next_coords();
void display_polygons();
void destroy_polygons();
#endif

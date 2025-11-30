#ifndef COLLISION_H
#define COLLISION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmath.h"

#if defined __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

void init_cube(int pres, int rot, int translate);
void init_norm_k(int cpt, struct point center, float rayon);
struct line polygon_get_diag(float *ray, int cpt);
void compute_coord(int i, int j);
void manage_cam();
void init_transforms();
void manage_collisions();
void copy_poly4(struct poly4* src, struct poly4* dest);
void delete_poly4(struct poly4* *sprime);

#endif

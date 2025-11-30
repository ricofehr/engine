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

void initcube(int pres, int rot, int translate);
int isonface(struct point inter, int cpt, int mat);
int isonsegment(struct line d, struct point p);
struct point intersecline(struct line d, struct line e, int cpt);
void initnormk(int cpt, int mat, struct point center, float rayon);
struct line centerpolygon(float *ray, int cpt);
GLfloat *multim(GLfloat *a, GLfloat *b);
void computecoord(int i, int j, int l);
struct point intersec(struct vector vcm, struct point ctr, int cpt, int pol);
void collisioncam();
struct point intersec2segments(struct line u, struct line v);
GLfloat *inversematr(GLfloat *mat);
struct point intersec3plans(struct vector *n, float *k);
int issamedir(struct vector v1, struct vector v2);
struct point intersecline(struct line d, struct line e, int cpt);
GLfloat *inversematr(GLfloat *mat);
void coli(int numero);
struct point intersecquad(struct line e,int cpt);
void initangletrans();
GLfloat *matrtrans(struct vector u, float fact);
void interpol();
GLfloat *transpose(GLfloat *matr);
void copypoly4(struct poly4* src, struct poly4* dest, int mal);
struct point ontheline(struct line d, float fact);
void changetrans(int nb, float fctdep, float fact);
void deletepoly4(struct poly4* *sprime);
void deletepoly4__(struct poly4 *sprime);

#endif

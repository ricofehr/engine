#include<stdio.h>
#include<stdlib.h>
#include<GL/glut.h>
#include "fonctionMath.h"


void ajout_cube_coord(int pres,int rot,int trs);
void chge_coord(int i,int j,int l);
void collision_cam();
point intersec(vecteur vcm,point ctre,int cpt,int pol);
GLfloat Dist_entre_pts(point A,point B);
void trve_norm_k(int cpt,int mat,point cre,float ry);
point milieu(point p,point p1);
int est_sur_face(point inter,int cpt,int mat);
int est_sur_seg(droite d, point p);
point intersec_2_drseg(droite d,droite e,int );
point intersec_seg_droite_sur(droite d,droite e,int cpt);
float *multim(float *a, float *b);
GLfloat *inverse_matr(GLfloat *mat);
point intersec_3_plans(vecteur *n,float *k);
void colli_polyg();
void coli(int numero);
point intersec_seg_droite_sur_rec(droite e,int cpt);
void init_angle_trans();
droite centre_polyg(float *ray,int cpt);
GLfloat *matrtrans(vecteur u,float fact);
void inter_pol();
GLfloat *transpose(GLfloat *matr);
point inter_2seg(droite u,droite v);
void copy_Poly4(Poly4* src,Poly4* dest,int mal);
point sur_droite(droite d,float fact);
void change_trans(int nb,float fctdep,float fact);
void suppr_Poly4(Poly4* *sprime);
void suppr_Pt_equa(Poly4 *sprime);

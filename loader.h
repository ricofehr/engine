#include <stdio.h>
#include <GL/glut.h>





#define EXIT {fclose(fichier);return;}
#define CTOI(C) (*(int*)&C)	//récupère en int un nombre pointé par un char*

void LoadBMP(char *File,int numtex);
void LoadTextures(const char fichier[]);
void LoadPoly4();
void AffichePoly4();

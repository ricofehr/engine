#define PI float(3.14159265358979323846)
#include <math.h>




typedef struct Vecteur {
	float x,y,z;
}vecteur;
typedef vecteur point;

typedef struct Droite
{
	point orig;
	vecteur dir;
}droite;

struct Poly4{
	GLuint texture;
	GLfloat *pt;
	GLfloat k;
	Vecteur norm;
	Droite *equa;
	int nb;
	int col;
	bool move;
	int polcol;
	int nbfces;
	int mero;
	int numpol;
	int nbangle;
	int nbtrans;
	float ray;
	float rayfc;
	point ctre;
	point ctrefc;

};

struct tst_move{
	GLfloat dt[4];
	point inter[4];
};

vecteur Normal(vecteur v1, vecteur v2);
vecteur setVector(vecteur p1, vecteur p2);
float Magnitude(vecteur normal);
vecteur Normalise(vecteur v);
void rotationpoint(float *x, float *y, float *z,
                   float centrex, float centrey, float centrez,
                   float anglex, float angley, float anglez);
void mult(float a[4], float b[4][4]);
float Dot(vecteur v1, vecteur v2);
double AngleBetweenVectors(vecteur v1, vecteur v2);
double absolu(double x);
int fps();
void view(int ix, int iy);
void deplacement(int direction,int type);
void souris();
void test();

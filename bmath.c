
#include "bmath.h"
#include <stdio.h>
#include <stdlib.h>

extern struct move col;
//extern float camx,camy,camz,vuex,vuey,vuez;
//extern unsigned char keys[];

/* initvector: Init vector coordinates */
struct vector initvector(float xx, float yy, float zz)
{
	struct vector v;
	v.x = xx;
	v.y = yy;
	v.z = zz;
	return v;
}

/* vectorproduct: Vector product => 0 if v1 and v2 are parallell  */
struct vector vectorproduct(struct vector v1, struct vector v2)
{
	struct vector normal;

	normal.x = ((v1.y * v2.z) - (v1.z * v2.y));
	normal.y = ((v1.z * v2.x) - (v1.x * v2.z));
	normal.z = ((v1.x * v2.y) - (v1.y * v2.x));

	return normal;
}

/* setvector: Compute vector between 2 points  */
struct vector setvector(struct point p1, struct point p2)
{
	struct vector v;

	v.x = p1.x - p2.x;
	v.y = p1.y - p2.y;
	v.z = p1.z - p2.z;

	return v;
}

/* magnitude: return Vector size */
double magnitude(struct vector v)
{
	return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

/* normalize: Normalize vector v */
struct vector normalize(struct vector v)
{
	double magn = magnitude(v);
	v.x /= magn;
	v.y /= magn;
	v.z /= magn;
	return v;
}

/* dot: Scalar product between 2 vectors (0 if right angle) */
float dot(struct vector v1, struct vector v2)
{
	return ((v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z));
}

/* Execute a rotation of target point v from origin center with angle rotate */
struct vector rotatepoint(struct vector p, struct point center, struct point rotate)
{
	struct vector result = {0.0f, 0.0f, 0.0f};
	float *matrix = (float *) malloc(4*sizeof(float));
	float matrotox[4][4], matrotoy[4][4], matrotoz[4][4];
	float anglex, angley, anglez;

 	anglex = (rotate.x * PI) / 180;
	angley = (rotate.y * PI) / 180;
	anglez = (rotate.z * PI) / 180;

	/* Rotate Matrix for x, y, z coordinates */
	float matrotx[4][4] = {
		{1, 0, 0, 0},
		{0, (float) cos(anglex), (float) sin(anglex), 0},
		{0, (float) -sin(anglex), (float) cos(anglex), 0},
		{0, 0, 0, 1}
	};

	float matroty[4][4] = {
		{(float) cos(angley), 0, (float) -sin(angley), 0},
		{0, 1, 0, 0},
		{(float) sin(angley), 0, (float) cos(angley), 0},
		{0, 0, 0, 1}
	};

	float matrotz[4][4] = {
		{(float) cos(anglez), (float) sin(anglez), 0, 0},
		{(float) -sin(anglez), (float) cos(anglez), 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1}
	};

	/* Translate point into center landmark */
	result.x = p.x - center.x;
	result.y = p.y - center.y;
	result.z = p.z - center.z;
	/* Matrix multiplication for rotate coordinates */
	matrix[0] = result.x;
	matrix[1] = result.y;
	matrix[2] = result.z;
	matrix[3] = 0;

	if (rotate.x != 0)
		matrixmult(matrix, matrotx);
	if (rotate.y != 0)
		matrixmult(matrix, matroty);
	if (rotate.z != 0)
		matrixmult(matrix, matrotz);

	/* Translate agin point into initial landmark */
	result.x = matrix[0] + center.x;
	result.y = matrix[1] + center.y;
	result.z = matrix[2] + center.z;
	
	return result;
}

/* matrixmult: compute matrix multiplication between a (4x1) and b (4x4) */
void matrixmult(float *a, float b[4][4])
{
	float tmp[4];
	int i;

	for (i = 0; i < 4; i++)
		tmp[i] = a[0] * b[0][i] +
			a[1] * b[1][i] +
			a[2] * b[2][i] +
			a[3] * b[3][i];

	for (i = 0; i < 4; i++)
		a[i] = tmp[i];
}

/* anglevectors: compute angle between 2 vectors v1 and v2 */
double anglevectors(struct vector v1, struct vector v2)
{							
	float dotProduct = dot(v1, v2);				
	float vectorsMagnitude = magnitude(v1) * magnitude(v2) ;
	return acos(dotProduct / vectorsMagnitude);
}

/* Compute inverse of matrice */
float *inversematrix(float *matr)
{
	int i = 0;
	float *result = (float*) malloc(16 * sizeof(float));

	for(i = 0; i < 16; i++) {
		result[i] = matr[(i%4) * 4 + i/4];
	}
    	return result;
}

/* findCenter: compute center between 2 points */
struct point findcenter(struct point p1, struct point p2)
{
	struct point equi = {0.0f, 0.0f, 0.0f};
	equi.x = (p1.x + p2.x)/2;
	equi.y = (p1.y + p2.y)/2;
	equi.z = (p1.z + p2.z)/2;
	return equi;
}

/* computeDistance: Compute distance betwwen 2 points */
float computedist(struct point p1, struct point p2)
{
	return (sqrt(pow((p1.x-p2.x), 2) + pow((p1.y-p2.y), 2) + pow((p1.z-p2.z), 2)));
}

/* absolute: compute absolute of x */
double absolute(double x)
{
	if (x >= 0)
		return (x);
	else
		return (-x);
}

/* fps: compute frame per second current value */
int fps()
{
	static int resultat = 0;
	static int frames = 0;
	static float lastTime = 0;
	float currentTime;

	++frames;
	currentTime = 0 * 0.001f;
	if ((currentTime - lastTime) < 1.0) {
		return resultat;
	} else {
		resultat=frames;
		frames = 0;
		lastTime = currentTime;
		return(resultat);
	}	
}

/*
void view(int ix, int iy)
{
		float cosinus,sinus;
        cosinus=float(absolu(camx-vuex)/sqrt((camx-vuex)*(camx-vuex)+(camz-vuez)*(camz-vuez)));
        sinus=float(absolu(camz-vuez)/sqrt((camx-vuex)*(camx-vuex)+(camz-vuez)*(camz-vuez)));


        rotationpoint(&vuex,&vuey,&vuez,camx,camy,camz,0,ix/2.0f,0);


        if ((camx<=vuex)&&(camz<=vuez))
        rotationpoint(&vuex,&vuey,&vuez,camx,camy,camz,(-iy*sinus)/2.0f,0,(iy*cosinus)/2);
        if ((camx<vuex)&&(camz>vuez))
        rotationpoint(&vuex,&vuey,&vuez,camx,camy,camz,-(-iy*sinus)/2.0f,0,(iy*cosinus)/2);
        if ((camx>vuex)&&(camz>vuez))
        rotationpoint(&vuex,&vuey,&vuez,camx,camy,camz,-(-iy*sinus)/2.0f,0,-(iy*cosinus)/2);
        if ((camx>vuex)&&(camz<vuez))
        rotationpoint(&vuex,&vuey,&vuez,camx,camy,camz,(-iy*sinus)/2.0f,0,-(iy*cosinus)/2);


}



void deplacement(int direction,int type)
{
	collision_cam();
	Vecteur cible;
	Vecteur cam={camx,camy,camz};
	Vecteur vue={vuex,vuey,vuez};
	static float prevTime=0;
	float curTime=0*0.001f;
	static float speed;
	if(curTime!=prevTime) speed=curTime-prevTime;
	prevTime=curTime;
	if(speed>0.02) speed=0.02f;
	cible=setVector(vue,cam);
	if(direction==3) cible=setVector(cam,vue);

	cible=Normalise(cible);
	cible.x=cible.x*10*speed;
	cible.y=cible.y*10*speed;
	cible.z=cible.z*10*speed;
	//printf("dt:%f: %f:%f:%f  magn:%f\n",col.dt[0],col.dt[1],col.dt[2],col.dt[3],Magnitude(cible));
	if(direction==3 && col.dt[1]<=(Magnitude(cible)+0.25))
	{	
		cible.x=-cible.x;
		cible.y=0.0f;
		cible.z=-cible.z;
	}

	if(direction==2)
	{
		cible.y=0; rotationpoint(&cible.x,&cible.y,&cible.z,0,0,0,0,-90,0);
		if(col.dt[3]<(Magnitude(cible)+0.25))
		{
			cible.x=-cible.x;
			cible.y=0.0f;
			cible.z=-cible.z;
		}
	}
	if(direction==4)
	{
		cible.y=0; rotationpoint(&cible.x,&cible.y,&cible.z,0,0,0,0,90,0);
		if(col.dt[2]<(Magnitude(cible)+0.25))
		{
			cible.x=-cible.x;
			cible.y=0.0f;
			cible.z=-cible.z;
		}
	}
	
	
	if(direction==1 && col.dt[0]<(Magnitude(cible)+0.25))
	{	
		cible.x=-cible.x;
		cible.y=0.0f;
		cible.z=-cible.z;
	}

	camx+=cible.x;
	if(type==3) camy+=cible.y;
	camz+=cible.z;
	vuex+=cible.x;
	if(type==3) vuey+=cible.y;
	vuez+=cible.z;
}
*/

/*
void souris()
{
	POINT mousePos;
	GetCursorPos(&mousePos);
	if( (mousePos.x == 320) && (mousePos.y == 240) ) return;
	SetCursorPos(320,240);
	view(320-mousePos.x,240-mousePos.y);
}

void test(){
	if(keys[GLUT_KEY_UP]) deplacement(1,2);
	if(keys[GLUT_KEY_RIGHT]) deplacement(2,2);
	if(keys[GLUT_KEY_DOWN]) deplacement(3,2);
	if(keys[GLUT_KEY_LEFT]) deplacement(4,2);
}
*/

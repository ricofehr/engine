
#include "collisions.h"
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>

extern tst_move col;
extern float camx,camy,camz,vuex,vuey,vuez;
//Renvoie la normale à deux vecteurs 
extern unsigned char keys[];
												
vecteur Normal(vecteur v1, vecteur v2)
{
	vecteur normal;									
												
	normal.x = ((v1.y * v2.z) - (v1.z * v2.y));
	normal.y = ((v1.z * v2.x) - (v1.x * v2.z));
	normal.z = ((v1.x * v2.y) - (v1.y * v2.x));

	return normal;								
}

//Renvoie le vecteur de deux points
vecteur setVector(vecteur p1, vecteur p2)
{
	vecteur v;							

	v.x = p1.x - p2.x;				
	v.y = p1.y - p2.y;					
	v.z = p1.z - p2.z;					

	return v;									
}


// Calcule la norme d'un vecteur
float Magnitude(vecteur normal)
{

	return (float)sqrt( (normal.x * normal.x) + 
						(normal.y * normal.y) + 
						(normal.z * normal.z) );
}


// Change la norme du vecteur à 1

vecteur Normalise(vecteur v)
{
	float magnitude = Magnitude(v);		

	v.x /= magnitude;								
	v.y /= magnitude;								
	v.z /= magnitude;								

	return v;										
}

//********************* ROTATIONPOINT ******************************************ROTATIONPOINT
void rotationpoint(float *x, float *y, float *z,
                   float centrex, float centrey, float centrez,
                   float anglex, float angley, float anglez)
{

	float matrix[4];

	anglex=anglex*PI/180;
	angley=angley*PI/180;
	anglez=anglez*PI/180;

	//Matrices de rotation:
	float matrotx[4][4]=		{{1,0,0,0},
   								{0,float(cos(anglex)),float(sin(anglex)),0},
             					{0,float(-sin(anglex)),float(cos(anglex)),0},
          						{0,0,0,1}};

	float matroty[4][4]=		{{float(cos(angley)),0,float(-sin(angley)),0},
             					{0,1,0,0},
             					{float(sin(angley)),0,float(cos(angley)),0},
             					{0,0,0,1}};

	float matrotz[4][4]= 	    {{float(cos(anglez)),float(sin(anglez)),0,0},
             					{float(-sin(anglez)),float(cos(anglez)),0,0},
             					{0,0,1,0},
             					{0,0,0,1}};

	*x=*x-centrex;
	*y=*y-centrey;
	*z=*z-centrez;
	matrix[0]=*x;
	matrix[1]=*y;
	matrix[2]=*z;
	matrix[3]=0;

	if (anglex!=0) mult(matrix,matrotx);
	if (angley!=0) mult(matrix,matroty);
	if (anglez!=0) mult(matrix,matrotz);

	*x=matrix[0]+centrex;
	*y=matrix[1]+centrey;
	*z=matrix[2]+centrez;
	
}


//************************* MULT ***********************************************MULT
// Multiplication Matricielle
void mult(float a[4], float b[4][4])
{
  float temp[4];                             //A=A*B
  int i;

    for (i = 0; i < 4; i++)
      temp[i] =    a[0] * b[0][i] +
                   a[1] * b[1][i] +
                   a[2] * b[2][i] +
                   a[3] * b[3][i];

  for (i = 0; i < 4; i++)
      a[i] = temp[i];
}



//************************************************************* DOT
// Produit scalaire entre deux vecteur
// Retourne 0 s'il y a angle droit

float Dot(vecteur v1, vecteur v2) 
{

	return ( (v1.x * v2.x) + 
		(v1.y * v2.y) + (v1.z * v2.z) );
}


//*************************************************** AngleBetweenVectors
// Calcule l'angle entre deux vecteurs

double AngleBetweenVectors(vecteur v1, vecteur v2)
{							

	float dotProduct = Dot(v1, v2);				

	float vectorsMagnitude = Magnitude(v1) * Magnitude(v2) ;

	double angle = acos( dotProduct / vectorsMagnitude );
	
	return( angle );
}





//****************************** ABSOLU ****************************************ABSOLU
double absolu(double x)
{
if (x>=0) return (x); else return (-x);
}







//**************************  F P S  *******************************************FPS
int fps()
{
	static int resultat=0;
	static int frames=0;
	static float lastTime=0;
	float currentTime;

	++frames;
	currentTime= 0 * 0.001f;
	if((currentTime-lastTime)<1.0)return resultat;
	else
	{
		//printf("\n");
		resultat=frames;
		frames = 0;
		lastTime = currentTime;
		return(resultat);
    }
	
}



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

void souris()
{
/*	
	POINT mousePos;
	GetCursorPos(&mousePos);
	if( (mousePos.x == 320) && (mousePos.y == 240) ) return;
	SetCursorPos(320,240);
	view(320-mousePos.x,240-mousePos.y);
*/
}

void test(){
	if(keys[GLUT_KEY_UP]) deplacement(1,2);
	if(keys[GLUT_KEY_RIGHT]) deplacement(2,2);
	if(keys[GLUT_KEY_DOWN]) deplacement(3,2);
	if(keys[GLUT_KEY_LEFT]) deplacement(4,2);
}

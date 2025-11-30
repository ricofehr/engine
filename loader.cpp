#include "loader.h"
#include "collisions.h"
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>

extern GLuint *Nom;
extern Poly4 *tabPoly4;
extern Poly4 *Polinit;
extern int nbPoly4;
extern int nbload;
extern tst_move col;
extern int angle;
////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////FONCTION LOADBMP//////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

void LoadBMP(char *File,int numtex)
{
	unsigned char	*Data;
	FILE			*fichier;
	unsigned char	Header[0x36];
	GLuint			DataPos,DataSize;
	GLint			Components;
	GLsizei			Width,Height;
	GLenum			Format,Type;

	//Lit le fichier et son header
	fichier = fopen(File,"rb");
	if (!fichier){
		printf("%s non trouvé\n",File);
		return;
	}
	if (fread(Header,1,0x36,fichier)!=0x36) EXIT;
	if (Header[0]!='B' || Header[1]!='M')	EXIT;
	if (CTOI(Header[0x1E])!=0)				EXIT;
	if (CTOI(Header[0x1C])!=24)				EXIT;

	//Récupère les infos du fichier
	DataPos			= CTOI(Header[0x0A]);
	DataSize		= CTOI(Header[0x22]);
	//Récupère les infos de l'image
	Width			= CTOI(Header[0x12]);
	Height			= CTOI(Header[0x16]);	
	Type = GL_UNSIGNED_BYTE;
	Format = GL_RGB;
	Components = 3;
	
	//!!!!
	if (DataSize==0) DataSize=Width*Height*Components;
	if (DataPos==0)  DataPos=0x36;

	//Charge l'image
	fseek(fichier,DataPos,0);
	Data = new unsigned char[DataSize];
	if (!Data) EXIT;

	if (fread(Data,1,DataSize,fichier)!=DataSize) 
	{
		delete Data;
		fclose(fichier);
		return;
	}
	fclose(fichier);

	//Inverse R et B
	unsigned char t;
	for (int x=0;x<Width*Height;x++) 
	{
		t=Data[x*3];
		Data[x*3]=Data[x*3+2];
		Data[x*3+2]=t;
	}

	//Envoie la texture à OpenGL
	//glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	/*      paramétrage de la texture */
    glBindTexture(GL_TEXTURE_2D,numtex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	
	glTexImage2D
	( 	
		GL_TEXTURE_2D, 	//target
		0,				//mipmap level
		Components,		//nb couleurs
		Width,			//largeur
		Height,			//hauteur
		0,			 	//largeur du bord
		Format,			//type des couleurs
		Type,			//codage de chaque composante
		Data			//Image
	);
    printf("%s\n\n", File);
}

//////////////////////////////////////////////////////////////////////////////////////
////////////////////////LoaderTextures//////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
void LoadTextures(const char fichier[]){
	
	FILE *f;
	int compteur=0;
	int i=0;
	char texture[50];
	
	f = fopen(fichier,"rb"); 
	if(!f) return;
	
	fscanf(f,"%d\n",&compteur);
	if((Nom=(GLuint*)malloc(sizeof(GLuint)*compteur))==NULL) return;
	glGenTextures(compteur+1,Nom);
	while(i < compteur ){
		fscanf(f,"%s",texture) ;
		LoadBMP(texture,i+1);
		i++;
		printf("%d :: %d \n", i, compteur) ;
	}
	fclose(f);
	printf("%d :: %d \n", i, compteur) ;
	return;
}

//////////////////////////////////////////////////////////
///////////////LoaderPolygones////////////////////////////
//////////////////////////////////////////////////////////

void LoadPoly4(){
	
	FILE *f;
	int i=0;
	
	f = fopen("polygones4.txt","rb");
	if(!f){
		printf("polygones4.txt est introuvable\n");
		return;
	}
	fscanf(f,"%d\n",&nbload);
	if((Polinit=(Poly4*)realloc(Polinit,sizeof(Poly4)*nbload))==NULL){
		printf("le malloc a merder\n");
		return;
	}
	
	while(i<nbload) {
		Polinit[i].numpol=0;
		Polinit[i].nb=4;
		Polinit[i].pt=(GLfloat *)malloc(16*sizeof(GLfloat));
		fscanf(f,"%d %f %f %f %f %f %f %f %f %f %f %f %f",
		&Polinit[i].texture,
		&Polinit[i].pt[0],&Polinit[i].pt[1],&Polinit[i].pt[2],
		&Polinit[i].pt[4],&Polinit[i].pt[5],&Polinit[i].pt[6],
		&Polinit[i].pt[8],&Polinit[i].pt[9],&Polinit[i].pt[10],
		&Polinit[i].pt[12],&Polinit[i].pt[13],&Polinit[i].pt[14]);
		Polinit[i].pt[3]=Polinit[i].pt[7]=Polinit[i].pt[11]=1.0f;
		Polinit[i].pt[15]=1.0f;
		Polinit[i].move=0;Polinit[i].col=-1;
		Polinit[i].nbtrans=Polinit[i].nbangle=-1;
		Polinit[i].mero=i;
		Polinit[i].nbfces=6;
		Polinit[i].equa=NULL;
		i++;
	}
	
	fclose(f);
	
	
	return;
}

void AffichePoly4(){
	int i,j;
	for(i=0;i<nbPoly4;i++){
		if(i==1)continue;
		glBindTexture(GL_TEXTURE_2D,Nom[tabPoly4[i].texture]);
		glBegin(GL_QUADS);
		if(tabPoly4[i].texture==2) j=10;
		else j=3;
		glTexCoord2i(j,0);glVertex3f(tabPoly4[i].pt[0],tabPoly4[i].pt[1],tabPoly4[i].pt[2]); 
		glTexCoord2i(j,j);glVertex3f(tabPoly4[i].pt[4],tabPoly4[i].pt[5],tabPoly4[i].pt[6]);
		glTexCoord2i(0,j);glVertex3f(tabPoly4[i].pt[8],tabPoly4[i].pt[9],tabPoly4[i].pt[10]);
		glTexCoord2i(0,0);glVertex3f(tabPoly4[i].pt[12],tabPoly4[i].pt[13],tabPoly4[i].pt[14]);
		glEnd();
		
	}
	return;
}

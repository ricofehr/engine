#include <GL/glut.h>
#include <stdio.h>
#include <unistd.h>
//#include <glext.h>
#include "loader.h"
//#include "fonctionMath.h"
#include "font.h"
#include "collisions.h"

int pres=0,in=0;
int WindowName;
struct tst_move col;
//col.dt=(GLfloat *)malloc(2*sizeof(GLfloat));
int nbMov4=0;
//col.inter=(point *)malloc(2*sizeof(point));
GLuint id_cube;
GLuint id_cube2;
GLuint *Nom=NULL; 
float *angle;
float *varangle;
vecteur *vartrans;
vecteur *trans;
int nbangle;
int nbtrans;
int modeFilDeFer = 0; // définit si on choisit le mode fil de fer ou polygonal
int modeZBuffer  = 1; // définit le mode ZBuffer ON ou OFF
int modeCull     = 0; // définit l'algorithme de masquage des faces ON ou OFF
int lumiere[3]={0,0,0}; // definit si il y a une source de lumiere
int Colli[4]={0,0,0,0};
int identMenuLumiere;   // identité du menu lumiere
int identMenuPrincipal; // identité du menu principal

int LightPos[4] = {-2,2,0,0}; 
int MatSpec [4] = {1,1,1,1};

Poly4 *tabPoly4=NULL;
Poly4 *tabMov4=NULL;
Poly4 *Polinit=NULL;
int nbPoly4=0;
int nbload=0;

//float camx=0.0,camy=20.0,camz=-10.0,vuex=0.0,vuey=0.0,vuez=0.0;

float camx=0.0,camy=40.0,camz=20.0,vuex=0.0,vuey=0.0,vuez=0.0;
//GLfloat *ptcam=(GLfloat *)malloc(16*sizeof(GLfloat));
unsigned char keys[256];


void InitGL(void) ;    // initialisations
void Draw(void) ; // affichage
void Reshape (int w, int h) ; // projection
void Scene(void); //animation
void CreerCube();
void CreerMonde();
void GestionClavier(unsigned char key, int x, int y);
void GestionSpecial(int key, int x, int y);
void ToucheRelachee(int key, int x, int y);
void BoutonSouris(int button, int state, int x, int y);
void MouvementSouris(int x, int y);
void gestionMenuLumiere(int value);
void gestionMenuPrincipal(int value);// gestion du menu principal

//////////////////////////////////////////////////////////////////
/////////////MAIN/////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
int main( int argc, char *argv[ ])
{
     col.dt[0]=100000.0f;
     col.dt[1]=100000.0f;
     LoadPoly4();
     init_angle_trans();
     
     ++in;
     glutInit(&argc, argv); 
     glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE); 
     glutInitWindowSize(1024,780);  //Optionnel  
     WindowName = glutCreateWindow("== Moteur 3D =="); 
     //glutFullScreen(); //Optionnel
     /* Chargement des textures */

     LoadTextures("textures.txt");
     printf("okokoko\n\n");
     InitGL();
     
     glutDisplayFunc(Draw);
     glutReshapeFunc(Reshape);
     glutIdleFunc(Scene);
     //glutKeyboardFunc(GestionClavier);
     //glutSpecialFunc(GestionSpecial);
     //glutSpecialUpFunc(ToucheRelachee);
     //glutMouseFunc(BoutonSouris);
     //glutPassiveMotionFunc(MouvementSouris);
     glutInitDisplayMode(GLUT_DEPTH);// Pour initialiser le Z buffer
     glutMainLoop(); // gestion des événements           
     free(trans);
     free(vartrans);
     free(angle);
     free(varangle);
     suppr_Poly4(&Polinit);
     return 0; 
}

/////////////////////////////////////////////////////////////////
//////////////Creation d'un cube a face colorees/////////////////
/////////////////////////////////////////////////////////////////

void CreerCube(){
     // Définission de la liste de précompilation
     id_cube = glGenLists(1);
     
     // Création de la liste
     glNewList(id_cube, GL_COMPILE);
     glBegin(GL_QUADS); //Et c'est parti pour le cube !
     //glColor3i(1,0,0);
     glTexCoord2i(0,0);glVertex3i(-1,1,-1);
     glTexCoord2i(1,0);glVertex3i(+1,1,-1);
     glTexCoord2i(1,1);glVertex3i(+1,+3,-1); 
     glTexCoord2i(0,1);glVertex3i(-1,+3,-1);
     //face arriere 
     glTexCoord2i(0,0);glVertex3i(-1,1,+1);
     glTexCoord2i(1,0);glVertex3i(+1,1,+1); 
     glTexCoord2i(1,1);glVertex3i(+1,+3,+1); 
     glTexCoord2i(0,1);glVertex3i(-1,+3,+1); 
     // face avant 
     glTexCoord2i(0,0);glVertex3i(+1,1,-1); 
     glTexCoord2i(1,0);glVertex3i(+1,1,+1);
     glTexCoord2i(1,1);glVertex3i(+1,+3,+1);
     glTexCoord2i(0,1);glVertex3i(+1,+3,-1);
     //face droite 
     glTexCoord2i(0,0);glVertex3i(-1,1,-1); 
     glTexCoord2i(1,0);glVertex3i(-1,1,+1); 
     glTexCoord2i(1,1);glVertex3i(-1,+3,+1);
     glTexCoord2i(0,1);glVertex3i(-1,+3,-1); 
     //face gauche 
     glTexCoord2i(1,0);glVertex3i(-1,+3,-1); 
     glTexCoord2i(1,1);glVertex3i(+1,+3,-1); 
     glTexCoord2i(0,1);glVertex3i(+1,+3,+1);
     glTexCoord2i(0,0);glVertex3i(-1,+3,+1);
     //face dessus
     glTexCoord2i(1,0);glVertex3i(-1,1,+1); 
     glTexCoord2i(1,1);glVertex3i(+1,1,+1);
     glTexCoord2i(0,1);glVertex3i(+1,1,-1);
     glTexCoord2i(0,0);glVertex3i(-1,1,-1);
     //face dessous
     glEnd(); 

     glEndList();
}



//////////////////////////////////////////////////////////////////////////
/////////////////////Initialisation d'opengl//////////////////////////////
//////////////////////////////////////////////////////////////////////////

void InitGL(void){

     glClearColor(0.0,0.0,0.0,0); //Change la couleur du fond
     glMaterialiv(GL_FRONT_AND_BACK,GL_SPECULAR,MatSpec);  
     glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,100);
     glEnable(GL_TEXTURE_2D); 
     CreerCube();
     
     
     //SetCursorPos(640,480);
     // création du menu lumiere
     
     identMenuLumiere = glutCreateMenu(gestionMenuLumiere);
     glutAddMenuEntry("Lumiere 1 : ON", 1);
     glutAddMenuEntry("Lumiere 2 : ON", 2);
     glutAddMenuEntry("Lumiere 3 : ON", 3);
     // création du menu principal
     identMenuPrincipal = glutCreateMenu(gestionMenuPrincipal);
     
     glutAddSubMenu("Lumiere", identMenuLumiere);
     glutAddMenuEntry("Mode Fil de fer", 1);
     glutAddMenuEntry("ZBuffer : ON", 2);
     glutAddMenuEntry("Cull : ON", 3);
     glutAddMenuEntry("Quitter", 4);
     glutAttachMenu(GLUT_RIGHT_BUTTON);  // accrochage au bouton droit
     
 }

///////////////////////////////////////////////////////////
//////////////////Affichage////////////////////////////////
///////////////////////////////////////////////////////////
void Draw() 
{   
     int i,mal=0;
     float ray;
     droite diag;
     printf("debutdraw\n");
     if(tabPoly4!=NULL)
     {
     		if(tabMov4!=NULL)suppr_Poly4(&tabMov4);//tabMov4=NULL;
     		tabMov4=(Poly4*)malloc(nbPoly4*sizeof(Poly4));
     		copy_Poly4(tabPoly4,tabMov4,1);
     		nbMov4=nbPoly4;
     }
     mal=0;
     if(tabPoly4!=NULL)suppr_Poly4(&tabPoly4);
     nbPoly4=nbload;
     //tabPoly4=NULL;
     tabPoly4=(Poly4*)malloc(nbload*sizeof(Poly4));
     
     //initialisation de tabPoly4 sur les coords de Polinit
     copy_Poly4(Polinit,tabPoly4,1);
     
     //multiplie les coords par la matrice active et les enregistre ds tabPoly4	
     chge_coord(0,nbPoly4,1);
     diag=centre_polyg(&ray,0);
     for(i=0;i<nbPoly4;i++){
     	trve_norm_k(i,0,diag.orig,ray);
     }
     
     //Efface le frame buffer et le Z-buffer
     glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
     
     // Mode fil de fer ou polygonal
     if(modeFilDeFer) glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
     else glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
     
     // Montre ou efface les faces cachées avec le ZBuffer
     if(modeZBuffer) glEnable(GL_DEPTH_TEST);
     else glDisable(GL_DEPTH_TEST);
     
     // Suppression des faces cachées avec l'algorithme Calimberti-Montanari
     if(modeCull) glEnable(GL_CULL_FACE);
     else glDisable(GL_CULL_FACE);

     
     /*
     glPushMatrix();
     glRotatef(angle[0],0.0,1.0,0.0);
     //glCallList(id_cube);//cube 1
     ajout_cube_coord(1,0,-1);
     glPopMatrix();
     */
     	

     glPushMatrix();
     glTranslatef(trans[0].x,trans[0].y,trans[0].z);
     //glCallList(id_cube);//cube 1
     ajout_cube_coord(1,-1,0);
     glPopMatrix();
     	
     
     glPushMatrix();
     glTranslatef(trans[1].x,trans[1].y,trans[1].z);
     //glCallList(id_cube);//cube 1
     ajout_cube_coord(1,-1,1);
     glPopMatrix();
     	
     /*
     glPushMatrix();
     glRotatef(angle[1],0.0f,1.0f,0.0f);
     glTranslatef(4.0f,0.0f,0.0f);
     //glCallList(id_cube);//cube 1
     ajout_cube_coord(1,1,-1);
     glPopMatrix();
      */
     glPushMatrix();
     glTranslatef(trans[2].x,trans[2].y,trans[2].z);
     ajout_cube_coord(1,-1,2);
     glPopMatrix();
     
     glPushMatrix();
     glTranslatef(trans[3].x,trans[3].y,trans[3].z);
     ajout_cube_coord(1,-1,3);
     glPopMatrix();

     glPushMatrix();
     glTranslatef(trans[4].x,trans[4].y,trans[4].z);
     ajout_cube_coord(1,-1,4);
     glPopMatrix();
     
     
     glPushMatrix();
     glTranslatef(trans[5].x,trans[5].y,trans[5].z);
     ajout_cube_coord(1,-1,5);
     glPopMatrix();
     
     drawFPS();
     //souris();
     //test();
     glLoadIdentity();
     if(tabMov4!=NULL)inter_pol();
     AffichePoly4();
     
     glutSwapBuffers();
     glFlush();
     fflush(NULL);
     gluLookAt(camx,camy,camz,vuex,vuey,vuez,0.0,0.0,1.0);
     usleep(100*1000) ;
     printf("finDraw\n");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////Reshape/////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Reshape(int width, int height) 
{
     printf("debutReshape\n");
     glViewport(0,0,width,height); 
     glMatrixMode(GL_PROJECTION); 
     gluPerspective(45, // angle d'ouverture
       float(width)/float(height), // rapport des axes
       1.0, // distance plan avant
       100.0); // distance plan arrière
     //Pour les explications, lire le tutorial sur OGL et win 
     glMatrixMode(GL_MODELVIEW);
     printf("finReshape\n"); 
}   



//////////////////////////////////////////////////////////////////
////////////////////Scene//////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void Scene()
{
     // attente en ms
     int i;

     if(tabMov4!=NULL){
     	for(i=0;i<nbangle;i++){
     		angle[i]+=varangle[i];
     	}

     	for(i=0;i<nbtrans;i++){
     	  trans[i].x+=vartrans[i].x;
        trans[i].z+=vartrans[i].z;
      }
     }
     glutPostRedisplay();
     //Demande de recalculer la scène 
}
//////////////////////////////////////////////////////////////////
////////////////Gestion menu Lumiere////////////////////////////
//////////////////////////////////////////////////////////////////
void gestionMenuLumiere(int value)
{
      switch(value)
      {
        case 1 : lumiere[0] = !lumiere[0];
     	  if(lumiere[0]){
     			  glutChangeToMenuEntry(1,"Lumiere 1 : OFF",1);
     			  glutChangeToMenuEntry(2,"Lumiere 2 : ON",2);
     			  glutChangeToMenuEntry(3,"Lumiere 3 : ON",3);
     			  lumiere[1]=lumiere[2]=0;
     	  }else glutChangeToMenuEntry(1,"Lumiere 1 : ON",1);
                 break;
        case 2 : lumiere[1] = !lumiere[1];
     	  if(lumiere[1]){
     			  glutChangeToMenuEntry(1,"Lumiere 1 : ON",1);
     			  glutChangeToMenuEntry(2,"Lumiere 2 : OFF",2);
     			  glutChangeToMenuEntry(3,"Lumiere 3 : ON",3);
     			  lumiere[0]=lumiere[2]=0;
     	  }else glutChangeToMenuEntry(2,"Lumiere 2 : ON",2);
                 break;
        case 3 : lumiere[2] = !lumiere[2];
     	  if(lumiere[2]){
     			  glutChangeToMenuEntry(1,"Lumiere 1 : ON",1);
     			  glutChangeToMenuEntry(2,"Lumiere 2 : ON",2);
     			  glutChangeToMenuEntry(3,"Lumiere 3 : OFF",3);
     			  lumiere[0]=lumiere[1]=0;
     	  }else glutChangeToMenuEntry(3,"Lumiere 2 : ON",3);
                 break;
      }
}

//////////////////////////////////////////////////////////////////
////////////////Gestion menu principal////////////////////////////
//////////////////////////////////////////////////////////////////
void gestionMenuPrincipal(int value)
{
      switch(value)
      {
        case 1 : modeFilDeFer = !modeFilDeFer;
                 if(modeFilDeFer) glutChangeToMenuEntry(2,"Mode Polygones", 1);
                 else glutChangeToMenuEntry(2,"Mode Fil De Fer",1);
                 break;
        case 2 : modeZBuffer = !modeZBuffer;
                 if(modeZBuffer) glutChangeToMenuEntry(3,"ZBuffer : OFF",2);
                 else glutChangeToMenuEntry(3,"ZBuffer : ON", 2);
                 break;
        case 3 : modeCull = !modeCull;
                 if(modeCull) glutChangeToMenuEntry(4,"Cull : OFF",3);
                 else glutChangeToMenuEntry(4,"Cull : ON", 3);
                 break;
        case 4 : exit(0);
                 break;
      }
}
///////////////////////////////////////////////////////////////////
///////////////Gestion Clavier et souris//////////////////////////
////////////////////////////////////////////////////////////////////

void GestionClavier(unsigned char key, int x, int y)  
{  
     printf("vous avez appuye sur %c\n",key);  
     printf("position de la souris : ");  
     printf("%d,%d\n",x,y);   
     if (key==27) exit(0);   
} 

void GestionSpecial(int key, int x, int y)  
{  
     switch (key){  
     case GLUT_KEY_F1 : printf("F1\n");break;
     case GLUT_KEY_F2 : printf("F2\n");break; 
     case GLUT_KEY_F3 : printf("F3\n");break; 
     case GLUT_KEY_F4 : printf("F4\n");break; 
     case GLUT_KEY_F5 : printf("F5\n");break;
     case GLUT_KEY_F6 : printf("F6\n");break; 
     case GLUT_KEY_F7 : printf("F7\n");break;
     case GLUT_KEY_F8 : printf("F8\n");break;
     case GLUT_KEY_F9 : printf("F9\n");break; 
     case GLUT_KEY_F10 : printf("F10\n");break;
     case GLUT_KEY_F11 : printf("F11\n");break;
     case GLUT_KEY_F12 : printf("F12\n");break;
     case GLUT_KEY_LEFT : keys[GLUT_KEY_LEFT]=1;break;
     case GLUT_KEY_UP :  keys[GLUT_KEY_UP]=1;break;
     case GLUT_KEY_RIGHT :keys[GLUT_KEY_RIGHT]=1;break; 
     case GLUT_KEY_DOWN : keys[GLUT_KEY_DOWN]=1;break; 
     case GLUT_KEY_PAGE_UP : printf("Page up\n");break;
     case GLUT_KEY_PAGE_DOWN : printf("Page down\n");break; 
     case GLUT_KEY_HOME : printf("Home\n");break; 
     case GLUT_KEY_END : printf("End\n");break; 
     case GLUT_KEY_INSERT : printf("Insert\n");break; 
     }  
}

void ToucheRelachee(int key, int x, int y)  
{  
     switch (key){  
     case GLUT_KEY_F1 : printf("F1\n");break;
     case GLUT_KEY_F2 : printf("F2\n");break; 
     case GLUT_KEY_F3 : printf("F3\n");break; 
     case GLUT_KEY_F4 : printf("F4\n");break; 
     case GLUT_KEY_F5 : printf("F5\n");break;
     case GLUT_KEY_F6 : printf("F6\n");break; 
     case GLUT_KEY_F7 : printf("F7\n");break;
     case GLUT_KEY_F8 : printf("F8\n");break;
     case GLUT_KEY_F9 : printf("F9\n");break; 
     case GLUT_KEY_F10 : printf("F10\n");break;
     case GLUT_KEY_F11 : printf("F11\n");break;
     case GLUT_KEY_F12 : printf("F12\n");break;
     case GLUT_KEY_LEFT : keys[GLUT_KEY_LEFT]=0;break;
     case GLUT_KEY_UP :  keys[GLUT_KEY_UP]=0;break;
     case GLUT_KEY_RIGHT :keys[GLUT_KEY_RIGHT]=0;break; 
     case GLUT_KEY_DOWN : keys[GLUT_KEY_DOWN]=0;break; 
     case GLUT_KEY_PAGE_UP : printf("Page up\n");break;
     case GLUT_KEY_PAGE_DOWN : printf("Page down\n");break; 
     case GLUT_KEY_HOME : printf("Home\n");break; 
     case GLUT_KEY_END : printf("End\n");break; 
     case GLUT_KEY_INSERT : printf("Insert\n");break; 
     }  
}

//quand un bouton de la souris est presser
void BoutonSouris(int button, int state, int x, int y){
     switch(state){
     case GLUT_DOWN : printf("position de la souris : (%d,%d)\n",x,y);
     				 break;
     }
}

void MouvementSouris(int x, int y){
     return;
}

/////////////////////////////////////////////////////////////////////
////////////////////FIN//////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////  


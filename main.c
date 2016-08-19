#include <stdio.h>
#include <unistd.h>
#include "loader.h"
#include "font.h"
#include "collisions.h"
#include "bmath.h"

#if defined __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

int pres=0,in=0;
int wname;
struct move col;
int nbmov4=0;
GLuint id_cube;
GLuint id_cube2;
GLuint *texturenames=NULL;
float *angle;
float *varangle;
struct vector *vartrans;
struct vector *trans;
int cntangle;
int cnttrans;
int modeFilDeFer = 0;
int modeZBuffer  = 1;
int modeCull     = 0; // face hiding
int light[3]={0,0,0};
int colli[4]={0,0,0,0};
int identlight;
int identmenu;

int LightPos[4] = {-2,2,0,0};
int MatSpec [4] = {1,1,1,1};

struct poly4 *tabpoly4=NULL;
struct poly4 *tabmov4=NULL;
struct poly4 *polinit=NULL;
int cntpoly4=0;
int cntload=0;

//float camx=0.0,camy=20.0,camz=-10.0,vuex=0.0,vuey=0.0,vuez=0.0;
float camx=0.0, camy=40.0, camz=20.0, vuex=0.0, vuey=0.0, vuez=0.0;
//GLfloat *ptcam=(GLfloat *)malloc(16*sizeof(GLfloat));
unsigned char keys[256];

void initGL(void);
void draw(void);
void reshape (int w, int h);
void scene(void);
void createcube();
void createworld();
void managekeyboard(unsigned char key, int x, int y);
void managespecial(int key, int x, int y);
void keyup(int key, int x, int y);
void mousebutton(int button, int state, int x, int y);
void mousemove(int x, int y);
void managelight(int value);
void managemenu(int value);

/* main */
int main( int argc, char *argv[ ])
{
	col.dt[0]=100000.0f;
	col.dt[1]=100000.0f;

	loadpoly4();
	initangletrans();
    	++in;
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(1024,780);
	wname = glutCreateWindow("== 3D Engine ==");
	//glutFullScreen(); //Optionnel

	loadtextures("txt/textures.txt");
	initGL();

	glutDisplayFunc(draw);
	glutReshapeFunc(reshape);
	glutIdleFunc(scene);
	//glutKeyboardFunc(managekeyboard);
	//glutSpecialFunc(managespecial);
	//glutSpecialUpFunc(keyup);
	//glutMouseFunc(mousebutton);
	//glutPassiveMotionFunc(mousemove);
	glutInitDisplayMode(GLUT_DEPTH);
	glutMainLoop();

	free(trans);
	free(vartrans);
	free(angle);
	free(varangle);
	deletepoly4(&polinit);
	return 0;
}

/* createcube: create an opengl cube */
void createcube() {
	id_cube = glGenLists(1);
     	glNewList(id_cube, GL_COMPILE);
	glBegin(GL_QUADS);
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
	//face line
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

/* initGL: opengl init */
void initGL(void) {
	glClearColor(0.0, 0.0, 0.0, 0);
	glMaterialiv(GL_FRONT_AND_BACK, GL_SPECULAR, MatSpec);
	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 100);
	glEnable(GL_TEXTURE_2D);
	createcube();

     	identlight = glutCreateMenu(managelight);
	glutAddMenuEntry("Light 1 : ON", 1);
	glutAddMenuEntry("Light 2 : ON", 2);
	glutAddMenuEntry("Light 3 : ON", 3);

	identmenu = glutCreateMenu(managemenu);

	glutAddSubMenu("Light", identlight);
	glutAddMenuEntry("Mode Fil de fer", 1);
	glutAddMenuEntry("ZBuffer : ON", 2);
	glutAddMenuEntry("Cull : ON", 3);
	glutAddMenuEntry("Exit", 4);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

/* draw: Main display function */
void draw()
{
	int i;
	float ray;
	struct line diag;
	printf("begindraw\n");

	if (tabpoly4 != NULL) {
     		if (tabmov4 != NULL)
			deletepoly4(&tabmov4);
     		tabmov4 = (struct poly4*) malloc(cntpoly4*sizeof(struct poly4));
     		copypoly4(tabpoly4, tabmov4, 1);
     		nbmov4 = cntpoly4;
	}

	if(tabpoly4 != NULL)
		deletepoly4(&tabpoly4);
	cntpoly4=cntload;
	tabpoly4 = (struct poly4*) malloc(cntload*sizeof(struct poly4));
	copypoly4(polinit, tabpoly4, 1);
	computecoord(0, cntpoly4, 1);
	diag = centerpolygon(&ray, 0);
	for (i=0; i < cntpoly4; i++) {
		initnormk(i, 0, diag.orig, ray);
	}

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	if(modeFilDeFer)
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	if(modeZBuffer)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	if(modeCull)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	glPushMatrix();
	glTranslatef(trans[0].x, trans[0].y, trans[0].z);
	//glCallList(id_cube);//cube 1
	initcube(1,-1,0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(trans[1].x, trans[1].y, trans[1].z);
	//glCallList(id_cube);//cube 1
	initcube(1,-1,1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(trans[2].x, trans[2].y, trans[2].z);
	initcube(1,-1,2);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(trans[3].x, trans[3].y, trans[3].z);
	initcube(1,-1,3);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(trans[4].x, trans[4].y, trans[4].z);
	initcube(1,-1,4);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(trans[5].x, trans[5].y, trans[5].z);
	initcube(1,-1,5);
	glPopMatrix();

	drawFPS();
	
	glLoadIdentity();
	if(tabmov4 != NULL)
		interpol();
	displaypoly4();
	
	glutSwapBuffers();
	glFlush();
	fflush(NULL);
	gluLookAt(camx, camy, camz, vuex, vuey, vuez, 0.0, 0.0, 1.0);
	usleep(100*1000) ;
	printf("endDraw\n");
}

/* reshape: reshape opengl function */
void reshape(int width, int height)
{
	printf("beginReshape\n");
	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	gluPerspective(45, /* open angle */
		(float) width / (float) height,
		1.0, /* foreground plan distance */
		100.0); /* background plan distance */
	glMatrixMode(GL_MODELVIEW);
	printf("endReshape\n");
}

/* scene: scene opengl function */
void scene()
{
	int i;
	if (tabmov4 != NULL) {
		for(i = 0; i < cntangle; i++) {
			angle[i] += varangle[i];
		}

		for (i = 0; i < cnttrans; i++) {
			trans[i].x += vartrans[i].x;
			trans[i].z += vartrans[i].z;
		}
	}
	glutPostRedisplay();
}

/* managelight: manage light menu */
void managelight(int value)
{
	switch(value) {
	case 1 :
		light[0] = !light[0];
		if (light[0]) {
			glutChangeToMenuEntry(1,"Light 1 : OFF",1);
			glutChangeToMenuEntry(2,"Light 2 : ON",2);
			glutChangeToMenuEntry(3,"Light 3 : ON",3);
			light[1] = light[2] = 0;
		} else {
			glutChangeToMenuEntry(1,"Light 1 : ON",1);
		}
		break;
        case 2 :
		light[1] = !light[1];
		if (light[1]) {
			glutChangeToMenuEntry(1, "Light 1 : ON", 1);
			glutChangeToMenuEntry(2, "Light 2 : OFF", 2);
			glutChangeToMenuEntry(3, "Light 3 : ON", 3);
			light[0] = light[2] = 0;
		} else {
			glutChangeToMenuEntry(2, "Light 2 : ON", 2);
		}
		break;
        case 3 :
		light[2] = !light[2];
		if (light[2]) {
			glutChangeToMenuEntry(1, "Light 1 : ON", 1);
			glutChangeToMenuEntry(2, "Light 2 : ON", 2);
			glutChangeToMenuEntry(3, "Light 3 : OFF", 3);
     			light[0] = light[1] = 0;
		} else {
			glutChangeToMenuEntry(3, "Light 2 : ON", 3);
		}
		break;
	}
}

/* managemenu: manage menu action */
void managemenu(int value)
{
	switch(value) {
	case 1 :
		modeFilDeFer = !modeFilDeFer;
		if (modeFilDeFer)
			glutChangeToMenuEntry(2,"Mode Polygones", 1);
		else
			glutChangeToMenuEntry(2,"Mode Fil De Fer",1);
		break;
        case 2 :
		modeZBuffer = !modeZBuffer;
		if (modeZBuffer)
			glutChangeToMenuEntry(3,"ZBuffer : OFF",2);
		else
			glutChangeToMenuEntry(3,"ZBuffer : ON", 2);
		break;
        case 3 :
		modeCull = !modeCull;
		if (modeCull)
			glutChangeToMenuEntry(4,"Cull : OFF",3);
		else
			glutChangeToMenuEntry(4,"Cull : ON", 3);
		break;
        case 4 :
		exit(0);
		break;
	}
}

/* managekeybaord */
void managekeyboard(unsigned char key, int x, int y)
{
	printf("key %c\n", key);
	printf("mouse position : ");
	printf("%d,%d\n", x, y);
	if (key == 27)
		exit(0);
}

/* managespecial: manage special keybaord buttons */
void managespecial(int key, int x, int y)
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

/* keyup: manage keyboard buttons */
void keyup(int key, int x, int y)
{
	switch (key) {
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

/* mousebutton: manage mouse clicks */
void mousebutton(int button, int state, int x, int y) {
	switch(state){
		case GLUT_DOWN :
			printf("position de la souris : (%d,%d)\n",x,y);
			break;
	}
}

/* mousemove: */
void mousemove(int x, int y){
	return;
}

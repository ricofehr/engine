#include <stdio.h>
#include <unistd.h>
#include "loader.h"
#include "font.h"
#include "collision.h"
#include "bmath.h"

#if defined __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

static int wname;
static int nbmov4=0;
static int modeFilDeFer = 0;
static int modeZBuffer  = 1;
static int modeCull     = 0; /* face hiding */
static int light[3]={0,0,0};
static int identlight;
static int identmenu;
static int MatSpec [4] = {1,1,1,1};

GLuint *texturenames=NULL;
float *angle;
float *varangle;
struct vector *vartrans;
struct vector *trans;
int cntangle;
int cnttrans;
struct move col;
struct poly4 *tabpoly4=NULL;
struct poly4 *tabmov4=NULL;
struct poly4 *polinit=NULL;
int cntpoly4=0;
int cntload=0;

//float camx=0.0,camy=20.0,camz=-10.0,vuex=0.0,vuey=0.0,vuez=0.0;
float camx=0.0, camy=40.0, camz=20.0, vuex=0.0, vuey=0.0, vuez=0.0;
//GLfloat *ptcam=(GLfloat *)malloc(16*sizeof(GLfloat));
static unsigned char keys[256];

void initGL(void);
void draw(void);
void reshape (int w, int h);
void scene(void);
void create_cube();
void create_world();
void control_keyboard(unsigned char key, int x, int y);
void control_special(int key, int x, int y);
void control_key_up(int key, int x, int y);
void control_mouse_click(int button, int state, int x, int y);
void control_mouse_move(int x, int y);
void manage_lights(int value);
void manage_menu(int value);

int main( int argc, char *argv[ ])
{
	col.dt[0]=100000.0f;
	col.dt[1]=100000.0f;

	load_poly4();
	init_transforms();
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(1024,780);
	wname = glutCreateWindow("== 3D Engine ==");
	//glutFullScreen();

	load_textures("txt/textures.txt");
	initGL();

	glutDisplayFunc(draw);
	glutReshapeFunc(reshape);
	glutIdleFunc(scene);
	//glutKeyboardFunc(control_keyboard);
	//glutSpecialFunc(control_special);
	//glutSpecialUpFunc(control_key_up);
	//glutMouseFunc(control_mouse_button);
	//glutPassiveMotionFunc(control_mouse_move);
	glutInitDisplayMode(GLUT_DEPTH);
	glutMainLoop();

	free(trans);
	free(vartrans);
	free(angle);
	free(varangle);
	delete_poly4(&polinit);
	return 0;
}

/**
*	create_cube: create an opengl cube
*
*	Create an opengl cube with 6 faces.
*/

void create_cube() {
	//id_cube = glGenLists(1);
     	//glNewList(id_cube, GL_COMPILE);
	glBegin(GL_QUADS);
	/* Back side */
	glTexCoord2i(0,0);glVertex3i(-1,1,-1);
	glTexCoord2i(1,0);glVertex3i(+1,1,-1);
	glTexCoord2i(1,1);glVertex3i(+1,+3,-1);
	glTexCoord2i(0,1);glVertex3i(-1,+3,-1);
	/* Front side */
	glTexCoord2i(0,0);glVertex3i(-1,1,+1);
	glTexCoord2i(1,0);glVertex3i(+1,1,+1);
	glTexCoord2i(1,1);glVertex3i(+1,+3,+1);
	glTexCoord2i(0,1);glVertex3i(-1,+3,+1);
	/* Right side */
	glTexCoord2i(0,0);glVertex3i(+1,1,-1);
	glTexCoord2i(1,0);glVertex3i(+1,1,+1);
	glTexCoord2i(1,1);glVertex3i(+1,+3,+1);
	glTexCoord2i(0,1);glVertex3i(+1,+3,-1);
	/* Left side */
	glTexCoord2i(0,0);glVertex3i(-1,1,-1);
	glTexCoord2i(1,0);glVertex3i(-1,1,+1);
	glTexCoord2i(1,1);glVertex3i(-1,+3,+1);
	glTexCoord2i(0,1);glVertex3i(-1,+3,-1);
	/* Roof side */
	glTexCoord2i(1,0);glVertex3i(-1,+3,-1);
	glTexCoord2i(1,1);glVertex3i(+1,+3,-1);
	glTexCoord2i(0,1);glVertex3i(+1,+3,+1);
	glTexCoord2i(0,0);glVertex3i(-1,+3,+1);
	/* Floor side */
	glTexCoord2i(1,0);glVertex3i(-1,1,+1);
	glTexCoord2i(1,1);glVertex3i(+1,1,+1);
	glTexCoord2i(0,1);glVertex3i(+1,1,-1);
	glTexCoord2i(0,0);glVertex3i(-1,1,-1);
	glEnd();

	glEndList();
}

/** 
*	initGL - opengl init
*
*	Init opengl windows, menu, lght, and settings
*/

void initGL(void) {
	glClearColor(0.0, 0.0, 0.0, 0);
	glMaterialiv(GL_FRONT_AND_BACK, GL_SPECULAR, MatSpec);
	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 100);
	glEnable(GL_TEXTURE_2D);
	create_cube();

     	identlight = glutCreateMenu(manage_lights);
	glutAddMenuEntry("Light 1 : ON", 1);
	glutAddMenuEntry("Light 2 : ON", 2);
	glutAddMenuEntry("Light 3 : ON", 3);

	identmenu = glutCreateMenu(manage_menu);

	glutAddSubMenu("Light", identlight);
	glutAddMenuEntry("Mode Fil de fer", 1);
	glutAddMenuEntry("ZBuffer : ON", 2);
	glutAddMenuEntry("Cull : ON", 3);
	glutAddMenuEntry("Exit", 4);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

/**
*	draw - Main display function
*
*	Display function triggered when opengl must display again all polygons
*/

void draw()
{
	int i;
	float ray;
	struct line diag;
	printf("begindraw\n");

	if (tabpoly4 != NULL) {
     		if (tabmov4 != NULL)
			delete_poly4(&tabmov4);
     		tabmov4 = (struct poly4*) malloc(cntpoly4*sizeof(struct poly4));
     		copy_poly4(tabpoly4, tabmov4);
     		nbmov4 = cntpoly4;
	}

	if(tabpoly4 != NULL)
		delete_poly4(&tabpoly4);
	cntpoly4=cntload;
	tabpoly4 = (struct poly4*) malloc(cntload*sizeof(struct poly4));
	copy_poly4(polinit, tabpoly4);
	compute_coord(0, cntpoly4);
	diag = polygon_get_diag(&ray, 0);
	for (i=0; i < cntpoly4; i++) {
		init_norm_k(i, diag.orig, ray);
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
	init_cube(1,-1,0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(trans[1].x, trans[1].y, trans[1].z);
	init_cube(1,-1,1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(trans[2].x, trans[2].y, trans[2].z);
	init_cube(1,-1,2);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(trans[3].x, trans[3].y, trans[3].z);
	init_cube(1,-1,3);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(trans[4].x, trans[4].y, trans[4].z);
	init_cube(1,-1,4);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(trans[5].x, trans[5].y, trans[5].z);
	init_cube(1,-1,5);
	glPopMatrix();

	draw_fps();
	
	glLoadIdentity();
	if(tabmov4 != NULL)
		manage_collisions();
	display_poly4();
	
	glutSwapBuffers();
	glFlush();
	fflush(NULL);
	gluLookAt(camx, camy, camz, vuex, vuey, vuez, 0.0, 0.0, 1.0);
	usleep(100*1000) ;
	printf("endDraw\n");
}

/**
*	reshape	- reshape opengl function
*	
*	Triggered when opengl shape the opengl view
*/

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

/**
*	scene - scene opengl function
*
*	Compute new polygons coordinates before display again them
*/

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

/**
*	manage_lights - manage lights menu
*	@value: flag for change lights properties
*
*	Manage changes about opengl lights in the window.
*/

void manage_lights(int value)
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

/**
*	manage_menu - manage menu action
*	@value: flag to change
*
*	Manage a change triggered from the window menu.
*/

void manage_menu(int value)
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

/** 
*	control_keyboard - manages keyboard buttons 
*	@key: button pressed
*	@x: position x of the cursor
*	@y: position y of the cursor
*
*	Execute actions for special characters pressed. Special characters are
*	F1 to F12, arrows and page buttons.
*/

void control_keyboard(unsigned char key, int x, int y)
{
	printf("key %c\n", key);
	printf("mouse position : ");
	printf("%d,%d\n", x, y);
	if (key == 27)
		exit(0);
}

/** 
*	control_special - manages special keyboard buttons 
*	@key: button pressed
*	@x: position x of the cursor
*	@y: position y of the cursor
*
*	Execute actions for special characters pressed. Special characters are
*	F1 to F12, arrows and page buttons.
*/

void control_special(int key, int x, int y)
{
	switch (key){
	case GLUT_KEY_F1 : printf("F1\n"); break;
	case GLUT_KEY_F2 : printf("F2\n"); break;
	case GLUT_KEY_F3 : printf("F3\n"); break;
	case GLUT_KEY_F4 : printf("F4\n"); break;
	case GLUT_KEY_F5 : printf("F5\n"); break;
	case GLUT_KEY_F6 : printf("F6\n"); break;
	case GLUT_KEY_F7 : printf("F7\n"); break;
	case GLUT_KEY_F8 : printf("F8\n"); break;
	case GLUT_KEY_F9 : printf("F9\n"); break;
	case GLUT_KEY_F10 : printf("F10\n"); break;
	case GLUT_KEY_F11 : printf("F11\n"); break;
	case GLUT_KEY_F12 : printf("F12\n"); break;
	case GLUT_KEY_LEFT : keys[GLUT_KEY_LEFT]=1; break;
	case GLUT_KEY_UP :  keys[GLUT_KEY_UP]=1; break;
	case GLUT_KEY_RIGHT :keys[GLUT_KEY_RIGHT]=1; break;
	case GLUT_KEY_DOWN : keys[GLUT_KEY_DOWN]=1; break;
	case GLUT_KEY_PAGE_UP : printf("Page up\n"); break;
	case GLUT_KEY_PAGE_DOWN : printf("Page down\n"); break;
	case GLUT_KEY_HOME : printf("Home\n"); break;
	case GLUT_KEY_END : printf("End\n"); break;
	case GLUT_KEY_INSERT : printf("Insert\n"); break;
	}
}

/**
*	control_mouse_click - manage mouse clicks
*	@button: button pressed on the mouse
*	@state: status of button (up, down)
*	@x: position x of the mouse cursor
*	@y: position y of the mouse cursor
*
*	Analyse the button pressed and execute action associated.
*/

void control_mouse_click(int button, int state, int x, int y) {
	switch(state){
		case GLUT_DOWN :
			printf("position de la souris : (%d,%d)\n",x,y);
			break;
	}
}

/** 
*	control_mouse_move - manage mouse moves
*	@x: position x of the mouse cursor
*	@y: position y of the mouse cursor
*/

void control_mouse_move(int x, int y){
	return;
}

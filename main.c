/*
* Main function who launchs 3d engine
* @author Eric Fehr (ricofehr@nextdeploy.io, @github: ricofehr)
*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include "loader.h"
#include "font.h"
#include "polygon.h"
#include "bmath.h"

#if defined __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

static int wname;
static int mode_Zbuffer  = 1;
static int mat_spec[4] = {1,1,1,1};

/* Camera view */
static float camx=15.0, camy=6.0, camz=-21.0, vuex=0.0, vuey=3.0, vuez=0.0;

void initGL(void);
void draw(void);
void reshape(int w, int h);
void scene(void);

int main(int argc, char *argv[])
{
	int cnt, num;
	char *p;
	long conv;

	/* cnt_cubes can be override with optional parameter */
	if (argc >= 2) {
		errno = 0;
		conv = strtol(argv[1], &p, 10);
		if (errno != 0 || *p != '\0' || conv > INT_MAX) {
			printf("Bad cubes count parameter\n");
			exit(1);
		}

		cnt_cubes = (int)conv;
		if (cnt_cubes > 256)
			cnt_cubes = 256;
	}
	cnt = cnt_cubes;

	/* The third parameter disable textures */
	if (argc == 3)
		mode_grid = 1;

	/* Init world */
	init_transforms();
	load_universe();
	while (cnt--) {
		init_cube(1);
	}

	/* Init opengl viewport */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1024,780);
	wname = glutCreateWindow("== 3D Engine ==");
	load_textures("txt/textures.txt");
	initGL();

	/* Draw triggers */
	glutDisplayFunc(draw);
	glutReshapeFunc(reshape);
	glutIdleFunc(scene);
	glutInitDisplayMode(GLUT_DEPTH);
	/* gl loop */
	glutMainLoop();

	/* free memory */
	free_transforms();
	destroy_polygons();

	return 0;
}

/**
*	initGL - opengl init
*
*	Init opengl windows, menu, lght, and settings
*/

void initGL(void) {
	glClearColor(0.0, 0.0, 0.0, 0);
	glMaterialiv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_spec);
	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 100);
	glEnable(GL_TEXTURE_2D);
}

/**
*	draw - Main display function
*
*	Display function triggered when opengl must display again all polygons
*/

void draw()
{
	if (debug)
		printf("beginDraw\n");

	next_hop();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	display_polygons();
	if (debug)
		draw_fps();
	glLoadIdentity();
	gluLookAt(camx, camy, camz, vuex, vuey, vuez, 0.0f, 1.0f, 0.0f);

	glFlush();
	fflush(NULL);
	glutSwapBuffers();
	usleep(70*1000) ;

	if (debug)
		printf("endDraw\n");
}

/**
*	reshape	- reshape opengl function
*
*	Triggered when opengl shape the opengl view
*/

void reshape(int width, int height)
{
	if (debug)
		printf("beginReshape\n");

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, /* open angle */
		(float) width / (float) height,
		1.0, /* foreground plan distance */
		100.0); /* background plan distance */

	if (mode_Zbuffer)
		glEnable(GL_DEPTH_TEST);

	if (debug)
		printf("endReshape\n");
}

/**
*	scene - scene opengl function
*
*	Compute new polygons coordinates before display again them
*/

void scene()
{
	glutPostRedisplay();
}

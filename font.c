/*
* Manage texts display on opengl viewport
* @author Eric Fehr (ricofehr@nextdeploy.io, @github: ricofehr)
*/

#include <stdio.h>
#include <string.h>
#include "font.h"

/**
*	draw_string: display a string
*	@s: string to display
*
*	Display a string at current position of cursor
*/

static void draw_string(char *s)
{
	unsigned int i;

	for (i = 0; i < strlen(s); i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, s[i]);
}

/**
*	draw_string_big - display a string in big size font
*	@s: string to display
*
*	Display a string with bigger font at current position of cursor
*/

static void draw_string_big(char *s)
{
	unsigned int i;

	for (i = 0; i < strlen(s); i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24 , s[i]);
}

/**
*	move_2dtext - move text cursor on x,y coordinates into 3d viewport
*	@x: position x of cursor
*	@y: position y of cursor
*
*	Move current cursor at x and y positions
*/

static void move_2dtext(int x, int y)
{
	glPushAttrib(GL_TRANSFORM_BIT | GL_VIEWPORT_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glViewport(x - 1, y - 1, 2, 2);
	glRasterPos4f(0.0f, 0.0f, 0.0f, 1.0f);

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glPopAttrib();
}

/**
*	write_2dtext: display a string into the opengpl viewport
*	@x: the x position of the text
*	@y: the y position of the text
*	@chaine: the string to display
*
*	Display string into opengpl viewport.
*/

static void write_2dtext(int x, int y, const char *chaine,...)
{
	char texte[256];
	va_list argsPtr;

	va_start(argsPtr, chaine);
	vsprintf(texte, chaine, argsPtr);
	va_end(argsPtr);

	move_2dtext(x, y);
	draw_string_big(texte);
}

/**
*	draw_fps - Display FPS on the screen
*
*	Display the count of frame per second (FPS) on the opengl viewport.
*/

void draw_fps()
{

	glDisable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);
	write_2dtext(500, 600, "FPS : %d",fps());
	glEnable(GL_TEXTURE_2D);
}

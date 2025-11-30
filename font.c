#include <stdio.h>
#include <string.h>
#include "font.h"

/* drawString: display a string s in opengl */
void drawString (char *s)
{
  unsigned int i;
  for (i = 0; i < strlen(s); i++)
    glutBitmapCharacter (GLUT_BITMAP_HELVETICA_10, s[i]);
};

/* drawStringBig: display a string s in opengl with big size font */
void drawStringBig (char *s)
{
  unsigned int i;
  for (i = 0; i < strlen(s); i++)
    glutBitmapCharacter (GLUT_BITMAP_TIMES_ROMAN_24 , s[i]);
};

/* move2dText: move text cursor on x,y coordiantes into 3d viewport */
void move2dText(int x, int y)
{

	glPushAttrib(GL_TRANSFORM_BIT | GL_VIEWPORT_BIT) ;

	glMatrixMode(GL_PROJECTION) ;
	glPushMatrix() ;
	glLoadIdentity() ;

	glMatrixMode(GL_MODELVIEW) ;
	glPushMatrix() ;
	glLoadIdentity() ;

	y = 480 - 40 - y ;

	glViewport(x-1, y-1, 0, 0) ;
	glRasterPos4f(0.0f, 0.0f, 0.0f, 1.0f) ;

	glPopMatrix() ;
	glMatrixMode(GL_PROJECTION) ;
	glPopMatrix() ;

	glPopAttrib() ;
}

/* glText2d: get a string chaine and display it into opengl veiwport with x,y coordinates */
void glText2d(int x, int y, const char *chaine,...){

	char texte[256] ;
	va_list argsPtr ;

	va_start(argsPtr, chaine) ;
	vsprintf(texte, chaine, argsPtr) ;
	va_end(argsPtr) ;

	move2dText(x, y) ;
	drawStringBig (texte);
}

/* Display FPS on the screen */
void drawFPS(){

	glDisable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f) ;
	glText2d(400, 275, "FPS : %d",fps()) ;
	glColor3f(1.0f, 0.0f, 0.0f) ;
	glText2d(750, 275, "VIE : %d",100) ;
	glColor3f(1.0f, 1.0f, 1.0f) ;
	glEnable(GL_TEXTURE_2D);
}

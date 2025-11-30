#include "loader.h"
#include <stdio.h>
#include <stdlib.h>

/* loadBMP: load a bmp texture from File, and with indtex index */
void loadBMP(char *filename, int indtex)
{
	unsigned char	*data;
	FILE			*bmp;
	unsigned char	header[0x36];
	/* Data position and size */
	GLuint			dpos, dsize, rgb;
	GLint			components;
	GLsizei			width, height;
	GLenum			format, type;
	int             x = 0;
	unsigned char   t;

	/* Read the file and his header */
	bmp = fopen(filename, "rb");
	if (!bmp) {
		printf("%s not found\n", filename);
		return;
	}
	/* Read and ensure the validity of the header */
	if (fread(header, 1, 0x36, bmp) != 0x36)
		EXIT;
	if (header[0] != 'B' || header[1] != 'M')
		EXIT;
	if (CTOI(header[0x1E]) != 0)
		EXIT;
	if (CTOI(header[0x1C]) != 24)
		EXIT;

	/* Load image details from header file */
	dpos = CTOI(header[0x0A]);
	dsize = CTOI(header[0x22]);
	width = CTOI(header[0x12]);
	height = CTOI(header[0x16]);	
	type = GL_UNSIGNED_BYTE;
	format = GL_RGB;
	rgb = 3;
	
	/* If dsize or dpos are missing, init them */
	if (dsize == 0)
		dsize = width * height * components;
	if (dpos == 0)
		dpos = 0x36;

	/* Load bmp image */
	fseek(bmp, dpos, 0);
	data = (unsigned char *) malloc(dsize*sizeof(unsigned char));
	if (!data)
		EXIT;

	if (fread(data, 1, dsize, bmp) != dsize) {
		free(data);
		data = NULL;
		fclose(bmp);
		return;
	}
	fclose(bmp);

	/* Inverse Red and Blue value */
	for (x = 0; x < width*height; x++) {
		t = data[x*3];
		data[x*3] = data[x*3+2];
		data[x*3+2] = t;
	}

	/* Texture settings */
	glBindTexture(GL_TEXTURE_2D, indtex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	/* Send texture to opengl */
	glTexImage2D
	( 	
		GL_TEXTURE_2D, 	//target
		0,				//mipmap level
		rgb,		    //colors count
		width,			//width
		height,			//height
		0,			 	//border padding
		format,			//colors type
		type,			//coding type
		data			//Image
	);

	/* debug purpose */
	printf("%s\n\n", filename);
}

/* loadtextures: load textures used by engine */
void loadtextures(const char filename[]){
	
	FILE *f;
	int cnt = 0;
	int i = 0;
	char texture[50];
	
	f = fopen(filename, "rb");
	if (!f)
		return;
	/* Get texture count and load them */	
	fscanf(f, "%d\n", &cnt);
	if ((texturenames = (unsigned int*) malloc(sizeof(unsigned int) * cnt)) == NULL)
		return;

	glGenTextures(cnt+1, texturenames);
	while (i < cnt) {
		fscanf(f, "%s", texture) ;
		loadBMP(texture, i+1);
		i++;
	}
	fclose(f);
	return;
}

/* loadpoly4: load coordiantes of quad polygon from txt file */
void loadpoly4() {

	FILE *f;
	int i = 0, j = 0;
	
	f = fopen("txt/polygones4.txt", "rb");
	if (!f) {
		printf("txt/polygones4.txt is not found\n");
		return;
	}
    	/* read count of polygons */
	fscanf(f, "%d\n", &cntload);
	if ((polinit = (struct poly4*) realloc(polinit, sizeof(struct poly4)*cntload)) == NULL){
		printf("Malloc issue\n");
		return;
	}

	/* init each quad with coordinates */
	while (i < cntload) {
		polinit[i].indpol = 0;
		/* Sides number of quad */
		polinit[i].cntsides = 4;
		/* 3 coordinates (x, y, z) for each point + transparent value */
		polinit[i].pt = (GLfloat *) malloc(16 * sizeof(GLfloat));
		for (j = 0; j < 16; j++)
            		polinit[i].pt[j] = 1.0f;
		/* Read quad coordiantes from file (let transparent with init 1.0 value) */
		fscanf(f, "%d %f %f %f %f %f %f %f %f %f %f %f %f",
		    &polinit[i].texture,
		    &polinit[i].pt[0], &polinit[i].pt[1], &polinit[i].pt[2],
		    &polinit[i].pt[4], &polinit[i].pt[5], &polinit[i].pt[6],
		    &polinit[i].pt[8], &polinit[i].pt[9], &polinit[i].pt[10],
		    &polinit[i].pt[12], &polinit[i].pt[13], &polinit[i].pt[14]);
		/* Move and collision flags */
		polinit[i].move = 0;
		polinit[i].col = -1;
		/* Translation and angle number */
		polinit[i].indtrans = polinit[i].indangle = -1;
		/* Quad index */
		polinit[i].indpol = i;
		polinit[i].cntfaces = 6;
		/* Equation direction */
		polinit[i].equa = NULL;
		i++;
	}
	
	fclose(f);
	return;
}

/* displaypoly4: Shape quad polygons in opengl */
void displaypoly4(){
	int i,j;
	for (i = 0; i < cntpoly4; i++) {
		if (i == 1)
			continue;

		if (tabpoly4[i].texture == 2)
			j = 10;
		else
			j = 3;
		/* Set texture */
		glBindTexture(GL_TEXTURE_2D, texturenames[tabpoly4[i].texture]);
		/* Shape quad polygon */
		glBegin(GL_QUADS);
		glTexCoord2i(j, 0);
		glVertex3f(tabpoly4[i].pt[0], tabpoly4[i].pt[1], tabpoly4[i].pt[2]);
		
		glTexCoord2i(j, j);
		glVertex3f(tabpoly4[i].pt[4], tabpoly4[i].pt[5], tabpoly4[i].pt[6]);
		
		glTexCoord2i(0, j);
		glVertex3f(tabpoly4[i].pt[8], tabpoly4[i].pt[9], tabpoly4[i].pt[10]);
		
		glTexCoord2i(0, 0);
		glVertex3f(tabpoly4[i].pt[12], tabpoly4[i].pt[13], tabpoly4[i].pt[14]);
		glEnd();
	}
	return;
}

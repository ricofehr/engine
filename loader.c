/*
* File loader functions
* @author Eric Fehr (ricofehr@nextdeploy.io, @github: ricofehr)
*/

#include "loader.h"
#include "polygon.h"
#include <stdio.h>
#include <stdlib.h>

/* Global variables used by loader */
extern unsigned int *texturenames;
extern struct polygon *poly_head;
extern int debug;

/**
*	load_bmp - load a bmp texture
*	@filename: texture file
*	@indtex: polygon index targetting for the texture
*
*	Load a texture from a bmp file and apply it on the polygon
*	indexed by indtex
*/

static void load_bmp(char *filename, int indtex)
{
	unsigned char *data;
	FILE *bmp;
	unsigned char header[0x36];
	/* Data position and size */
	GLuint dpos, dsize, rgb;
	GLint components;
	GLsizei width, height;
	GLenum format, type;
	int x = 0;
	unsigned char t;

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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	/* Send texture to opengl */
	glTexImage2D
	(
		GL_TEXTURE_2D, 	/* target */
		0,		/* mipmap level */
		rgb,		/* colors count */
		width,		/* width */
		height,		/* height */
		0,		/* border padding */
		format,		/* colors type */
		type,		/* coding type */
		data		/* Image */
	);

	/* debug purpose */
	if (debug)
		printf("Texture filename: %s\n", filename);
}

/**
*	load_textures - load textures used by engine
*	@filename:  txt files who list textures used for polygons
*
*	Open a txt file and get the texture filenames. Then, load these thanks
*	to load_bmp function
*/

void load_textures(const char filename[])
{
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
	for (i = 0; i < cnt; i++) {
		fscanf(f, "%s\n", texture) ;
		load_bmp(texture, i+1);
	}
	fclose(f);
	return;
}

/**
*	load_poly4 - load polygons from txt file
*
*	Open a file a laod polygons coordinates from this.
*/

void load_universe()
{
	FILE *f;
	int i = 0, j = 0, cntload, texture;
	struct polygon *current, *poly;
	GLfloat pts[16];

	f = fopen("txt/universe.txt", "rb");
	if (!f) {
		printf("txt/universe.txt is not found\n");
		return;
	}
    	/* read count of polygons */
	fscanf(f, "%d\n", &cntload);

	current = poly_head;
	/* init each quad with coordinates */
	for (i = 0; i < cntload; i++) {
		/* Alpha (opacity) = 1 */
		for (j = 3; j < 16; j+=4)
            		pts[j] = 1.0f;
		/* Read quad coordiantes from file (let transparent with init 1.0 value) */
		fscanf(f, "%d %f %f %f %f %f %f %f %f %f %f %f %f",
		    &texture,
		    &pts[0], &pts[1], &pts[2],
		    &pts[4], &pts[5], &pts[6],
		    &pts[8], &pts[9], &pts[10],
		    &pts[12], &pts[13], &pts[14]);

		poly = polygon_init(i, 4, 0, 0, 6, i, texture, pts);
		if (current == NULL) {
			poly->next = NULL;
			poly->prev = NULL;
			poly_head = poly;
		} else {
			poly->next = poly_head;
			poly->prev = current;
			poly_head->prev = poly;
			current->next = poly;
		}
		current = poly;
	}

	fclose(f);
	return;
}

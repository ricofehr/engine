/*
* Some math helpers function
* @author Eric Fehr (ricofehr@nextdeploy.io, @github: ricofehr)
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bmath.h"

/**
*	matrix_mult - compute matrix multiplication
*	@a: a 4x1 matrix
*	@b: a 4x4 matrix
*
*	Compute a multiplication between a (4x1) and b (4x4)
*/

void matrix_mult(float *a, float b[4][4])
{
	float tmp[4];
	int i;

	for (i = 0; i < 4; i++)
		tmp[i] = a[0] * b[0][i] +
			 a[1] * b[1][i] +
			 a[2] * b[2][i] +
			 a[3] * b[3][i];

	for (i = 0; i < 4; i++)
		a[i] = tmp[i];
}

/**
*	matrix_inverse - compute inverse of matrice
*	@matr: targetted matrix
*
*	Compute inverse of matr
*/

float *matrix_inverse(const float *matr)
{
	int i = 0;
	float *result = (float*) malloc(16 * sizeof(float));

	for(i = 0; i < 16; i++) {
		result[i] = matr[(i%4) * 4 + i/4];
	}
    	return result;
}

/**
*	fps - frame per second
*
*	Compute the frame per second current value
*/

int fps()
{
	static int resultat = 0;
	static int frames = 0;
	static int last_time = 0;
	int current_time;

	++frames;
	current_time = (int)time(NULL);
	if ((current_time - last_time) < 1.0) {
		return resultat;
	} else {
		resultat = frames;
		frames = 0;
		last_time = current_time;
		return resultat;
	}
}

/**
*	gl_matrix_mult - Compute 2 matrix in opengl version
*	@a: first matrix
*	@b: second matrix
*
*	Compute a multiplication of 2 matrix with opengl types
*/

void gl_matrix_mult(GLfloat *a, GLfloat *b)
{
	GLfloat tmp[16];
	int i=0, j=0, k=0, m=0;

	while (m < 16) {
		tmp[m] = 0.0f;
		for(i=k; i < k+16; i+=4) {
			tmp[m] += a[i] * b[j];
			j++;
		}

		if(k!=3) {
			j -= 4;
			k++;
		} else {
			k = 0;
		}
		++m;
	}

	for (i = 0; i < 16; i++)
		b[i] = tmp[i];
}

/**
*	inverse_matr - compute inverse of matrix
*	@mat: matrix targetted
*
*	Compute inverse a matrix with opengl types
*/

GLfloat *inverse_matr(GLfloat *mat)
{
	GLfloat* ret, *zero = NULL;
	int i, j, k = 0, row;
	float inv[4][8], tmp[8], multiple, divisor;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 8; j++) {
			if (j < 4)
				inv[i][j] = mat[k++];
			else if ((j - 4) == i)
				inv[i][j] = 1;
			else
				inv[i][j] = 0;
		}
	}

	for (row = 0; row < 4; row++) {
		if (inv[row][row] == 0) {
			k = row;
			for (i = 0; i < 8; i++)
				tmp[i] = inv[row][i];
			while (!inv[k][row] && k < 4)
				++k;
			if(k == 4)
				return zero;

			for (i = 0; i < 8; i++)
				inv[row][i] = inv[k][i];
			for (i = 0; i < 8; i++)
				inv[k][i] = tmp[i];
        	}

		divisor =inv[row][row];
		for (j = 0; j < 8; j++)
			inv[row][j]/=divisor;

		for (i = 0; i < 3; i++) {
			if (i != row) {
				multiple = inv[i][row];
				for (j = 0; j < 8; j++)
					inv[i][j]-=multiple * inv[row][j];
			}
		}
	}

	ret=(GLfloat*)malloc(16*sizeof(GLfloat));
	for (i = 0; i < 16; i++)
		ret[i] = inv[i/4][i%4+4];
	return ret;
}


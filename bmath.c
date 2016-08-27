
#include "bmath.h"
#include <stdio.h>
#include <stdlib.h>

/**
*	vector_init - Init vector coordinates
*	@xx: x coordinate of the vector
*	@yy: y coordinate of the vector
*	@zz: z coordiante of the vector
*
*	Init a new vector struct and return this
*/

struct vector vector_init(float xx, float yy, float zz)
{
	struct vector v;

	v.x = xx;
	v.y = yy;
	v.z = zz;
	return v;
}

/**
*	vector_product -  vector product 
*	@v1: first vector
*	@v2: second vector
*
*	Compute the vector product between v1 and v2.
*	Return 0 if v1 and v2 are parallells
*/

struct vector vector_product(struct vector v1, struct vector v2)
{
	struct vector normal;

	normal.x = ((v1.y * v2.z) - (v1.z * v2.y));
	normal.y = ((v1.z * v2.x) - (v1.x * v2.z));
	normal.z = ((v1.x * v2.y) - (v1.y * v2.x));
	return normal;
}

/**
*	vector_set - Init vector with 2 points
*	@p1: first point
*	@p2: second point
*
*	Init a vector struct with p1 and p2 points
*/

struct vector vector_set(struct point p1, struct point p2)
{
	struct vector v;

	v.x = p1.x - p2.x;
	v.y = p1.y - p2.y;
	v.z = p1.z - p2.z;
	return v;
}

/**
*	vector_magnitude - return Vector size
*	@v: vector targetted
*
*	Compute size of v
*/

double vector_magnitude(struct vector v)
{
	return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

/**
*	vector_normalize - return a normalized vector
*	@v: vector targetted
*
*	Compute a normalized vector from v
*/

struct vector vector_normalize(struct vector v)
{
	double magn = vector_magnitude(v);

	v.x /= magn;
	v.y /= magn;
	v.z /= magn;
	return v;
}

/**
*	vector_dot - Scalar product between 2 vectors
*	@v1: first vector
*	@v2: second vector
*
*	Compute the scalar product between 2 vectors.
*	Return 0 if right angle
*/

float vector_dot(struct vector v1, struct vector v2)
{
	return ((v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z));
}

/**
*	matrix_mult - compute matrix multiplication
*	@a: a 4x1 matrix
*	@b: a 4x4 matrix
*
*	Compute a multiplication between a (4x1) and b (4x4)
*/

static void matrix_mult(float *a, float b[4][4])
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
*	Execute a rotation of point 
*	@p: rotate vector
*	@center: rotate center
*	@rotate: point to rotate
*
*	Make a rotation of rotate and return vector from origin center
*/

struct vector point_rotate(struct vector p, struct point center, struct point rotate)
{
	struct vector result = {0.0f, 0.0f, 0.0f};
	float *matrix = (float *) malloc(4*sizeof(float));
	float matrotox[4][4], matrotoy[4][4], matrotoz[4][4];
	float anglex, angley, anglez;

 	anglex = (rotate.x * PI) / 180;
	angley = (rotate.y * PI) / 180;
	anglez = (rotate.z * PI) / 180;

	/* Rotate Matrix for x, y, z coordinates */
	float matrotx[4][4] = {
		{1, 0, 0, 0},
		{0, (float) cos(anglex), (float) sin(anglex), 0},
		{0, (float) -sin(anglex), (float) cos(anglex), 0},
		{0, 0, 0, 1}
	};

	float matroty[4][4] = {
		{(float) cos(angley), 0, (float) -sin(angley), 0},
		{0, 1, 0, 0},
		{(float) sin(angley), 0, (float) cos(angley), 0},
		{0, 0, 0, 1}
	};

	float matrotz[4][4] = {
		{(float) cos(anglez), (float) sin(anglez), 0, 0},
		{(float) -sin(anglez), (float) cos(anglez), 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1}
	};

	/* Translate point into center landmark */
	result.x = p.x - center.x;
	result.y = p.y - center.y;
	result.z = p.z - center.z;
	/* Matrix multiplication for rotate coordinates */
	matrix[0] = result.x;
	matrix[1] = result.y;
	matrix[2] = result.z;
	matrix[3] = 0;

	if (rotate.x != 0)
		matrix_mult(matrix, matrotx);
	if (rotate.y != 0)
		matrix_mult(matrix, matroty);
	if (rotate.z != 0)
		matrix_mult(matrix, matrotz);

	/* Translate agin point into initial landmark */
	result.x = matrix[0] + center.x;
	result.y = matrix[1] + center.y;
	result.z = matrix[2] + center.z;
	
	return result;
}

/**
*	vector_angle - compute angle between vectors
*	@v1: first vector
*	@v2: second vector
*
*	Compute angle between 2 vectors v1 and v2
*/

double vector_angle(struct vector v1, struct vector v2)
{							
	float dotProduct = vector_dot(v1, v2);				
	float vectorsMagnitude = vector_magnitude(v1) * vector_magnitude(v2);

	return acos(dotProduct / vectorsMagnitude);
}

/**
*	matrix_inverse - compute inverse of matrice
*	@matr: targetted matrix
*
*	Compute inverse of matr
*/

static float *matrix_inverse(const float *matr)
{
	int i = 0;
	float *result = (float*) malloc(16 * sizeof(float));

	for(i = 0; i < 16; i++) {
		result[i] = matr[(i%4) * 4 + i/4];
	}
    	return result;
}

/**
*	segment_center - compute center on a segment
*	@p1: first edge on the segment
*	@p2: second edge on the segment
*
*	Compute the center point between two edges
*/

struct point segment_center(struct point p1, struct point p2)
{
	struct point equi = {0.0f, 0.0f, 0.0f};

	equi.x = (p1.x + p2.x)/2;
	equi.y = (p1.y + p2.y)/2;
	equi.z = (p1.z + p2.z)/2;
	return equi;
}

/**
*	segment_dist - compute segment distance
*	@p1: first edge of the segment
*	@p2: second edge of the segment
*
*	Compute the distance between 2 points
*/

float segment_dist(struct point p1, struct point p2)
{
	return (sqrt(pow((p1.x-p2.x), 2) + pow((p1.y-p2.y), 2) + pow((p1.z-p2.z), 2)));
}

/**
*	absolute - compute absolute
*	@x: number to compute
*
*	Return absolute value of x
*/

double absolute(double x)
{
	if (x >= 0)
		return (x);
	else
		return (-x);
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
	static float lastTime = 0;
	float currentTime;

	++frames;
	currentTime = 0 * 0.001f;
	if ((currentTime - lastTime) < 1.0) {
		return resultat;
	} else {
		resultat=frames;
		frames = 0;
		lastTime = currentTime;
		return(resultat);
	}	
}

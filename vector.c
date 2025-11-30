/*
* Manage vector, line and point objects
* @author Eric Fehr (ricofehr@nextdeploy.io, @github: ricofehr)
*/

#include <stdio.h>
#include <stdlib.h>
#include "vector.h"
#include "bmath.h"

extern inline double absolute(double x);


/**
*	vector_is_same_dir - check if v1 and v2 are the same direction
*	@v1: first vector
*	@v2: second vector
*
*	Return 1 if v1 and v2 are in same direction
*/

static int vector_is_same_dir(struct vector v1, struct vector v2)
{
    if (v1.x * v2.x < 0 || v1.y * v2.y < 0 || v1.z * v2.z < 0)
	    return 0;
    else
	    return 1;
}


/**
*	vector_product -  vector product
*	@v1: first vector
*	@v2: second vector
*
*	Compute the vector product between v1 and v2.
*	Return 0 if v1 and v2 are parallells
*/

static struct vector vector_product(struct vector v1, struct vector v2)
{
	float x, y, z;

	x = ((v1.y * v2.z) - (v1.z * v2.y));
	y = ((v1.z * v2.x) - (v1.x * v2.z));
	z = ((v1.x * v2.y) - (v1.y * v2.x));
	return vector_init(x, y, z);
}

/**
*	vector_magnitude - return Vector size
*	@v: vector targetted
*
*	Compute size of v
*/

static double vector_magnitude(struct vector v)
{
	return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

/**
*	vector_normalize - return a normalized vector
*	@v: vector targetted
*
*	Compute a normalized vector from v
*/

static struct vector vector_normalize(struct vector v)
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

static float vector_dot(struct vector v1, struct vector v2)
{
	return ((v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z));
}

/**
*	vector_rotate - execute a rotation of point
*	@p: rotate vector
*	@center: rotate center
*	@rotate: point to rotate
*
*	Make a rotation of rotate and return vector from origin center
*/

static struct vector vector_rotate(struct vector p, struct point center, struct point rotate)
{
	float x, y, z;
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

	/* Matrix multiplication for rotate coordinates */
	matrix[0] = p.x - center.x;
	matrix[1] = p.y - center.y;
	matrix[2] = p.z - center.z;
	matrix[3] = 0;

	if (rotate.x != 0)
		matrix_mult(matrix, matrotx);
	if (rotate.y != 0)
		matrix_mult(matrix, matroty);
	if (rotate.z != 0)
		matrix_mult(matrix, matrotz);

	/* Translate agin point into initial landmark */
	x = matrix[0] + center.x;
	y = matrix[1] + center.y;
	z = matrix[2] + center.z;

	return vector_init(x, y, z);
}

/**
*	vector_get_angle - compute angle between vectors
*	@v1: first vector
*	@v2: second vector
*
*	Compute angle between 2 vectors v1 and v2
*/

static double vector_get_angle(struct vector v1, struct vector v2)
{
	float dotProduct = vector_dot(v1, v2);
	float vectorsMagnitude = vector_magnitude(v1) * vector_magnitude(v2);

	return acos(dotProduct / vectorsMagnitude);
}

/**
*	vector_to_matrix - create a translation matrix
*	@u: the translation vector
*	@fact: the factor translation
*
*	Compute a translation matrix from vector u and a
*	factor multiple
*/

static GLfloat *vector_to_matrix(struct vector u, float fact)
{
	int i;
	GLfloat *ret = (GLfloat*) malloc(16*sizeof(GLfloat));

	u.x = u.x * fact;
	u.y = u.y * fact;
	u.z = u.z * fact;
	for (i=0; i < 16; i++)
		ret[i] = 0.0f;

	ret[0] = ret[5] = ret[10] = ret[15] = 1.0f;
	ret[12] = u.x;
	ret[13] = u.y;
	ret[14] = u.z;
	return ret;
}

/**
*	line_compute_point - return a point on a line
*	@d: line targetted
*	@fact: factor of the point location beside vector line
*
*	Return a point into line d with position fact
*/

static struct point vector_translate(struct vector v, struct point p, float fact)
{
	struct point ret;

	ret.x = p.x + fact*v.x;
	ret.y = p.y + fact*v.y;
	ret.z = p.z + fact*v.z;
	return ret;
}

/**
*	segment_center - compute center on a segment
*	@s: segment targetted
*
*	Compute the center point of segment
*/

static struct point segment_center(segment s)
{
	struct point equi = {0.0f, 0.0f, 0.0f};

	equi.x = (s.orig.x + s.orig2.x)/2;
	equi.y = (s.orig.y + s.orig2.y)/2;
	equi.z = (s.orig.z + s.orig2.z)/2;
	return equi;
}

/**
*	segment_magnitude - compute segment distance
*	@s: segment targetted
*
*	Compute the segment magnitude
*/

static float segment_magnitude(segment s)
{
	return (sqrt(pow((s.orig.x-s.orig2.x), 2)
	             + pow((s.orig.y-s.orig2.y), 2)
	             + pow((s.orig.z-s.orig2.z), 2)));
}

/**
*	intersec_segments - compute the intersection point between segments
*
*	Compute the intersection point between segments u and v
*/

static struct point segment_intersec(segment u, segment v)
{
	struct point inter = { 100000, 100000, 100000 };
	float t1, dd;
	struct vector uv = vector_set(v.orig, u.orig);
	struct vector uv_prod = u.dir.vector_product(u.dir, v.dir);
	struct vector uv_prod_v = uv.vector_product(uv, v.dir);

	t1 = uv_prod_v.vector_dot(uv_prod_v, uv_prod);
	dd = uv_prod.vector_magnitude(uv_prod);
	if (!dd)
		return inter;
	t1 /= (dd * dd);
	if(t1 >= 0 && t1 <= 1)
		return u.line_compute_point(u, t1);
	else
		return inter;
}

/**
*	intersec_line - return an intersection point
*	@d: first line
*	@e: second line
*
*	Return the intersection point between segment d with line e
*	If no intersection point, return an infinity point {100000, 1000000, 100000}
*/

static struct point segment_intersec_line(segment d, struct line e)
{
	struct point inter = {100000.0f, 100000.0f, 100000.0f};
	struct point tmp = {0.0f, 0.0f, 0.0f}, tmp2 = {0.0f, 0.0f, 0.0f};
	struct vector v = vector_set(d.orig, e.orig);
	struct vector de = vector_set(d.orig2, e.orig);
	struct vector vtmp;
	float dist, d1, d2, dtmp;
	float dist2 = de.vector_magnitude(de);
	float i, j, k, l;
	float me = e.dir.vector_magnitude(e.dir);
	float md = d.dir.vector_magnitude(d.dir);
	double interv = 0.0f;

	i = j = k = l = 0.0f;

	/* Swap dist and v if needed */
	if (dist2 < v.vector_magnitude(v)) {
		dist2 = v.vector_magnitude(v);
		v = de;
	}
	dist = v.vector_magnitude(v);
	if (me > dist)
		return inter;

	d1 = 0.1f * md;
	d2 = 0.1f * me;
	interv = sqrt(d1*d1 + d2*d2);
	/* inter is between k factor and l factor on e */
	do {
		tmp = e.line_compute_point(e, j);
		vtmp = vector_set(tmp, e.orig);
		dtmp = vtmp.vector_magnitude(vtmp);
		if (!k && dtmp > dist + 0.2 * me)
			k = j;
		if (k && dtmp > dist2) {
			l = j;
			break;
		}
		j += 0.2;
	} while (1);

	/* find inter on d */
	while (i<1.00) {
		j=k;
		tmp = d.line_compute_point(d, i);
		while (j<l) {
			tmp2 = e.line_compute_point(e, j);
			vtmp = vector_set(tmp, tmp2);
			/* if distance between the 2 points is very very low, it's the one */
			if(vtmp.vector_magnitude(vtmp) <= interv)
				return tmp;
			j += 0.1;
		}
		i += 0.05;
	}
	return inter;
}

/**
*	is_on_segment - check a point on a segment
*	@d: segment targetted
*	@p: point targetted
*
*	return 1 if p is on d (from orig to vector dist size), else return 0
*/

static int segment_contains(segment d, struct point p)
{
	float u = 0.0f;
	struct vector v = vector_set(p, d.orig);

	while (u <= 1.00f) {
		if (absolute(v.x - u*d.dir.x) < 0.05 * d.dir.x &&
			absolute(v.y - u*d.dir.y) < 0.05 * d.dir.y &&
			absolute(v.z - u*d.dir.z) < 0.05 * d.dir.z) {
			return 1;
		}
		u += 0.025;
	}
	return 0;
}

/**
*	line_compute_point - return a point on a line
*	@d: line targetted
*	@fact: factor of the point location beside vector line
*
*	Return a point into line d with position fact
*/

static struct point line_compute_point(struct line d, float fact)
{
	struct point ret;

	ret.x = d.orig.x + fact*d.dir.x;
	ret.y = d.orig.y + fact*d.dir.y;
	ret.z = d.orig.z + fact*d.dir.z;
	return ret;
}

/**
*	vector_init - Init vector coordinates
*	@xx: x coordinate of the vector
*	@yy: y coordinate of the vector
*	@zz: z coordiante of the vector
*
*	Init a new vector struct and return this
*/

struct vector vector_init(float x, float y, float z)
{
	struct vector v;

	v.x = x;
	v.y = y;
	v.z = z;

	v.vector_is_same_dir = vector_is_same_dir;
	v.vector_product = vector_product;
	v.vector_magnitude = vector_magnitude;
	v.vector_normalize = vector_normalize;
	v.vector_dot = vector_dot;
	v.vector_get_angle = vector_get_angle;
	v.vector_rotate = vector_rotate;
	v.vector_to_matrix = vector_to_matrix;
	v.vector_translate = vector_translate;

	return v;
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
	return vector_init(p2.x - p1.x, p2.y - p1.y, p2.z - p1.z);
}

segment segment_init(struct point orig, struct point orig2)
{
	segment s;

	s.orig = orig;
	s.orig2 = orig2;
	s.dir = vector_set(orig, orig2);

	s.segment_center = segment_center;
	s.segment_magnitude = segment_magnitude;
	s.segment_intersec = segment_intersec;
	s.segment_intersec_line = segment_intersec_line;
	s.segment_contains = segment_contains;
	s.line_compute_point = line_compute_point;

	return s;
}

struct line line_init(struct point orig, struct vector dir)
{
	struct point orig2 = {orig.x + dir.x, orig.y + dir.y, orig.z + dir.z};

	return segment_init(orig, orig2);
}


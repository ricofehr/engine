/*
* Manage polygons object: init, transforms, collisions, ...
* @author Eric Fehr (ricofehr@nextdeploy.io, @github: ricofehr)
*/

#include "polygon.h"
#include <unistd.h>
#include <time.h>

/* Global vars used by collision functions */
extern struct polygon *poly_head;
extern unsigned int *texturenames;
extern inline double absolute(double x);
extern int debug;
extern int cnt_cubes;
extern int mode_grid;

static struct vector *init_trans;
static struct vector *init_vartrans;
static int is_matrix = 0;
#define INDPOL_UNIVERSE 0

/* Default coordinates for the 6 faces of a cube */
static GLfloat cube[6][16] = {
        {-0.5f, 1.0f, -0.5f, 1.0f, 0.5f, 1.0f, -0.5f, 1.0f, 0.5f, 0.0f, -0.5f, 1.0f, -0.5f, 0.0f, -0.5f, 1.0f},
        {-0.5f, 1.0f, 0.5f, 1.0f, 0.5f, 1.0f, 0.5f, 1.0f, 0.5f, 0.0f, 0.5f, 1.0f, -0.5f, 0.0f, 0.5f, 1.0f},
        {0.5f, 1.0f, -0.5f, 1.0f, 0.5f, 1.0f, 0.5f, 1.0f, 0.5f, 0.0f, 0.5f, 1.0f, 0.5f, 0.0f, -0.5f, 1.0f},
        {-0.5f, 1.0f, -0.5f, 1.0f, -0.5f, 1.0f, 0.5f, 1.0f, -0.5f, 0.0f, 0.5f, 1.0f, -0.5f, 0.0f, -0.5f, 1.0f},
	{-0.5f, 1.0f, -0.5f, 1.0f, 0.5, 1.0f, -0.5f, 1.0f, 0.5f, 1.0f, 0.5f, 1.0f, -0.5f, 1.0f, 0.5f, 1.0f},
	{-0.5f, 0.0f, -0.5f, 1.0f, 0.5, 0.0f, -0.5f, 1.0f, 0.5f, 0.0f, 0.5f, 1.0f, -0.5f, 0.0f, 0.5f, 1.0f},
};

/**
*	init_transforms - generates translation transforms
*
*	Generates ramdomly on the fly translations transform arrays.
*	init_trans is the array who defines initial cube placement in opengl viewport
*	init_vartrans is the array who defines current translation between two hop moves
*/

void init_transforms() {
	int i, r;
	GLfloat x, y, z;
	int x0, z0;

	init_trans = (struct vector *) malloc(cnt_cubes * sizeof(struct vector));
	init_vartrans = (struct vector *) malloc(cnt_cubes * sizeof(struct vector));

	srand(time(NULL));
	for (i = 0; i < cnt_cubes; i++) {
		r = rand();

		x = rand() % 70 * 0.01f;
		x = x < 0.02 ? x + 0.04f : x;
		x = (r % 2 == 0) ? -x : x;
		x = (i % 34 == 0) ? 0 : x;

		y = rand() % 30 * 0.01f;
		y = y < 0.02 ? y + 0.04f : y;
		y = (i % 17 == 0) ? y : 0.0f;

		z = rand() % 70 * 0.01f;
		z = z < 0.02 ? z + 0.04f : z;
		z = (r % 2 == 0) ? -z : z;
		z = (i % 34 == 0) ? 0 : z;

		x0 = rand() % 15;
		x0 = (x0 % 2 == 0) ? -x0 : x0;
		z0 = rand() % 15;
		z0 = (z0 % 2 == 0) ? -z0 : z0;

		init_trans[i] = vector_init(x0 + x, (r % 5) * 2.0f, z0 + z);
		init_vartrans[i] = vector_init(x, y, z);
	}
}

/**
*	free_transforms - Destroy init_trans and init_vartrans global arrays
*
*	Free memory allocated for init_trans and init_vartrans global arrays
*/

void free_transforms()
{
	free(init_trans);
	free(init_vartrans);
	init_trans = NULL;
	init_vartrans = NULL;
}

/**
*	intersec_quad - intersection between an line and a polygon
*       @poly: the polygon targetted
*	@e: the line targetted
*
*	Compute intersection point between one line e and one plan
*	==== Not used currently
*/

static struct point poly_intersec_quad(const struct polygon *poly, struct line e)
{
	struct point inter = {10000,10000,10000};
	struct line dr;
	float me = e.dir.vector_magnitude(e.dir), dist, u;
	struct vector v = vector_set(poly->equa[2].orig, poly->equa[0].orig);
	double angles = poly->equa[0].dir.vector_get_angle(poly->equa[0].dir, e.dir);
	double angle2 = poly->equa[0].dir.vector_get_angle(poly->equa[0].dir, v);

	if (!e.dir.vector_is_same_dir(e.dir, v))
		return inter;
	if (angles > PI/2 || angles < 0) {
		return inter;
	} else if (angles <= absolute(angle2)) {
		dr = poly->equa[1];
		dist = poly->equa[0].dir.vector_magnitude(poly->equa[0].dir);
	} else if (angles != PI/2) {
		dr = poly->equa[2];
		angles = PI/2 - angles;
		dist = poly->equa[3].dir.vector_magnitude(poly->equa[3].dir);
	}

    	if (angles == PI/2) {
		dr = poly->equa[3];
        	if (me <= dr.dir.vector_magnitude(dr.dir)) {
			inter = e.orig2;
        	} else {
			return inter;
		}
	} else if(!angles) {
		dr = poly->equa[0];
		if (me <= dr.dir.vector_magnitude(dr.dir))
			inter = e.orig2;
		else
			return inter;
	} else {
		dist = dist / cos(angles);
		u = dist / me;
		if (u < 1.0)
			return inter;
		inter = e.line_compute_point(e, u);
	}

	if (dr.dir.x)
		u = (inter.x - dr.orig.x) / dr.dir.x;
	else if(dr.dir.y)
		u = (inter.y - dr.orig.y) / dr.dir.y;
	else if(dr.dir.z)
		u = (inter.z - dr.orig.z) / dr.dir.z;

	if (u >= 0.0 &&  u <= 1.0)
		return inter;

	inter.x = 100000.0f;
	inter.y = 100000.0f;
	inter.z = 100000.0f;
    	return inter;
}

/**
*	poly_contains - check if a point is on a polygon
*	@poly: polygon targetted
*	@inter: point targetted
*
*	return 1 if point inter is on cpt face, else return 0
*	==== Not used currently
*/

static int poly_contains(struct polygon *poly, struct point inter)
{
	struct line d;
	struct point tmp = {poly->pts[0], poly->pts[1], poly->pts[2]};
	struct point tmp2;
	struct vector vtmp;

	vtmp = vector_set(inter, tmp);
	tmp2 = poly->poly_intersec_quad(poly, line_init(tmp, vtmp));
	if (tmp2.x != 100000)
		return 1;
	else
		return 0;
}

/**
*	intersec - Compute the intersect point with a polygon
*	@poly: polygon targetted
*	@d: line targetted
*
*	Compute the intersection point between a line (d.orig, d.dir) and poly
*	==== Not used currently
*/

static struct point poly_intersec(struct polygon *poly, struct line d)
{
	struct point inter = {100000.0f, 100000.0f, 100000.0f};
	struct vector v1, v2, vcm, norm, vctr;
	struct point ctr;
	GLfloat u, k;
	struct point p[3];
	int i = 0, j;

	vcm = d.dir;
	ctr = d.orig;
	vctr = vector_init(ctr.x, ctr.y, ctr.z);
	/* Parse the 4 points of the cpt face */
	for(j = 0; j < 3; j++) {
		p[j].x = (float) poly->pts[i++];
		p[j].y = (float) poly->pts[i++];
		p[j].z = (float) poly->pts[i++];
		i++;
	}
	v1 = vector_set(p[0], p[1]);
	v2 = vector_set(p[2], p[1]);
	norm = v1.vector_product(v1, v2);
	if (!vcm.vector_dot(vcm, norm))
		return inter;

	k = norm.vector_dot(norm, vector_init(p[0].x, p[0].y, p[0].z));
	u = (k - vctr.vector_dot(vctr, norm)) / vcm.vector_dot(vcm, norm);
	inter = d.line_compute_point(d, u);
	if (poly->poly_contains(poly, inter))
		return inter;

	inter.x = 100000.0f;
	inter.y = 100000.0f;
	inter.z = 100000.0f;
	return inter;
}

/**
*	intersec_3_plans - intersection point between 3 plans
*	@n: the 3 vectors array
*	@k: the 3 k-norm array
*
*	Compute intersction point between 3 plans with theirs translation
*	vector and theirs k norm.
*	==== Not used currently
*/

static struct point intersec_3_plans(struct vector *n, float *k)
{
	int i;
	GLfloat m[16];
	GLfloat *inv;
	struct point ret,inter={10000,10000,10000};

	for (i=0; i < 12; i+=4) {
		m[i] = n[i/4].x;
		m[i+1] = n[i/4].y;
		m[i+2] = n[i/4].z;
		m[i+3] = -k[i/4];
	}

	for (i=12; i < 15; i++)
		m[i]=0.0f;
	m[15]=1.0f;

	inv = inverse_matr(m);
	if (inv == NULL)
		return inter;
	ret.x = inv[3];
	ret.y = inv[7];
	ret.z = inv[11];
	free(inv);

	return ret;
}

/**
*	pl_compute_coord - compute coordinates with current active matrix
*	@poly: polygon targetted
*
*	Compute opengl polygon poly with current active matrix
*/

static void poly_compute_coord(struct polygon *poly)
{
	GLfloat *tabfl;

	if (poly->move) {
		tabfl = poly->vartrans.vector_to_matrix(poly->vartrans, poly->distcol);
		gl_matrix_mult(tabfl, poly->pts);
		free(tabfl);
	}

	if (poly->col != NULL) {
		poly->vartrans.x *= -1;
		poly->vartrans.y *= -1;
		poly->vartrans.z *= -1;
		poly->oldcol = poly->col;
	}

	poly->col = NULL;
	poly->distcol = 1.0f;
}

/**
*	init_cube: init a cube with coordinate and transform values
*	@moved: cube is moving or not (0/1)
*
*	Create a new cube into tabpoly4 global arrays and init his properties
*/

void init_cube(int move)
{
	struct polygon *current, *poly;
	GLfloat *tabfl;
	int indpol, i, j, cnt = 0, texture;
	GLfloat fact[64] = {0.25f, 1.0f, 1.0f, 0.5f, 2.0f, 0.5f, 0.25f, 2.0f,
			    0.25f, 0.5f, 0.5f, 0.5f, 0.25f, 0.5f, 0.25f, 0.5f,
			    0.25f, 0.5f, 0.5f, 0.25f, 0.25f, 0.5f, 0.5f, 0.5f,
			    0.5f, 0.5f, 0.5f, 0.5f, 0.25f, 0.5f, 0.25f, 0.5f,
			    0.25f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.25f, 0.5f,
			    0.5f, 0.5f, 1.0f, 0.5f, 1.0f, 0.5f, 0.25f, 0.5f,
			    0.25f, 0.5f, 1.0f, 0.25f, 0.25f, 0.5f, 0.5f, 0.5f,
			    1.0f, 0.5f, 0.5f, 0.5f, 0.25f, 0.5f, 0.25f, 0.5f};
	GLfloat t[16];

	indpol = 0;
	current = poly_head;
	if (current != NULL && current->next != NULL) {
		for (;current->next != poly_head; cnt++)
			current = current->next;
	}

	if (current != NULL)
		indpol = current->indpol + 1;

    	/* Init 6 faces for a cube */
	for (i = 0; i < 6; i++) {
		/* Init cube points */
		for (j = 0; j < 16; j++) {
			if ((j+1) % 4)
				t[j] = cube[i][j] * fact[indpol % 64];
			else
				t[j] = 1.0f;
		}

		texture = indpol % 2;
		if (init_vartrans[indpol % cnt_cubes].y != 0.0f)
			texture = 5;

		poly = polygon_init(cnt++, 4, move, indpol,
		                    6, i, texture, t);

		tabfl = poly->trans.vector_to_matrix(poly->trans, 1.0f);
		gl_matrix_mult(tabfl, poly->pts);
		free(tabfl);

		if (current != NULL) {
			current->next = poly;
			poly->prev = current;
			poly->next = poly_head;
			poly_head->prev = poly;
		} else {
			poly->prev = NULL;
			poly->next = NULL;
			poly_head = poly;
		}
		current = poly;
	}
}

/**
*	poly_pivot_collision - Pivot collision function for a polygon
*	@poly: polygon targetted
*
*	This function detect for each polygon of the universe if there will be a collision
*	with the polygon poly.
*/

static void poly_pivot_collision(struct polygon *poly)
{
	struct polygon *current;
	int indpol = -1;

	/* Loop on the polygon list but avoid same polygon and last collision one */
	current = poly_head;
	do {
		if ((current != poly->oldcol || (current->indpol != 0 && current->oldcol != poly)) &&
		    current->indpol != poly->indpol &&
		    (indpol == INDPOL_UNIVERSE || current->indpol != indpol))
			poly->poly_detect_collision(poly, current);

		indpol = current->indpol;
		current = current->next;
	} while (current != poly_head);
}

/**
*	poly_detect_collision - Detect a collision between polygons
*	@poly1: first polygon
*	@poly2: second polygon
*
*	Detect a collision between 2 polygons and set properties for manage this
*/

static void poly_detect_collision(struct polygon *poly1, struct polygon *poly2)
{
	struct polygon *oldcol1;
	struct polygon *oldcol2;
	struct polygon *samepol;
	int indpol = -1;
	int fact = 1, fact2 = 0;
	double distcol = 0;
	/* First polygon point (x,y,z) and dimensions (h,w,d) */
	GLfloat x1, y1, z1, h1, w1, d1;
	GLfloat x2, y2, z2, h2, w2, d2;

	x1 = poly1->pts[0];
	y1 = poly1->pts[1];
	z1 = poly1->pts[2];
	x2 = poly2->pts[0];
	y2 = poly2->pts[1];
	z2 = poly2->pts[2];

	h1 = poly1->pts[13] - y1;
	w1 = poly1->pts[4] - x1;
	d1 = 0.0f;
	if (poly1->next != NULL)
		d1 = poly1->next->pts[2] - z1;

	/* indpol=0 => universe, specific case */
	if (poly2->indpol != 0) {
		h2 = poly2->pts[13] - y2;
		w2 = poly2->pts[4] - x2;
		d2 = 0.0f;
		if (poly2->next != NULL)
			d2 = poly2->next->pts[2] - z2;
	} else {
		switch (poly2->indfaces) {
		/* Front */
		case 0:
			h2 = poly2->pts[13] - y2;
			w2 = poly2->pts[4] - x2;
			d2 = 10.0f;
			z2 -= 10.0f;
			break;
		/* Back */
		case 1:
			h2 = poly2->pts[13] - y2;
			w2 = poly2->pts[4] - x2;
			d2 = 10.0f;
			break;
		/* Left Side */
		case 2:
			w2 = 10.0f;
			d2 = poly2->pts[6] - z2;
			h2 = poly2->pts[13] - y2;
			break;
		/* Right Side */
		case 3:
			w2 = 10.0f;
			x2 -= 10.0f;
			d2 = poly2->pts[6] - z2;
			h2 = poly2->pts[13] - y2;
			break;
		/* Top */
		case 4:
			h2 = -10.0f;
			y2 += 10.0f;
			w2 = poly2->pts[4] - x2;
			d2 = poly2->pts[14] - z2;
			break;
		/* Roof */
		case 5:
			h2 = -10.0f;
			w2 = poly2->pts[4] - x2;
			d2 = poly2->pts[14] - z2;
			break;
		}
	}

	/* 1/10 precision */
	while (fact < 11) {
		x1 += 0.1 * poly1->vartrans.x;
		y1 += 0.1 * poly1->vartrans.y;
		z1 += 0.1 * poly1->vartrans.z;
		x2 += 0.1 * poly2->vartrans.x;
		y2 += 0.1 * poly2->vartrans.y;
		z2 += 0.1 * poly2->vartrans.z;
		if ((x2 >= x1 + w1)      /* Too much at right */
		    || (x2 + w2 <= x1) /* Too much at left */
		    || (y2 + h2 >= y1) /* Too much at top */
		    || (y2 <= y1 + h1)  /* Too much at bottom */
		    || (z2 >= z1 + d1)   /* Too much at back */
		    || (z2 + d2 <= z1)) { /* Too much at front */
		    	fact++;
			continue;
		} else {
			x1 -= 0.1 * poly1->vartrans.x;
			y1 -= 0.1 * poly1->vartrans.y;
			z1 -= 0.1 * poly1->vartrans.z;
			x2 -= 0.1 * poly2->vartrans.x;
			y2 -= 0.1 * poly2->vartrans.y;
			z2 -= 0.1 * poly2->vartrans.z;

			/* 1/100 precision */
			fact2 = 1;
			while (1) {
				x1 += 0.001 * poly1->vartrans.x;
				y1 += 0.001 * poly1->vartrans.y;
				z1 += 0.001 * poly1->vartrans.z;
				x2 += 0.001 * poly2->vartrans.x;
				y2 += 0.001 * poly2->vartrans.y;
				z2 += 0.001 * poly2->vartrans.z;
				if ((x2 >= x1 + w1)
		    			|| (x2 + w2 <= x1)
		    			|| (y2 + h2 >= y1)
		    			|| (y2 <= y1 + h1)
		    			|| (z2 >= z1 + d1)
		    			|| (z2 + d2 <= z1)) {
						++fact2;
						continue;
				}

				break;
			}
			distcol = 0.1 * (fact - 1) + 0.001 * (fact2 - 1);

			if (debug) {
				printf("%d:%d:%f:", poly1->indpol, poly2->indpol, distcol);
				printf("%.1f:%.1f:%.1f:%.1f:%.1f:%.1f:", x1, y1, z1, w1, h1, d1);
				printf("%.1f:%.1f:%.1f:%.1f:%.1f:%.1f\n", x2, y2, z2, w2, h2, d2);
			}

			if ((poly1->col == NULL || distcol < poly1->distcol) &&
			    (poly2->col == NULL || distcol < poly2->distcol)) {
				oldcol1 = poly1->col;
				oldcol2 = poly2->col;

				poly1->col = poly2;
				poly1->distcol = distcol;
				samepol = poly1->next;
				while (samepol->indpol == poly1->indpol) {
					samepol->col = poly2;
					samepol->distcol = distcol;
					samepol = samepol->next;
				}

				if (poly2->move) {
					poly2->col = poly1;
					poly2->distcol = distcol;
					samepol = poly2->next;
					while (samepol->indpol == poly2->indpol) {
						samepol->col = poly1;
						samepol->distcol = distcol;
						samepol = samepol->next;
					}
				}

				if (oldcol1 != NULL && oldcol1->move) {
					oldcol1->col = NULL;
					oldcol1->distcol = 1.0f;
					samepol = oldcol1->next;
					while (samepol->indpol == oldcol1->indpol) {
						samepol->col = NULL;
						samepol->distcol = 1.0f;
						samepol = samepol->next;
					}
				}

				if (oldcol2 != NULL && oldcol2->move) {
					oldcol2->col = NULL;
					oldcol2->distcol = 1.0f;
					samepol = oldcol2->next;
					while (samepol->indpol == oldcol2->indpol) {
						samepol->col = NULL;
						samepol->distcol = 1.0f;
						samepol = samepol->next;
					}
				}

				/* Recompute for polygon unbinded */
				if (oldcol1 != NULL)
					oldcol1->poly_pivot_collision(oldcol1);

				if (oldcol2 != NULL)
					oldcol2->poly_pivot_collision(oldcol2);

				oldcol1 = NULL;
				oldcol2 = NULL;
			}
			fact = 11;
		}
	}
}

/**
*	polygon_init: init polygon with his properties
*
*/

struct polygon *polygon_init(int counter, int cntsides, int move, int indpol,
                             int cntfaces, int indfaces, int texture, GLfloat pts[])
{
	struct polygon *poly;
	struct point p[3], tmp, tmp2;
	struct vector v1, v2, vtmp;
	struct line diag;
	float rayon;
	int i, j;

	/* Allocate polygon object */
	poly = (struct polygon*) malloc(sizeof(struct polygon));
	if (poly == NULL)
		return poly;

	/* Init main properties */
	poly->counter = counter;
	poly->indpol = indpol;
	poly->cntfaces = cntfaces;
	poly->indfaces = indfaces;
	poly->move = move;
	poly->texture = texture;
	poly->cntsides = cntsides;
	poly->pts = (GLfloat*) malloc((cntsides * 4) * sizeof(GLfloat));
	for(j = 0; j < (cntsides * 4); j++)
		poly->pts[j] = pts[j];
	poly->col = NULL;
	poly->oldcol = NULL;
	poly->distcol = 1.0f;

	/* Compute lines equation for the cntsides segments of the face */
	poly->equa = (struct line*) malloc(cntsides * sizeof(struct line));
	for(i = 0; i < cntsides; i++) {
		if(i == cntsides-1)
			j = 0;
		else
			j = i+1;
		/* tmp and tmp2 are the 2 points of current segment */
		tmp.x = poly->pts[4*i];
		tmp.y = poly->pts[4*i+1];
		tmp.z = poly->pts[4*i+2];
		tmp2.x = poly->pts[4*j];
		tmp2.y = poly->pts[4*j+1];
		tmp2.z = poly->pts[4*j+2];
		/* Init equation line for current segment */
		poly->equa[i].dir = vector_set(tmp2, tmp);
		poly->equa[i].orig = tmp;
	}

	/* Polygon center */
	poly->center.x = (poly->equa[0].orig.x + poly->equa[(cntsides/2)].orig.x)/2;
	poly->center.y = (poly->equa[0].orig.y + poly->equa[(cntsides/2)].orig.y)/2;
	poly->center.z = (poly->equa[0].orig.z + poly->equa[(cntsides/2)].orig.z)/2;
	vtmp = vector_set(poly->equa[0].orig, poly->center);
	poly->rayon = vtmp.vector_magnitude(vtmp);

	/* Init transmation transform */
	if (poly->move) {
		poly->trans = init_trans[poly->indpol%cnt_cubes];
		poly->vartrans = init_vartrans[poly->indpol%cnt_cubes];
	} else {
		poly->vartrans = vector_init(0.0f, 0.0f, 0.0f);
	}

	/* Init polygon functions */
	poly->poly_compute_coord = poly_compute_coord;
	poly->poly_intersec_quad = poly_intersec_quad;
	poly->poly_contains = poly_contains;
	poly->poly_intersec = poly_intersec;
	poly->poly_pivot_collision = poly_pivot_collision;
	poly->poly_detect_collision = poly_detect_collision;

	return poly;
}

/**
*	next_coords - Compute new coordinates for polygons
*
*	This function loop on polygons list and compute new coordinates
*/

void next_coords()
{
	struct polygon *current;

	current = poly_head;
	do {
		current->poly_compute_coord(current);
		current = current->next;
	} while (current != poly_head);
}

/**
*	next_hop - Next polygons move
*
*	This function manage polygons move with collision detection
*	and coordinates compute.
*/

void next_hop()
{
	struct polygon *current;
	int indpol = -1;

	current = poly_head;
	do {
		if (current->move && current->indpol != indpol)
			current->poly_pivot_collision(current);

		indpol = current->indpol;
		current = current->next;
	} while (current != poly_head);

	next_coords();
}

/**
*	display_polygons - Shape quad polygons in opengl
*
*	Dispaly all cubes in opengl viewport.
*/

void display_polygons()
{
	int i,j;
	struct polygon *current = poly_head;

	do {
		j = 2;
		if (current->texture == 2)
			j = 10;
		if (current->texture == 1)
			j = 1;
		if (current->texture == 4)
			j = 1;

		if (mode_grid)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		/* Because camera position, we want this 2 room sides transparent */
		if (current->texture == 3 && (current->indfaces == 0 || current->indfaces == 2))
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		/* Set texture */
		glBindTexture(GL_TEXTURE_2D, texturenames[current->texture]);
		/* Shape quad polygon */
		glBegin(GL_QUADS);
		glTexCoord2i(0, 0);
		glVertex3f(current->pts[0], current->pts[1], current->pts[2]);

		glTexCoord2i(j, 0);
		glVertex3f(current->pts[4], current->pts[5], current->pts[6]);

		glTexCoord2i(j, j);
		glVertex3f(current->pts[8], current->pts[9], current->pts[10]);

		glTexCoord2i(0, j);
		glVertex3f(current->pts[12], current->pts[13], current->pts[14]);
		glEnd();

		/* First display init opengl matrix and compute coordinates with this */
		if (is_matrix == 0) {
			glPushMatrix();
			glMultMatrixf(current->pts);
			glGetFloatv(GL_MODELVIEW_MATRIX, current->pts);
			glPopMatrix();
		}

		current = current->next;
	} while (current != poly_head);

	is_matrix = 1;
	return;
}

/**
*	destroy_polygons - Reset polygons stack
*
*	Destroy all polygons
*/
void destroy_polygons()
{
	struct polygon *current = poly_head, *tmp;

	if (current->prev != NULL)
			current->prev->next = NULL;

	while (current != NULL) {
		tmp = current->next;
		free(current);
		current = tmp;
	}

	poly_head = NULL;
}

#include "collisions.h"

extern struct poly4 *tabpoly4;
extern struct poly4 *tabmov4;
extern int cntpoly4;
extern int cntmov4;
extern struct move col;
extern float camx,camy,camz,vuex,vuey,vuez;
extern int cntangle;
extern int cnttrans;
extern float *angle;
extern float *varangle;
extern struct vector *trans;
extern struct vector *vartrans;


/* initcube: init a cube with coordinate and transform values */
void initcube(int moved, int rot, int translate)
{
	int i,j,num;
	float ry;
	struct line diag;
	/* Default coordinates for the 6 faces of the cube */
	GLfloat t[6][16]={
        {-1.0f, 2.0f, -1.0f, 1.0f, 1.0f, 2.0f, -1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, -1.0f, 0.0f, -1.0f, 1.0f},
        {-1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f},
        {1.0f, 2.0f, -1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f},
        {-1.0f, 2.0f, -1.0f, 1.0f, -1.0f, 2.0f, 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 0.0f, -1.0f, 1.0f},
        {-1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 2.0f, -1.0f, 1.0f, -1.0f, 2.0f, -1.0f, 1.0f},
        {-1.0f, 0.0f, -1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f}
	};

	if (cntpoly4)
		num = tabpoly4[cntpoly4-1].indpol + 1;
	else
		num = 0;
	
	tabpoly4 = (struct poly4*) realloc(tabpoly4, (cntpoly4+6) * sizeof(struct poly4));
    	/* Init faces properties */
	for (i = 0; i < 6; i++) {
		tabpoly4[cntpoly4+i].indangle = rot;
		tabpoly4[cntpoly4+i].indtrans = translate;
		tabpoly4[cntpoly4+i].indpol = num;
		if(moved)
			tabpoly4[cntpoly4+i].move = 1;
		else
			tabpoly4[cntpoly4+i].move = 0;
	
		tabpoly4[cntpoly4+i].nb = i;
		tabpoly4[cntpoly4+i].cntfaces = 6;
		tabpoly4[cntpoly4+i].pt = (GLfloat*) malloc(16 * sizeof(GLfloat));
		tabpoly4[cntpoly4+i].texture = 3;
		tabpoly4[cntpoly4+i].cntsides = 4;
		tabpoly4[cntpoly4+i].col = -1;
		for(j = 0; j < 16; j++)
			tabpoly4[cntpoly4+i].pt[j] = t[i][j];
	}

	computecoord(cntpoly4, cntpoly4+6, 0);
	diag = centerpolygon(&ry, cntpoly4);
	for(i = cntpoly4; i < (cntpoly4+6); i++)
		initnormk(i, 0, diag.orig, ry);
	cntpoly4 += 6;
}

/* isonface: return 1 if point inter is on cpt face, else return 0 */
int isonface(struct point inter, int cpt, int mat)
{
	struct line d;
	struct point tmp = {tabpoly4[cpt].pt[0], tabpoly4[cpt].pt[1], tabpoly4[cpt].pt[2]};
	d.dir = setvector(inter, tmp);
	d.orig = tmp;
	tmp = intersecquad(d, cpt);
	if (tmp.x != 100000)
		return 1;
	else
		return 0;
}

/* isonsegment: return 1 if p is on d (from orig to vector dist size), else return 0 */
int isonsegment(struct line d, struct point p)
{
	float u = 0.0f;
	struct vector v = setvector(p, d.orig);
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

/* intersecSegmentToLine: return intersect point between segment d with line e */
/* if no intersec point, return {100000, 1000000, 100000} */
struct point intersecline(struct line d, struct line e, int cpt)
{
	struct point inter = {100000.0f, 100000.0f, 100000.0f};
	struct point tmp = {0.0f, 0.0f, 0.0f}, tmp2 = {0.0f, 0.0f, 0.0f};
	struct point orig2 = {d.orig.x + d.dir.x, d.orig.y + d.dir.y, d.orig.z + d.dir.z};
	struct vector v = setvector(d.orig, e.orig);
	float dist, d1, d2, dtmp;
	float dist2 = magnitude(setvector(orig2, e.orig));
	float i, j, k, l;
	float me = magnitude(e.dir);
	float md = magnitude(d.dir);
	double interv = 0.0f;
	i = j = k = l = 0.0f;
	
	if (dist2 < magnitude(v)) {
		dist2 = magnitude(v);
		v = setvector(orig2, e.orig);
	}
	dist = magnitude(v);
	if (me > dist)
		return inter;

	d1 = 0.1f * md;
	d2 = 0.1f * me;
	interv = sqrt(d1*d1 + d2*d2);
	/* inter is between k factor and l factor on e */
	do {
		tmp.x = e.orig.x + j*e.dir.x;
		tmp.y = e.orig.y + j*e.dir.y;
		tmp.z = e.orig.z + j*e.dir.z;
		dtmp = magnitude(setvector(tmp, e.orig));
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
		tmp.x = d.orig.x + i*d.dir.x;
		tmp.y = d.orig.y + i*d.dir.y;
		tmp.z = d.orig.z + i*d.dir.z;
		while (j<l) {
			tmp2.x = e.orig.x + j*e.dir.x;
			tmp2.y = e.orig.y + j*e.dir.y;
			tmp2.z = e.orig.z + j*e.dir.z;
			/* if distance between the 2 points is very very low, it's the one */
			if(magnitude(setvector(tmp, tmp2)) <= interv)
				return tmp;
			j += 0.1;
		}
		i += 0.05;
	}
	return inter;
}

/* Init vector size norm, and compute segments equation for the cpt number face */
void initnormk(int cpt, int mat, struct point center, float rayon)
{
	struct point p[3], tmp, tmp2;
	struct vector v1, v2;
	int j, i=0, cntsides = tabpoly4[cpt].cntsides;
	/* Parse the 4 points of the cpt face */
	for(j = 0; j < 3; j++) {
		p[j].x = (float) tabpoly4[cpt].pt[i++];
		p[j].y = (float) tabpoly4[cpt].pt[i++];
		p[j].z = (float) tabpoly4[cpt].pt[i++];
		i++;
	}
	
	v1 = setvector(p[0], p[1]);
	v2 = setvector(p[2], p[1]);
	tabpoly4[cpt].norm = vectorproduct(v1, v2);
	tabpoly4[cpt].k = dot(tabpoly4[cpt].norm, initvector(p[0].x, p[0].y, p[0].z));
	/* Compute lines equation for the 4 segments of the face */
	tabpoly4[cpt].equa = (struct line*) malloc(cntsides * sizeof(struct line));
	for(i = 0; i < cntsides; i++) {
		if(i == cntsides-1)
			j = 0;
		else
			j = i+1;
		/* tmp and tmp2 are the 2 points of current segment */
		tmp.x = tabpoly4[cpt].pt[4*i];
		tmp.y = tabpoly4[cpt].pt[4*i+1];
		tmp.z = tabpoly4[cpt].pt[4*i+2];
		tmp2.x = tabpoly4[cpt].pt[4*j];
		tmp2.y = tabpoly4[cpt].pt[4*j+1];
		tmp2.z = tabpoly4[cpt].pt[4*j+2];
		/* Init equation line for current segment */
		tabpoly4[cpt].equa[i].dir = setvector(tmp2, tmp);
		tabpoly4[cpt].equa[i].orig = tmp;
	}
	/* Cube center */
	tabpoly4[cpt].center = center;
	tabpoly4[cpt].ray = rayon;
	/* Face center */
	tabpoly4[cpt].centerfc.x = (tabpoly4[cpt].equa[0].orig.x + tabpoly4[cpt].equa[2].orig.x)/2;
	tabpoly4[cpt].centerfc.y = (tabpoly4[cpt].equa[0].orig.y + tabpoly4[cpt].equa[2].orig.y)/2;
	tabpoly4[cpt].centerfc.z = (tabpoly4[cpt].equa[0].orig.z + tabpoly4[cpt].equa[2].orig.z)/2;
	tabpoly4[cpt].rayfc = magnitude(setvector(tabpoly4[cpt].equa[0].orig, tabpoly4[cpt].centerfc));
}

/* centerpolygon: Return equation line from center of cpt polygon and compute diagonal into ray value */
struct line centerpolygon(float *ray, int cpt)
{
	struct line ret;
	int cptt = cpt - tabpoly4[cpt].nb;
	struct point tp0 = {tabpoly4[cptt].pt[0], tabpoly4[cptt].pt[1], tabpoly4[cptt].pt[2]};
	struct point tp2 = {tabpoly4[cptt].pt[8], tabpoly4[cptt].pt[9], tabpoly4[cptt].pt[10]};

	if (tabpoly4[cpt].cntfaces > 1) {
		struct point tp1 = {tabpoly4[cptt+1].pt[8], tabpoly4[cptt+1].pt[9], tabpoly4[cptt+1].pt[10]};
		ret.dir = setvector(tp1,tp0);
	} else {
		ret.dir = setvector(tp2,tp0);
	}

	ret.dir.x /= 2;
	ret.dir.y /= 2;
	ret.dir.z /= 2;
	ret.orig.x = tp0.x + ret.dir.x;
	ret.orig.y = tp0.y + ret.dir.y;
	ret.orig.z = tp0.z + ret.dir.z;

	/* Compute daigonale size from center of face */
	*ray = magnitude(ret.dir);

	return ret;
}


/* GL Matrix multiplication */
/* Custom algo for match opengl matrix spec */
GLfloat *multim(GLfloat *a, GLfloat *b)
{
	GLfloat* tmp = (GLfloat*) malloc(16 * sizeof(GLfloat));
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

	return tmp;
}

/* computeCoord: compute coordinates with current acive matrix */
void computecoord(int i, int j, int l)
{
	while(i<j) {
		glPushMatrix();
		glMultMatrixf(tabpoly4[i].pt);
		glGetFloatv(GL_MODELVIEW_MATRIX, tabpoly4[i].pt);
		glPopMatrix();
		i++;
	}
}

/* intersec: Compute the intersect point between line (ctre, vcm) and the cpt face  */
struct point intersec(struct vector vcm, struct point ctr, int cpt, int pol)
{
	struct point inter, inter2 = {100000.0f, 100000.0f, 100000.0f};
	struct vector v;
	GLfloat u, k;
	v = tabpoly4[cpt].norm;
	k = tabpoly4[cpt].k;
	if (!dot(vcm,v))
		return inter2;
	u = (k - dot(initvector(ctr.x, ctr.y, ctr.z), v)) / dot(vcm, v);
	inter.x = ctr.x + u*vcm.x;
	inter.y = ctr.y + u*vcm.y;
	inter.z = ctr.z + u*vcm.z;
	if (isonface(inter, cpt, 0))
		return inter;
	else
		return inter2;
}

/* collisionCam: manage collisions about camera  */
void collisioncam()
{
	col.dt[0] = 100000.0f;
	col.dt[1] = 100000.0f;
	col.dt[2] = 100000.0f;
	col.dt[3] = 100000.0f;
	GLfloat dstcce;
	int i;
        struct point ctr = { 0.0f, -1.0f, 0.0f }, inter;
	struct point crotate = { 0.0f, 0.0f, 0.0f };
	struct point arotate = { 0.0f, 90.0f, 0.0f };
 	struct vector vcm, vcm1, vcm2;

	vcm.x = 0.0f;
	vcm.y = 0.0f;
	vcm.z = -1.0f;
	normalize(vcm);

	vcm1 = vcm;
	vcm1 = rotatepoint(vcm1, crotate, arotate);
	normalize(vcm1);

	for(i = 0; i < cntpoly4; i++) {
		inter = intersec(vcm,ctr,i,0);
		dstcce = computedist(ctr,inter);
		vcm2 = setvector(inter,ctr);
		if (vcm2.x*vcm.x >= 0 && vcm.z*vcm2.z >= 0) {
			if (dstcce < col.dt[0]) {
				col.dt[0] = dstcce - 1.0;
				col.inter[0] = inter;
			}
		} else if (dstcce < col.dt[1]) {
			col.dt[1] = dstcce;
			col.inter[1] = inter;
		}

		inter = intersec(vcm1, ctr, i, 0);
		dstcce = computedist(ctr, inter);
		vcm2 = setvector(inter, ctr);
		if (vcm2.x*vcm1.x >= 0  && vcm1.z*vcm2.z >= 0) {
			if (dstcce < col.dt[2]) {
				col.dt[2] = dstcce;
				col.inter[2] = inter;
			}
		} else if (dstcce < col.dt[3]) {
			col.dt[3] = dstcce;
			col.inter[3] = inter;
		}
	}
}

/* intersec2segments: compute the intersection point between 2 segments u and v */
struct point intersec2segments(struct line u, struct line v)
{
	struct point inter = { 100000, 100000, 100000 };
	float t1, dd;

	t1 = dot(vectorproduct(setvector(v.orig, u.orig), v.dir), vectorproduct(u.dir, v.dir));
	dd = magnitude(vectorproduct(u.dir, v.dir));
	if (!dd)
		return inter;
	t1 /= (dd * dd);
	if(t1 >= 0 && t1 <= 1) {
		inter.x = u.orig.x + t1*u.dir.x;
		inter.y = u.orig.y + t1*u.dir.y;
		inter.z = u.orig.z + t1*u.dir.z;
	}
	
	return inter;
}


/* matrtrans: compute translation matrice from vector u */
GLfloat *matrtrans(struct vector u, float fact)
{
	int i;
	GLfloat *ret = (GLfloat*) malloc(16*sizeof(GLfloat));
	u.x = u.x * fact;
	u.y = u.y * fact;
	u.z = u.z * fact;
	for (i=0; i < 16; i++)
		ret[i] = 0.0f;
	
	ret[0] = ret[5] = ret[10] = ret[15] = 1.0f;
	ret[12] = -u.x;
	ret[13] = u.z;
	ret[14] = u.y;

	return ret;
}


//retourne le point d intersection entre 3 plans definits par leurs normes et leurs cste k
struct point intersec3plans(struct vector *n, float *k)
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

	inv = inversematr(m);
	if (inv == NULL)
		return inter;
	ret.x = inv[3];
	ret.y = inv[7];
	ret.z = inv[11];
	free(inv);

	return ret;
}

/* inversematr: compute inverse of matrix mat */
GLfloat *inversematr(GLfloat *mat)
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

/* issamedir: Return 1 if v1 and v2 are in same direction */
int issamedir(struct vector v1, struct vector v2)
{
    if (v1.x * v2.x < 0 || v1.y * v2.y < 0 || v1.z * v2.z < 0)
	    return 0;
    else
	    return 1;
}

/* coli: swap tabpoly and tabmov for the cube targeted by number */
void coli(int number)
{
	int i = 0, j = 0, k;
	while (tabpoly4[i].indpol != number)
		i++;

	while (tabmov4[j].indpol != number)
		j++;
	
	for (k = 0; k < tabpoly4[i].cntfaces; k++) {
		memcpy(tabpoly4[i+k].equa, tabmov4[j+k].equa, tabpoly4[i+k].nb*sizeof(struct line));
		memcpy(tabpoly4[i+k].pt, tabmov4[j+k].pt, 16*sizeof(GLfloat) );
		tabpoly4[i+k].center = tabmov4[j+k].center;
		tabpoly4[i+k].norm = tabmov4[j+k].norm;
		tabpoly4[i+k].k = tabmov4[j+k].k;
	}
}

/* intersecquad: intersection between one line e and one plan (polygon number is cpt) */
struct point intersecquad(struct line e, int cpt)
{
	struct point inter = {10000,10000,10000};
	struct line dr;
	float me = magnitude(e.dir), dist, u;
	struct vector v = setvector(tabpoly4[cpt].equa[2].orig, tabpoly4[cpt].equa[0].orig);
	double angles = anglevectors(tabpoly4[cpt].equa[0].dir, e.dir);
	double angle2 = anglevectors(tabpoly4[cpt].equa[0].dir, v);
	
	if (!issamedir(e.dir, setvector(tabpoly4[cpt].equa[2].orig, tabpoly4[cpt].equa[0].orig)))
		return inter;
	if (angles > PI/2 || angles < 0) {
		return inter;
	} else if (angles <= absolute(angle2)) {
		dr = tabpoly4[cpt].equa[1];
		dist = magnitude(tabpoly4[cpt].equa[0].dir);
	} else if (angles != PI/2) {
		dr = tabpoly4[cpt].equa[2];
		angles = PI/2 - angles;
		dist = magnitude(tabpoly4[cpt].equa[3].dir);
	}

    	if (angles == PI/2) {
		dr = tabpoly4[cpt].equa[3];
        	if (me <= magnitude(dr.dir)) {
        		inter.x = e.orig.x + e.dir.x;
        		inter.y = e.orig.y + e.dir.y;
        		inter.z = e.orig.z + e.dir.z;
        	} else {
			return inter;
		}
	} else if(!angles) {
		dr=tabpoly4[cpt].equa[0];
		if (me <= magnitude(dr.dir)) {
			inter.x=e.orig.x+e.dir.x;
			inter.y=e.orig.y+e.dir.y;
			inter.z=e.orig.z+e.dir.z;
		} else {
			return inter;
		}
	} else {
		dist = dist / cos(angles);
		u = dist / me;
		if(u < 1.0)
			return inter;
		inter.x = e.orig.x + u*e.dir.x;
		inter.y = e.orig.y + u*e.dir.y;
		inter.z = e.orig.z + u*e.dir.z;
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

/* Init translations */
void initangletrans()
{
	int i;
	cntangle = 2;
	cnttrans = 6;
	angle = (float*) malloc(2*sizeof(float));
	varangle = (float*) malloc(2*sizeof(float));
	trans = (struct vector*) malloc(6*sizeof(struct vector));
	vartrans = (struct vector*) malloc(6*sizeof(struct vector));
	angle[0] = -40;
	angle[1] = 80;
	varangle[0] = 1.0;
	varangle[1] = 1.0;
	for (i = 0; i < cnttrans; i++) {
		vartrans[i].y = 0.0f;
		trans[i].y = 0.0f;
	}
	vartrans[0].x = 2.5f;
	vartrans[0].z = 2.1f;
	vartrans[1].x = 2.1f;
	vartrans[1].z = 2.5f;
	vartrans[2].x = 1.5f;
	vartrans[2].z = -1.1f;
	vartrans[3].x = -1.1f;
	vartrans[3].z = 1.5f;
	vartrans[4].x = 0.5f;
	vartrans[4].z = 0.5f;
	vartrans[5].x = 0.5f;
	vartrans[5].z = 0.5f;
	trans[0].x = -4.0f;
	trans[0].z = -2.0f;
	trans[1].x = -6.0f;
	trans[1].z = -6.0f;
	trans[2].x = 4.0f;
	trans[2].z = -12.0f;
	trans[3].x = 6.0f;
	trans[3].z = 6.0f;
	trans[4].x = 0.0f;
	trans[4].z = -12.0f;
	trans[5].x = -3.0f;
	trans[5].z = 12.0f;
}

/* interpol: manage collisions */
void interpol()
{
	int h, i, m, k=0, cntp, npi=-10, *tmoin, nb, npk, dr, mv, *ncol;
	struct line d, e, ct, drte[5];
	GLdouble disveci,cpt,disveck;
	GLfloat ray,dist,distct,*tabfl;
	float *idepl;
	double depli,deplk,j,cpt2,dist2,distct2;
	struct vector u,v;
	struct point inter,inter2,tmpp;
	struct poly4 *tabtmp = (struct poly4*) malloc(cntpoly4*sizeof(struct poly4));
	copypoly4(tabpoly4, tabtmp, 1);

    	i = 0;
    	cntp = tabmov4[cntpoly4-1].indpol+1;
    	idepl = (float*) malloc(cntp*sizeof(float));
	tmoin = (int*) malloc(cntp*sizeof(int));
	ncol = (int*) malloc(cntp*sizeof(int));
	for (m = 0; m < cntp; m++) {
		idepl[m] = 1.5f;
		tmoin[m] = 0;
		ncol[m] = -1;
	}

	while (i < cntpoly4) {
		if (!tabmov4[i].move) {
			i++;
			continue;
		}
		
		npi = tabmov4[i].indpol;
		d.dir.x = vartrans[tabmov4[i].indtrans].x;
		d.dir.y = vartrans[tabmov4[i].indtrans].y;
		d.dir.z = vartrans[tabmov4[i].indtrans].z;
		for (m = 0; m < 4; m++) {
			drte[m].orig = tabmov4[i].equa[m].orig;
			drte[m].dir = d.dir;
		}

		drte[4].orig = tabmov4[i].centerfc;
		drte[4].dir = d.dir;
		disveci = magnitude(d.dir);
		if (!d.dir.y)
			h=2;
		else
			h=0;
	
		npk = -10;
		while (h < cntpoly4) {
                	if (tabmov4[h].indpol == tabmov4[i].indpol) {
				h++;
				continue;
			}

			npk = tabmov4[h].indpol;
			e.orig = tabmov4[h].centerfc;
			v = setvector(tabmov4[i].centerfc, e.orig);
			if (tabpoly4[h].indtrans != -1) {
				e.dir.x = vartrans[tabpoly4[h].indtrans].x;
				e.dir.y = vartrans[tabpoly4[h].indtrans].y;
				e.dir.z = vartrans[tabpoly4[h].indtrans].z;
				disveck = magnitude(e.dir);
			} else {
				e.dir.x = e.dir.y = e.dir.z = disveck = 0;
			}

			distct = magnitude(setvector(e.orig, drte[4].orig));
			if (magnitude(v) > (disveci + disveck + tabmov4[i].rayfc + tabmov4[h].rayfc)) {
				h++;
				continue;
			}
               		
			for (j=0.0f; j < 1.10f; j+=0.01f) {
				inter2 = ontheline(drte[4], j);
				tmpp = ontheline(e, j);
				
				if(tabpoly4[h].move) {
					mv=1;
					free(tabpoly4[h].pt);
					tabfl = matrtrans(vartrans[tabmov4[h].indtrans], j);
					tabpoly4[h].pt = multim(tabfl, tabmov4[h].pt);
					free(tabfl);
					tabfl=NULL;
				} else {
					mv=0;
					memcpy(tabpoly4[h].pt, tabmov4[h].pt, 16*sizeof(GLfloat));
				}
				ct = centerpolygon(&ray, h);
				initnormk(h, 1, ct.orig, ray);
				distct2 = magnitude(setvector(inter2,tmpp));
				if (j && mv && distct2 >= distct)
					break;
				else
					distct = distct2;
				if (j < 0.03f)
					continue;
				inter = intersec(drte[4].dir, inter2, h, 0);
				if (inter.x == 100000.0f)
					continue;
				dist = magnitude(setvector(inter, inter2));
 				if ((!dist) || distct > disveci)
					continue;

                                //u = setvector(inter, drte[4].orig);
				//if (!issamedir(u, drte[4].dir))
				//	continue;

				if (j > idepl[npi])
					continue;

				if (tmoin[npk] && j > idepl[npk])
					continue;
				
				if(tmoin[npi])
					tmoin[ncol[npi]] = 0;

				if(tabmov4[h].move) {
					if(tmoin[npk])
						tmoin[ncol[npk]] = 0;
					tmoin[npk] = 1;
					ncol[npk] = npi;
					idepl[npk] = (j - 0.02f);
				}

				ncol[npi] = npk;
				idepl[npi] = (j - 0.02f);
				tmoin[npi] = 1;
			}
			h++;
		}
		tabtmp[i].col = -1;
		i++;
	}
	printf("finInterpol0\n");

	deletepoly4(&tabpoly4);
	tabpoly4 = (struct poly4 *) malloc(cntpoly4*sizeof(struct poly4));
	copypoly4(tabtmp, tabpoly4, 1);
	deletepoly4(&tabtmp);

	for (i = 0; i < cntp; i++) {
		h = 0;
		while (tabmov4[h].indpol != i )
			h++;
		nb = tabmov4[h].indtrans;
		if (nb == -1)
			continue;
		if (!tmoin[i])
			continue;
		
		for (m=0; m < 6; m++) {
			free(tabpoly4[h+m].pt);
			tabfl = matrtrans(vartrans[nb], idepl[i]);
			tabpoly4[h+m].pt = multim(tabfl, tabmov4[h+m].pt);
            		free(tabfl);
            		tabpoly4[h+m].col = ncol[i];
            		ct = centerpolygon(&ray,h+m);
            		initnormk(h+m, 1, ct.orig, ray);
		}
		if (!tmoin[i])
			continue;

		changetrans(nb, idepl[i], -1);
	}

	free(tmoin);tmoin=NULL;
	free(idepl);idepl=NULL;
	free(ncol);ncol=NULL;
	printf("finInterpol\n");
}

/* changetrans: change translation vector for nb polygon */
void changetrans(int nb,float fctdep,float fact)
{
	trans[nb].x -= vartrans[nb].x;
	trans[nb].x += fctdep * vartrans[nb].x;
	trans[nb].y -= vartrans[nb].y;
	trans[nb].y += fctdep * vartrans[nb].y;
	trans[nb].z -= vartrans[nb].z;
	trans[nb].z += fctdep * vartrans[nb].z;

	vartrans[nb].x *= fact;
	vartrans[nb].y *= fact;
	vartrans[nb].z *= fact;
}

/* ontheline: return point on the line d with position fact */
struct point ontheline(struct line d, float fact)
{
	struct point ret;
	ret.x = d.orig.x + fact*d.dir.x;
	ret.y = d.orig.y + fact*d.dir.y;
	ret.z = d.orig.z + fact*d.dir.z;
	return ret;
}

/* copypoly4: clone an array of poly4 struct */
void copypoly4(struct poly4* src, struct poly4* dest, int mal)
{
	int i;
	memcpy(dest, src, cntpoly4*sizeof(struct poly4));
	for(i = 0; i < cntpoly4; i++) {
		dest[i].pt = NULL;
		dest[i].equa = NULL;
		dest[i].pt = (GLfloat*) malloc(16*sizeof(GLfloat));
		memcpy(dest[i].pt, src[i].pt, 16*sizeof(GLfloat));
		if (src[i].equa != NULL) {
			dest[i].equa = (struct line*) malloc(src[i].nb*sizeof(struct line));
			memcpy(dest[i].equa, src[i].equa, src[i].nb*sizeof(struct line));
		}
	}
}

/* deletepoly4: delete and clean poly4 array */
void deletepoly4(struct poly4* *sprime)
{
	deletepoly4__(*sprime);
    	free(*sprime);
    	*sprime=NULL;
}

void deletepoly4__(struct poly4 *sprime)
{
	int i;
	for(i = 0; i < cntpoly4; i++) {
		free(sprime[i].pt);
		sprime[i].pt=NULL;
		if(sprime[i].equa != NULL)
			free(sprime[i].equa);
		sprime[i].equa = NULL;
	}
}
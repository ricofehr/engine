#include "collisions.h"
//#include "fonctionMath.h"
#include <string.h>

extern Poly4 *tabPoly4;
extern Poly4 *tabMov4;
extern int nbPoly4;
extern int nbMov4;
extern tst_move col;
extern float camx,camy,camz,vuex,vuey,vuez;
extern int nbangle;
extern int nbtrans;
extern float *angle;
extern float *varangle;
extern    vecteur *trans;
extern vecteur *vartrans;


//initialise un cube (coordo,transformation....)
void ajout_cube_coord(int pres,int rot,int trs)
{
    int i,j,num;
    
    float ry;
    droite diag;
    GLfloat t[6][16]={{-1.0f,2.0f,-1.0f,1.0f,1.0f,2.0f,-1.0f,1.0f,1.0f,0.0f,-1.0f,1.0f,-1.0f,0.0f,-1.0f,1.0f},
        {-1.0f,2.0f,1.0f,1.0f,1.0f,2.0f,1.0f,1.0f,1.0f,0.0f,1.0f,1.0f,-1.0f,0.0f,1.0f,1.0f},
        {1.0f,2.0f,-1.0f,1.0f,1.0f,2.0f,1.0f,1.0f,1.0f,0.0f,1.0f,1.0f,1.0f,0.0f,-1.0f,1.0f},
        {-1.0f,2.0f,-1.0f,1.0f,-1.0f,2.0f,1.0f,1.0f,-1.0f,0.0f,1.0f,1.0f,-1.0f,0.0f,-1.0f,1.0f},
        {-1.0f,2.0f,1.0f,1.0f,1.0f,2.0f,1.0f,1.0f,1.0f,2.0f,-1.0f,1.0f,-1.0f,2.0f,-1.0f,1.0f},
        {-1.0f,0.0f,-1.0f,1.0f,1.0f,0.0f,-1.0f,1.0f,1.0f,0.0f,1.0f,1.0f,-1.0f,0.0f,1.0f,1.0f}};
    if(nbPoly4)num=tabPoly4[nbPoly4-1].numpol+1;
    else num=0;
    tabPoly4=(Poly4*)realloc(tabPoly4,(nbPoly4+6)*sizeof(Poly4));
    for(i=0;i<6;i++)
    {
        tabPoly4[nbPoly4+i].nbangle=rot;
        tabPoly4[nbPoly4+i].nbtrans=trs;
        tabPoly4[nbPoly4+i].numpol=num;
        if(pres)tabPoly4[nbPoly4+i].move=1;
        else tabPoly4[nbPoly4+i].move=0;
        tabPoly4[nbPoly4+i].mero=i;
        tabPoly4[nbPoly4+i].nbfces=6;
        tabPoly4[nbPoly4+i].pt=(GLfloat*)malloc(16*sizeof(GLfloat));
        tabPoly4[nbPoly4+i].texture=3;
        tabPoly4[nbPoly4+i].nb=4;
        tabPoly4[nbPoly4+i].col=-1;
        for(j=0;j<16;j++)
        {
            tabPoly4[nbPoly4+i].pt[j]=t[i][j];
        }
        
    }
    
    chge_coord(nbPoly4,nbPoly4+6,0);
    diag=centre_polyg(&ry,nbPoly4);
    for(i=nbPoly4;i<(nbPoly4+6);i++)trve_norm_k(i,0,diag.orig,ry);
    nbPoly4+=6;
    

    
}


//calcule la transpose d une matrice et la retourne
GLfloat *transpose(GLfloat *matr)
{
    GLfloat *ret=(GLfloat*)malloc(16*sizeof(GLfloat));
    int i;
    for(i=0;i<16;i++)
    {
        ret[i]=matr[(i%4)*4+i/4];
    }
    return ret;
}


//si inter est sur la face n° cpt, la fction retourne 1 sinon elle retourne 0
int est_sur_face(point inter,int cpt,int mat)
{
    droite d;
    point iter2;
    Poly4* tmep;
    tmep=tabPoly4;
    point tmp={tmep[cpt].pt[0],tmep[cpt].pt[1],tmep[cpt].pt[2]};
    d.dir=setVector(inter,tmp);
    d.orig=tmp;
    iter2=intersec_seg_droite_sur_rec(d,cpt);
    if(absolu(iter2.x)<10000 && absolu(iter2.y)<10000 && absolu(iter2.z)<10000)return 1;
    else return 0;
}

//calcule les coordo de i, milieu de [p p1]
point milieu(point p,point p1)
{
    point equi;
    equi.x=(p.x+p1.x)/2;
    equi.y=(p.y+p1.y)/2;
    equi.z=(p.z+p1.z)/2;
    return equi;
}

//si p est sur le seg d,alors la fction retourne 1 sinon elle retourne 0
int est_sur_seg(droite d, point p)
{
    float u=0.0;
    vecteur v=setVector(p,d.orig);
    while(u<=1.00)
    {
        if(absolu(v.x-u*d.dir.x)<0.05*d.dir.x && absolu(v.y-u*d.dir.y)<0.05*d.dir.y && absolu(v.z-u*d.dir.z)<0.05*d.dir.z)return 1;
        u+=0.02;
    }
    return 0;
}



point intersec_seg_droite_sur(droite d,droite e,int cpt)
{
    point inter={10000,10000,10000};
    float i=0.0,j=1.0,k=0.0,l,interv;
    point tmp,tmp2,fce1=d.orig,fce2={d.orig.x+d.dir.x,d.orig.y+d.dir.y,d.orig.z+d.dir.z};
    vecteur v3=setVector(fce1,e.orig),v;
    vecteur v33=setVector(fce2,e.orig),ab,d22={-d.dir.x,-d.dir.y,-d.dir.z};
    float dist3=Magnitude(v3),dist33=Magnitude(v33),dist;
    float medir=Magnitude(e.dir);
    float mddir=Magnitude(d.dir),d1,d2;
    if(dist3<dist33){dist=dist33;v=v33;dist33=dist3;dist3=dist;}
    else v=v3;
    if(medir>dist3 || e.dir.x*v.x<0 || e.dir.y*v.y<0 || e.dir.z*v.z<0)return inter;
    
    d1=0.1*mddir;
    d2=0.1*medir;
    interv=sqrt(d1*d1+d2*d2);
    do
    {
        tmp2.x=e.orig.x+j*e.dir.x;
        tmp2.y=e.orig.y+j*e.dir.y;
        tmp2.z=e.orig.z+j*e.dir.z;
        dist=Magnitude(setVector(tmp2,e.orig));
        if(!k && dist>dist33+0.2*medir)k=j;
        if(k && dist>dist3){l=j;break;}
        j+=0.2;
    }
    while(1);

    while(i<1.00)
    {
        j=k;
        tmp.x=d.orig.x+i*d.dir.x;
        tmp.y=d.orig.y+i*d.dir.y;
        tmp.z=d.orig.z+i*d.dir.z;
        while(j<l)
        {
            tmp2.x=e.orig.x+j*e.dir.x;
            tmp2.y=e.orig.y+j*e.dir.y;
            tmp2.z=e.orig.z+j*e.dir.z;
            ab.x=tmp.x-tmp2.x;
            ab.y=tmp.y-tmp2.y;
            ab.z=tmp.z-tmp2.z;
            dist=Magnitude(ab);
            if(dist<=interv)return tmp;
            j+=0.1;
        }
        i+=0.05;
    }
    return inter;
}


//calcule la norme ,la valeur k ainsi que les equa de droite de la face cpt
void trve_norm_k(int cpt,int mat,point cre,float ry)
{
    Poly4* tmep;
    tmep=tabPoly4;
    point p[3],tmp,tmp2;
    vecteur v1,v2;
    int j,i=0,nb=tmep[cpt].nb;
    for(j=0;j<3;j++)
    {
        p[j].x=tmep[cpt].pt[i++];
        p[j].y=tmep[cpt].pt[i++];
        p[j].z=tmep[cpt].pt[i++];
        i++;
    }
    v1=setVector(p[0],p[1]);
    v2=setVector(p[2],p[1]);
    tmep[cpt].norm=Normal(v1,v2);
    tmep[cpt].k=Dot(tmep[cpt].norm,p[0]);
    tmep[cpt].equa=(droite*)malloc(nb*sizeof(droite));
    for(i=0;i<nb;i++)
    {
        if(i==nb-1)j=0;
        else j=i+1;
        tmp.x=tmep[cpt].pt[4*i];tmp.y=tmep[cpt].pt[4*i+1];tmp.z=tmep[cpt].pt[4*i+2];
        tmp2.x=tmep[cpt].pt[4*j];tmp2.y=tmep[cpt].pt[4*j+1];tmp2.z=tmep[cpt].pt[4*j+2];
        tmep[cpt].equa[i].dir=setVector(tmp2,tmp);
        tmep[cpt].equa[i].orig=tmp;
    }
    tmep[cpt].ctre=cre;
    tmep[cpt].ray=ry;
    tmep[cpt].ctrefc.x=(tmep[cpt].equa[0].orig.x+tmep[cpt].equa[2].orig.x)/2;
    tmep[cpt].ctrefc.y=(tmep[cpt].equa[0].orig.y+tmep[cpt].equa[2].orig.y)/2;
    tmep[cpt].ctrefc.z=(tmep[cpt].equa[0].orig.z+tmep[cpt].equa[2].orig.z)/2;
    tmep[cpt].rayfc=Magnitude(setVector(tmep[cpt].equa[0].orig,tmep[cpt].ctrefc));
    
}

//retourne la droite d,de vect le rayon de la face et pour orig, son centre
droite centre_polyg(float *ray,int cpt)
{
    int cptt=cpt-tabPoly4[cpt].mero;
    droite tempr;
    point tp0={tabPoly4[cptt].pt[0],tabPoly4[cptt].pt[1],tabPoly4[cptt].pt[2]};
     
    point tp2={tabPoly4[cptt].pt[8],tabPoly4[cptt].pt[9],tabPoly4[cptt].pt[10]};
    
    
    if(tabPoly4[cpt].nbfces>1)
    {
        point tp1={tabPoly4[cptt+1].pt[8],tabPoly4[cptt+1].pt[9],tabPoly4[cptt+1].pt[10]};
        tempr.dir=setVector(tp1,tp0);
    }
    else {tempr.dir=setVector(tp2,tp0);}
    
    tempr.dir.x/=2;
    tempr.dir.y/=2;
    tempr.dir.z/=2;
    tempr.orig.x=tp0.x+tempr.dir.x;
    tempr.orig.y=tp0.y+tempr.dir.y;
    tempr.orig.z=tp0.z+tempr.dir.z;
    
    *ray=Magnitude(tempr.dir);
    
    return tempr;
}


//multiplie 2 matrices entre elles et renvoie le resultat
//!!fction spec. aux matrices opengl
// !! matrice 4*4 divisé par colonne (0-3:1ere clonne)
GLfloat *multim(GLfloat *a, GLfloat *b)        
{
  GLfloat* temp=(GLfloat*)malloc(16*sizeof(GLfloat));                             //A=A*B
  int i=0,j=0,k=0,m=0;
  
  while(m<16)
  {
        temp[m]=0.0;
    for(i=k;i<k+16;i+=4)
    {
    
        temp[m] += a[i] * b[j];
        j++;
    }
    if(k!=3){j-=4;k++;}
    else k=0;
    ++m;
  }
    return temp;
}

//calcule les cordoo de faces suivant la matrice active
void chge_coord(int i,int j,int l)
{
    
    
    while(i<j)
    {
        glPushMatrix();
        glMultMatrixf(tabPoly4[i].pt);
        glGetFloatv(GL_MODELVIEW_MATRIX,tabPoly4[i].pt);
        glPopMatrix();
        i++;
    }
    
}


//renvoie la distance entre 2 points
GLfloat Dist_entre_pts(point A,point B)
{
    return (sqrt(pow((A.x-B.x),2)+pow((A.y-B.y),2)+pow((A.z-B.z),2)));
}


//retourne le point d intersection entre la droite d (ctre,vcm) et la face n°cpt
point intersec(vecteur vcm,point ctr,int cpt,int pol)
{
    point inter,inter2={10000.0f,10000.0f,10000.0f};
    vecteur v;
    GLfloat u,kk;
    v=tabPoly4[cpt].norm;
    kk=tabPoly4[cpt].k;
    if(!Dot(vcm,v))return inter2;
    u=(kk-Dot(ctr,v))/Dot(vcm,v);
    inter.x=ctr.x + u*vcm.x;
    inter.y=ctr.y + u*vcm.y;
    inter.z=ctr.z + u*vcm.z;
    if(est_sur_face(inter,cpt,0))return inter;
    else return inter2;
}
 

//gere les collisions avec la camera
void collision_cam()
{
        col.dt[0]=10000.0f;
        col.dt[1]=10000.0f;
        col.dt[2]=10000.0f;
        col.dt[3]=10000.0f;
        GLfloat dstcce;
        int i;
        
        point ctr={0.0f,-1.0f,0.0f},inter;
        vecteur vcm,vcm1,vcm2;
        
        vcm.x=0.0f,vcm.y=0.0f,vcm.z=-1.0f;
        Normalise(vcm);
        vcm1=vcm;
        rotationpoint(&vcm1.x,&vcm1.y,&vcm1.z,0,0,0,0,90,0);
        Normalise(vcm1);
        for(i=0;i<nbPoly4;i++)
        {
            inter=intersec(vcm,ctr,i,0);
            dstcce=Dist_entre_pts(ctr,inter);
            vcm2=setVector(inter,ctr);
            if(vcm2.x*vcm.x>=0  && vcm.z*vcm2.z>=0)
            {if(dstcce<col.dt[0]){col.dt[0]=dstcce-1.0;col.inter[0]=inter;}}
            else if(dstcce<col.dt[1]){col.dt[1]=dstcce;col.inter[1]=inter;}
            
            
            inter=intersec(vcm1,ctr,i,0);
            dstcce=Dist_entre_pts(ctr,inter);
            vcm2=setVector(inter,ctr);
            if(vcm2.x*vcm1.x>=0  && vcm1.z*vcm2.z>=0)
            {if(dstcce<col.dt[2]){col.dt[2]=dstcce;col.inter[2]=inter;}}
            else if(dstcce<col.dt[3]){col.dt[3]=dstcce;col.inter[3]=inter;}
            
        }        
}


//retourne le point d'intersection entre deux segments
point inter_2seg(droite u,droite v)
{
    point inter2={10000,10000,10000},inter;
    float t1,dd;
    t1=Dot(Normal(setVector(v.orig,u.orig),v.dir),Normal(u.dir,v.dir));
    dd=Magnitude(Normal(u.dir,v.dir));
    if(!dd)return inter2;
    t1/=(dd*dd);
    inter.x=u.orig.x+t1*u.dir.x;
    inter.y=u.orig.y+t1*u.dir.y;
    inter.z=u.orig.z+t1*u.dir.z;
    if(t1>=0 && t1<=1)return inter;
    else return inter2;
}


//retourne la matrice de translation a partir d un vecteur u et du facteur fact
GLfloat *matrtrans(vecteur u,float fact)
{
    int i;
    GLfloat *ret=(GLfloat*)malloc(16*sizeof(GLfloat));
    u.x=u.x*fact;
    u.y=u.y*fact;
    u.z=u.z*fact;
    for(i=0;i<16;i++)ret[i]=0.0;
    ret[0]=ret[5]=ret[10]=ret[15]=1.0;
    ret[12]=-u.x;
    ret[13]=u.z;
    ret[14]=u.y;
    return ret;
}
    

//retourne le point d intersection entre 3 plans definits par leurs normes et leurs cste k
point intersec_3_plans(vecteur *n,float *k)
{
    GLfloat m[16];
    GLfloat *inv;
    int i;
    point ret,inter={10000,10000,10000};
    for(i=0;i<12;i+=4)
    {
        m[i]=n[i/4].x;
        m[i+1]=n[i/4].y;
        m[i+2]=n[i/4].z;
        m[i+3]=-k[i/4];
    }
    for(i=12;i<15;i++)m[i]=0.0f;
    m[15]=1.0f;
    inv=inverse_matr(m);
    if(inv==NULL)return inter;
    ret.x=inv[3];
    ret.y=inv[7];
    ret.z=inv[11];
    free(inv);
    return ret;
}

//retourne l inverse de la matrice rentre en parametre
GLfloat *inverse_matr(GLfloat *mat)
{
    GLfloat* ret,*zero=NULL;
    int i,j,k=0,row;
    float inv[4][8],tmp[8],multiple,divisor;
    for(i=0;i<4;i++)
    {
        for(j=0;j<8;j++)
        {
            if(j<4)inv[i][j]=mat[k++];
            else if((j-4)==i)inv[i][j]=1;
            else inv[i][j]=0;
        }
    }

   for(row=0;row<4;row++)
    {
    
        
        if(inv[row][row]==0)
        {    
            k=row;
            for(i=0;i<8;i++)tmp[i]=inv[row][i];
            while(!inv[k][row] && k<4)++k;
            if(k==4)return zero;
            for(i=0;i<8;i++)inv[row][i]=inv[k][i];
            for(i=0;i<8;i++)inv[k][i]=tmp[i];
        }
        
        divisor =inv[row][row];
        for(j=0;j<8;j++)
        {
            inv[row][j]/=divisor;
        }
         
        for(i=0;i<3;i++)
        {
             if(i!=row)
             {
                    multiple = inv[i][row];
                    for(j=0;j<8;j++)
                    {
                         inv[i][j]-=multiple * inv[row][j];
                    }
             }
         }
        
   }
        
    
    ret=(GLfloat*)malloc(16*sizeof(GLfloat));
    for(i=0;i<16;i++)ret[i]=inv[i/4][i%4+4];
    return ret;
}


//retourne 1 si v1 et v2 st de mm sens sinon retourne 0
int vect_mem_sens(vecteur v1,vecteur v2)
{
    if(v1.x*v2.x<0 || v1.y*v2.y<0 || v1.z*v2.z <0)return 0;
    else return 1;
}


void coli(int numero)
{
    int i=0,j=0,k;
    while(tabPoly4[i].numpol!=numero)i++;
    while(tabMov4[j].numpol!=numero)j++;
    for(k=0;k<tabPoly4[i].nbfces;k++)
    {
        memcpy(tabPoly4[i+k].equa, tabMov4[j+k].equa,tabPoly4[i+k].nb*sizeof(droite) );
        memcpy(tabPoly4[i+k].pt,tabMov4[j+k].pt,16*sizeof(GLfloat) );
        tabPoly4[i+k].ctre=tabMov4[j+k].ctre;
        tabPoly4[i+k].norm=tabMov4[j+k].norm;
        tabPoly4[i+k].k=tabMov4[j+k].k;
    }
        
    
}


//retourne le point d intersection entre une droite et une surface
point intersec_seg_droite_sur_rec(droite e,int cpt)
{
    point inter2={10000,10000,10000},inter;
    if(!vect_mem_sens(e.dir,setVector(tabPoly4[cpt].equa[2].orig,tabPoly4[cpt].equa[0].orig)))return inter2;
    droite dr;
    float dist3;
    float medir=Magnitude(e.dir),dist;
    float u;
    vecteur v5=setVector(tabPoly4[cpt].equa[2].orig,tabPoly4[cpt].equa[0].orig);
    double angles=AngleBetweenVectors(tabPoly4[cpt].equa[0].dir,e.dir);
    double angle2=AngleBetweenVectors(tabPoly4[cpt].equa[0].dir,v5);
    if(angles>PI/2 || angles<0)return inter2;
    else if(angles<=absolu(angle2)){dr=tabPoly4[cpt].equa[1];dist=Magnitude(tabPoly4[cpt].equa[0].dir);}
    else if(angles!=PI/2){dr=tabPoly4[cpt].equa[2];angles=PI/2-angles;dist=Magnitude(tabPoly4[cpt].equa[3].dir);}
     
    if(angles==PI/2)
    {
        dr=tabPoly4[cpt].equa[3];
        if(Magnitude(e.dir)<=Magnitude(dr.dir)){
        inter.x=e.orig.x+e.dir.x;
        inter.y=e.orig.y+e.dir.y;
        inter.z=e.orig.z+e.dir.z;
        }
        else  return inter2;
        
    }
    else if(!angles)
    {
        dr=tabPoly4[cpt].equa[0];
        if(medir<=Magnitude(dr.dir)){
        inter.x=e.orig.x+e.dir.x;
        inter.y=e.orig.y+e.dir.y;
        inter.z=e.orig.z+e.dir.z;
        }
        else  return inter2;
    
    }
    else
    {
        dist3=dist/cos(angles);
        u=dist3/medir;
        if(u<1.0)return inter2;
        inter.x=e.orig.x+u*e.dir.x;
        inter.y=e.orig.y+u*e.dir.y;
        inter.z=e.orig.z+u*e.dir.z;
    
    }
    
    if(dr.dir.x)u=(inter.x-dr.orig.x)/dr.dir.x;
    else if(dr.dir.y)u=(inter.y-dr.orig.y)/dr.dir.y;
    else if(dr.dir.z)u=(inter.z-dr.orig.z)/dr.dir.z;
    if(u>=0.0 &&  u<=1.0)return inter;
    else return inter2;

}



//initialise les translations
void init_angle_trans()
{
    int i;
    nbangle=2;
    nbtrans=6;
    angle=(float*)malloc(2*sizeof(float));
    varangle=(float*)malloc(2*sizeof(float));
    trans=(vecteur*)malloc(6*sizeof(vecteur));
    vartrans=(vecteur*)malloc(6*sizeof(vecteur));
    angle[0]=-40;
    angle[1]=80;
    varangle[0]=1.0;
    varangle[1]=1.0;
    for(i=0;i<nbtrans;i++){vartrans[i].y=0.0f;trans[i].y=0.0f;}
    
    vartrans[0].x=2.5f;
    vartrans[0].z=2.1f;
    vartrans[1].x=2.1f;
    vartrans[1].z=2.5f;
    //vartrans[3].y=vartrans[2].y=3.25f;
    vartrans[2].x=1.5f;
    vartrans[2].z=-1.1f;
    vartrans[3].x=-1.1f;
    vartrans[3].z=1.5f;
    vartrans[4].x=0.5f;
    vartrans[4].z=0.5f;
    vartrans[5].x=0.5f;
    vartrans[5].z=0.5f;
    trans[0].x=-4.0f;
    trans[0].z=-2.0f;
    trans[1].x=-6.0f;
    trans[1].z=-6.0f;
    trans[2].x=4.0f;
    trans[2].z=-12.0f;
    trans[3].x=6.0f;
    trans[3].z=6.0f;
    trans[4].x=0.0f;
    trans[4].z=-12.0f;
    trans[5].x=-3.0f;
    trans[5].z=12.0f;
}


//gere les collisions entre les polygs
void inter_pol()
{
    
    int h,i,m,k=0,nbp,npp=0,*tmoin,nb,npk,dr,mv,*ncol;
    droite d,e,ct,drte[5];
    GLdouble disveci,cpt,disveck;
    GLfloat ray,dist,distct,*tabfl;
    float *idepl;
    double depli,deplk,j,cpt2,dist2,distct2;
    vecteur u,v;point inter,inter2,tmpp,erig[5];
    //sauvegarde de TabPoly4
    Poly4 *tabPol=(Poly4* )malloc(nbPoly4*sizeof(Poly4));//=(Poly4*)malloc(nbPoly4*sizeof(Poly4));
    copy_Poly4(tabPoly4,tabPol,1);
    
    //de 0 à 5:décor (ne bouge pas)
    i=6;
    //idepl:tab des facteurs de trans pour chaque poly
    nbp=tabMov4[nbPoly4-1].numpol+1;
    idepl=(float*)malloc(nbp*sizeof(float));
    //tmoin:est à 0 si pas de col pdt trans
    tmoin=(int*)malloc(nbp*sizeof(int));
    //ncol indique le polyg en colli
    ncol=(int*)malloc(nbp*sizeof(int));
    //init des 2 tabs
    for(m=0;m<nbp;m++){idepl[m]=2000;tmoin[m]=0;ncol[m]=-1;}
    
    while(i<nbPoly4)
    {
            //printf("i:%d:%d\n",i,nbPoly4);
            //init de d.dir par rapport à la rot de glulookat(ici vue du haut)
            if(!tabMov4[i].move){i++;continue;}
            d.dir.x=-vartrans[tabPoly4[i].nbtrans].x;
            d.dir.y=vartrans[tabPoly4[i].nbtrans].y;
            d.dir.z=vartrans[tabPoly4[i].nbtrans].z;
            //init des 5 vecteurs d'une face,projetant le déplacement
            for(m=0;m<5;m++){drte[m]=tabMov4[i].equa[m];drte[m].dir=d.dir;}
            drte[4].orig=tabMov4[i].ctrefc;
            drte[4].dir=d.dir;
            disveci=Magnitude(d.dir);
            if(!d.dir.y)h=2;
            else h=0;
            npp=tabMov4[i].numpol;
            
            while(h<nbPoly4)
            {
                //printf("h:%d:%d\n",h,nbPoly4);
                npk = -10 ;
                if(tabMov4[h].numpol==tabMov4[i].numpol ||tabMov4[h].numpol==tabMov4[i].col){h++;continue;}
                //e:droite représantant la trans de h
                e.orig=tabMov4[h].ctrefc;
                if(tabMov4[h].nbtrans!=-1)
                {
                    e.dir.x=-vartrans[tabPoly4[h].nbtrans].x;
                    e.dir.y=vartrans[tabPoly4[h].nbtrans].y;
                    e.dir.z=vartrans[tabPoly4[h].nbtrans].z;
                    disveck=Magnitude(e.dir);
                    v=setVector(tabMov4[i].ctrefc,e.orig);
                    if (disveck) {
                        if(Magnitude(v)>(disveci+disveck+tabMov4[i].rayfc+tabMov4[h].rayfc)){h++;continue;}
                    } else {
                        if(Magnitude(v)>(disveci+tabMov4[i].rayfc)){h++;continue;}
                    }
                }
                else {e.dir.x=e.dir.y=e.dir.z=disveck=0;}
                distct=Magnitude(setVector(e.orig,drte[4].orig));
                disveck=Magnitude(e.dir);
                if(disveck && disveck>0)cpt=disveck;
                else cpt=1;
                //cpt2=distance entre la position en j et en j+1(voir la boucle)
                cpt2=disveci / cpt;
                printf("cpt:%f:%d:%d:%d\n", cpt, h, tabPoly4[h].nbtrans, tabMov4[h].nbtrans);
                j=1;
                    
                    while(j<=cpt)
                    {
                        
                        if((cpt-j)<1 && (cpt-j)>0)j=cpt;
                        depli=(j/cpt);
                        deplk=(j/cpt);
                        //test sur depli
                        if(depli>(idepl[npp]+1/(2*cpt)))break;
                        inter2=sur_droite(drte[4],depli);
                        tmpp=sur_droite(e,depli);
                        if(tabPoly4[h].move)
                        {
                            mv=1;free(tabPoly4[h].pt);
                            tabPoly4[h].pt=multim(tabfl=matrtrans(vartrans[tabMov4[h].nbtrans],depli),tabMov4[h].pt);
                            free(tabfl);tabfl=NULL;
                        }
                        else {mv=0;memcpy(tabPoly4[h].pt,tabMov4[h].pt,16*sizeof(GLfloat));}
                        free(tabPoly4[h].equa);
                        ct=centre_polyg(&ray,h);
                        trve_norm_k(h,1,ct.orig,ray);
                        for(m=0;m<4;m++)erig[m]=tabPoly4[h].equa[m].orig;
                        erig[4]=tabPoly4[h].ctrefc;
                        //si les polyg entre t et t+1 s'éloigne,on passe au suivant
                        distct2=Magnitude(setVector(inter2,tmpp));
                        if(j && mv && distct2>=distct)break;                        
                        else distct=distct2;
                        //tests sur les 5 vecteurs de la face
                        
                        for(m=0;m<20;m++)
                        {
                            depli=(j-1+m*0.05)/cpt;
                            while((j-1+m*0.05)<0)m++;
                            for(dr=0;dr<5;dr++)
                            {
                                d.orig=drte[dr].orig;
                                d.dir=drte[dr].dir;
                                if(mv)
                                {
                                    free(tabPoly4[h].pt);free(tabPoly4[h].equa);
                                    tabPoly4[h].pt=multim(tabfl=matrtrans(vartrans[tabMov4[h].nbtrans],depli),tabMov4[h].pt);
                                    free(tabfl);tabfl=NULL;
                                    ct=centre_polyg(&ray,h);trve_norm_k(h,1,ct.orig,ray);
                                }
                                inter=intersec(d.dir,d.orig,h,0);
                                if(inter.x==10000)continue;
                                inter2=sur_droite(d,depli);
                                //vérifie que l'intersec se situe au même niveau sur les 2 vecs
                                dist2=Magnitude(setVector(inter,inter2));
                                if(dist2>=cpt2 && mv)continue;
                                u=setVector(inter,d.orig);
                                dist=Magnitude(u);
                                if((!dist) || dist>(disveci+disveck))continue;
                                if(!vect_mem_sens(u,d.dir)){continue;}
                                
                                deplk=dist/disveci;
                                //vérifie qu'il n y a pas de col plus proches pour les pol
                                if(deplk>=idepl[npp]){continue;}
                                
                                if(npk != -10 && tmoin[npk] && depli>=idepl[npk]){continue;}
                                
                                if(npk != -10 && tabMov4[h].move)
                                {
                                    if(tmoin[npk])tmoin[ncol[npk]]=0;
                                    tmoin[npk]=1;
                                    ncol[npk]=npp;
                                    idepl[npk]=depli;
                                }
                                
                                printf("npp:%d:%d:%f:%f\n",npp,npk,deplk,cpt);
                                if(tmoin[npp])tmoin[ncol[npp]]=0;
                                ncol[npp]=npk;
                                idepl[npp]=deplk;
                                tmoin[npp]=1;
                            }
                        }
                        j++;
                        
                        npk=tabMov4[h].numpol;        
                            
                            
                    
                    }
                h++;
            }
    tabPol[i].col=-1;
    i++;
    }
    printf("finInterpol0\n");    
    
    //réinitialise tabPoly4 par les valeurs de tabpol
    suppr_Poly4(&tabPoly4);
    tabPoly4=(Poly4 *)malloc(nbPoly4*sizeof(Poly4));
    copy_Poly4(tabPol,tabPoly4,1);
    suppr_Poly4(&tabPol);
    //effectue les changements de coordo et de trans par rapport aux collis 
    for(i=0;i<nbp;i++)
    {
        h=0;
        if(!tmoin[i])continue;
        while(tabMov4[h].numpol!=i)h++;
        nb=tabMov4[h].nbtrans;
        if(nb==-1)continue;
        for(m=0;m<6;m++)
        {
            free(tabPoly4[h+m].pt);
            tabPoly4[h+m].pt=multim(tabfl=matrtrans(vartrans[nb],idepl[i]),tabMov4[h+m].pt);
            free(tabfl);
            tabPoly4[h+m].col=ncol[i];
            ct=centre_polyg(&ray,h+m);
            free(tabPoly4[h+m].equa);
            trve_norm_k(h+m,1,ct.orig,ray);
            
        }
            
        change_trans(nb,idepl[i],-1);
            
    }
    
    free(tmoin);tmoin=NULL;
    free(idepl);idepl=NULL;
    free(ncol);ncol=NULL;
    printf("finInterpol\n");    
}

//change le vecteur de translation n°nb en le multipliant par fact
void change_trans(int nb,float fctdep,float fact)
{
    trans[nb].x-=vartrans[nb].x;trans[nb].x+=fctdep*vartrans[nb].x;
    trans[nb].y-=vartrans[nb].y;trans[nb].y+=fctdep*vartrans[nb].y;
    trans[nb].z-=vartrans[nb].z;trans[nb].z+=fctdep*vartrans[nb].z;
    vartrans[nb].x*=fact;
    vartrans[nb].y*=fact;
    vartrans[nb].z*=fact;
}

//retourne le point sur la droite d a la position fact
point sur_droite(droite d,float fact)
{
    point ret;
    ret.x=d.orig.x+fact*d.dir.x;
    ret.y=d.orig.y+fact*d.dir.y;
    ret.z=d.orig.z+fact*d.dir.z;
    return ret;
}

//fait une copy de tableau de faces
void copy_Poly4(Poly4* src,Poly4* dest,int mal)
{
    int i;
    memcpy(dest,src,nbPoly4*sizeof(Poly4));
    for(i=0;i<nbPoly4;i++)
    {
        dest[i].pt=NULL;dest[i].equa=NULL;
        dest[i].pt=(GLfloat*)malloc(16*sizeof(GLfloat));
        memcpy(dest[i].pt,src[i].pt,16*sizeof(GLfloat));
        if(src[i].equa != NULL)
        {
            dest[i].equa=(droite*)malloc(src[i].nb*sizeof(droite));
            memcpy(dest[i].equa,src[i].equa,src[i].nb*sizeof(droite));
        }                                        
    }
    
}

void suppr_Poly4(Poly4* *sprime)
{
    
    suppr_Pt_equa(*sprime);
    free(*sprime);
    *sprime=NULL;
}

void suppr_Pt_equa(Poly4 *sprime)
{
    int i;
    for(i=0;i<nbPoly4;i++)
    {
        free(sprime[i].pt);
        sprime[i].pt=NULL;
        if(sprime[i].equa!=NULL)free(sprime[i].equa);
        sprime[i].equa=NULL;
    }
}

/* 
 * This is a renamed, and re-engineered (Petr Krysl 1995) version of: 
 *
 * 2d and 3d Vector C Library
 * by Andrew Glassner
 * from "Graphics Gems", Academic Press, 1990
*/

#include "vectP.h"

/******************/
/*   2d Library	  */
/******************/

/* returns squared length of input vector */
double 
CkitV2SquaredLength(a)
     CkitVector2 *a;
{
  return((a->x * a->x)+(a->y * a->y));
}

/* returns length of input vector */
double 
CkitV2Length(a)
     CkitVector2 *a;
{
  return(sqrt(CkitV2SquaredLength(a)));
}

/* negates the input vector and returns it */
CkitVector2 *
CkitV2Negate(v)
     CkitVector2 *v;
{
  v->x = -v->x;  v->y = -v->y;
  return(v);
}

/* normalizes the input vector and returns it */
CkitVector2 *
CkitV2Normalize(v)
     CkitVector2 *v;
{
  double len = CkitV2Length(v);
  if (len != 0.0) { v->x /= len; v->y /= len; }
  return(v);
}


/* scales the input vector to the new length and returns it */
CkitVector2 *
CkitV2Scale(v, newlen)
     CkitVector2 *v;
     double newlen;
{
  double len = CkitV2Length(v);
  if (len != 0.0) { v->x *= newlen/len;	v->y *= newlen/len; }
  return(v);
}

/* return vector sum c = a+b */
CkitVector2 *
CkitV2Add(a, b, c)
     CkitVector2 *a, *b, *c;
{
  c->x = a->x+b->x;  c->y = a->y+b->y;
  return(c);
}

/* return vector difference c = a-b */
CkitVector2 *
CkitV2Sub(a, b, c)
     CkitVector2 *a, *b, *c;
{
  c->x = a->x-b->x;  c->y = a->y-b->y;
  return(c);
}

/* return the dot product of vectors a and b */
double 
CkitV2Dot(a, b)
     CkitVector2 *a, *b;
{
  return((a->x*b->x)+(a->y*b->y));
}

/* linearly interpolate between vectors by an amount alpha */
/* and return the resulting vector. */
/* When alpha=0, result=lo.  When alpha=1, result=hi. */
CkitVector2 *
CkitV2Lerp(lo, hi, alpha, result)
     CkitVector2 *lo, *hi, *result;
     double alpha;
{
  result->x = LERP(alpha, lo->x, hi->x);
  result->y = LERP(alpha, lo->y, hi->y);
  return(result);
}


/* make a linear combination of two vectors and return the result. */
/* result = (a * ascl) + (b * bscl) */
CkitVector2 *
CkitV2Combine (a, b, result, ascl, bscl)
     CkitVector2 *a, *b, *result;
     double ascl, bscl;
{
  result->x = (ascl * a->x) + (bscl * b->x);
  result->y = (ascl * a->y) + (bscl * b->y);
  return(result);
}

/* multiply two vectors together component-wise */
CkitVector2 *
CkitV2Mul (a, b, result)
     CkitVector2 *a, *b, *result;
{
  result->x = a->x * b->x;
  result->y = a->y * b->y;
  return(result);
}

/* return the distance between two points */
double 
CkitV2DistanceBetween2Points(a, b)
     CkitPoint2 *a, *b;
{
  double dx = a->x - b->x;
  double dy = a->y - b->y;
  return(sqrt((dx*dx)+(dy*dy)));
}

/* return the vector perpendicular to the input vector a */
CkitVector2 *
CkitV2MakePerpendicular(a, ap)
     CkitVector2 *a, *ap;
{
  ap->x = -a->y;
  ap->y = a->x;
  return(ap);
}

/* create, initialize, and return a new vector */
CkitVector2 *
CkitV2New(x, y)
     double x, y;
{
  CkitVector2 *v = NEWTYPE(CkitVector2);
  v->x = x;  v->y = y;
  return(v);
}


/* create, initialize, and return a duplicate vector */
CkitVector2 *
CkitV2Duplicate(a)
     CkitVector2 *a;
{
  CkitVector2 *v = NEWTYPE(CkitVector2);
  v->x = a->x;  v->y = a->y;
  return(v);
}

/* multiply a point by a projective matrix and return the transformed point */
CkitPoint2 *
CkitV2MulPointByProjMatrix(pin, m, pout)
     CkitPoint2 *pin, *pout;
     CkitMatrix3 *m;
{
  double w;
  pout->x = (pin->x * m->element[0][0]) +
    (pin->y * m->element[1][0]) + m->element[2][0];
  pout->y = (pin->x * m->element[0][1]) +
    (pin->y * m->element[1][1]) + m->element[2][1];
  w    = (pin->x * m->element[0][2]) +
    (pin->y * m->element[1][2]) + m->element[2][2];
  if (w != 0.0) { pout->x /= w;  pout->y /= w; }
  return(pout);
}

/* multiply together matrices c = ab */
/* note that c must not point to either of the input matrices */
CkitMatrix3 *
CkitV2MatMul(a, b, c)
     CkitMatrix3 *a, *b, *c;
{
  int i, j, k;
  for (i=0; i<3; i++) {
    for (j=0; j<3; j++) {
      c->element[i][j] = 0;
      for (k=0; k<3; k++) c->element[i][j] +=
        a->element[i][k] * b->element[k][j];
    }
  }
  return(c);
}

/* transpose matrix a, return b */
CkitMatrix3 *
CkitTransposeMatrix3(a, b)
     CkitMatrix3 *a, *b;
{
  int i, j;
  for (i=0; i<3; i++) {
    for (j=0; j<3; j++)
      b->element[i][j] = a->element[j][i];
  }
  return(b);
}




/******************/
/*   3d Library	  */
/******************/

/* returns squared length of input vector */
double 
CkitV3SquaredLength(a)
     CkitVector3 *a;
{
  return((a->x * a->x)+(a->y * a->y)+(a->z * a->z));
}

/* returns length of input vector */
double 
CkitV3Length(a)
     CkitVector3 *a;
{
  return(sqrt(CkitV3SquaredLength(a)));
}

/* negates the input vector and returns it */
CkitVector3 *
CkitV3Negate(v)
     CkitVector3 *v;
{
  v->x = -v->x;  v->y = -v->y;  v->z = -v->z;
  return(v);
}

/* normalizes the input vector and returns it */
CkitVector3 *
CkitV3Normalize(v)
     CkitVector3 *v;
{
  double len = CkitV3Length(v);
  if (len != 0.0) { v->x /= len;	v->y /= len; v->z /= len; }
  return(v);
} 

/* scales the input vector to the new length and returns it */
CkitVector3 *
CkitV3Scale(v, newlen)
     CkitVector3 *v;
     double newlen;
{
  double len = CkitV3Length(v);
  if (len != 0.0) {
    v->x *= newlen/len;   v->y *= newlen/len;  v->z *= newlen/len;
  }
  return(v);
}


/* return vector sum c = a+b */
CkitVector3 *
CkitV3Add(a, b, c)
     CkitVector3 *a, *b, *c;
{
  c->x = a->x+b->x;  c->y = a->y+b->y;  c->z = a->z+b->z;
  return(c);
}

/* return vector difference c = a-b */
CkitVector3 *
CkitV3Sub(a, b, c)
     CkitVector3 *a, *b, *c;
{
  c->x = a->x-b->x;  c->y = a->y-b->y;  c->z = a->z-b->z;
  return(c);
}

/* return the dot product of vectors a and b */
double 
CkitV3Dot(a, b)
     CkitVector3 *a, *b;
{
  return((a->x*b->x)+(a->y*b->y)+(a->z*b->z));
}

/* linearly interpolate between vectors by an amount alpha */
/* and return the resulting vector. */
/* When alpha=0, result=lo.  When alpha=1, result=hi. */
CkitVector3 *
CkitV3Lerp(lo, hi, alpha, result)
     CkitVector3 *lo, *hi, *result;
     double alpha;
{
  result->x = LERP(alpha, lo->x, hi->x);
  result->y = LERP(alpha, lo->y, hi->y);
  result->z = LERP(alpha, lo->z, hi->z);
  return(result);
}

/* make a linear combination of two vectors and return the result. */
/* result = (a * ascl) + (b * bscl) */
CkitVector3 *
CkitV3Combine (a, b, result, ascl, bscl)
     CkitVector3 *a, *b, *result;
     double ascl, bscl;
{
  result->x = (ascl * a->x) + (bscl * b->x);
  result->y = (ascl * a->y) + (bscl * b->y);
  result->z = (ascl * a->z) + (bscl * b->z);
  return(result);
}


/* multiply two vectors together component-wise and return the result */
CkitVector3 *
CkitV3Mul (a, b, result)
     CkitVector3 *a, *b, *result;
{
  result->x = a->x * b->x;
  result->y = a->y * b->y;
  result->z = a->z * b->z;
  return(result);
}

/* return the distance between two points */
double 
CkitV3DistanceBetween2Points(a, b)
     CkitPoint3 *a, *b;
{
  double dx = a->x - b->x;
  double dy = a->y - b->y;
  double dz = a->z - b->z;
  return(sqrt((dx*dx)+(dy*dy)+(dz*dz)));
}

/* return the cross product c = a cross b */
CkitVector3 *
CkitV3Cross(a, b, c)
     CkitVector3 *a, *b, *c;
{
  c->x = (a->y*b->z) - (a->z*b->y);
  c->y = (a->z*b->x) - (a->x*b->z);
  c->z = (a->x*b->y) - (a->y*b->x);
  return(c);
}

/* create, initialize, and return a new vector */
CkitVector3 *
V3New(x, y, z)
     double x, y, z;
{
  CkitVector3 *v = NEWTYPE(CkitVector3);
  v->x = x;  v->y = y;  v->z = z;
  return(v);
}

/* create, initialize, and return a duplicate vector */
CkitVector3 *
CkitV3Duplicate(a)
     CkitVector3 *a;
{
  CkitVector3 *v = NEWTYPE(CkitVector3);
  v->x = a->x;  v->y = a->y;  v->z = a->z;
  return(v);
}


/* multiply a point by a matrix and return the transformed point */
CkitPoint3 *
CkitV3MulPointByMatrix(pin, m, pout)
     CkitPoint3 *pin, *pout;
     CkitMatrix3 *m;
{
  pout->x = (pin->x * m->element[0][0]) + (pin->y * m->element[1][0]) +
    (pin->z * m->element[2][0]);
  pout->y = (pin->x * m->element[0][1]) + (pin->y * m->element[1][1]) +
    (pin->z * m->element[2][1]);
  pout->z = (pin->x * m->element[0][2]) + (pin->y * m->element[1][2]) +
    (pin->z * m->element[2][2]);
  return(pout);
}

/* multiply a point by a projective matrix and return the transformed point */
CkitPoint3 *
CkitV3MulPointByProjMatrix(pin, m, pout)
     CkitPoint3 *pin, *pout;
     CkitMatrix4 *m;
{
  double w;
  pout->x = (pin->x * m->element[0][0]) + (pin->y * m->element[1][0]) +
    (pin->z * m->element[2][0]) + m->element[3][0];
  pout->y = (pin->x * m->element[0][1]) + (pin->y * m->element[1][1]) +
    (pin->z * m->element[2][1]) + m->element[3][1];
  pout->z = (pin->x * m->element[0][2]) + (pin->y * m->element[1][2]) +
    (pin->z * m->element[2][2]) + m->element[3][2];
  w =    (pin->x * m->element[0][3]) + (pin->y * m->element[1][3]) +
    (pin->z * m->element[2][3]) + m->element[3][3];
  if (w != 0.0) { pout->x /= w;  pout->y /= w;  pout->z /= w; }
  return(pout);
}

/* multiply together matrices c = ab */
/* note that c must not point to either of the input matrices */
CkitMatrix4 *
CkitV3MatMul(a, b, c)
     CkitMatrix4 *a, *b, *c;
{
  int i, j, k;
  for (i=0; i<4; i++) {
    for (j=0; j<4; j++) {
      c->element[i][j] = 0;
      for (k=0; k<4; k++) c->element[i][j] +=
        a->element[i][k] * b->element[k][j];
    }
  }
  return(c);
}

/***********************/
/*   Useful Routines   */
/***********************/

/* return roots of ax^2+bx+c */
/* stable algebra derived from Numerical Recipes by Press et al.*/
int 
CkitQuadraticRoots(a, b, c, roots)
     double a, b, c, *roots;
{
  double d, q;
  int count = 0;
  d = (b*b)-(4*a*c);
  if (d < 0.0) { *roots = *(roots+1) = 0.0;  return(0); }
  q =  -0.5 * (b + (SGN(b)*sqrt(d)));
  if (a != 0.0)  { *roots++ = q/a; count++; }
  if (q != 0.0) { *roots++ = c/q; count++; }
  return(count);
}


/* generic 1d regula-falsi step.  f is function to evaluate */
/* interval known to contain root is given in left, right */
/* returns new estimate */
double 
CkitRegulaFalsi(f, left, right)
     double (*f)(), left, right;
{
  double d = (*f)(right) - (*f)(left);
  if (d != 0.0) return (right - (*f)(right)*(right-left)/d);
  return((left+right)/2.0);
}

/* generic 1d Newton-Raphson step. f is function, df is derivative */
/* x is current best guess for root location. Returns new estimate */
double 
CkitNewtonRaphson(f, df, x)
     double (*f)(), (*df)(), x;
{
  double d = (*df)(x);
  if (d != 0.0) return (x-((*f)(x)/d));
  return(x-1.0);
}


/* hybrid 1d Newton-Raphson/Regula Falsi root finder. */
/* input function f and its derivative df, an interval */
/* left, right known to contain the root, and an error tolerance */
/* Based on Blinn */
double 
Ckit_NR_RegulaFalsi(left, right, tolerance, f, df)
     double left, right, tolerance;
     double (*f)(), (*df)();
{
  double newx = left;
  while (ABS((*f)(newx)) > tolerance) {
    newx = CkitNewtonRaphson(f, df, newx);
    if (newx < left || newx > right)
      newx = CkitRegulaFalsi(f, left, right);
    if ((*f)(newx) * (*f)(left) <= 0.0) right = newx;
    else left = newx;
  }
  return(newx);
}

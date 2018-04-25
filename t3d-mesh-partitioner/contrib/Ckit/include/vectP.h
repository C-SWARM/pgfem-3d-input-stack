/*  
 * This is a renamed, and re-engineered (Petr Krysl 1995) version of: 
 * GraphicsGems.h
 * Version 1.0 - Andrew Glassner
 * from "Graphics Gems", Academic Press, 1990
 */

#ifndef VECTP_H
#   define VECTP_H 1

#include <math.h>
#include <stdlib.h>
#include "ckitcfg.h"
#include "memoryP.h"			/* Memory management routines */

/* Set precision here */
#ifdef CKITFLOAT_PREC
typedef CKITFLOAT_PREC CkitFloat;
#else
typedef double CkitFloat;
#endif

/*********************/
/* 2d geometry types */
/*********************/

typedef struct CkitPoint2Struct {	/* 2d point */
  CkitFloat x, y;
} CkitPoint2;
typedef CkitPoint2 CkitVector2;

typedef struct CkitIntPoint2Struct {	/* 2d integer point */
  int x, y;
} CkitIntPoint2;

typedef struct CkitMatrix3Struct {	/* 3-by-3 matrix */
  CkitFloat element[3][3];
} CkitMatrix3;

typedef struct CkitBox2dStruct {		/* 2d box */
  CkitPoint2 min, max;
} CkitBox2;


/*********************/
/* 3d geometry types */
/*********************/

typedef struct CkitPoint3Struct {	/* 3d point */
  CkitFloat x, y, z;
} CkitPoint3;
typedef CkitPoint3 CkitVector3;

typedef struct CkitIntPoint3Struct {	/* 3d integer point */
  int x, y, z;
} CkitIntPoint3;


typedef struct CkitMatrix4Struct {	/* 4-by-4 matrix */
  CkitFloat element[4][4];
} CkitMatrix4;

typedef struct CkitBox3dStruct {		/* 3d box */
  CkitPoint3 min, max;
} CkitBox3;



/***********************/
/* one-argument macros */
/***********************/

/* absolute value of a */
#define ABS(a)		(((a)<0) ? -(a) : (a))

/* round a to nearest int */
#define ROUND(a)	floor((a)+0.5)

/* take sign of a, either -1, 0, or 1 */
#define ZSGN(a)		(((a)<0) ? -1 : (a)>0 ? 1 : 0)

/* take binary sign of a, either -1, or 1 if >= 0 */
#define SGN(a)		(((a)<0) ? -1 : 1)

/* shout if something that should be true isn't */
#define ASSERT(x) \
if (!(x)) fprintf(stderr," Assert failed: x\n");

/* square a */
#define SQR(a)		((a)*(a))


/***********************/
/* two-argument macros */
/***********************/

#ifndef MIN
/* find minimum of a and b */
#define MIN(a,b)	(((a)<(b))?(a):(b))

/* find maximum of a and b */
#define MAX(a,b)	(((a)>(b))?(a):(b))
#endif

#if 0 /* This is hardly useful, as it is architecture dependent */
/* swap a and b (see Gem by Wyvill) */
#define SWAP(a,b)	{ a^=b; b^=a; a^=b; }
#endif

/* linear interpolation from l (when a=0) to h (when a=1)*/
/* (equal to (a*h)+((1-a)*l) */
#define LERP(a,l,h)	((l)+(((h)-(l))*(a)))

/* clamp the input to the specified range */
#define CLAMP(v,l,h)	((v)<(l) ? (l) : (v) > (h) ? (h) : v)


/****************************/
/* memory allocation macros */
/****************************/

/* create a new instance of a structure (see Gem by Hultquist) */
#define NEWSTRUCT(x)	(struct x *)(make_node(sizeof(struct x)))

/* create a new instance of a type */
#define NEWTYPE(x)	(x *)(make_node(sizeof(x)))


/********************/
/* useful constants */
/********************/

#if 0 /* Not enough precision in them.  Sorry. */
#define PI		3.141592	/* the venerable pi */
#define PITIMES2	6.283185	/* 2 * pi */
#define PIOVER2		1.570796	/* pi / 2 */
#define E		2.718282	/* the venerable e */
#define SQRT2		1.414214	/* sqrt(2) */
#define SQRT3		1.732051	/* sqrt(3) */
#define GOLDEN		1.618034	/* the golden ratio */
#define DTOR		0.017453	/* convert degrees to radians */
#define RTOD		57.29578	/* convert radians to degrees */
#endif


/************/
/* booleans */
/************/

#ifndef TRUE
#define TRUE		1
#define FALSE		0
#endif
#ifndef ON
#define ON		1
#define OFF		0
#endif

typedef int boolean;			/* boolean data type */
typedef boolean flag;			/* flag data type */


/* PROTOTYPES */
#ifdef STDC_HEADERS


/******************/
/*   2d Library	  */
/******************/

/* returns squared length of input vector */
double 
CkitV2SquaredLength(CkitVector2 *a);

/* returns length of input vector */
double 
CkitV2Length(CkitVector2 *a);

/* negates the input vector and returns it */
CkitVector2 *
CkitV2Negate(CkitVector2 *v);

/* normalizes the input vector and returns it */
CkitVector2 *
CkitV2Normalize(CkitVector2 *v);

/* scales the input vector to the new length and returns it */
CkitVector2 *
CkitV2Scale(CkitVector2 *v, double newlen);

/* return vector sum c = a+b */
CkitVector2 *
CkitV2Add(CkitVector2 *a, CkitVector2 *b, CkitVector2 *c);

/* return vector difference c = a-b */
CkitVector2 *
CkitV2Sub(CkitVector2 *a, CkitVector2 *b, CkitVector2 *c);

/* return the dot product of vectors a and b */
double 
CkitV2Dot(CkitVector2 *a, CkitVector2 *b);

/* linearly interpolate between vectors by an amount alpha */
/* and return the resulting vector. */
/* When alpha=0, result=lo.  When alpha=1, result=hi. */
CkitVector2 *
CkitV2Lerp(CkitVector2 *lo, CkitVector2 *hi, double alpha,
           CkitVector2 *result);

/* make a linear combination of two vectors and return the result. */
/* result = (a * ascl) + (b * bscl) */
CkitVector2 *
CkitV2Combine (CkitVector2 *a, CkitVector2 *b, CkitVector2 *result,
               double ascl, double bscl);

/* multiply two vectors together component-wise */
CkitVector2 *
CkitV2Mul (CkitVector2 *a, CkitVector2 *b, CkitVector2 *result);

/* return the distance between two points */
double 
CkitV2DistanceBetween2Points(CkitPoint2 *a, CkitPoint2 *b);

/* return the vector perpendicular to the input vector a */
CkitVector2 *
CkitV2MakePerpendicular(CkitVector2 *a, CkitVector2 *ap);

/* create, initialize, and return a new vector */
CkitVector2 *
CkitV2New(double x, double y);

/* create, initialize, and return a duplicate vector */
CkitVector2 *
CkitV2Duplicate(CkitVector2 *a);

/* multiply a point by a projective matrix and return the transformed point */
CkitPoint2 *
CkitV2MulPointByProjMatrix(CkitPoint2 *pin, CkitMatrix3 *m, CkitPoint2 *pout);

/* multiply together matrices c = ab */
/* note that c must not point to either of the input matrices */
CkitMatrix3 *
CkitV2MatMul(CkitMatrix3 *a, CkitMatrix3 *b, CkitMatrix3 *c);

/* transpose matrix a, return b */
CkitMatrix3 *
CkitTransposeMatrix3(CkitMatrix3 *a, CkitMatrix3 *b);

/* assigns a=b */
#define CkitV2AssignByPtr(a, b) \
   { (a)->x = (b)->x; (a)->y = (b)->y; } 

/* assigns a=b with a and b being vectors, not pointers */
#define CkitV2Assign(a, b) \
   { (a).x = (b).x; (a).y = (b).y; } 


/******************/
/*   3d Library	  */
/******************/

/* returns squared length of input vector */
double 
CkitV3SquaredLength(CkitVector3 *a);

/* returns length of input vector */
double 
CkitV3Length(CkitVector3 *a);

/* negates the input vector and returns it */
CkitVector3 *
CkitV3Negate(CkitVector3 *v);

/* normalizes the input vector and returns it */
CkitVector3 *
CkitV3Normalize(CkitVector3 *v);

/* scales the input vector to the new length and returns it */
CkitVector3 *
CkitV3Scale(CkitVector3 *v, double newlen);

/* return vector sum c = a+b */
CkitVector3 *
CkitV3Add(CkitVector3 *a, CkitVector3 *b, CkitVector3 *c);

/* return vector difference c = a-b */
CkitVector3 *
CkitV3Sub(CkitVector3 *a, CkitVector3 *b, CkitVector3 *c);

/* return the dot product of vectors a and b */
double 
CkitV3Dot(CkitVector3 *a, CkitVector3 *b);

/* linearly interpolate between vectors by an amount alpha */
/* and return the resulting vector. */
/* When alpha=0, result=lo.  When alpha=1, result=hi. */
CkitVector3 *
CkitV3Lerp(CkitVector3 *lo, CkitVector3 *hi, double alpha,
           CkitVector3 *result);

/* make a linear combination of two vectors and return the result. */
/* result = (a * ascl) + (b * bscl) */
CkitVector3 *
CkitV3Combine (CkitVector3 *a, CkitVector3 *b, CkitVector3 *result,
               double ascl, double bscl);

/* multiply two vectors together component-wise and return the result */
CkitVector3 *
CkitV3Mul (CkitVector3 *a, CkitVector3 *b, CkitVector3 *result);

/* return the distance between two points */
double 
CkitV3DistanceBetween2Points(CkitPoint3 *a, CkitPoint3 *b);

/* return the cross product c = a cross b */
CkitVector3 *
CkitV3Cross(CkitVector3 *a, CkitVector3 *b, CkitVector3 *c);

/* create, initialize, and return a new vector */
CkitVector3 *
V3New(double x, double y, double z);

/* create, initialize, and return a duplicate vector */
CkitVector3 *
CkitV3Duplicate(CkitVector3 *a);

/* multiply a point by a matrix and return the transformed point */
CkitPoint3 *
CkitV3MulPointByMatrix(CkitPoint3 *pin, CkitMatrix3 *m, CkitPoint3 *pout);

/* multiply a point by a projective matrix and return the transformed point */
CkitPoint3 *
CkitV3MulPointByProjMatrix(CkitPoint3 *pin, CkitMatrix4 *m, CkitPoint3 *pout);

/* multiply together matrices c = ab */
/* note that c must not point to either of the input matrices */
CkitMatrix4 *
CkitV3MatMul(CkitMatrix4 *a, CkitMatrix4 *b, CkitMatrix4 *c);

/* assigns a=b */
#define CkitV3AssignByPtr(a, b) \
   { (a)->x = (b)->x; (a)->y = (b)->y; (a)->z = (b)->z; } 

/* assigns a=b with a and b being vectors, not pointers */
#define CkitV3Assign(a, b) \
   { (a).x = (b).x; (a).y = (b).y; (a).z = (b).z; } 


/***********************/
/*   Useful Routines   */
/***********************/

/* return roots of ax^2+bx+c */
/* stable algebra derived from Numerical Recipes by Press et al.*/
int 
CkitQuadraticRoots(double a, double b, double c, double roots[]);

/* generic 1d regula-falsi step.  f is function to evaluate */
/* interval known to contain root is given in left, right */
/* returns new estimate */
double 
CkitRegulaFalsi(double (*f)(double), double left, double right);

/* generic 1d Newton-Raphson step. f is function, df is derivative */
/* x is current best guess for root location. Returns new estimate */
double 
NewtonRaphson(double (*f)(double), double (*df)(double), double x);

/* hybrid 1d Newton-Raphson/Regula Falsi root finder. */
/* input function f and its derivative df, an interval */
/* left, right known to contain the root, and an error tolerance */
/* Based on Blinn */
double 
Ckit_NR_RegulaFalsi(double left, double right, double tolerance,
                    double (*f)(double), double (*df)(double));

#else  /* !STDC_HEADERS */

extern double CkitV2SquaredLength(), CkitV2Length();
extern double CkitV2Dot(), CkitV2DistanceBetween2Points();
extern CkitVector2 *CkitV2Negate(), *CkitV2Normalize(),
                   *CkitV2Scale(), *CkitV2Add(), *CkitV2Sub();
extern CkitVector2 *CkitV2Lerp(), *CkitV2Combine(),
                   *CkitV2Mul(), *CkitV2MakePerpendicular();
extern CkitVector2 *CkitV2New(), *CkitV2Duplicate();
extern CkitPoint2 *CkitV2MulPointByProjMatrix();
extern CkitMatrix3 *VCkit2MatMul(), *CkitTransposeMatrix3();

extern double CkitV3SquaredLength(), CkitV3Length();
extern double CkitV3Dot(), CkitV3DistanceBetween2Points();
extern CkitVector3 *CkitV3Normalize(), *CkitV3Scale(),
                   *CkitV3Add(), *CkitV3Sub();
extern CkitVector3 *CkitV3Lerp(), *CkitV3Combine(),
                   *CkitV3Mul(), *CkitV3Cross();
extern CkitVector3 *CkitV3New(), *CkitV3Duplicate();
extern CkitPoint3 *CkitV3MulPointByMatrix(), *CkitV3MulPointByProjMatrix();
extern CkitMatrix4 *CkitV3MatMul();

extern double CkitRegulaFalsi(), CkitNewtonRaphson(), Ckit_NR_RegulaFalsi();

#endif /* !STDC_HEADERS */

#if 0 /* Use the following when renaming the routines to fit your needs */

/* Use like this: copy the lines that you need into a header of your own, */
/* replace XXX by a name  that you like.  Example:  I want to call */
/* the function CkitV2Lerp by the name LinearInterp2D.  Then I place */
/* the definition */
/* #define LinearInterp2D CkitV2Lerp */
/* into a header that is included in the source using */
/* the name LinearInterp2D */

#define XXX CkitV2SquaredLength
#define XXX CkitV2Length
#define XXX CkitV2Dot 
#define XXX CkitV2DistanceBetween2Points
#define XXX CkitV2Negate 
#define XXX CkitV2Normalize
#define XXX CkitV2Scale 
#define XXX CkitV2Add 
#define XXX CkitV2Sub
#define XXX CkitV2Lerp 
#define XXX CkitV2Combine
#define XXX CkitV2Mul 
#define XXX CkitV2MakePerpendicular
#define XXX CkitV2New 
#define XXX CkitV2Duplicate
#define XXX CkitV2MulPointByProjMatrix
#define XXX Ckit2MatMul 
#define XXX CkitTransposeMatrix3

 
#define XXX CkitV3SquaredLength 
#define XXX CkitV3Length
#define XXX CkitV3Dot 
#define XXX CkitV3DistanceBetween2Points
#define XXX CkitV3Normalize 
#define XXX CkitV3Scale
#define XXX CkitV3Add 
#define XXX CkitV3Sub
#define XXX CkitV3Lerp 
#define XXX CkitV3Combine
#define XXX CkitV3Mul 
#define XXX CkitV3Cross
#define XXX CkitV3New 
#define XXX CkitV3Duplicate
#define XXX CkitV3MulPointByMatrix CkitV3MulPointByProjMatrix
#define XXX CkitMatrix4 
#define XXX CkitV3MatMul

#define XXX CkitRegulaFalsi 
#define XXX CkitNewtonRaphson 
#define XXX Ckit_NR_RegulaFalsi

#endif

#ifdef WANT_VECTP_ALIASES

/* Some aliases of the vector library routines: */
#define v2_assign_by_ptr CkitV2AssignByPtr
#define v2_assign CkitV2Assign
#define v2_length CkitV2Length
#define v2_dot CkitV2Dot 
#define v2_distance CkitV2DistanceBetween2Points
#define v2_negate CkitV2Negate 
#define v2_normalize CkitV2Normalize
#define v2_scale CkitV2Scale 
#define v2_add CkitV2Add 
#define v2_sub CkitV2Sub
#define v2_lerp CkitV2Lerp 
#define v2_comb CkitV2Combine
#define v2_perpend CkitV2MakePerpendicular

#define v3_assign_by_ptr CkitV3AssignByPtr
#define v3_assign CkitV3Assign
#define v3_length CkitV3Length
#define v3_dot CkitV3Dot 
#define v3_distance CkitV3DistanceBetween2Points
#define v3_normalize CkitV3Normalize 
#define v3_scale CkitV3Scale
#define v3_add CkitV3Add 
#define v3_sub CkitV3Sub
#define v3_lerp CkitV3Lerp 
#define v3_comb CkitV3Combine
#define v3_mul CkitV3Mul 
#define v3_cross CkitV3Cross

#endif

#endif

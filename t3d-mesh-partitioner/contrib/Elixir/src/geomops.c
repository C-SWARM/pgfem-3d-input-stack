/*

              ####### #         ###   #     #   ###   ######
              #       #          #     #   #     #    #     #
              #       #          #      # #      #    #     #
              #####   #          #       #       #    ######
              #       #          #      # #      #    #   #
              #       #          #     #   #     #    #    #
              ####### #######   ###   #     #   ###   #     #

                   
                  Copyright: 1994 Petr Krysl

   Czech Technical University in Prague, Faculty of Civil Engineering,
      Dept. Structural Mechanics, 166 29 Prague, Czech Republic,
                  email: pk@power2.fsv.cvut.cz
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include "Econfig.h" 
#include <math.h>
#include <stdlib.h>

#include "Eview.h"
#include "Egraphic.h"
#include "Ebezier.h"
#include "Egeomops.h"



int 
Sign_of_FPNum(FPNum a)
{
    if (a < 0.) {
        return -1;
    } else if (a > 0.) {
        return 1;
    } else {
        return 0;
    }
}



/* returns >0 for points p1 and p2 in the same half-plane defined by the */
/* line l; <0 for points p1, p2 falling on opposite sides; 0 for points */
/* lying on the line */

int 
PointsInTheSameHalfPlane(WC2DRec *flp, WC2DRec *llp, WC2DRec *p1, WC2DRec *p2)
{
    FPNum dx, dx1, dx2, dy, dy1, dy2;
    
    dx  = llp->x  - flp->x;
    dy  = llp->y  - flp->y;
    dx1 = p1->x - flp->x;
    dy1 = p1->y - flp->y;
    dx2 = p2->x - llp->x;
    dy2 = p2->y - llp->y;

    return Sign_of_FPNum(dx*dy1 - dy*dx1) * Sign_of_FPNum(dx*dy2 - dy*dx2);
}




BOOLEAN 
LinesIntersect(WC2DRec *fp1, WC2DRec *lp1, /* first line points */
               WC2DRec *fp2, WC2DRec *lp2  /* second line points */
               )
{  /* from Sedgewick, p. 313 */
     EBoxVC2DRec b1, b2;      /* Note that boxes must have first corner */
				/* as lower-left one and second corner as */
				/* upper-right one. */
     
     b1.left   = min(fp1->x, lp1->x);
     b1.bottom = min(fp1->y, lp1->y);
     b1.right  = max(fp1->x, lp1->x);
     b1.top    = max(fp1->y, lp1->y);
     b2.left   = min(fp2->x, lp2->x);
     b2.bottom = min(fp2->y, lp2->y);
     b2.right  = max(fp2->x, lp2->x);
     b2.top    = max(fp2->y, lp2->y);

     if(BoxesOverlap(&b1, &b2) &&
	PointsInTheSameHalfPlane(fp1, lp1, fp2, lp2) <= 0 &&
	PointsInTheSameHalfPlane(fp2, lp2, fp1, lp1) <= 0)
       return YES;
     else
       return NO;
}


/* Returns YES if the boxes overlap (one may contain the other) */

BOOLEAN 
BoxesOverlap(EBoxVC2DRec *b1, EBoxVC2DRec *b2)
{
  if ((b1->left   <= b2->right) && (b1->right  >= b2->left) && 
      (b1->bottom <= b2->top)   && (b1->top    >= b2->bottom))
    return YES;
  else
    return NO;
}


/* Returns YES if the boundaries of the two boxes intersect. */

BOOLEAN 
BoxesIntersect(EBoxVC2DRec *b1, EBoxVC2DRec *b2)
{
#define L left
#define R right
#define B bottom
#define T top  
#define B1(w) b1->w
#define B2(w) b2->w

  if ((B1(L) >= B2(L)) && (B1(R) <= B2(R)) &&
      (B1(B) >= B2(B)) && (B1(T) <= B2(T)))
    return NO; /* box 2 contains box 1 */
  if ((B2(L) >= B1(L)) && (B2(R) <= B1(R)) &&
      (B2(B) >= B1(B)) && (B2(T) <= B1(T)))
    return NO; /* box 1 contains box 2 */
  if ((
       (B1(L) <= B2(R)) && (B2(L) <= B1(R)) &&
       (B1(B) <= B2(T)) && (B2(B) <= B1(T)))
      ||
      (
       (B2(L) <= B1(R)) && (B1(L) <= B2(R)) &&
       (B2(B) <= B1(T)) && (B1(B) <= B2(T))))
    return YES;	/* boxes overlap (but do not contain one another - see above)*/
  return NO;
#undef L 
#undef R 
#undef B 
#undef T 
#undef B1
#undef B2
}



BOOLEAN 
BoxContainsPoint(EBoxVC2DRec *b, WC2DRec *ap)
{
  if ((ap->x >= b->left)   && (ap->x <= b->right) &&
      (ap->y >= b->bottom) && (ap->y <= b->top))
    return YES;
  else
    return NO;
}



FPNum 
DistOfPnts(FPNum p1x, FPNum p1y, FPNum p2x, FPNum p2y)
{
  double tmp1, tmp2;

  tmp1 = p1x - p2x;
  tmp1 *= tmp1;
  tmp2 = p1y - p2y;
  tmp2 *= tmp2; 
  return (FPNum)sqrt(tmp1+tmp2);
}



FPNum 
DistOfPnts3D(double *p1, double *p2)
{
  double sum = 0, tmp = 0;

  tmp = *p1 - *p2;
  tmp *= tmp;
  sum += tmp;
  
  tmp = *(p1+1) - *(p2+1);
  tmp *= tmp;
  sum += tmp;
  
  tmp = *(p1+2) - *(p2+2);
  tmp *= tmp;
  sum += tmp;
  
  return (FPNum)sqrt(sum);
}




#if 0                           /* if needed, the routine may be enabled */
                                /* but note: we need the typedef below. */

typedef struct _ECircle2DWCRec {
  WC2DRec     center;
  FPNum             radius;
  union {
    AttributesRec            any;
    struct {
      BOOLEAN         have_any;
      unsigned short     style;
      unsigned short     width;
      EPixel        color;
    }                      circle;
  }                       attributes;
}                                            ECircle2DWCRec;

BOOLEAN 
BoxIntersectsCircle(EBoxVC2DRec *b, ECircle2DWCRec *c)
{
  FPNum min_r, max_r;
  FPNum cx, cy, r, tmp;

#define IN(px,py)         (((px) >= (b->left))   && \
			   ((px) <= (b->right))  && \
			   ((py) >= (b->bottom)) && \
			   ((py) <= (b->top)))
  
  cx = c->center.x;
  cy = c->center.y;
  r  = c->radius;
  
  tmp = DistOfPnts(cx, cy, b->left, b->bottom);
  min_r = tmp, max_r = tmp;
  
  tmp = DistOfPnts(cx, cy, b->right, b->bottom);
  min_r = min(tmp, min_r), max_r = max(tmp, max_r);
  if (min_r <= r && max_r >= r)
    return YES;	/* one corner is in the circle, the other is out */
  
  tmp = DistOfPnts(cx, cy, b->left, b->top);
  min_r = min(tmp, min_r), max_r = max(tmp, max_r);
  if (min_r <= r && max_r >= r)
    return YES;	/* one corner is in the circle, the other is out */
  
  tmp = DistOfPnts(cx, cy, b->right, b->top);
  min_r = min(tmp, min_r), max_r = max(tmp, max_r);
  if (min_r <= r && max_r >= r)
    return YES;	/* one corner is in the circle, the other is out */
  
  if (IN(cx-r,cy) && (!IN(cx+r,cy)))
    return YES;
  if (IN(cx+r,cy) && (!IN(cx-r,cy)))
    return YES;
  if (IN(cx,cy-r) && (!IN(cx,cy+r)))
    return YES;
  if (IN(cx,cy+r) && (!IN(cx,cy-r)))
    return YES;
  return NO;
}


/* returns YES if the box intersects the circle or if the box contains */
/* the circle; NO otherwise. */ 

BOOLEAN 
BoxOverlapsCircle(EBoxVC2DRec *b, ECircle2DWCRec *c)
{
  FPNum min_r, max_r;
  FPNum cx, cy, r, tmp;

#define IN(px,py)         (((px) >= (b->left))   && \
			   ((px) <= (b->right))  && \
			   ((py) >= (b->bottom)) && \
			   ((py) <= (b->top)))
  
  cx = c->center.x;
  cy = c->center.y;
  r  = c->radius;
  
  tmp = DistOfPnts(cx, cy, b->left, b->bottom);
  min_r = tmp, max_r = tmp;
  
  tmp = DistOfPnts(cx, cy, b->right, b->bottom);
  min_r = min(tmp, min_r), max_r = max(tmp, max_r);
  if (min_r <= r && max_r >= r)
    return YES;	/* one corner is in the circle, the other is out */
  
  tmp = DistOfPnts(cx, cy, b->left, b->top);
  min_r = min(tmp, min_r), max_r = max(tmp, max_r);
  if (min_r <= r && max_r >= r)
    return YES;	/* one corner is in the circle, the other is out */
  
  tmp = DistOfPnts(cx, cy, b->right, b->top);
  min_r = min(tmp, min_r), max_r = max(tmp, max_r);
  if (min_r <= r && max_r >= r)
    return YES;	/* one corner is in the circle, the other is out */
  
  if (min_r <= r && max_r <= r)
    return NO;	/* box is inside the circle */

  if (IN(cx-r,cy) && (!IN(cx+r,cy)))
    return YES; /* one of the circle handles is in the box the opposite not */
  if (IN(cx+r,cy) && (!IN(cx-r,cy)))
    return YES; /* one of the circle handles is in the box the opposite not */ 
  if (IN(cx,cy-r) && (!IN(cx,cy+r)))
    return YES; /* one of the circle handles is in the box the opposite not */
  if (IN(cx,cy+r) && (!IN(cx,cy-r)))
    return YES; /* one of the circle handles is in the box the opposite not */

  if ((IN(cx-r,cy) && (IN(cx+r,cy))) ||
      (IN(cx,cy-r) && (IN(cx,cy+r))))
    return YES;	/* circle is within box */
  
  return NO;
}
#endif /* disabled routines; see above */


BOOLEAN 
BoxIntersectsLine(EBoxVC2DRec *b, WC2DRec *flp, WC2DRec *llp)
{
  EBoxVC2DRec lbox;
  BOOLEAN intersects = NO;

  lbox.left    = min(flp->x, llp->x);
  lbox.right   = max(flp->x, llp->x);
  lbox.bottom  = min(flp->y, llp->y);
  lbox.top     = max(flp->y, llp->y);

  if (BoxesOverlap(b, &lbox)) {
    WC2DRec p1, p2, p3, p4;
    p1.x = b->left;  p1.y = b->bottom;
    p2.x = b->right; p2.y = b->bottom;
    p3.x = b->right; p3.y = b->top;
    p4.x = b->left;  p4.y = b->top;
    intersects =
      LinesIntersect(flp, llp, &p1, &p2) ||
	LinesIntersect(flp, llp, &p2, &p3) ||
	  LinesIntersect(flp, llp, &p3, &p4) ||
	    LinesIntersect(flp, llp, &p4, &p1);
    
  }
  return intersects;
}


int 
PointsWDApproxEq(FPNum x1, FPNum y1, FPNum z1, FPNum v1,
                 FPNum x2, FPNum y2, FPNum z2, FPNum v2,
                 FPNum eps)
{
  return (fabs(x2 - x1) <= eps &&
	  fabs(y2 - y1) <= eps &&
	  fabs(z2 - z1) <= eps &&
	  fabs(v2 - v1) <= eps 
	  );
}



int 
NextPointWD(FPNum sx, FPNum sy, FPNum sz,
            FPNum ex, FPNum ey, FPNum ez,
            FPNum sv, FPNum ev, FPNum lower, FPNum delta,
            FPNum *nx, FPNum *ny, FPNum *nz, FPNum *ns, FPNum *ne)
{
  double r;
#define EPS 1.0e-9

  if (fabs(ev - sv) < EPS ) {
    *nx = ex; *ny = ey; *nz = ez; *ns = ev; *ne = ev;
    return 1;
  }
  if (lower + delta <= ev) 
    *ns = lower + delta;
  else
    *ns = ev;

  r  = 1. / (ev - sv);
  r  *=  (*ns - sv);
  *nx = sx + r * (ex - sx);
  *ny = sy + r * (ey - sy);
  *nz = sz + r * (ez - sz);
  *ne = ev;
  return 1;
}

  


BOOLEAN 
BoxIntersectsTriangle(EBoxVC2DRec *b, VC2DRec *f, VC2DRec *s, VC2DRec *t)
{
  EBoxVC2DRec lbox;
  BOOLEAN intersects = NO;
  FPNum tmp;
  WC2DRec fp, lp;

  tmp = min(f->u, s->u); lbox.left   = min(tmp, t->u);
  tmp = min(f->v, s->v); lbox.bottom = min(tmp, t->v);
  tmp = max(f->u, s->u); lbox.right  = max(tmp, t->u);
  tmp = max(f->v, s->v); lbox.top    = max(tmp, t->v);

  if (BoxesOverlap(b, &lbox)) {
    fp.x   = f->u; fp.y = f->v;
    lp.x   = s->u; lp.y = s->v;
    if (!(intersects = BoxIntersectsLine(b, &fp, &lp))) {
      fp.x = s->u; fp.y = s->v;
      lp.x = t->u; lp.y = t->v;
      if (!(intersects = BoxIntersectsLine(b, &fp, &lp))) {
	fp.x = f->u; fp.y = f->v;
	lp.x = t->u; lp.y   = t->v;
	intersects = BoxIntersectsLine(b, &fp, &lp);
      }
    }
  }
  return intersects;
}
  


BOOLEAN 
BoxOverlapsTriangle(EBoxVC2DRec *b, VC2DRec *f, VC2DRec *s, VC2DRec *t)
{
  EBoxVC2DRec lbox;
  BOOLEAN intersects = NO;
  FPNum tmp;
  WC2DRec ap1, ap2, ap3;

  tmp = min(f->u, s->u); lbox.left   = min(tmp, t->u);
  tmp = min(f->v, s->v); lbox.bottom = min(tmp, t->v);
  tmp = max(f->u, s->u); lbox.right  = max(tmp, t->u);
  tmp = max(f->v, s->v); lbox.top    = max(tmp, t->v);

  if (BoxesOverlap(b, &lbox)) {
    ap1.x = f->u, ap1.y = f->v;
    ap2.x = s->u, ap2.y = s->v;
    ap3.x = t->u, ap3.y = t->v;
    intersects =
      BoxContainsPoint(b, &ap1) &&
	BoxContainsPoint(b, &ap2) &&
	  BoxContainsPoint(b, &ap3); /* Box contains triangle */
    if (!intersects) {
      ap1.x = b->left; ap1.y = b->bottom;
      intersects = TriangleContainsPoint(f, s, t, (VC2DRec *)&ap1);
      if (!intersects) {
	ap1.x = b->right; ap1.y = b->top;
	intersects = TriangleContainsPoint(f, s, t, (VC2DRec *)&ap1);
      }
      if (!intersects) {
	ap1.x = b->right; ap1.y = b->bottom;
	intersects = TriangleContainsPoint(f, s, t, (VC2DRec *)&ap1);
      }
      if (!intersects) {
	ap1.x = b->left; ap1.y = b->top;
	intersects = TriangleContainsPoint(f, s, t, (VC2DRec *)&ap1);
      }
      if (!intersects) 
	intersects = BoxIntersectsTriangle(b, f, s, t);
    }
  }
  return intersects;
}



BOOLEAN 
TriangleContainsPoint(VC2DRec *p1, VC2DRec *p2, VC2DRec *p3, VC2DRec  *apoint)
{
  FPNum tmp;
  WC2DRec lp;
  int number_of_xsections;

  if (p1->v > p2->v)		/* tmp = triangle extent in v dir */
    if (p2->v > p3->v)
      tmp = p1->v - p3->v;
    else
      tmp = p1->v - p2->v;
  else
    if (p1->v > p3->v)
      tmp = p2->v - p3->v;
    else
      tmp = p2->v - p1->v;
  
  lp.x = apoint->u;
  lp.y = 10 * tmp;

  number_of_xsections = 0;
  if (LinesIntersect((WC2DRec *)apoint, &lp, (WC2DRec *)p1, (WC2DRec *)p2))
    number_of_xsections++;
  if (LinesIntersect((WC2DRec *)apoint, &lp, (WC2DRec *)p1, (WC2DRec *)p3))
    number_of_xsections++;
  if (LinesIntersect((WC2DRec *)apoint, &lp, (WC2DRec *)p3, (WC2DRec *)p2))
    number_of_xsections++;
  if (number_of_xsections == 1 || number_of_xsections == 3)
    return YES;
  else
    return NO;
}

  


BOOLEAN 
ClipLineToView(FPNum xw_min, FPNum yw_min, FPNum xw_max, FPNum yw_max,
               FPNum *x1, FPNum *y1, FPNum *z1,
               FPNum *x2, FPNum *y2, FPNum *z2)
{
  FPNum u1 = 0., u2 = 1.;
  FPNum dx = *x2 - *x1;

  if (TestLineClip(-dx, *x1 - xw_min, &u1, &u2) == YES) {
    if (TestLineClip(dx, xw_max - *x1, &u1, &u2) == YES) {
      FPNum dy = *y2 - *y1, dz = *z2 - *z1;

      if (TestLineClip(-dy, *y1 - yw_min, &u1, &u2) == YES) {
	if (TestLineClip(dy, yw_max - *y1, &u1, &u2) == YES) {
	  if (u2 < 1.) {
	    *x2 = *x1 + u2 * dx;
	    *y2 = *y1 + u2 * dy;
	    *z2 = *z1 + u2 * dz;
	  }
	  if (u1 > 0.) {
	    *x1 = *x1 + u1 * dx;
	    *y1 = *y1 + u1 * dy;
	    *z1 = *z1 + u1 * dz;
	  }
	  return YES;
	}
      }
    }
  }
  return NO;
}



BOOLEAN 
TestLineClip(FPNum p, FPNum q, FPNum *u1, FPNum *u2)
{ 
  FPNum r;
  BOOLEAN result;

/* Liang-Barski line clipping */

  result = YES;
  if (p < 0.) { /* segment entering view */
    r = q / p;
    if (r > *u2)
      result = NO;
    else
      if (r > *u1)
	*u1 = r;
  } else {
    if (p > 0.) { /* segment leaving view */
      r = q / p;
      if (r < *u1)
	result = NO;
      else
	if (r < *u2)
	  *u2 = r;
    } else {			/* segment parallel to boundary */
      if (q < 0.)
	result = NO;
    }
  }
  return result;
}



BOOLEAN 
BoxIntersectsCBezCurve(EBoxVC2DRec *b, WC2DRec points[4])
{
  BOOLEAN intersects = NO;
  FPNum t, deltat, b0, b1, b2, b3;
  WC2DRec p1, p2;
  WC2DRec bp1, bp2, bp3, bp4;
  FPNum poly_length, box_dim;
  int ni, i;

  bp1.x = b->left; bp1.y = b->bottom;
  bp2.x = b->right; bp2.y = b->bottom;
  bp3.x = b->left; bp3.y = b->top;
  bp4.x = b->right; bp4.y = b->top;
  poly_length =
    DistOfPnts(points[0].x, points[0].y, points[1].x, points[1].y) +
      DistOfPnts(points[2].x, points[2].y, points[1].x, points[1].y) +
	DistOfPnts(points[2].x, points[2].y, points[3].x, points[3].y);
  box_dim = b->right - b->left + b->top - b->bottom;
  box_dim *= 0.5;
  ni = poly_length/box_dim;
	if(ni == 0)ni = 1;
  deltat = 1./ni;
  
  p1.x = points[0].x;
  p1.y = points[0].y;
  for (t = deltat, i = 0; i < ni; t += deltat, i++) {
    CubicBernstein(t, &b0, &b1, &b2, &b3);
    p2.x = 
      b0 * points[0].x +
	b1 * points[1].x +
	  b2 * points[2].x +
	    b3 * points[3].x;
    p2.y = 
      b0 * points[0].y +
	b1 * points[1].y +
	  b2 * points[2].y +
	    b3 * points[3].y;
      intersects =
	LinesIntersect(&p1, &p2, &bp1, &bp2) ||
	  LinesIntersect(&p1, &p2, &bp2, &bp3) ||
	    LinesIntersect(&p1, &p2, &bp3, &bp4) ||
	      LinesIntersect(&p1, &p2, &bp4, &bp1);
    if (intersects)
      break;
    p1.x = p2.x;
    p1.y = p2.y;
  }
  return intersects;
}


BOOLEAN 
BoxIntersectsRBezCurve(EBoxVC2DRec *b, int order,
                       WC2DRec *points, float *weights)
{
  BOOLEAN intersects = NO;
  FPNum t, deltat;
  double *bt, weight, tmp;
  WC2DRec p1, p2;
  WC2DRec bp1, bp2, bp3, bp4;
  FPNum poly_length = 0.0, box_dim;
  int ni, i, j;
  
  if((bt = (double *)calloc(order, sizeof(double))) == NULL){
    fprintf(stderr, "Memory allocation in BoxIntersectsRBezCurve failed");
    return(NO);
  }
  bp1.x = b->left; bp1.y = b->bottom;
  bp2.x = b->right; bp2.y = b->bottom;
  bp3.x = b->left; bp3.y = b->top;
  bp4.x = b->right; bp4.y = b->top;
  for(i = 1; i < order; i++)
    poly_length += DistOfPnts(points[i - 1].x, points[i - 1].y,
                              points[i].x, points[i].y);
  
  box_dim = b->right - b->left + b->top - b->bottom;
  box_dim *= 0.5;
  ni = poly_length/box_dim;
	if(ni == 0)ni = 1;
  deltat = 1./ni;
  
  p1.x = points[0].x;
  p1.y = points[0].y;
  for (t = deltat, i = 0; i < ni; t += deltat, i++) {
    Bernstein(order, (double)t, bt);
    p2.x = p2.y = 0.0;
    weight = 0.0;
    for(j = 0; j < order; j++){
      weight += (tmp = weights[j] * bt[j]);
      p2.x += points[j].x * tmp;
      p2.y += points[j].y * tmp;
    }
    p2.x /= weight;
    p2.y /= weight;
    intersects =
      LinesIntersect(&p1, &p2, &bp1, &bp2) ||
        LinesIntersect(&p1, &p2, &bp2, &bp3) ||
          LinesIntersect(&p1, &p2, &bp3, &bp4) ||
            LinesIntersect(&p1, &p2, &bp4, &bp1);
    if (intersects)
      break;
    p1.x = p2.x;
    p1.y = p2.y;
  }
  free(bt);
  return intersects;
}

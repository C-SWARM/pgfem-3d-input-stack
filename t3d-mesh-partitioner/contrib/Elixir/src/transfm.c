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
#include "Etransfm.h"

/* ======================================================================== */
/* PRIVATE */
/* ======================================================================== */

/* ======================================================================== */
/* PUBLIC */
/* ======================================================================== */

void 
AdjustVectorToShrink(FPNum *v, int nnodes, FPNum shrink)
{
  FPNum cv;
  int i;

  cv = v[0];
  for (i=1; i<nnodes; i++)
    cv += v[i];

  cv /= nnodes;
  for (i=0; i<nnodes; i++)
    v[i] = cv + shrink * (v[i] - cv);
}



BOOLEAN 
ComputeRotMatVects(WCRec *vectorUp, /* IN: vector up */
                   WCRec *normal,   /* INOUT: normal */
                   WCRec *vectorU,  /* OUT: vector U */
                   WCRec *vectorV   /* OUT: vector V */
                   )
{
  CrossProd3(vectorU, vectorUp, normal);
  CrossProd3(vectorV, normal, vectorU);
  if (!NormalizeVect3(vectorU))
    return NO;
  if (!NormalizeVect3(vectorV))
    return NO;
  if (!NormalizeVect3(normal))
    return NO;
  return YES;
}



void 
TransfPntWCToVC(WCRec *vectorU,
                WCRec *vectorV,
                WCRec *normal,
                WCRec *shift,
                WCRec *w,
                VCRec *v)
{
  double x, y, z;

  x = w->x - shift->x;  /* V + S = G ... shifts view coords origin */
  y = w->y - shift->y;
  z = w->z - shift->z;
  v->u = x * vectorU->x + y * vectorU->y + z * vectorU->z;
  v->v = x * vectorV->x + y * vectorV->y + z * vectorV->z;
  v->n = x *  normal->x + y *  normal->y + z *  normal->z;
}



void 
TransfPntVCToWC(WCRec *vectorU,
                WCRec *vectorV,
                WCRec *normal,
                WCRec *shift,
                VCRec *v, 
                WCRec *w)
{
  double x, y, z;

  x   = v->u * vectorU->x + v->v * vectorV->x + v->n * normal->x;
  y   = v->u * vectorU->y + v->v * vectorV->y + v->n * normal->y;
  z   = v->u * vectorU->z + v->v * vectorV->z + v->n * normal->z;
  w->x = x + shift->x;
  w->y = y + shift->y;
  w->z = z + shift->z;
}



void 
CrossProd3(WCRec *reslt_vec, WCRec *Vec1, WCRec *Vec2)
{
  reslt_vec->x = Vec1->y * Vec2->z - Vec1->z * Vec2->y;
  reslt_vec->y = Vec1->z * Vec2->x - Vec1->x * Vec2->z;
  reslt_vec->z = Vec1->x * Vec2->y - Vec1->y * Vec2->x; 
}



FPNum 
DotProd3(WCRec *Vec1, WCRec *Vec2)
{
  return (Vec1->x * Vec2->x + Vec1->y * Vec2->y + Vec1->z * Vec2->z);
}



BOOLEAN 
NormalizeVect3(WCRec *vec)
{
  double tmp, length;

  length = 0;
  tmp = vec->x; tmp *= tmp;
  length += tmp;
  tmp = vec->y; tmp *= tmp;
  length += tmp;
  tmp = vec->z; tmp *= tmp;
  length += tmp;
  if (length < (double)0) {
    return NO;
  } else if (length == (double)0) {
    vec->x = vec->y = vec->z = 0;
  } else {
    length = sqrt(length);
    vec->x /= length;
    vec->y /= length;
    vec->z /= length;
  }
  return YES;
}




void 
RotVectAboutVect(WCRec *aboutv, /* IN: vector to rotate about */
                 WCRec *avec    /* INOUT: original and rotated vector */
                 )
{
  double ad, cosad, sadad, cad2, rb1;
  WCRec rxb1 = {0.0, 0.0, 0.0};
		
  ad = sqrt(aboutv->x*aboutv->x + aboutv->y*aboutv->y + aboutv->z*aboutv->z);
  if (ad > 0) {
    cosad = cos(ad);
    sadad = sin(ad)/ad;
    cad2 = (1.- cosad)/ad/ad;
    rb1 = DotProd3(aboutv, avec);
    CrossProd3(&rxb1, aboutv, avec);
    avec->x = avec->x *cosad + aboutv->x*rb1*cad2 + rxb1.x*sadad;
    avec->y = avec->y *cosad + aboutv->y*rb1*cad2 + rxb1.y*sadad;
    avec->z = avec->z *cosad + aboutv->z*rb1*cad2 + rxb1.z*sadad;
  }
}




BOOLEAN 
VectorsColinear(FPNum epsilon, WCRec *v1, WCRec *v2)
{
  double l1, l2, dotp;

  l1   = sqrt(v1->x * v1->x + v1->y * v1->y + v1->z * v1->z);
  l2   = sqrt(v2->x * v2->x + v2->y * v2->y + v2->z * v2->z);
  dotp = DotProd3(v1, v2);
  if (l1 * l2 - fabs(dotp) < epsilon)
    return YES;
  else
    return NO;
}


void
MirrorPointInPlane(WCRec *point_on_plane, WCRec *unit_normal,
                   WCRec *point_to_mirror)
{
  double twodist;
  WCRec v;

  v.x = point_on_plane->x - point_to_mirror->x;
  v.y = point_on_plane->y - point_to_mirror->y;
  v.z = point_on_plane->z - point_to_mirror->z;
  twodist = 2 * DotProd3(&v, unit_normal);
  point_to_mirror->x += twodist * unit_normal->x;
  point_to_mirror->y += twodist * unit_normal->y;
  point_to_mirror->z += twodist * unit_normal->z;
}

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


#ifndef EGEOMOPS_H
#define EGEOMOPS_H

#ifdef __cplusplus
extern "C" {
#endif


int Sign_of_FPNum(FPNum a);
int PointsInTheSameHalfPlane(WC2DRec *f, WC2DRec *l, WC2DRec *p1, WC2DRec *p2);
FPNum DistOfPnts(FPNum p1x, FPNum p1y, FPNum p2x, FPNum p2y);
FPNum DistOfPnts3D(FPNum *p1, FPNum *p2);
BOOLEAN LinesIntersect(WC2DRec *f1, WC2DRec *l1, WC2DRec *f2, WC2DRec *l2);
BOOLEAN BoxesIntersect(EBoxVC2DRec *b1, EBoxVC2DRec *b2);
BOOLEAN BoxesOverlap(EBoxVC2DRec *b1, EBoxVC2DRec *b2);
BOOLEAN BoxContainsPoint(EBoxVC2DRec *b, WC2DRec *ap);
BOOLEAN BoxIntersectsLine(EBoxVC2DRec *b, WC2DRec *f, WC2DRec *l);
int NextPointWD(FPNum sx, FPNum sy, FPNum sz,
		FPNum ex, FPNum ey, FPNum ez,
		FPNum sv, FPNum ev, FPNum lower, FPNum delta,
		FPNum *nx, FPNum *ny, FPNum *nz, FPNum *ns, FPNum *ne);
int PointsWDApproxEq(FPNum x1, FPNum y1, FPNum z1, FPNum v1,
		     FPNum x2, FPNum y2, FPNum z2, FPNum v2,
		     FPNum eps);
BOOLEAN TriangleContainsPoint(VC2DRec *f, VC2DRec *s, VC2DRec *t, VC2DRec *p);
BOOLEAN BoxIntersectsTriangle(EBoxVC2DRec *b,
			      VC2DRec *f, VC2DRec *s, VC2DRec *t);
BOOLEAN BoxOverlapsTriangle(EBoxVC2DRec *b,
			      VC2DRec *f, VC2DRec *s, VC2DRec *t);
BOOLEAN TestLineClip(FPNum p, FPNum q, FPNum *u1, FPNum *u2);
BOOLEAN ClipLineToView(FPNum xw_min, FPNum yw_min, FPNum xw_max, FPNum yw_max,
		       FPNum *x1, FPNum *y1, FPNum *z1,
		       FPNum *x2, FPNum *y2, FPNum *z2);
BOOLEAN BoxIntersectsCBezCurve(EBoxVC2DRec *b, WC2DRec points[4]);
BOOLEAN 
BoxIntersectsRBezCurve(EBoxVC2DRec *b, int order,
                       WC2DRec *points, float *weights);


#ifdef __cplusplus
}
#endif

#endif

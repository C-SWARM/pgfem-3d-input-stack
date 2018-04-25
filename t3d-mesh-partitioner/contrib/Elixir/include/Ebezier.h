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


#ifndef EBEZIER_H
#define EBEZIER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <stdlib.h>
#include "Etypes.h"

void 
CubicBernstein(FPNum t, FPNum *b0, FPNum *b1, FPNum *b2, FPNum *b3);
void 
CubicBernsteinDeriv(FPNum t, FPNum *db0, FPNum *db1, FPNum *db2, FPNum *db3);
void 
CubicBernsteinZeroDerivative(FPNum t,
                             FPNum *b0, FPNum *b1, FPNum *b2, FPNum *b3);
void 
CubicBernstein1stDerivative(FPNum t,
                            FPNum *db0, FPNum *db1, FPNum *db2, FPNum *db3);
void 
CubicBernstein2ndDerivative(FPNum t,
                            FPNum *ddb0, FPNum *ddb1,
                            FPNum *ddb2, FPNum *ddb3);
void
CubicBernstein3rdDerivative(FPNum t,
                            FPNum *dddb0, FPNum *dddb1,
                            FPNum *dddb2, FPNum *dddb3);

void
CubBernsteinDeriv0(FPNum t, FPNum *b0, FPNum *b1, FPNum *b2, FPNum *b3);
void
CubBernsteinDeriv1(FPNum t, FPNum *db0, FPNum *db1, FPNum *db2, FPNum *db3);
void
CubBernsteinDeriv2(FPNum t,
                   FPNum *ddb0, FPNum *ddb1, FPNum *ddb2, FPNum *ddb3);
void
CubBernsteinDeriv3(FPNum t,
                   FPNum *dddb0, FPNum *dddb1, FPNum *dddb2, FPNum *dddb3);
void
CBCurvePointOn(FPNum t, WCRec points[4], WCRec *p);
void 
BCPatchPointOn(FPNum u, FPNum v, WCRec points[4][4], WCRec *p);
void 
BCPatchUVDeriv(FPNum u, FPNum v,
               int u_order, int v_order,
               WCRec points[4][4],
               WCRec *dr);

/* Works for Bezier polynomials of arbitrary order */
void
Bernstein(int order, double t, double *bt);

#ifdef __cplusplus
}
#endif

#endif

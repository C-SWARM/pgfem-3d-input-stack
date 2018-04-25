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
#include "Ebezier.h"

void 
CubicBernstein(FPNum t, FPNum *b0, FPNum *b1, FPNum *b2, FPNum *b3)
{
  double t2, t3, one_minus_t;

  one_minus_t = 1 - t;
  t2          = t * t;
  t3          = t2 * t;
  *b0         = one_minus_t * one_minus_t * one_minus_t;
  *b1         = 3 * t * one_minus_t * one_minus_t;
  *b2         = 3 * t2 * one_minus_t;
  *b3         = t3;
}



void 
CubicBernsteinDeriv(FPNum t, FPNum *db0, FPNum *db1, FPNum *db2, FPNum *db3)
{
  double t2;

  t2           = t * t;
  *db0         = -3.0 + 6.0 * t - 3.0 * t2;
  *db1         = 3.0 - 12.0 * t + 9.0 * t2;
  *db2         = 6.0 * t - 9.0 * t2;
  *db3         = 3.0 * t2;
}



void
CubBernsteinDeriv0(FPNum t, FPNum *b0, FPNum *b1, FPNum *b2, FPNum *b3)
{
  double t2, t3, one_minus_t;

  one_minus_t = 1 - t;
  t2          = t * t;
  t3          = t2 * t;
  *b0         = one_minus_t * one_minus_t * one_minus_t;
  *b1         = 3 * t * one_minus_t * one_minus_t;
  *b2         = 3 * t2 * one_minus_t;
  *b3         = t3;
}

void
CubBernsteinDeriv1(FPNum t, FPNum *db0, FPNum *db1, FPNum *db2, FPNum *db3)
{
  double t2;

  t2           = t * t;
  *db0         = -3.0 + 6.0 * t - 3.0 * t2;
  *db1         = 3.0 - 12.0 * t + 9.0 * t2;
  *db2         = 6.0 * t - 9.0 * t2;
  *db3         = 3.0 * t2;
}

void
CubBernsteinDeriv2(FPNum t,
                   FPNum *ddb0, FPNum *ddb1, FPNum *ddb2, FPNum *ddb3)
{
  *ddb0         = 6.0 - 6.0 * t;
  *ddb1         = -12.0 + 18.0 * t;
  *ddb2         = 6.0 - 18.0 * t;
  *ddb3         = 6.0 * t;
}

void
CubBernsteinDeriv3(FPNum t,
                   FPNum *dddb0, FPNum *dddb1, FPNum *dddb2, FPNum *dddb3)
{
  *dddb0         = -6.0;
  *dddb1         = 18.0;
  *dddb2         = -18.0;
  *dddb3         = 6.0;
}



void
CBCurvePointOn(FPNum t, WCRec points[4], WCRec *p)
{
  FPNum bt[4], tmp;
  int k;
  
  CubicBernstein(t, &bt[0], &bt[1], &bt[2], &bt[3]);
  p->x = p->y = p->z = 0.0;
  for(k = 0; k < 4; k++){
    tmp = bt[k];
    p->x += tmp * points[k].x;
    p->y += tmp * points[k].y;	
    p->z += tmp * points[k].z;
  }
}


void 
BCPatchPointOn(FPNum u, FPNum v, WCRec points[4][4], WCRec *p)
{
  FPNum bu[4],bv[4], tmp;
  int k, m;
  
  CubicBernstein(u, &bu[0], &bu[1], &bu[2], &bu[3]);
  CubicBernstein(v, &bv[0], &bv[1], &bv[2], &bv[3]);
  p->x = p->y = p->z = 0.0;
  for (k = 0; k < 4; k++) {
    for (m = 0; m < 4; m++) {
      tmp = bu[k] * bv[m];
      p->x += tmp * points[k][m].x;
      p->y += tmp * points[k][m].y;
      p->z += tmp * points[k][m].z;
    }
  }
}

void 
BCPatchUVDeriv(FPNum u, FPNum v,
               int u_order, int v_order,
               WCRec points[4][4],
               WCRec *dr)
{
  FPNum dbu[4],dbv[4], tmp;
  int k, m;
  
  switch(u_order){
  case 0: 
    CubBernsteinDeriv0(u, &dbu[0], &dbu[1], &dbu[2], &dbu[3]);
    break;
  case 1:
    CubBernsteinDeriv1(u, &dbu[0], &dbu[1], &dbu[2], &dbu[3]);
    break;
  case 2:
    CubBernsteinDeriv2(u, &dbu[0], &dbu[1], &dbu[2], &dbu[3]);
    break;
  case 3:
    CubBernsteinDeriv3(u, &dbu[0], &dbu[1], &dbu[2], &dbu[3]);
    break;
  }
  switch(v_order){
  case 0: 
    CubBernsteinDeriv0(v, &dbv[0], &dbv[1], &dbv[2], &dbv[3]);
    break;
  case 1:
    CubBernsteinDeriv1(v, &dbv[0], &dbv[1], &dbv[2], &dbv[3]);
    break;
  case 2:
    CubBernsteinDeriv2(v, &dbv[0], &dbv[1], &dbv[2], &dbv[3]);
    break;
	case 3:
    CubBernsteinDeriv3(v, &dbv[0], &dbv[1], &dbv[2], &dbv[3]);
    break;
  }
  dr->x = dr->y = dr->z = 0.0;
  for (k = 0; k < 4; k++) {
    for (m = 0; m < 4; m++) {
      tmp = dbu[k] * dbv[m];
      dr->x += tmp * points[k][m].x;
      dr->y += tmp * points[k][m].y;
      dr->z += tmp * points[k][m].z;
    }
  }
}



void
Bernstein(int order, double t, double *bt)
{
	double compl = 1.0 - t;
	int k;

	switch(order){
	case 1:
		*bt   = 1.0;
		return;
	case 2:
		*bt++ = compl;
		*bt   = t;
		return;
	case 3:
		*bt++ = compl * compl;
		*bt++ = 2.0 * compl * t;
		*bt   = t * t;
		return;
	case 4:
		*bt++ = compl * compl * compl;
		*bt++ = 3.0 * compl * compl * t;
		*bt++ = 3.0 * compl * t * t;
		*bt   = t * t * t;
		return;
	default:
		Bernstein(order - 1, t, bt + 1);
		*bt = *(bt + 1) * compl;
		for(bt++, k = 1; k < order - 1; k++, bt++)
			*bt = *(bt + 1) * compl + *bt * t;
		*bt = *bt * t;
		return;
	}
}


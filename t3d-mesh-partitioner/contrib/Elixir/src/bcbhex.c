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
#include <stdlib.h>

#include "Eview.h"
#include "Egraphic.h"
#include "Egeomops.h"
#include "Ecolors.h"
#include "Eactset.h"
#include "Ebezier.h"

#define BCBHEX_PRIVATE_HEADER
#include "Ebcbhex.h"


  
/* ========================================================================= */
/* PRIVATE  */
/* ========================================================================= */

#define STRING_BUFF_SIZE 1024

static char string_rep_buffer[STRING_BUFF_SIZE];

/* ========================================================================= */
/* PUBLIC  */
/* ========================================================================= */

GraphicObj *
CreateBCBHex(WCRec *points)
{
  EBCBHexWCRec *thedp;
  GraphicObj *g_p;

  thedp = (EBCBHexWCRec *)make_node(sizeof(EBCBHexWCRec));
  g_p = EGCreateGraphics(&bcbhex_graphic_methods, (caddr_t)thedp,
			 sizeof(EBCBHexWCRec));
  ModifyGeometry(g_p, points);
  thedp->attributes.spec.tessel_intervals = 4;
  thedp->attributes.spec.show_polygon     = YES;
  thedp->attributes.spec.show_surface     = YES;
  thedp->attributes.spec.fill_style       = FILL_SOLID;
  thedp->attributes.spec.color            = 0x0; 
  thedp->attributes.spec.edge_flag        = NO;
  thedp->attributes.spec.edge_color       = 0x0;
  thedp->attributes.spec.have_any         = NO;
  return g_p;
}
 

#define ASSIGN_P_P(ARRAY, ARRAY_I, PNTS4x4, PNTS4x4_I, PNTS4x4_J) \
   {                                                              \
      PNTS4x4[PNTS4x4_I][PNTS4x4_J].x = ARRAY[ARRAY_I].x;         \
      PNTS4x4[PNTS4x4_I][PNTS4x4_J].y = ARRAY[ARRAY_I].y;         \
      PNTS4x4[PNTS4x4_I][PNTS4x4_J].z = ARRAY[ARRAY_I].z;         \
    }

#define ASSIGN_P_P1DIM(ARRAY, ARRAY_I, PNTS4, PNTS4_I)            \
   {                                                              \
      PNTS4[PNTS4_I].x = ARRAY[ARRAY_I].x;                        \
      PNTS4[PNTS4_I].y = ARRAY[ARRAY_I].y;                        \
      PNTS4[PNTS4_I].z = ARRAY[ARRAY_I].z;                        \
    }

#define ASSIGN_TO_4x4_ARRAY(ARRAY, PNTS4x4,     \
                            P00, P10, P20, P30, \
                            P01, P11, P21, P31, \
                            P02, P12, P22, P32, \
                            P03, P13, P23, P33) \
   {                                            \
      ASSIGN_P_P(ARRAY, P00, PNTS4x4, 0, 0);    \
      ASSIGN_P_P(ARRAY, P10, PNTS4x4, 1, 0);    \
      ASSIGN_P_P(ARRAY, P20, PNTS4x4, 2, 0);    \
      ASSIGN_P_P(ARRAY, P30, PNTS4x4, 3, 0);    \
                                                \
      ASSIGN_P_P(ARRAY, P01, PNTS4x4, 0, 1);    \
      ASSIGN_P_P(ARRAY, P11, PNTS4x4, 1, 1);    \
      ASSIGN_P_P(ARRAY, P21, PNTS4x4, 2, 1);    \
      ASSIGN_P_P(ARRAY, P31, PNTS4x4, 3, 1);    \
                                                \
      ASSIGN_P_P(ARRAY, P02, PNTS4x4, 0, 2);    \
      ASSIGN_P_P(ARRAY, P12, PNTS4x4, 1, 2);    \
      ASSIGN_P_P(ARRAY, P22, PNTS4x4, 2, 2);    \
      ASSIGN_P_P(ARRAY, P32, PNTS4x4, 3, 2);    \
                                                \
      ASSIGN_P_P(ARRAY, P03, PNTS4x4, 0, 3);    \
      ASSIGN_P_P(ARRAY, P13, PNTS4x4, 1, 3);    \
      ASSIGN_P_P(ARRAY, P23, PNTS4x4, 2, 3);    \
      ASSIGN_P_P(ARRAY, P33, PNTS4x4, 3, 3);    \
   }

static void 
Draw(EView *v_p, GraphicObj *p)
{
  EBCBHexWCRec *thedp;
  WCRec points[4][4];

  thedp = (EBCBHexWCRec *)(p->spec.data);

  ASSIGN_TO_4x4_ARRAY(thedp->points, points,
                      0, 1, 2, 3,
                      4, 5, 6, 7,
                      8, 9, 10, 11,
                      12, 13, 14, 15);
  EVBCBezS3DPrimitive(v_p, p, points, thedp->attributes.spec.color,
		      thedp->attributes.spec.fill_style,
                      thedp->attributes.spec.edge_flag,
                      thedp->attributes.spec.edge_color,
		      thedp->attributes.spec.tessel_intervals,
		      thedp->attributes.spec.show_polygon,
		      NO, DRAW_MODE_DRAW);
  ASSIGN_TO_4x4_ARRAY(thedp->points, points,
                      0, 4, 8, 12,
                      16, 20, 22, 24,
                      28, 32, 34, 36,
                      40, 44, 48, 52);
  EVBCBezS3DPrimitive(v_p, p, points, thedp->attributes.spec.color,
		      thedp->attributes.spec.fill_style,
                      thedp->attributes.spec.edge_flag,
                      thedp->attributes.spec.edge_color,
		      thedp->attributes.spec.tessel_intervals,
		      thedp->attributes.spec.show_polygon,
		      NO, DRAW_MODE_DRAW);
  ASSIGN_TO_4x4_ARRAY(thedp->points, points,
                      12, 13, 14, 15,
                      24, 25, 26, 27,
                      36, 37, 38, 39,
                      52, 53, 54, 55);
  EVBCBezS3DPrimitive(v_p, p, points, thedp->attributes.spec.color,
		      thedp->attributes.spec.fill_style,
                      thedp->attributes.spec.edge_flag,
                      thedp->attributes.spec.edge_color,
		      thedp->attributes.spec.tessel_intervals,
		      thedp->attributes.spec.show_polygon,
		      NO, DRAW_MODE_DRAW);
  ASSIGN_TO_4x4_ARRAY(thedp->points, points,
                      3, 2, 1, 0,
                      19, 18, 17, 16,
                      31, 30, 29, 28,
                      43, 42, 41, 40);
  EVBCBezS3DPrimitive(v_p, p, points, thedp->attributes.spec.color,
		      thedp->attributes.spec.fill_style,
                      thedp->attributes.spec.edge_flag,
                      thedp->attributes.spec.edge_color,
		      thedp->attributes.spec.tessel_intervals,
		      thedp->attributes.spec.show_polygon,
		      NO, DRAW_MODE_DRAW);
  ASSIGN_TO_4x4_ARRAY(thedp->points, points,
                      43, 42, 41, 40,
                      47, 46, 45, 44,
                      51, 50, 49, 48,
                      55, 54, 53, 52);
  EVBCBezS3DPrimitive(v_p, p, points, thedp->attributes.spec.color,
		      thedp->attributes.spec.fill_style,
                      thedp->attributes.spec.edge_flag,
                      thedp->attributes.spec.edge_color,
		      thedp->attributes.spec.tessel_intervals,
		      thedp->attributes.spec.show_polygon,
		      NO, DRAW_MODE_DRAW);
  ASSIGN_TO_4x4_ARRAY(thedp->points, points,
                      3, 19, 31, 43,
                      7, 21, 33, 47,
                      11, 23, 35, 51,
                      15, 27, 39, 55);
  EVBCBezS3DPrimitive(v_p, p, points, thedp->attributes.spec.color,
		      thedp->attributes.spec.fill_style,
                      thedp->attributes.spec.edge_flag,
                      thedp->attributes.spec.edge_color,
		      thedp->attributes.spec.tessel_intervals,
		      thedp->attributes.spec.show_polygon,
		      NO, DRAW_MODE_DRAW);
}



static void 
BoundingBoxVC(EView *v_p, GraphicObj *p, VCRec *ll, VCRec *ur)
{
  EBCBHexWCRec *thedp;
  VCRec vcpnt;
  int i;
  
  thedp = (EBCBHexWCRec *)(p->spec.data);

  EVWCtoVC(v_p, &(thedp->points[0]), &vcpnt);
  ll->u = vcpnt.u;
  ll->v = vcpnt.v;
  ll->n = vcpnt.n;
  ur->u = vcpnt.u;
  ur->v = vcpnt.v;
  ur->n = vcpnt.n;

  for (i = 1; i < 56; i++) {
    EVWCtoVC(v_p, &(thedp->points[i]), &vcpnt);
    ll->u = min(vcpnt.u, ll->u);
    ll->v = min(vcpnt.v, ll->v);
    ll->n = min(vcpnt.n, ll->n);
    ur->u = max(vcpnt.u, ur->u);
    ur->v = max(vcpnt.v, ur->v);
    ur->n = max(vcpnt.n, ur->n);
  }
}
  


static void 
BoundingBoxWC(GraphicObj *p, WCRec *ll, WCRec *ur)
{
  EBCBHexWCRec *thedp;
  int i;
  
  thedp = (EBCBHexWCRec *)(p->spec.data);

  ll->x = thedp->points[0].x;
  ll->y = thedp->points[0].y;
  ll->z = thedp->points[0].z;
  ur->x = thedp->points[0].x;
  ur->y = thedp->points[0].y;
  ur->z = thedp->points[0].z;
  for (i = 1; i < 56; i++) {
    ll->x = min(thedp->points[i].x, ll->x);
    ll->y = min(thedp->points[i].y, ll->y);
    ll->z = min(thedp->points[i].z, ll->z);
    ur->x = max(thedp->points[i].x, ur->x);
    ur->y = max(thedp->points[i].y, ur->y);
    ur->z = max(thedp->points[i].z, ur->z);
  }
}



static void 
XorDraw(EView *v_p, GraphicObj *g_p)
{
  EBCBHexWCRec *thedp;
  WCRec points[4][4];

  thedp = (EBCBHexWCRec *)(g_p->spec.data);

  ASSIGN_TO_4x4_ARRAY(thedp->points, points,
                      0, 1, 2, 3,
                      4, 5, 6, 7,
                      8, 9, 10, 11,
                      12, 13, 14, 15);

  ASSIGN_TO_4x4_ARRAY(thedp->points, points,
                      0, 4, 8, 12,
                      16, 20, 22, 24,
                      28, 32, 34, 36,
                      40, 44, 48, 52);
  EVBCBezS3DPrimitive(v_p, g_p, points, thedp->attributes.spec.color,
		      FILL_HOLLOW, NO, 0,
		      thedp->attributes.spec.tessel_intervals,
		      thedp->attributes.spec.show_polygon,
		      NO,
		      DRAW_MODE_XORDRAW);
  ASSIGN_TO_4x4_ARRAY(thedp->points, points,
                      12, 13, 14, 15,
                      24, 25, 26, 27,
                      36, 37, 38, 39,
                      52, 53, 54, 55);
  EVBCBezS3DPrimitive(v_p, g_p, points, thedp->attributes.spec.color,
		      FILL_HOLLOW, NO, 0,
		      thedp->attributes.spec.tessel_intervals,
		      thedp->attributes.spec.show_polygon,
		      NO,
		      DRAW_MODE_XORDRAW);
  ASSIGN_TO_4x4_ARRAY(thedp->points, points,
                      3, 2, 1, 0,
                      19, 18, 17, 16,
                      31, 30, 29, 28,
                      43, 42, 41, 40);
  EVBCBezS3DPrimitive(v_p, g_p, points, thedp->attributes.spec.color,
		      FILL_HOLLOW, NO, 0,
		      thedp->attributes.spec.tessel_intervals,
		      thedp->attributes.spec.show_polygon,
		      NO,
		      DRAW_MODE_XORDRAW);
  ASSIGN_TO_4x4_ARRAY(thedp->points, points,
                      43, 42, 41, 40,
                      47, 46, 45, 44,
                      51, 50, 49, 48,
                      55, 54, 53, 52);
  EVBCBezS3DPrimitive(v_p, g_p, points, thedp->attributes.spec.color,
		      FILL_HOLLOW, NO, 0,
		      thedp->attributes.spec.tessel_intervals,
		      thedp->attributes.spec.show_polygon,
		      NO,
		      DRAW_MODE_XORDRAW);
  ASSIGN_TO_4x4_ARRAY(thedp->points, points,
                      3, 19, 31, 43,
                      7, 21, 33, 47,
                      11, 23, 35, 51,
                      15, 27, 39, 55);
  EVBCBezS3DPrimitive(v_p, g_p, points, thedp->attributes.spec.color,
		      FILL_HOLLOW, NO, 0,
		      thedp->attributes.spec.tessel_intervals,
		      thedp->attributes.spec.show_polygon,
		      NO,
		      DRAW_MODE_XORDRAW);
}

 


static GraphicObj *
ModifyGeometry(GraphicObj *g_p, WCRec *points)
{
  EBCBHexWCRec *thedp;
  int i;

  thedp = (EBCBHexWCRec *)(g_p->spec.data);

  for (i = 0; i < 56; i++) {
    thedp->points[i].x = points[i].x; 
    thedp->points[i].y = points[i].y;
    thedp->points[i].z = points[i].z;
  }
  return g_p;
}


static BOOLEAN 
GetGeometry(GraphicObj *g_p, WCRec *points, int *npoints)
{
  EBCBHexWCRec *thedp = (EBCBHexWCRec *)(g_p->spec.data);
  int i;

  for (i = 0; i < 56; i++) {
    points[i].x = thedp->points[i].x; 
    points[i].y = thedp->points[i].y;
    points[i].z = thedp->points[i].z;
  }
  *npoints = 56;
  return YES;
}


static void 
Hilite(EView *v_p, GraphicObj *g_p)
{
  EBCBHexWCRec *thedp;
  WCRec points[4][4];
  
  thedp = (EBCBHexWCRec *)(g_p->spec.data);

  ASSIGN_TO_4x4_ARRAY(thedp->points, points,
                      0, 1, 2, 3,
                      4, 5, 6, 7,
                      8, 9, 10, 11,
                      12, 13, 14, 15);
  EVBCBezS3DPrimitive(v_p, g_p, points, thedp->attributes.spec.color,
		      FILL_HOLLOW,  NO, 0,
		      2,
		      thedp->attributes.spec.show_polygon,
		      NO,
		      DRAW_MODE_HILITE);
  ASSIGN_TO_4x4_ARRAY(thedp->points, points,
                      0, 4, 8, 12,
                      16, 20, 22, 24,
                      28, 32, 34, 36,
                      40, 44, 48, 52);
  EVBCBezS3DPrimitive(v_p, g_p, points, thedp->attributes.spec.color,
		      FILL_HOLLOW,  NO, 0,
		      2,
		      thedp->attributes.spec.show_polygon,
		      NO,
		      DRAW_MODE_HILITE);
  ASSIGN_TO_4x4_ARRAY(thedp->points, points,
                      12, 13, 14, 15,
                      24, 25, 26, 27,
                      36, 37, 38, 39,
                      52, 53, 54, 55);
  EVBCBezS3DPrimitive(v_p, g_p, points, thedp->attributes.spec.color,
		      FILL_HOLLOW, NO, 0,
		      2,
		      thedp->attributes.spec.show_polygon,
		      NO,
		      DRAW_MODE_HILITE);
  ASSIGN_TO_4x4_ARRAY(thedp->points, points,
                      3, 2, 1, 0,
                      19, 18, 17, 16,
                      31, 30, 29, 28,
                      43, 42, 41, 40);
  EVBCBezS3DPrimitive(v_p, g_p, points, thedp->attributes.spec.color,
		      FILL_HOLLOW, NO, 0,
		      2,
		      thedp->attributes.spec.show_polygon,
		      NO,
		      DRAW_MODE_HILITE);
  ASSIGN_TO_4x4_ARRAY(thedp->points, points,
                      43, 42, 41, 40,
                      47, 46, 45, 44,
                      51, 50, 49, 48,
                      55, 54, 53, 52);
  EVBCBezS3DPrimitive(v_p, g_p, points, thedp->attributes.spec.color,
		      FILL_HOLLOW, NO, 0,
		      2,
		      thedp->attributes.spec.show_polygon,
		      NO,
		      DRAW_MODE_HILITE);
  ASSIGN_TO_4x4_ARRAY(thedp->points, points,
                      3, 19, 31, 43,
                      7, 21, 33, 47,
                      11, 23, 35, 51,
                      15, 27, 39, 55);
  EVBCBezS3DPrimitive(v_p, g_p, points, thedp->attributes.spec.color,
		      FILL_HOLLOW, NO, 0,
		      2,
		      thedp->attributes.spec.show_polygon,
		      NO,
		      DRAW_MODE_HILITE);
}



static void 
Unhilite(EView *v_p, GraphicObj *g_p)
{
  EBCBHexWCRec *thedp;
  WCRec points[4][4];
  
  thedp = (EBCBHexWCRec *)(g_p->spec.data);

  ASSIGN_TO_4x4_ARRAY(thedp->points, points,
                      0, 1, 2, 3,
                      4, 5, 6, 7,
                      8, 9, 10, 11,
                      12, 13, 14, 15);
  EVBCBezS3DPrimitive(v_p, g_p, points, thedp->attributes.spec.color,
		      FILL_HOLLOW, NO, 0,
		      2,
		      thedp->attributes.spec.show_polygon,
		      NO,
		      DRAW_MODE_UNHILITE);
  ASSIGN_TO_4x4_ARRAY(thedp->points, points,
                      0, 4, 8, 12,
                      16, 20, 22, 24,
                      28, 32, 34, 36,
                      40, 44, 48, 52);
  EVBCBezS3DPrimitive(v_p, g_p, points, thedp->attributes.spec.color,
		      FILL_HOLLOW, NO, 0,
		      2,
		      thedp->attributes.spec.show_polygon,
		      NO,
		      DRAW_MODE_UNHILITE);
  ASSIGN_TO_4x4_ARRAY(thedp->points, points,
                      12, 13, 14, 15,
                      24, 25, 26, 27,
                      36, 37, 38, 39,
                      52, 53, 54, 55);
  EVBCBezS3DPrimitive(v_p, g_p, points, thedp->attributes.spec.color,
		      FILL_HOLLOW, NO, 0,
		      2,
		      thedp->attributes.spec.show_polygon,
		      NO,
		      DRAW_MODE_UNHILITE);
  ASSIGN_TO_4x4_ARRAY(thedp->points, points,
                      3, 2, 1, 0,
                      19, 18, 17, 16,
                      31, 30, 29, 28,
                      43, 42, 41, 40);
  EVBCBezS3DPrimitive(v_p, g_p, points, thedp->attributes.spec.color,
		      FILL_HOLLOW, NO, 0,
		      2,
		      thedp->attributes.spec.show_polygon,
		      NO,
		      DRAW_MODE_UNHILITE);
  ASSIGN_TO_4x4_ARRAY(thedp->points, points,
                      43, 42, 41, 40,
                      47, 46, 45, 44,
                      51, 50, 49, 48,
                      55, 54, 53, 52);
  EVBCBezS3DPrimitive(v_p, g_p, points, thedp->attributes.spec.color,
		      FILL_HOLLOW, NO, 0,
		      2,
		      thedp->attributes.spec.show_polygon,
		      NO,
		      DRAW_MODE_UNHILITE);
  ASSIGN_TO_4x4_ARRAY(thedp->points, points,
                      3, 19, 31, 43,
                      7, 21, 33, 47,
                      11, 23, 35, 51,
                      15, 27, 39, 55);
  EVBCBezS3DPrimitive(v_p, g_p, points, thedp->attributes.spec.color,
		      FILL_HOLLOW, NO, 0,
		      2,
		      thedp->attributes.spec.show_polygon,
		      NO,
		      DRAW_MODE_UNHILITE);
}



static void 
Erase(EView *v_p, GraphicObj *g_p)
{
  EBCBHexWCRec *thedp;
  WCRec points[4][4];
  
  thedp = (EBCBHexWCRec *)(g_p->spec.data);
  
  ASSIGN_TO_4x4_ARRAY(thedp->points, points,
                      0, 1, 2, 3,
                      4, 5, 6, 7,
                      8, 9, 10, 11,
                      12, 13, 14, 15);
  EVBCBezS3DPrimitive(v_p, g_p, points, thedp->attributes.spec.color,
		      thedp->attributes.spec.fill_style,  NO, 0,
		      thedp->attributes.spec.tessel_intervals,
		      thedp->attributes.spec.show_polygon,
		      NO,
		      DRAW_MODE_ERASE); 
  ASSIGN_TO_4x4_ARRAY(thedp->points, points,
                      0, 4, 8, 12,
                      16, 20, 22, 24,
                      28, 32, 34, 36,
                      40, 44, 48, 52);
  EVBCBezS3DPrimitive(v_p, g_p, points, thedp->attributes.spec.color,
		      thedp->attributes.spec.fill_style, NO, 0,
		      thedp->attributes.spec.tessel_intervals,
		      thedp->attributes.spec.show_polygon,
		      NO,
		      DRAW_MODE_ERASE); 
  ASSIGN_TO_4x4_ARRAY(thedp->points, points,
                      12, 13, 14, 15,
                      24, 25, 26, 27,
                      36, 37, 38, 39,
                      52, 53, 54, 55);
  EVBCBezS3DPrimitive(v_p, g_p, points, thedp->attributes.spec.color,
		      thedp->attributes.spec.fill_style, NO, 0,
		      thedp->attributes.spec.tessel_intervals,
		      thedp->attributes.spec.show_polygon,
		      NO,
		      DRAW_MODE_ERASE); 
  ASSIGN_TO_4x4_ARRAY(thedp->points, points,
                      3, 2, 1, 0,
                      19, 18, 17, 16,
                      31, 30, 29, 28,
                      43, 42, 41, 40);
  EVBCBezS3DPrimitive(v_p, g_p, points, thedp->attributes.spec.color,
		      thedp->attributes.spec.fill_style, NO, 0,
		      thedp->attributes.spec.tessel_intervals,
		      thedp->attributes.spec.show_polygon,
		      NO,
		      DRAW_MODE_ERASE); 
  ASSIGN_TO_4x4_ARRAY(thedp->points, points,
                      43, 42, 41, 40,
                      47, 46, 45, 44,
                      51, 50, 49, 48,
                      55, 54, 53, 52);
  EVBCBezS3DPrimitive(v_p, g_p, points, thedp->attributes.spec.color,
		      thedp->attributes.spec.fill_style, NO, 0,
		      thedp->attributes.spec.tessel_intervals,
		      thedp->attributes.spec.show_polygon,
		      NO,
		      DRAW_MODE_ERASE); 
  ASSIGN_TO_4x4_ARRAY(thedp->points, points,
                      3, 19, 31, 43,
                      7, 21, 33, 47,
                      11, 23, 35, 51,
                      15, 27, 39, 55);
  EVBCBezS3DPrimitive(v_p, g_p, points, thedp->attributes.spec.color,
		      thedp->attributes.spec.fill_style, NO, 0,
		      thedp->attributes.spec.tessel_intervals,
		      thedp->attributes.spec.show_polygon,
		      NO,
		      DRAW_MODE_ERASE);  
}



static GraphicObj *
Translate(GraphicObj *g_p, WCRec *by)
{
  EBCBHexWCRec *p;
  int i;

  p = (EBCBHexWCRec *)g_p->spec.data;
  for (i = 0; i < 56; i++) {
    p->points[i].x += by->x;
    p->points[i].y += by->y;
    p->points[i].z += by->z;
  }
  return g_p;
}



static GraphicObj *
Rotate(GraphicObj *g_p, WCRec *center, WCRec *axial)
{
  EBCBHexWCRec *c;
  WCRec x_c;
  int i;

  c = (EBCBHexWCRec *)g_p->spec.data;
  for (i = 0; i < 56; i++ ) {
    x_c.x = c->points[i].x - center->x;
    x_c.y = c->points[i].y - center->y;
    x_c.z = c->points[i].z - center->z;
    RotVectAboutVect(axial, &x_c);
    c->points[i].x = center->x + x_c.x;
    c->points[i].y = center->y + x_c.y;
    c->points[i].z = center->z + x_c.z;
  }
  return g_p;
}



static GraphicObj  *
MirrorPlane(GraphicObj *g_p, WCRec *point_on_plane,
                  WCRec *unit_plane_normal)
{
  EBCBHexWCRec *dp;
  int i;

  dp = (EBCBHexWCRec *)g_p->spec.data;
  for (i = 0; i < 56; i++)
    MirrorPointInPlane(point_on_plane, unit_plane_normal, &dp->points[i]);

  return g_p;
}



static GraphicObj  *
MirrorCenter(GraphicObj *g_p, WCRec *center)
{
  EBCBHexWCRec *dp;
  int i;

  dp = (EBCBHexWCRec *)g_p->spec.data;
  for (i = 0; i < 56; i++) {
    dp->points[i].x = 2.*center->x - dp->points[i].x;
    dp->points[i].y = 2.*center->x - dp->points[i].y;
    dp->points[i].z = 2.*center->x - dp->points[i].z;
  }
  return g_p;
}



static GraphicObj *
Scale(GraphicObj *g_p, WCRec *center, FPNum sx, FPNum sy, FPNum sz)
{
  return g_p;
}



static BOOLEAN 
HitByBox(EView *v_p, EBoxVC2DRec *box,
               ESelectCriteria sel_crit, GraphicObj *p)
{
  VCRec ll, ur, pvc;
  WCRec wcpts[4];
  WC2DRec points[4];
  EBCBHexWCRec *thedp;
  int i;

  switch(sel_crit) {
  case INSIDE: /* only if the whole bounding box is inside */
    BoundingBoxVC(v_p, p, &ll, &ur);
    if (ll.u >= box->left   && ur.u <= box->right && 
	ll.v >= box->bottom && ur.v <= box->top)
      return YES;
    else
      return NO;
    break;
  case OVERLAP:
  case INTERSECT: /* hits the boundary curves */
  default:
    thedp = (EBCBHexWCRec *)p->spec.data;
    ASSIGN_P_P1DIM(thedp->points,  0, wcpts, 0);
    ASSIGN_P_P1DIM(thedp->points,  1, wcpts, 1);
    ASSIGN_P_P1DIM(thedp->points,  2, wcpts, 2);
    ASSIGN_P_P1DIM(thedp->points,  3, wcpts, 3);
    for (i = 0; i < 4; i++) {
      EVWCtoVC(v_p, &(wcpts[i]), &pvc);
      points[i].x = pvc.u; points[i].y = pvc.v;
    }
    if (BoxIntersectsCBezCurve(box, points))
      return YES;
    ASSIGN_P_P1DIM(thedp->points,  3, wcpts, 0);
    ASSIGN_P_P1DIM(thedp->points,  7, wcpts, 1);
    ASSIGN_P_P1DIM(thedp->points, 11, wcpts, 2);
    ASSIGN_P_P1DIM(thedp->points, 15, wcpts, 3);
    for (i = 0; i < 4; i++) {
      EVWCtoVC(v_p, &(wcpts[i]), &pvc);
      points[i].x = pvc.u; points[i].y = pvc.v;
    }
    if (BoxIntersectsCBezCurve(box, points))
      return YES;
    ASSIGN_P_P1DIM(thedp->points, 15, wcpts, 0);
    ASSIGN_P_P1DIM(thedp->points, 14, wcpts, 1);
    ASSIGN_P_P1DIM(thedp->points, 13, wcpts, 2);
    ASSIGN_P_P1DIM(thedp->points, 12, wcpts, 3);
    for (i = 0; i < 4; i++) {
      EVWCtoVC(v_p, &(wcpts[i]), &pvc);
      points[i].x = pvc.u; points[i].y = pvc.v;
    }
    if (BoxIntersectsCBezCurve(box, points))
      return YES;
    ASSIGN_P_P1DIM(thedp->points, 12, wcpts, 0);
    ASSIGN_P_P1DIM(thedp->points,  8, wcpts, 1);
    ASSIGN_P_P1DIM(thedp->points,  4, wcpts, 2);
    ASSIGN_P_P1DIM(thedp->points,  0, wcpts, 3);
    for (i = 0; i < 4; i++) {
      EVWCtoVC(v_p, &(wcpts[i]), &pvc);
      points[i].x = pvc.u; points[i].y = pvc.v;
    }
    if (BoxIntersectsCBezCurve(box, points))
      return YES;
    ASSIGN_P_P1DIM(thedp->points, 15, wcpts, 0);
    ASSIGN_P_P1DIM(thedp->points, 27, wcpts, 1);
    ASSIGN_P_P1DIM(thedp->points, 39, wcpts, 2);
    ASSIGN_P_P1DIM(thedp->points, 55, wcpts, 3);
    for (i = 0; i < 4; i++) {
      EVWCtoVC(v_p, &(wcpts[i]), &pvc);
      points[i].x = pvc.u; points[i].y = pvc.v;
    }
    if (BoxIntersectsCBezCurve(box, points))
      return YES;
    ASSIGN_P_P1DIM(thedp->points,  3, wcpts, 0);
    ASSIGN_P_P1DIM(thedp->points, 19, wcpts, 1);
    ASSIGN_P_P1DIM(thedp->points, 31, wcpts, 2);
    ASSIGN_P_P1DIM(thedp->points, 43, wcpts, 3);
    for (i = 0; i < 4; i++) {
      EVWCtoVC(v_p, &(wcpts[i]), &pvc);
      points[i].x = pvc.u; points[i].y = pvc.v;
    }
    if (BoxIntersectsCBezCurve(box, points))
      return YES;
    ASSIGN_P_P1DIM(thedp->points, 12, wcpts, 0);
    ASSIGN_P_P1DIM(thedp->points, 24, wcpts, 1);
    ASSIGN_P_P1DIM(thedp->points, 36, wcpts, 2);
    ASSIGN_P_P1DIM(thedp->points, 52, wcpts, 3);
    for (i = 0; i < 4; i++) {
      EVWCtoVC(v_p, &(wcpts[i]), &pvc);
      points[i].x = pvc.u; points[i].y = pvc.v;
    }
    if (BoxIntersectsCBezCurve(box, points))
      return YES;
    ASSIGN_P_P1DIM(thedp->points,  0, wcpts, 0);
    ASSIGN_P_P1DIM(thedp->points, 16, wcpts, 1);
    ASSIGN_P_P1DIM(thedp->points, 28, wcpts, 2);
    ASSIGN_P_P1DIM(thedp->points, 40, wcpts, 3);
    for (i = 0; i < 4; i++) {
      EVWCtoVC(v_p, &(wcpts[i]), &pvc);
      points[i].x = pvc.u; points[i].y = pvc.v;
    }
    if (BoxIntersectsCBezCurve(box, points))
      return YES;
    ASSIGN_P_P1DIM(thedp->points, 40, wcpts, 0);
    ASSIGN_P_P1DIM(thedp->points, 44, wcpts, 1);
    ASSIGN_P_P1DIM(thedp->points, 48, wcpts, 2);
    ASSIGN_P_P1DIM(thedp->points, 52, wcpts, 3);
    for (i = 0; i < 4; i++) {
      EVWCtoVC(v_p, &(wcpts[i]), &pvc);
      points[i].x = pvc.u; points[i].y = pvc.v;
    }
    if (BoxIntersectsCBezCurve(box, points))
      return YES;
    ASSIGN_P_P1DIM(thedp->points, 52, wcpts, 0);
    ASSIGN_P_P1DIM(thedp->points, 53, wcpts, 1);
    ASSIGN_P_P1DIM(thedp->points, 54, wcpts, 2);
    ASSIGN_P_P1DIM(thedp->points, 55, wcpts, 3);
    for (i = 0; i < 4; i++) {
      EVWCtoVC(v_p, &(wcpts[i]), &pvc);
      points[i].x = pvc.u; points[i].y = pvc.v;
    }
    if (BoxIntersectsCBezCurve(box, points))
      return YES;
    ASSIGN_P_P1DIM(thedp->points, 55, wcpts, 0);
    ASSIGN_P_P1DIM(thedp->points, 51, wcpts, 1);
    ASSIGN_P_P1DIM(thedp->points, 47, wcpts, 2);
    ASSIGN_P_P1DIM(thedp->points, 43, wcpts, 3);
    for (i = 0; i < 4; i++) {
      EVWCtoVC(v_p, &(wcpts[i]), &pvc);
      points[i].x = pvc.u; points[i].y = pvc.v;
    }
    if (BoxIntersectsCBezCurve(box, points))
      return YES;
    ASSIGN_P_P1DIM(thedp->points, 43, wcpts, 0);
    ASSIGN_P_P1DIM(thedp->points, 42, wcpts, 1);
    ASSIGN_P_P1DIM(thedp->points, 41, wcpts, 2);
    ASSIGN_P_P1DIM(thedp->points, 40, wcpts, 3);
    for (i = 0; i < 4; i++) {
      EVWCtoVC(v_p, &(wcpts[i]), &pvc);
      points[i].x = pvc.u; points[i].y = pvc.v;
    }
    if (BoxIntersectsCBezCurve(box, points))
      return YES;
    
    return NO;
    break;
  }
}



static EGraphicType 
Type(GraphicObj *g_p)
{
  return EG_BCB_HEXAHEDRON;
}



static char *
AsString(GraphicObj *g_p)
{
  EBCBHexWCRec *l;

  l = (EBCBHexWCRec *)g_p->spec.data;
  sprintf(string_rep_buffer,
	  "BCB_HEXAHEDRON [%f,%f,%f][%f,%f,%f][%f,%f,%f][%f,%f,%f]\n"
	  "               [%f,%f,%f][%f,%f,%f][%f,%f,%f][%f,%f,%f]\n"
	  "               [%f,%f,%f][%f,%f,%f][%f,%f,%f][%f,%f,%f]\n"
    "               [%f,%f,%f][%f,%f,%f][%f,%f,%f][%f,%f,%f]\n"
	  "               [%f,%f,%f][%f,%f,%f][%f,%f,%f][%f,%f,%f]\n"
	  "               [%f,%f,%f][%f,%f,%f][%f,%f,%f][%f,%f,%f]\n"
	  "               [%f,%f,%f][%f,%f,%f][%f,%f,%f][%f,%f,%f]\n"
	  "               [%f,%f,%f][%f,%f,%f][%f,%f,%f][%f,%f,%f]\n"
	  "               [%f,%f,%f][%f,%f,%f][%f,%f,%f][%f,%f,%f]\n"
	  "               [%f,%f,%f][%f,%f,%f][%f,%f,%f][%f,%f,%f]\n"
	  "               [%f,%f,%f][%f,%f,%f][%f,%f,%f][%f,%f,%f]\n"
	  "               [%f,%f,%f][%f,%f,%f][%f,%f,%f][%f,%f,%f]\n"
	  "               [%f,%f,%f][%f,%f,%f][%f,%f,%f][%f,%f,%f]\n"
	  "               [%f,%f,%f][%f,%f,%f][%f,%f,%f][%f,%f,%f]",
	  l->points[ 0].x, l->points[ 0].y, l->points[ 0].z,
          l->points[ 1].x, l->points[ 1].y, l->points[ 1].z,
          l->points[ 2].x, l->points[ 2].y, l->points[ 2].z,
          l->points[ 3].x, l->points[ 3].y, l->points[ 3].z,
          l->points[ 4].x, l->points[ 4].y, l->points[ 4].z,
          l->points[ 5].x, l->points[ 5].y, l->points[ 5].z,
          l->points[ 6].x, l->points[ 6].y, l->points[ 6].z,
          l->points[ 7].x, l->points[ 7].y, l->points[ 7].z,
          l->points[ 8].x, l->points[ 8].y, l->points[ 8].z,
          l->points[ 9].x, l->points[ 9].y, l->points[ 9].z,

          l->points[10].x, l->points[10].y, l->points[10].z,
          l->points[11].x, l->points[11].y, l->points[11].z,
          l->points[12].x, l->points[12].y, l->points[12].z,
          l->points[13].x, l->points[13].y, l->points[13].z,
          l->points[14].x, l->points[14].y, l->points[14].z,
          l->points[15].x, l->points[15].y, l->points[15].z,
          l->points[16].x, l->points[16].y, l->points[16].z,
          l->points[17].x, l->points[17].y, l->points[17].z,
          l->points[18].x, l->points[18].y, l->points[18].z,
          l->points[19].x, l->points[19].y, l->points[19].z,

          l->points[20].x, l->points[20].y, l->points[20].z,
          l->points[21].x, l->points[21].y, l->points[21].z,
          l->points[22].x, l->points[22].y, l->points[22].z,
          l->points[23].x, l->points[23].y, l->points[23].z,
          l->points[24].x, l->points[24].y, l->points[24].z,
          l->points[25].x, l->points[25].y, l->points[25].z,
          l->points[26].x, l->points[26].y, l->points[26].z,
          l->points[27].x, l->points[27].y, l->points[27].z,
          l->points[28].x, l->points[28].y, l->points[28].z,
          l->points[29].x, l->points[29].y, l->points[29].z,

          l->points[30].x, l->points[30].y, l->points[30].z,
          l->points[31].x, l->points[31].y, l->points[31].z,
          l->points[32].x, l->points[32].y, l->points[32].z,
          l->points[33].x, l->points[33].y, l->points[33].z,
          l->points[34].x, l->points[34].y, l->points[34].z,
          l->points[35].x, l->points[35].y, l->points[35].z,
          l->points[36].x, l->points[36].y, l->points[36].z,
          l->points[37].x, l->points[37].y, l->points[37].z,
          l->points[38].x, l->points[38].y, l->points[38].z,
          l->points[39].x, l->points[39].y, l->points[39].z,

          l->points[40].x, l->points[40].y, l->points[40].z,
          l->points[41].x, l->points[41].y, l->points[41].z,
          l->points[42].x, l->points[42].y, l->points[42].z,
          l->points[43].x, l->points[43].y, l->points[43].z,
          l->points[44].x, l->points[44].y, l->points[44].z,
          l->points[45].x, l->points[45].y, l->points[45].z,
          l->points[46].x, l->points[46].y, l->points[46].z,
          l->points[47].x, l->points[47].y, l->points[47].z,
          l->points[48].x, l->points[48].y, l->points[48].z,
          l->points[49].x, l->points[49].y, l->points[49].z,

          l->points[50].x, l->points[50].y, l->points[50].z,
          l->points[51].x, l->points[51].y, l->points[51].z,
          l->points[52].x, l->points[52].y, l->points[52].z,
          l->points[53].x, l->points[53].y, l->points[53].z,
          l->points[54].x, l->points[54].y, l->points[54].z,
          l->points[55].x, l->points[55].y, l->points[55].z);
  
  return string_rep_buffer; /* Don't free this memory!!! */
}



static LIST 
KeyPoints(GraphicObj *p)
{
  LIST temp_list;
  KeyPointRec *key_p;
  EBCBHexWCRec *l_p;
  int i;

  l_p       = (EBCBHexWCRec *)(p->spec.data);
  temp_list = make_list();

  for (i = 0; i < 56; i++) {
    key_p = (KeyPointRec*)add_to_tail(temp_list,
                                      make_node(sizeof(KeyPointRec)));
    key_p->coords.x = l_p->points[i].x;
    key_p->coords.y = l_p->points[i].y;
    key_p->coords.z = l_p->points[i].z;
    key_p->dist_from_hit = 0.0;
  }
  
  return temp_list;
}



static GraphicObj *
Reshape(GraphicObj *gp, EHandleNum hn, WCRec *pos)
{
  EBCBHexWCRec *_p;
  int i;
  
  _p = (EBCBHexWCRec *)(gp->spec.data);

  i = (hn-1);
  if (hn < 1 || hn > 56) 
    i = 0;

  _p->points[i].x = pos->x;
  _p->points[i].y = pos->y;
  _p->points[i].z = pos->z;
  return gp;
}



static EHandleNum 
ReshapeHandle(EView *v_p, GraphicObj *p, WCRec *nearp)
{
  EBCBHexWCRec *l_p;
  VCRec nvc, pvc;
  FPNum d, tmp;
  EHandleNum hn;
  int i;

  l_p       = (EBCBHexWCRec *)(p->spec.data);
  EVWCtoVC(v_p, nearp, &nvc);

  hn = 1;
  EVWCtoVC(v_p, &(l_p->points[0]), &pvc);
  d = DistOfPnts(pvc.u, pvc.v, nvc.u, nvc.v);
  for (i = 0; i < 56; i++) {
    EVWCtoVC(v_p, &(l_p->points[i]), &pvc);
    if (d > (tmp = DistOfPnts(pvc.u, pvc.v, nvc.u, nvc.v))) {
      hn = i + 1, d = tmp;
    }
  }
  return hn;
}



static void 
ShowHandles(EView *v_p, GraphicObj *p)
{
  EBCBHexWCRec *l_p;
  int i;

  l_p = (EBCBHexWCRec *)p->spec.data;

  for (i = 0; i < 56; i++) 
    EGDrawHandle(v_p, &(l_p->points[i]));
}
  


static void 
EraseHandles(EView *v_p, GraphicObj *p)
{
  EBCBHexWCRec *l_p;
  int i;

  l_p = (EBCBHexWCRec *)p->spec.data;

  for (i = 0; i < 56; i++)
    EGEraseHandle(v_p, &(l_p->points[i]));
}




static void 
XYofHandle(GraphicObj *p, EHandleNum hn, WCRec *pos)
{
  EBCBHexWCRec *l_p;
  int i;

  l_p = (EBCBHexWCRec *)p->spec.data;
  i = (hn-1);
  if (hn < 1 || hn > 56)
    i = 0;
  pos->x = l_p->points[i].x;
  pos->y = l_p->points[i].y;
  pos->z = l_p->points[i].z;
}



static void 
RetrieveFrom(FILE *file_p, GraphicSpecObj  *gsp)
{
  EBCBHexWCRec *p;
  int fill, edge_flag, sp, sc, ti, i;
  char color[64];
  EPixel pixel;
  BOOLEAN success;
  double x, y, z;
  
  if ((p = (EBCBHexWCRec *)make_node(sizeof(EBCBHexWCRec))) != NULL) {
    for (i = 0; i < 56; i++) {
      fscanf(file_p, "%lf %lf %lf", &x, &y, &z);
      p->points[i].x = x;
      p->points[i].y = y;
      p->points[i].z = z;
    }
    fscanf(file_p, "%d %d %d %d %d %d",
	   &(p->attributes.spec.have_any),
	   &sp, &sc, &fill, &edge_flag, &ti);
    p->attributes.spec.show_polygon     = sp;
    p->attributes.spec.show_surface     = sc;
    p->attributes.spec.fill_style       = fill;
    p->attributes.spec.edge_flag        = edge_flag;
    p->attributes.spec.tessel_intervals = ti;
    fscanf(file_p, "%s", color);
    pixel = ColorGetPixelFromString(color, &success);
    if (success)
      p->attributes.spec.color = pixel;
    fscanf(file_p, "%s", color);
    pixel = ColorGetPixelFromString(color, &success);
    if (success)
      p->attributes.spec.edge_color = pixel;
  }
  gsp->data        = (caddr_t)p;
  gsp->data_length = sizeof(EBCBHexWCRec);
}
 


static void 
StoreOn(FILE *file_p, GraphicObj *g_p)
{
  EBCBHexWCRec *p;
  int fill, edge_flag, sp, sc, ti, i;
  char *color, *edge_color;

  p = (EBCBHexWCRec *)g_p->spec.data;
  if (p->attributes.spec.have_any == YES) {
    fill      = p->attributes.spec.fill_style;
    edge_flag = p->attributes.spec.edge_flag;
    sp        = p->attributes.spec.show_polygon;
    sc        = p->attributes.spec.show_surface;
    ti        = p->attributes.spec.tessel_intervals;
    color     = ColorGetStringFromPixel(p->attributes.spec.color);
    if (color == NULL)
      color = "white";
    edge_color     = ColorGetStringFromPixel(p->attributes.spec.edge_color);
    if (edge_color == NULL)
      edge_color = "white";
  } else {
    fill  = FILL_HOLLOW;
    edge_flag = NO;
    sp    = YES;
    sc    = YES;
    ti    = 17;
    edge_color = color = "white";
  }
  for (i = 0; i < 56; i++) {
    fprintf(file_p, "%f %f %f\n",
            p->points[i].x, p->points[i].y, p->points[i].z);
  }
    fprintf(file_p, "%d %d %d %d %d %d\n%s\n%s\n",
	  p->attributes.spec.have_any,
	  sp, sc, fill, edge_flag, ti, color, edge_color);
}



static void 
ChangeAttributes(GraphicObj *p)
{
  EBCBHexWCRec *dp;
  unsigned long mask;

  mask = EASValsGetChangeMask();
  dp = (EBCBHexWCRec *)p->spec.data;
  if ((mask & SHOW_POLY_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.show_polygon = EASValsGetShowPoly();
    dp->attributes.spec.have_any     = YES;
  }
  if ((mask & SHOW_ENTITY_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.show_surface   = EASValsGetShowEntity();
    dp->attributes.spec.have_any     = YES;
  }
  if ((mask & TESSEL_INTERVALS_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.tessel_intervals = EASValsGetTesselIntervals();
    dp->attributes.spec.have_any         = YES;
  }
  if ((mask & FILL_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.fill_style   = EASValsGetFillStyle();
    dp->attributes.spec.have_any     = YES;
  }
  if ((mask & COLOR_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.color    = EASValsGetColor();
    dp->attributes.spec.have_any = YES;
  }
  if ((mask & EDGE_COLOR_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.edge_color    = EASValsGetEdgeColor();
    dp->attributes.spec.have_any = YES;
  }
  if ((mask & EDGE_FLAG_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.edge_flag    = EASValsGetEdgeFlag();
    dp->attributes.spec.have_any = YES;
  }
}
			       


static void 
CopyAttributesToActiveSet(GraphicObj *p)
{
  EBCBHexWCRec *dp;

  dp = (EBCBHexWCRec *)p->spec.data;

  if (dp->attributes.spec.have_any) {
    EASValsSetFillStyle(dp->attributes.spec.fill_style);
    EASValsSetColor(dp->attributes.spec.color);
    EASValsSetEdgeColor(dp->attributes.spec.edge_color);
    EASValsSetEdgeFlag(dp->attributes.spec.edge_flag);
    EASValsSetShowPoly(dp->attributes.spec.show_polygon);
    EASValsSetShowEntity(dp->attributes.spec.show_surface);
    EASValsSetTesselIntervals(dp->attributes.spec.tessel_intervals);
  }
}


static BOOLEAN
AttribValAsInAS(GraphicObj *p, unsigned long mask)  
{
  EBCBHexWCRec *dp;

  dp = (EBCBHexWCRec *)p->spec.data;

  if ((mask & SHOW_POLY_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.show_polygon != EASValsGetShowPoly())
      return NO;
  if ((mask & SHOW_ENTITY_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.show_surface != EASValsGetShowEntity())
      return NO;

  if ((mask & TESSEL_INTERVALS_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.tessel_intervals != EASValsGetTesselIntervals())
      return NO;

  if ((mask & FILL_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.fill_style != EASValsGetFillStyle())
      return NO;

  if ((mask & COLOR_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.color != EASValsGetColor())
      return NO;
  
  if ((mask & EDGE_COLOR_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.edge_color != EASValsGetEdgeColor())
      return NO;

  if ((mask & EDGE_FLAG_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.edge_flag != EASValsGetEdgeFlag())
      return NO;

    return YES;
}
 

#define ASSIGN_POINT_TO_POINT(DEST, SRC)                         \
  {                                                              \
     (DEST).x = (SRC).x; (DEST).y = (SRC).y; (DEST).z = (SRC).z; \
   }

GraphicObj *
BCBezS3DExtrudeToBCBHex(GraphicObj *bcbezs, GraphicObj *cbezc)
{
  GraphicObj *g_p;
  WCRec points[56];
  WCRec bcbspts[16];
  WCRec cbcpts[4], by;
  int npts, i, k;

  if (!EGGetGraphicsGeometry(bcbezs, bcbspts, &npts))
    return NULL;
  if (!EGGetGraphicsGeometry(cbezc, cbcpts, &npts))
    return NULL;

  /* adjust for curves that do not start at the u=0, v=0 corner of the patch */
  by.x = bcbspts[0].x - cbcpts[0].x;
  by.y = bcbspts[0].y - cbcpts[0].y;
  by.z = bcbspts[0].z - cbcpts[0].z;
  for (i = 0; i < 4; i++) {
    cbcpts[i].x += by.x;
    cbcpts[i].y += by.y;
    cbcpts[i].z += by.z;
  }
  /* Generate */
  by.x = cbcpts[1].x - bcbspts[0].x;
  by.y = cbcpts[1].y - bcbspts[0].y;
  by.z = cbcpts[1].z - bcbspts[0].z;
  for (i = 0; i < 16; i++) {
    k = i;
    ASSIGN_POINT_TO_POINT(points[k], bcbspts[i]);
    bcbspts[i].x += by.x;
    bcbspts[i].y += by.y;
    bcbspts[i].z += by.z;
  }
  ASSIGN_POINT_TO_POINT(points[16], bcbspts[0]);
  ASSIGN_POINT_TO_POINT(points[17], bcbspts[1]);
  ASSIGN_POINT_TO_POINT(points[18], bcbspts[2]);
  ASSIGN_POINT_TO_POINT(points[19], bcbspts[3]);
  ASSIGN_POINT_TO_POINT(points[21], bcbspts[7]);
  ASSIGN_POINT_TO_POINT(points[23], bcbspts[11]);
  ASSIGN_POINT_TO_POINT(points[27], bcbspts[15]);
  ASSIGN_POINT_TO_POINT(points[26], bcbspts[14]);
  ASSIGN_POINT_TO_POINT(points[25], bcbspts[13]);
  ASSIGN_POINT_TO_POINT(points[24], bcbspts[12]);
  ASSIGN_POINT_TO_POINT(points[22], bcbspts[8]);
  ASSIGN_POINT_TO_POINT(points[20], bcbspts[4]);
  by.x = cbcpts[2].x - bcbspts[0].x;
  by.y = cbcpts[2].y - bcbspts[0].y;
  by.z = cbcpts[2].z - bcbspts[0].z;
  for (i = 0; i < 16; i++) {
    bcbspts[i].x += by.x;
    bcbspts[i].y += by.y;
    bcbspts[i].z += by.z;
  }
  ASSIGN_POINT_TO_POINT(points[28], bcbspts[0]);
  ASSIGN_POINT_TO_POINT(points[29], bcbspts[1]);
  ASSIGN_POINT_TO_POINT(points[30], bcbspts[2]);
  ASSIGN_POINT_TO_POINT(points[31], bcbspts[3]);
  ASSIGN_POINT_TO_POINT(points[33], bcbspts[7]);
  ASSIGN_POINT_TO_POINT(points[35], bcbspts[11]);
  ASSIGN_POINT_TO_POINT(points[39], bcbspts[15]);
  ASSIGN_POINT_TO_POINT(points[38], bcbspts[14]);
  ASSIGN_POINT_TO_POINT(points[37], bcbspts[13]);
  ASSIGN_POINT_TO_POINT(points[36], bcbspts[12]);
  ASSIGN_POINT_TO_POINT(points[34], bcbspts[8]);
  ASSIGN_POINT_TO_POINT(points[32], bcbspts[4]);
  by.x = cbcpts[3].x - bcbspts[0].x;
  by.y = cbcpts[3].y - bcbspts[0].y;
  by.z = cbcpts[3].z - bcbspts[0].z;
  for (i = 0; i < 16; i++) {
    bcbspts[i].x += by.x;
    bcbspts[i].y += by.y;
    bcbspts[i].z += by.z;
  }
  for (i = 0; i < 16; i++) {
    k = i + 40;
    ASSIGN_POINT_TO_POINT(points[k], bcbspts[i]);
  }
  
  g_p = CreateBCBHex(points);
  return g_p;
}



static BOOLEAN
find_loops(void);
static BOOLEAN
common_vertex(int curve1, int curve2,
              WCRec *p, int *which1, int *which2);
static BOOLEAN
find_branches(int curv, WCRec *curp, int *b1, int *b2);
static void
hexpoints(WCRec *points);
static void
points_on_curve(int loopn, int cn, int p1, int p2, int p3, int p4,
                WCRec *points);
static void
points_inside(int i00, int i10, int i20, int i30,
              int i01, int i11, int i21, int i31,
              int i02, int i12, int i22, int i32,
              int i03, int i13, int i23, int i33,
              WCRec *points);

typedef enum { AS_DEF, REV } curve_sense;

#define INVALID -1
#define NONE -1
#define START 0
#define END   3

/* Error status codes */
#define CURVES_0_AND_1_SHARE_NO_VERTEX -1
#define DEAD_END                       -2
#define CENTER_HAS_LESS_THAN_3_CURVES  -3
#define BAD_DEGENERACY                 -4

typedef struct curve_in_loop {
  int           id;
  curve_sense   sense;
}   curve_in_loop;


typedef struct loop_rec {
  curve_in_loop c[4];           /* curve numbers (or NONE) */
}   loop_rec;

static loop_rec loops[6]; /* curve loops of the bounding surfaces */
static WCRec cp[12][4]; /* curve definition points */
static int status = 0; /* status of the operation */
static WCRec toreach, curp;

/* It is assumed that the first two curves meet at a point such that if */
/* the parametrization of the curve(s) is adjusted to have this point as */
/* origin, the normal to the surface given by these two curves */
/* computed from the cross product of the tangent vectors is the OUTER */
/* normal (this is required so that we can say where is inside). */

GraphicObj *
BCBezCurvesToBCBHex(GraphicObj *c[12])
{
  GraphicObj *g_p;
  WCRec points[56]; /* bcbhex definition points */
  int npts, i, j;

  /* the geometry */
  for (i = 0; i < 12; i++)     
    if (!EGGetGraphicsGeometry(c[i], &cp[i][0], &npts))
      return NULL;
  /* init the loops */
  for (j = 0; j < 6; j++) 
    for (i = 0; i < 4; i++) {
      loops[j].c[i].id    = NONE;
      loops[j].c[i].sense = AS_DEF;
    }
  /* find the loops */
  if (!find_loops()) return NULL;
  /* compute the coordinates */
  hexpoints(points);
  /* create the hex */
  g_p = CreateBCBHex(points);
  return g_p;
}

#define EPS 0.00001
#define POINTS_IDENTICAL(P1, P2)  \
     (   fabs((P1).x - (P2).x) < EPS  \
      && fabs((P1).y - (P2).y) < EPS  \
      && fabs((P1).z - (P2).z) < EPS)


#define CID(CURVENUM) CL(c[CURVENUM].id) 
#define CS(CURVENUM) CL(c[CURVENUM].sense)
#define CLOSED(aPoint) (POINTS_IDENTICAL(toreach, aPoint))
#define AT_END(CNum)   ((CS(CNum) == AS_DEF)? END: START)
#define AT_START(CNum) ((CS(CNum) == AS_DEF)? START: END)
#define POINT_ALONG_AT_END(CNum)   (cp[CID(CNum)][AT_END(CNum)])
#define POINT_ALONG_AT_START(CNum) (cp[CID(CNum)][AT_START(CNum)])

static BOOLEAN
find_loops(void)
{
  WCRec center, p, tp1, tp2;
  int w1, w2, b1[2], b2[2], nb1, nb2, i, j;
  
  if (!common_vertex(0, 1, &p, &w1, &w2)) {
    status = CURVES_0_AND_1_SHARE_NO_VERTEX;
    return NO;
  }
  ASSIGN_POINT_TO_POINT(center, p);
/* The first bounding surface -- loop 0 */
/*  curve */
/*   1           2  */
/*  12  13  14  15  curve 3 */
/*   8   9  10  11  NOTE: we are traversing counterclockwise, 12,8,4,0,...*/
/*   4   5   6   7  */
/*   0   1   2   3  curve 0 */
#define CL(ANYTHING) loops[0].ANYTHING 
  CID(0) = 1;
  if (w2 == END) CS(0) = AS_DEF; else CS(0) = REV;
  CID(1) = 0; 
  if (w1 == END) CS(1) = REV; else CS(1) = AS_DEF;
  ASSIGN_POINT_TO_POINT(toreach, POINT_ALONG_AT_START(0));
  ASSIGN_POINT_TO_POINT(curp, POINT_ALONG_AT_END(1));
  if (!CLOSED(curp)) {
    if (!find_branches(CID(1), &curp, b1, &nb1)){status = DEAD_END; return NO;}
    for (i = 0; i < nb1; i++) {
      common_vertex(CID(1), b1[i], &tp1, &w1, &w2);
      CID(2) = b1[i];
      if (w2 == END) CS(2) = REV; else CS(2) = AS_DEF;
      ASSIGN_POINT_TO_POINT(tp1, POINT_ALONG_AT_END(2));
      if (!CLOSED(tp1)) {
        if (!find_branches(CID(2), &tp1, b2, &nb2)){status=DEAD_END;return NO;}
        for (j = 0; j < nb2; j++) {
          common_vertex(CID(2), b2[j], &tp2, &w1, &w2);
          CID(3) = b2[j];
          if (w2 == END) CS(3) = REV; else CS(3) = AS_DEF;
          ASSIGN_POINT_TO_POINT(tp2, POINT_ALONG_AT_END(3));
          if (CLOSED(tp2))
            goto done0;
        }
      } else
        goto done0; /* closed by three curves */
    }
  } /* closed by only two curves */
 done0:
/* The second bounding surface -- loop  */
/*  curve */
/*   0           2  */
/*  40, 44, 48, 52  */
/*  28, 32, 34, 36, */
/*  16, 20, 22, 24, */
/*   0,  4,  8, 12, */
#undef CL
#define CL(ANYTHING) loops[1].ANYTHING
  if (!find_branches(0, &center, b1, &nb1)){ status = DEAD_END; return NO;}
  if (nb1 < 2) { status = CENTER_HAS_LESS_THAN_3_CURVES; return NO;}
  CID(0) = b1[0] == 1? b1[1]: b1[0];
  common_vertex(1, CID(0), &p, &w1, &w2);
  if (POINTS_IDENTICAL(center, p)) 
    if (w2 == END) CS(0) = AS_DEF; else CS(0) = REV;
  else
    if (w2 == START) CS(0) = AS_DEF; else CS(0) = REV;
  CID(1) = 1; CS(1) = (loops[0].c[0].sense == AS_DEF? REV: AS_DEF);
  ASSIGN_POINT_TO_POINT(toreach, POINT_ALONG_AT_START(0));
  ASSIGN_POINT_TO_POINT(curp, POINT_ALONG_AT_END(1));
  if (!CLOSED(curp)) {
    if (!find_branches(CID(1), &curp, b1, &nb1)){status = DEAD_END; return NO;}
    for (i = 0; i < nb1; i++) {
      common_vertex(CID(1), b1[i], &tp1, &w1, &w2);
      CID(2) = b1[i];
      if (w2 == END) CS(2) = REV; else CS(2) = AS_DEF;
      ASSIGN_POINT_TO_POINT(tp1, POINT_ALONG_AT_END(2));
      if (!CLOSED(tp1)) {
        if (!find_branches(CID(2), &tp1, b2, &nb2)){status=DEAD_END;return NO;}
        for (j = 0; j < nb2; j++) {
          common_vertex(CID(2), b2[j], &tp2, &w1, &w2);
          CID(3) = b2[j];
          if (w2 == END) CS(3) = REV; else CS(3) = AS_DEF;
          ASSIGN_POINT_TO_POINT(tp2, POINT_ALONG_AT_END(3));
          if (CLOSED(tp2))
            goto done1;
        }
      } else
        goto done1; /* closed by three curves */
    }
  } /* closed by only two curves */
 done1:
/* The third bounding surface -- loop  */
/*  curve */
/*   0           2  */
/*   3  19  31  43  curve 3 */
/*   2  18  30  42  NOTE: we are traversing counterclockwise, 12,8,4,0,...*/
/*   1  17  29  41  */
/*   0  16  28  40  curve 1 */
#undef CL
#define CL(ANYTHING) loops[2].ANYTHING
  CID(0) = 0;
  CS(0)  = (loops[0].c[1].sense == AS_DEF? REV: AS_DEF);
  CID(1) = loops[1].c[0].id;
  CS(1)  = (loops[1].c[0].sense == AS_DEF? REV: AS_DEF);
  ASSIGN_POINT_TO_POINT(toreach, POINT_ALONG_AT_START(0));
  ASSIGN_POINT_TO_POINT(curp, POINT_ALONG_AT_END(1));
  if (!CLOSED(curp)) {
    if (!find_branches(CID(1), &curp, b1, &nb1)){status = DEAD_END; return NO;}
    for (i = 0; i < nb1; i++) {
      common_vertex(CID(1), b1[i], &tp1, &w1, &w2);
      CID(2) = b1[i];
      if (w2 == END) CS(2) = REV; else CS(2) = AS_DEF;
      ASSIGN_POINT_TO_POINT(tp1, POINT_ALONG_AT_END(2));
      if (!CLOSED(tp1)) {
        if (!find_branches(CID(2), &tp1, b2, &nb2)){status=DEAD_END;return NO;}
        for (j = 0; j < nb2; j++) {
          common_vertex(CID(2), b2[j], &tp2, &w1, &w2);
          CID(3) = b2[j];
          if (w2 == END) CS(3) = REV; else CS(3) = AS_DEF;
          ASSIGN_POINT_TO_POINT(tp2, POINT_ALONG_AT_END(3));
          if (CLOSED(tp2))
            goto done2;
        }
      } else
        goto done2; /* closed by three curves */
    }
  } /* closed by only two curves */
  done2:
/* The fourth bounding surface -- loop  */
/*  curve */
/*   0           2  */
/*  43  47  51  55  curve 3 */
/*  42  46  50  54  NOTE: we are traversing counterclockwise, 43,42,41,40,...*/
/*  41  45  49  53  */
/*  40  44  48  52  curve 1 */
#undef CL
#define CL(ANYTHING) loops[3].ANYTHING
  CID(0) = loops[2].c[2].id;
  CS(0)  = (loops[2].c[2].sense == AS_DEF? REV: AS_DEF);
  CID(1) = loops[1].c[3].id;
  CS(1)  = (loops[1].c[3].sense == AS_DEF? REV: AS_DEF);
  ASSIGN_POINT_TO_POINT(toreach, POINT_ALONG_AT_START(0));
  ASSIGN_POINT_TO_POINT(curp, POINT_ALONG_AT_END(1));
  if (!CLOSED(curp)) {
    if (!find_branches(CID(1), &curp, b1, &nb1)){status = DEAD_END; return NO;}
    for (i = 0; i < nb1; i++) {
      common_vertex(CID(1), b1[i], &tp1, &w1, &w2);
      CID(2) = b1[i];
      if (w2 == END) CS(2) = REV; else CS(2) = AS_DEF;
      ASSIGN_POINT_TO_POINT(tp1, POINT_ALONG_AT_END(2));
      if (!CLOSED(tp1)) {
        if (!find_branches(CID(2), &tp1, b2, &nb2)){status=DEAD_END;return NO;}
        for (j = 0; j < nb2; j++) {
          common_vertex(CID(2), b2[j], &tp2, &w1, &w2);
          CID(3) = b2[j];
          if (w2 == END) CS(3) = REV; else CS(3) = AS_DEF;
          ASSIGN_POINT_TO_POINT(tp2, POINT_ALONG_AT_END(3));
          if (CLOSED(tp2))
            goto done3;
        }
      } else
        goto done3; /* closed by three curves */
    }
  } /* closed by only two curves */
  done3:
/* The fifth bounding surface -- loop  */
/*  curve */
/*   0           2  */
/*  15  27  39  55  curve 3 */
/*  11  23  25  51  NOTE: we are traversing counterclockwise, 12,8,4,0,...*/
/*   7  21  33  47  */
/*   3  19  31  43  curve 1 */
#undef CL
#define CL(ANYTHING) loops[4].ANYTHING
  CID(0) = loops[0].c[2].id;
  CS(0)  = (loops[0].c[2].sense == AS_DEF? REV: AS_DEF);
  CID(1) = loops[2].c[3].id;
  CS(1)  = (loops[2].c[3].sense == AS_DEF? REV: AS_DEF);
  CID(2) = loops[3].c[3].id;
  CS(2)  = (loops[3].c[3].sense == AS_DEF? REV: AS_DEF);
  ASSIGN_POINT_TO_POINT(toreach, POINT_ALONG_AT_START(0));
  ASSIGN_POINT_TO_POINT(curp, POINT_ALONG_AT_END(2));
  if (!CLOSED(curp)) {
    if (!find_branches(CID(2), &curp, b1, &nb1)){status = DEAD_END; return NO;}
    for (i = 0; i < nb1; i++) {
      common_vertex(CID(2), b1[i], &tp1, &w1, &w2);
      CID(3) = b1[i];
      if (w2 == END) CS(3) = REV; else CS(3) = AS_DEF;
      ASSIGN_POINT_TO_POINT(tp1, POINT_ALONG_AT_END(3));
      if (CLOSED(tp1)) 
        goto done4; /* closed by three curves */
    }
  } /* closed by only two curves */
  done4:
/* The sixth bounding surface -- loop  */
/*  curve */
/*   0           2  */
/*  52  53  54  55  curve 3 */
/*  36  37  38  39  NOTE: we are traversing counterclockwise, 12,8,4,0,...*/
/*  24  25  26  27  */
/*  12  13  14  15  curve 1 */
#undef CL
#define CL(ANYTHING) loops[5].ANYTHING
  CID(0) = loops[1].c[2].id;
  CS(0)  = (loops[1].c[2].sense == AS_DEF? REV: AS_DEF);
  CID(1) = loops[0].c[3].id;
  CS(1)  = (loops[0].c[3].sense == AS_DEF? REV: AS_DEF);
  CID(2) = loops[4].c[3].id;
  CS(2)  = (loops[4].c[3].sense == AS_DEF? REV: AS_DEF);
  CID(3) = loops[3].c[3].id;
  CS(3)  = (loops[3].c[3].sense == AS_DEF? REV: AS_DEF);
  
  return YES;
}

static BOOLEAN
find_branches(int curv, WCRec *curp, int b[2], int *nb)
{
  int i;
  int serial = 0, w1, w2;
  WCRec p;

  b[0] = b[1] = NONE;
  *nb = 0;
  for (i = 0; i < 12; i++) {
    if (curv != i) {
      if (common_vertex(curv, i, &p, &w1, &w2)) {
        if (POINTS_IDENTICAL(p, (*curp))) {
          b[serial] = i;
          *nb = ++serial;
          if (serial > 2) fprintf(stderr, "Suspect connectivity");
        }
      }
    }
  }
  return serial > 0;
}


static BOOLEAN
common_vertex(int curve1, int curve2,
              WCRec *p, int *which1, int *which2)
{
  if        (POINTS_IDENTICAL(cp[curve1][START], cp[curve2][START])) {
    ASSIGN_POINT_TO_POINT((*p), (cp[curve1][START]));
    *which1 = START; *which2 = START;
    return YES;
  } else if (POINTS_IDENTICAL(cp[curve1][START], cp[curve2][END])) {
    ASSIGN_POINT_TO_POINT((*p), cp[curve1][START]);
    *which1 = START; *which2 = END;
    return YES;
  } else if (POINTS_IDENTICAL(cp[curve1][END], cp[curve2][START])) {
    ASSIGN_POINT_TO_POINT((*p), cp[curve1][END]);
    *which1 = END; *which2 = START;
    return YES;
  } else if (POINTS_IDENTICAL(cp[curve1][END], cp[curve2][END])) {
    ASSIGN_POINT_TO_POINT((*p), cp[curve1][END]);
    *which1 = END; *which2 = END;
    return YES;
  }
  return NO;
}


static void
hexpoints(WCRec *points)
{
  int i;
  WCRec zero = {0, 0, 0};

  for (i = 0; i < 56; i++) ASSIGN_POINT_TO_POINT(points[i], zero);

/* Assign the points on the curves */
  points_on_curve(0, 0, 12,  8,  4,  0, points);
  points_on_curve(0, 1,  0,  1,  2,  3, points);
  points_on_curve(0, 2,  3,  7, 11, 15, points);
  points_on_curve(0, 3, 15, 14, 13, 12, points);
  points_on_curve(2, 1,  0, 16, 28, 40, points);
  points_on_curve(2, 2, 40, 41, 42, 43, points);
  points_on_curve(2, 3, 43, 31, 19,  3, points);
  points_on_curve(1, 2, 12, 24, 36, 52, points);
  points_on_curve(1, 3, 52, 48, 44, 40, points);
  points_on_curve(3, 2, 52, 53, 54, 55, points);
  points_on_curve(3, 3, 55, 51, 47, 43, points);
  points_on_curve(4, 3, 55, 39, 27, 15, points);
  
/* Compute the internal points inside the patches */
  points_inside(0, 1, 2, 3,
                4, 5, 6, 7,
                8, 9, 10, 11,
                12, 13, 14, 15, points);
  
  points_inside(0, 4, 8, 12,
                16, 20, 22, 24,
                28, 32, 34, 36,
                40, 44, 48, 52, points);
  
  points_inside(12, 13, 14, 15,
                24, 25, 26, 27,
                36, 37, 38, 39,
                52, 53, 54, 55, points);

  points_inside(3, 2, 1, 0,
                19, 18, 17, 16,
                31, 30, 29, 28,
                43, 42, 41, 40, points);

  points_inside(43, 42, 41, 40,
                47, 46, 45, 44,
                51, 50, 49, 48,
                55, 54, 53, 52, points);

  points_inside(3, 19, 31, 43,
                7, 21, 33, 47,
                11, 23, 35, 51,
                15, 27, 39, 55, points);
}


static void
points_on_curve(int loopn, int cn, int p1, int p2, int p3, int p4,
                WCRec *points)
{
  int ix, i1, i2, i3, i4;
  
  if (loops[loopn].c[cn].id == NONE) {
    while (loops[loopn].c[cn].id == NONE) {
      cn--;
      if (cn < 0) {
        status = BAD_DEGENERACY;
        return;
      }
    }
    if (loops[loopn].c[cn].sense == AS_DEF) ix = START;
    else                                    ix = END;
    ASSIGN_POINT_TO_POINT(points[p1], cp[loops[loopn].c[cn].id][ix]);
    ASSIGN_POINT_TO_POINT(points[p2], cp[loops[loopn].c[cn].id][ix]);
    ASSIGN_POINT_TO_POINT(points[p3], cp[loops[loopn].c[cn].id][ix]);
    ASSIGN_POINT_TO_POINT(points[p4], cp[loops[loopn].c[cn].id][ix]);
  } else {
    if (loops[loopn].c[cn].sense == AS_DEF) {
      i1 = 0; i2 = 1; i3 = 2; i4 = 3;
    } else {
      i1 = 3; i2 = 2; i3 = 1; i4 = 0;
    }
    ASSIGN_POINT_TO_POINT(points[p1], cp[loops[loopn].c[cn].id][i1]);
    ASSIGN_POINT_TO_POINT(points[p2], cp[loops[loopn].c[cn].id][i2]);
    ASSIGN_POINT_TO_POINT(points[p3], cp[loops[loopn].c[cn].id][i3]);
    ASSIGN_POINT_TO_POINT(points[p4], cp[loops[loopn].c[cn].id][i4]);
  }
}



#define SUBTR_POINTS(RESLT_VEC, VEC1, VEC2)                \
    {                                                      \
    (RESLT_VEC).x = (VEC1).x - (VEC2).x;                   \
    (RESLT_VEC).y = (VEC1).y - (VEC2).y;                   \
    (RESLT_VEC).z = (VEC1).z - (VEC2).z;                   \
    }
#define ADD_UP_POINTS(RESLT_VEC, VEC1, VEC2)               \
    {                                                      \
    (RESLT_VEC).x = (VEC1).x + (VEC2).x;                   \
    (RESLT_VEC).y = (VEC1).y + (VEC2).y;                   \
    (RESLT_VEC).z = (VEC1).z + (VEC2).z;                   \
    }


static void
points_inside(int i00, int i10, int i20, int i30,
              int i01, int i11, int i21, int i31,
              int i02, int i12, int i22, int i32,
              int i03, int i13, int i23, int i33,
              WCRec *points)
{
  WCRec by;

  SUBTR_POINTS(by, points[i10], points[i00]);
  ADD_UP_POINTS(points[i11], points[i01], by);
  SUBTR_POINTS(by, points[i20], points[i30]);
  ADD_UP_POINTS(points[i21], points[i31], by);
  SUBTR_POINTS(by, points[i13], points[i03]);
  ADD_UP_POINTS(points[i12], points[i02], by);
  SUBTR_POINTS(by, points[i23], points[i33]);
  ADD_UP_POINTS(points[i22], points[i32], by);
}

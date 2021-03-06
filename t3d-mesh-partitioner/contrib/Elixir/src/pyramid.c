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

/* This file holds the redefinition of graphic methods for the tetrahedron */

#include "Econfig.h"
#include <stdlib.h>

#include "Eview.h"
#include "Egraphic.h"
#include "Egeomops.h"
#include "Ecolors.h"
#include "Eactset.h"

#define PYRAMID_PRIVATE_HEADER
#include "Epyramid.h"

/* ========================================================================= */
/* PRIVATE DATA */
/* ========================================================================= */

#define STRING_BUFF_SIZE 256

static char string_rep_buffer[STRING_BUFF_SIZE];

#define ASSIGN_POINT_TO_POINT(DEST, SRC)                         \
  {                                                              \
     (DEST).x = (SRC).x; (DEST).y = (SRC).y; (DEST).z = (SRC).z; \
   }

/* ========================================================================= */
/* PUBLIC */
/* ========================================================================= */

GraphicObj *
CreatePyramid(WCRec *points) 
{
  EPyramidWCRec *thedp;
  GraphicObj *g_p;
 
  thedp = (EPyramidWCRec *)make_node(sizeof(EPyramidWCRec));
  g_p = EGCreateGraphics(&pyramid_graphic_methods, (caddr_t)thedp,
			 sizeof(EPyramidWCRec));
  ModifyGeometry(g_p, points);
  thedp->attributes.spec.fill_style = FILL_HOLLOW;
  thedp->attributes.spec.color      = 0x0;
  thedp->attributes.spec.edge_flag  = NO;
  thedp->attributes.spec.edge_color = 0x0;
  thedp->attributes.spec.shrink     = 1.0;
  thedp->attributes.spec.have_any   = YES;

  return g_p;  
}



static void 
Draw(EView *v_p, GraphicObj *p)
{
  EPyramidWCRec *thedp;

  thedp = (EPyramidWCRec *)(p->spec.data);

  EVTetraPrimitive(v_p, p,
                   &thedp->point_1, &thedp->point_2,
                   &thedp->point_3, &thedp->point_5,
                   thedp->attributes.spec.color,
                   thedp->attributes.spec.fill_style,
                   thedp->attributes.spec.edge_flag,
                   thedp->attributes.spec.edge_color,
                   thedp->attributes.spec.shrink,
                   DRAW_MODE_DRAW);
  EVTetraPrimitive(v_p, p,
                   &thedp->point_1, &thedp->point_3,
                   &thedp->point_4, &thedp->point_5,
                   thedp->attributes.spec.color,
                   thedp->attributes.spec.fill_style,
                   thedp->attributes.spec.edge_flag,
                   thedp->attributes.spec.edge_color,
                   thedp->attributes.spec.shrink,
                   DRAW_MODE_DRAW);
}



static EGraphicType 
Type(GraphicObj *g_p)
{
  return EG_PYRAMID;
}



static char *
AsString(GraphicObj *g_p)
{
  EPyramidWCRec *l;

  l = (EPyramidWCRec *)g_p->spec.data;
  sprintf(string_rep_buffer,
	  "PYRAMID [%f,%f,%f] [%f,%f,%f] \n"
    "        [%f,%f,%f] [%f,%f,%f] \n"
    "        [%f,%f,%f]",
	  l->point_1.x, l->point_1.y, l->point_1.z,
	  l->point_2.x, l->point_2.y, l->point_2.z, 
	  l->point_3.x, l->point_3.y, l->point_3.z,
					l->point_4.x, l->point_4.y, l->point_4.z,
          l->point_5.x, l->point_5.y, l->point_5.z);
  
  return string_rep_buffer; /* Don't free this memory!!! */
}



static void 
RetrieveFrom(FILE *file_p, GraphicSpecObj *gsp)
{
  EPyramidWCRec *p;
  int filled, edge_flag;
  char color[64];
  EPixel pixel;
  BOOLEAN success;
  double x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4, x5, y5, z5;
  float shrink;
  
  if ((p = (EPyramidWCRec *)make_node(sizeof(EPyramidWCRec)))
      != NULL) {
    fscanf(file_p,
	   "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
	   &x1, &y1, &z1, &x2, &y2, &z2, &x3, &y3, &z3, &x4, &y4, &z4, &x5, &y5, &z5);
    p->point_1.x = x1;
    p->point_1.y = y1;
    p->point_1.z = z1;
    p->point_2.x = x2;
    p->point_2.y = y2;
    p->point_2.z = z2;
    p->point_3.x = x3;
    p->point_3.y = y3;
    p->point_3.z = z3;
    p->point_4.x = x4;
    p->point_4.y = y4;
    p->point_4.z = z4;
    p->point_5.x = x5;
    p->point_5.y = y5;
    p->point_5.z = z5;
    fscanf(file_p,
	   "%d %d %d %f",
	   &(p->attributes.spec.have_any),
	   &filled, &edge_flag, &shrink);
    p->attributes.spec.fill_style = filled;
    p->attributes.spec.edge_flag  = edge_flag;
    p->attributes.spec.shrink     = shrink;
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
  gsp->data_length = sizeof(EPyramidWCRec);
}
 


static void 
StoreOn(FILE *file_p, GraphicObj *g_p)
{
  EPyramidWCRec *p;
  char *color, *edge_color;

  p = (EPyramidWCRec *)g_p->spec.data;
  if (p->attributes.spec.have_any == YES) {
    color = ColorGetStringFromPixel(p->attributes.spec.color);
    if (color == NULL)
      color = "white";
    edge_color = ColorGetStringFromPixel(p->attributes.spec.edge_color);
    if (edge_color == NULL)
      edge_color = "white";
  } else {
    edge_color = color = "white";
  }
  fprintf(file_p,
	  "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n%d %d %d %f\n%s\n%s\n",
	  p->point_1.x, p->point_1.y, p->point_1.z,
	  p->point_2.x, p->point_2.y, p->point_2.z, 
	  p->point_3.x, p->point_3.y, p->point_3.z,
	  p->point_4.x, p->point_4.y, p->point_4.z,
	  p->point_5.x, p->point_5.y, p->point_5.z,
	  (p->attributes.spec.have_any),
	  p->attributes.spec.fill_style,
	  p->attributes.spec.edge_flag,
	  p->attributes.spec.shrink,
	  color, edge_color);
}




static void 
BoundingBoxVC(EView *v_p, GraphicObj *g_p,
                     VCRec *ll_return, VCRec *ur_return)
{
  EPyramidWCRec *p;
  FPNum tmp;
  VCRec p1, p2, p3, p4, p5;
  
  p = (EPyramidWCRec *)(g_p->spec.data);
  EVWCtoVC(v_p, &(p->point_1), &p1);
  EVWCtoVC(v_p, &(p->point_2), &p2);
  EVWCtoVC(v_p, &(p->point_3), &p3);
  EVWCtoVC(v_p, &(p->point_4), &p4);
  EVWCtoVC(v_p, &(p->point_5), &p5);

  tmp = min(p1.u, p2.u); tmp = min(tmp, p3.u); tmp = min(tmp, p4.u);
  ll_return->u = min(tmp, p5.u);
  
  tmp = min(p1.v, p2.v); tmp = min(tmp, p3.v); tmp = min(tmp, p4.v);
  ll_return->v = min(tmp, p5.v);
  
  tmp = min(p1.n, p2.n); tmp = min(tmp, p3.n); tmp = min(tmp, p4.n);
  ll_return->n = min(tmp, p5.n);
  
  tmp = max(p1.u, p2.u);  tmp = max(tmp, p3.u); tmp = max(tmp, p4.u);
  ur_return->u = max(tmp, p5.u);
  
  tmp = max(p1.v, p2.v); tmp = max(tmp, p3.v); tmp = max(tmp, p4.v);
  ur_return->v = max(tmp, p5.v);
  
  tmp = max(p1.n, p2.n); tmp = max(tmp, p3.n); tmp = max(tmp, p4.n); 
  ur_return->n = max(tmp, p5.n);
}



static void 
BoundingBoxWC(GraphicObj *g_p,
			     WCRec *ll_return, WCRec *ur_return)
{
  EPyramidWCRec *p;
  FPNum tmp;
  
  p = (EPyramidWCRec *)(g_p->spec.data);

  tmp          = min(p->point_1.x, p->point_2.x);
  tmp          = min(tmp, p->point_3.x);
  tmp          = min(tmp, p->point_4.x);
  ll_return->x = min(tmp, p->point_5.x);
  tmp          = min(p->point_1.y, p->point_2.y);
  tmp          = min(tmp, p->point_3.y);
  tmp          = min(tmp, p->point_4.y);
  ll_return->y = min(tmp, p->point_5.y);
  tmp          = min(p->point_1.z, p->point_2.z);
  tmp          = min(tmp, p->point_3.z);
  tmp          = min(tmp, p->point_4.z);
  ll_return->z = min(tmp, p->point_5.z);

  tmp          = max(p->point_1.x, p->point_2.x);
  tmp          = max(tmp, p->point_3.x);
  tmp          = max(tmp, p->point_4.x);
  ur_return->x = max(tmp, p->point_5.x);
  tmp          = max(p->point_1.y, p->point_2.y);
  tmp          = max(tmp, p->point_3.y);
  tmp          = max(tmp, p->point_4.y);
  ur_return->y = max(tmp, p->point_5.y);
  tmp          = max(p->point_1.z, p->point_2.z);
  tmp          = max(tmp, p->point_3.z);
  tmp          = max(tmp, p->point_4.z);
  ur_return->z = max(tmp, p->point_5.z);  
}




static void 
XorDraw(EView *v_p, GraphicObj *g_p)
{
  EPyramidWCRec *thedp;

  thedp = (EPyramidWCRec *)(g_p->spec.data);

  EVTetraPrimitive(v_p, g_p,
                   &thedp->point_1, &thedp->point_2,
                   &thedp->point_3, &thedp->point_5,
                   thedp->attributes.spec.color,
                   thedp->attributes.spec.fill_style,
                   NO, 0,
                   thedp->attributes.spec.shrink,
                   DRAW_MODE_XORDRAW);
  EVTetraPrimitive(v_p, g_p,
                   &thedp->point_1, &thedp->point_3,
                   &thedp->point_4, &thedp->point_5,
                   thedp->attributes.spec.color,
                   thedp->attributes.spec.fill_style,
                   NO, 0,
                   thedp->attributes.spec.shrink,
                   DRAW_MODE_XORDRAW);
}

 


static GraphicObj *
ModifyGeometry(GraphicObj *g_p, WCRec *points)
{
  EPyramidWCRec *thedp;

  thedp = (EPyramidWCRec *)(g_p->spec.data);

  thedp->point_1.x = points[0].x;
  thedp->point_1.y = points[0].y;
  thedp->point_1.z = points[0].z;
  thedp->point_2.x = points[1].x;
  thedp->point_2.y = points[1].y;
  thedp->point_2.z = points[1].z;
  thedp->point_3.x = points[2].x;
  thedp->point_3.y = points[2].y;
  thedp->point_3.z = points[2].z;
  thedp->point_4.x = points[3].x;
  thedp->point_4.y = points[3].y;
  thedp->point_4.z = points[3].z;
  thedp->point_5.x = points[4].x;
  thedp->point_5.y = points[4].y;
  thedp->point_5.z = points[4].z;

  return g_p;
}



static BOOLEAN 
GetGeometry(GraphicObj *g_p, WCRec *points, int *npoints)
{
  EPyramidWCRec *thedp;

  thedp = (EPyramidWCRec *)(g_p->spec.data);
  points[0].x = thedp->point_1.x;
  points[0].y = thedp->point_1.y;
  points[0].z = thedp->point_1.z;
  points[1].x = thedp->point_2.x;
  points[1].y = thedp->point_2.y;
  points[1].z = thedp->point_2.z;
  points[2].x = thedp->point_3.x;
  points[2].y = thedp->point_3.y;
  points[2].z = thedp->point_3.z;
  points[3].x = thedp->point_4.x;
  points[3].y = thedp->point_4.y;
  points[3].z = thedp->point_4.z;
  points[4].x = thedp->point_5.x;
  points[4].y = thedp->point_5.y;
  points[4].z = thedp->point_5.z;
  *npoints    = 5;
  
  return YES;
}



static void 
Hilite(EView *v_p, GraphicObj *g_p)
{
  EPyramidWCRec *thedp;
  
  thedp = (EPyramidWCRec *)(g_p->spec.data);

  EVTetraPrimitive(v_p, g_p,
                   &thedp->point_1, &thedp->point_2,
                   &thedp->point_3, &thedp->point_5,
                   thedp->attributes.spec.color,
                   FILL_HOLLOW, NO, 0,
                   1.0,
                   DRAW_MODE_HILITE);
  EVTetraPrimitive(v_p, g_p,
                   &thedp->point_1, &thedp->point_3,
                   &thedp->point_4, &thedp->point_5,
                   thedp->attributes.spec.color,
                   FILL_HOLLOW, NO, 0,
                   1.0,
                   DRAW_MODE_HILITE);
}



static void 
Unhilite(EView *v_p, GraphicObj *g_p)
{
  EPyramidWCRec *thedp;
  
  thedp = (EPyramidWCRec *)(g_p->spec.data);

  EVTetraPrimitive(v_p, g_p,
                   &thedp->point_1, &thedp->point_2,
                   &thedp->point_3, &thedp->point_5,
                   thedp->attributes.spec.color,
                   FILL_HOLLOW, NO, 0,
                   1.0,
                   DRAW_MODE_UNHILITE);
  EVTetraPrimitive(v_p, g_p,
                   &thedp->point_1, &thedp->point_3,
                   &thedp->point_4, &thedp->point_5,
                   thedp->attributes.spec.color,
                   FILL_HOLLOW, NO, 0,
                   1.0,
                   DRAW_MODE_UNHILITE);
}



static void 
Erase(EView *v_p, GraphicObj *g_p)
{
  EPyramidWCRec *thedp;
  
  thedp = (EPyramidWCRec *)(g_p->spec.data);

  EVTetraPrimitive(v_p, g_p,
                   &thedp->point_1, &thedp->point_2,
                   &thedp->point_3, &thedp->point_5,
                   thedp->attributes.spec.color,
                   thedp->attributes.spec.fill_style,
                   NO, 0,
                   thedp->attributes.spec.shrink,
                   DRAW_MODE_ERASE);
  EVTetraPrimitive(v_p, g_p,
                   &thedp->point_1, &thedp->point_3,
                   &thedp->point_4, &thedp->point_5,
                   thedp->attributes.spec.color,
                   thedp->attributes.spec.fill_style,
                   NO, 0,
                   thedp->attributes.spec.shrink,
                   DRAW_MODE_ERASE);
}



static GraphicObj *
Translate(GraphicObj *g_p, WCRec *by)
{
  EPyramidWCRec *p;

  p = (EPyramidWCRec *)g_p->spec.data;
  p->point_1.x += by->x, p->point_1.y += by->y, p->point_1.z += by->z;
  p->point_2.x += by->x, p->point_2.y += by->y, p->point_2.z += by->z;
  p->point_3.x += by->x, p->point_3.y += by->y, p->point_3.z += by->z;
  p->point_4.x += by->x, p->point_4.y += by->y, p->point_4.z += by->z;
  p->point_5.x += by->x, p->point_5.y += by->y, p->point_5.z += by->z;

  return g_p;
}


static GraphicObj *
Rotate(GraphicObj *g_p, WCRec *center, WCRec *axial)
{
  EPyramidWCRec *p;
  WCRec x_c;

  p = (EPyramidWCRec *)g_p->spec.data;
  x_c.x = p->point_1.x - center->x;
  x_c.y = p->point_1.y - center->y;
  x_c.z = p->point_1.z - center->z;
  RotVectAboutVect(axial, &x_c);
  p->point_1.x = center->x + x_c.x;
  p->point_1.y = center->y + x_c.y;
  p->point_1.z = center->z + x_c.z;
  
  x_c.x = p->point_2.x - center->x;
  x_c.y = p->point_2.y - center->y;
  x_c.z = p->point_2.z - center->z;
  RotVectAboutVect(axial, &x_c);
  p->point_2.x = center->x + x_c.x;
  p->point_2.y = center->y + x_c.y;
  p->point_2.z = center->z + x_c.z;
  
  x_c.x = p->point_3.x - center->x;
  x_c.y = p->point_3.y - center->y;
  x_c.z = p->point_3.z - center->z;
  RotVectAboutVect(axial, &x_c);
  p->point_3.x = center->x + x_c.x;
  p->point_3.y = center->y + x_c.y;
  p->point_3.z = center->z + x_c.z;

  x_c.x = p->point_4.x - center->x;
  x_c.y = p->point_4.y - center->y;
  x_c.z = p->point_4.z - center->z;
  RotVectAboutVect(axial, &x_c);
  p->point_4.x = center->x + x_c.x;
  p->point_4.y = center->y + x_c.y;
  p->point_4.z = center->z + x_c.z;

  x_c.x = p->point_5.x - center->x;
  x_c.y = p->point_5.y - center->y;
  x_c.z = p->point_5.z - center->z;
  RotVectAboutVect(axial, &x_c);
  p->point_5.x = center->x + x_c.x;
  p->point_5.y = center->y + x_c.y;
  p->point_5.z = center->z + x_c.z;
  return g_p;
}



static GraphicObj  *
MirrorPlane(GraphicObj *g_p, WCRec *point_on_plane,
                   WCRec *unit_plane_normal)
{
  EPyramidWCRec *dp;

  dp = (EPyramidWCRec *)g_p->spec.data;
  MirrorPointInPlane(point_on_plane, unit_plane_normal, &dp->point_1);
  MirrorPointInPlane(point_on_plane, unit_plane_normal, &dp->point_2);
  MirrorPointInPlane(point_on_plane, unit_plane_normal, &dp->point_3);
  MirrorPointInPlane(point_on_plane, unit_plane_normal, &dp->point_4);
  MirrorPointInPlane(point_on_plane, unit_plane_normal, &dp->point_5);

  return g_p;
}



static GraphicObj  *
MirrorCenter(GraphicObj *g_p, WCRec *center)
{
  EPyramidWCRec *dp;

  dp = (EPyramidWCRec *)g_p->spec.data;
  dp->point_1.x = 2.*center->x - dp->point_1.x;
  dp->point_1.y = 2.*center->y - dp->point_1.y;
  dp->point_1.z = 2.*center->z - dp->point_1.z;
  dp->point_2.x = 2.*center->x - dp->point_2.x;
  dp->point_2.y = 2.*center->y - dp->point_2.y;
  dp->point_2.z = 2.*center->z - dp->point_2.z;
  dp->point_3.x = 2.*center->x - dp->point_3.x;
  dp->point_3.y = 2.*center->y - dp->point_3.y;
  dp->point_3.z = 2.*center->z - dp->point_3.z;
  dp->point_4.x = 2.*center->x - dp->point_4.x;
  dp->point_4.y = 2.*center->y - dp->point_4.y;
  dp->point_4.z = 2.*center->z - dp->point_4.z;
  dp->point_5.x = 2.*center->x - dp->point_5.x;
  dp->point_5.y = 2.*center->y - dp->point_5.y;
  dp->point_5.z = 2.*center->z - dp->point_5.z;  
  return g_p;
}


 
static GraphicObj *
Scale(GraphicObj *g_p, WCRec *center, 
		   FPNum sx, FPNum sy, FPNum sz)
{
  return g_p;
}



static BOOLEAN 
HitByBox(EView *v_p, EBoxVC2DRec *box,
			   ESelectCriteria sel_crit, GraphicObj *g_p)
{
  VCRec ll, ur;

  switch(sel_crit) {
  case INSIDE:
    BoundingBoxVC(v_p, g_p, &ll, &ur);
    if (ll.u >= box->left   && ur.u <= box->right && 
	ll.v >= box->bottom && ur.v <= box->top)
      return YES;
    else
      return NO;
    break;
  case OVERLAP:
    {
      VCRec p1, p2, p3, p4, p5;
      EPyramidWCRec *p;

      p = (EPyramidWCRec *)g_p->spec.data;
      EVWCtoVC(v_p, &(p->point_1), &p1);
      EVWCtoVC(v_p, &(p->point_2), &p2);
      EVWCtoVC(v_p, &(p->point_3), &p3);
      EVWCtoVC(v_p, &(p->point_4), &p4);
      EVWCtoVC(v_p, &(p->point_5), &p5);
      
      if (!BoxOverlapsTriangle(box,
                               (VC2DRec *)&p1,
                               (VC2DRec *)&p2,
                               (VC2DRec *)&p5))
        if (!BoxOverlapsTriangle(box,
                                 (VC2DRec *)&p2,
                                 (VC2DRec *)&p3,
                                 (VC2DRec *)&p5))
          if (!BoxOverlapsTriangle(box,
                                   (VC2DRec *)&p3,
                                   (VC2DRec *)&p4,
                                   (VC2DRec *)&p5))
            if (!BoxOverlapsTriangle(box,
                                     (VC2DRec *)&p4,
                                     (VC2DRec *)&p1,
                                     (VC2DRec *)&p5))
              if (!BoxOverlapsTriangle(box,
                                       (VC2DRec *)&p1,
                                       (VC2DRec *)&p2,
                                       (VC2DRec *)&p3))
                if (!BoxOverlapsTriangle(box,
                                         (VC2DRec *)&p1,
                                         (VC2DRec *)&p3,
                                         (VC2DRec *)&p4))
                  return NO;
      return YES;
      
    }
    break;
  case INTERSECT:
  default:
    {
      VCRec p1, p2, p3, p4, p5;
      EPyramidWCRec *p;
      
      p = (EPyramidWCRec *)g_p->spec.data;
      EVWCtoVC(v_p, &(p->point_1), &p1);
      EVWCtoVC(v_p, &(p->point_2), &p2);
      EVWCtoVC(v_p, &(p->point_3), &p3);
      EVWCtoVC(v_p, &(p->point_4), &p4);
      EVWCtoVC(v_p, &(p->point_5), &p5);
      if (!BoxIntersectsTriangle(box,
                                 (VC2DRec *)&p1,
                                 (VC2DRec *)&p2,
                                 (VC2DRec *)&p5))
        if (!BoxIntersectsTriangle(box,
                                   (VC2DRec *)&p2,
                                   (VC2DRec *)&p3,
                                   (VC2DRec *)&p5))
          if (!BoxIntersectsTriangle(box,
                                     (VC2DRec *)&p3,
                                     (VC2DRec *)&p4,
                                     (VC2DRec *)&p5))
            if (!BoxIntersectsTriangle(box,
                                       (VC2DRec *)&p4,
                                       (VC2DRec *)&p1,
                                       (VC2DRec *)&p5))
              return NO;
      return YES;
    }
    break;
  }
}



static LIST 
KeyPoints(GraphicObj *p)
{
  LIST temp_list;
  KeyPointRec *key_p;
  EPyramidWCRec *l_p;

  l_p       = (EPyramidWCRec *)(p->spec.data);

  temp_list = make_list();
  key_p = (KeyPointRec*)add_to_tail(temp_list, make_node(sizeof(KeyPointRec)));
  key_p->coords.x = l_p->point_1.x;
  key_p->coords.y = l_p->point_1.y;
  key_p->coords.z = l_p->point_1.z;
  key_p->dist_from_hit = 0.0;
  key_p = (KeyPointRec*)add_to_tail(temp_list, make_node(sizeof(KeyPointRec)));
  key_p->coords.x = l_p->point_2.x;
  key_p->coords.y = l_p->point_2.y;
  key_p->coords.z = l_p->point_2.z;
  key_p->dist_from_hit = 0.0;
  key_p = (KeyPointRec*)add_to_tail(temp_list, make_node(sizeof(KeyPointRec)));
  key_p->coords.x = l_p->point_3.x;
  key_p->coords.y = l_p->point_3.y;
  key_p->coords.z = l_p->point_3.z;
  key_p->dist_from_hit = 0.0;
  key_p = (KeyPointRec*)add_to_tail(temp_list, make_node(sizeof(KeyPointRec)));
  key_p->coords.x = l_p->point_4.x;
  key_p->coords.y = l_p->point_4.y;
  key_p->coords.z = l_p->point_4.z;
  key_p->dist_from_hit = 0.0;
  key_p = (KeyPointRec*)add_to_tail(temp_list, make_node(sizeof(KeyPointRec)));
  key_p->coords.x = l_p->point_5.x;
  key_p->coords.y = l_p->point_5.y;
  key_p->coords.z = l_p->point_5.z;
  key_p->dist_from_hit = 0.0;
  return temp_list;
}



static GraphicObj *
Reshape(GraphicObj *p, EHandleNum hn, WCRec *pos)
{
  EPyramidWCRec *_p;

  _p = (EPyramidWCRec *)(p->spec.data);
 
  if        (hn == 1) {
    _p->point_1.x = pos->x, _p->point_1.y = pos->y, _p->point_1.z = pos->z;
  } else if (hn == 2) {
    _p->point_2.x = pos->x, _p->point_2.y = pos->y, _p->point_2.z = pos->z;
  } else if (hn == 3) {
    _p->point_3.x = pos->x, _p->point_3.y = pos->y, _p->point_3.z = pos->z;
  } else if (hn == 4) {
    _p->point_4.x = pos->x, _p->point_4.y = pos->y, _p->point_4.z = pos->z;
  } else if (hn == 5) {
    _p->point_5.x = pos->x, _p->point_5.y = pos->y, _p->point_5.z = pos->z;
  } 
  return p;
}



static EHandleNum 
ReshapeHandle(EView *v_p, GraphicObj *p, WCRec *nearp)
{
  EPyramidWCRec *l_p;
  FPNum dist1, dist2, dist3, dist4, dist5, tmp;
  VCRec nvc, pvc;

  l_p       = (EPyramidWCRec *)(p->spec.data);
  EVWCtoVC(v_p, nearp, &nvc);
  EVWCtoVC(v_p, &(l_p->point_1), &pvc);
  dist1 = DistOfPnts(pvc.u, pvc.v, nvc.u, nvc.v);
  EVWCtoVC(v_p, &(l_p->point_2), &pvc);
  dist2 = DistOfPnts(pvc.u, pvc.v, nvc.u, nvc.v);
  EVWCtoVC(v_p, &(l_p->point_3), &pvc);
  dist3 = DistOfPnts(pvc.u, pvc.v, nvc.u, nvc.v);
  EVWCtoVC(v_p, &(l_p->point_4), &pvc);
  dist4 = DistOfPnts(pvc.u, pvc.v, nvc.u, nvc.v);
  EVWCtoVC(v_p, &(l_p->point_5), &pvc);
  dist5 = DistOfPnts(pvc.u, pvc.v, nvc.u, nvc.v);
	tmp = dist1;
	tmp = min(tmp, dist2);
	tmp = min(tmp, dist3);
	tmp = min(tmp, dist4);
	tmp = min(tmp, dist5);
	if(tmp == dist1) return (EHandleNum)1;
	if(tmp == dist2) return (EHandleNum)2;
	if(tmp == dist3) return (EHandleNum)3;
	if(tmp == dist4) return (EHandleNum)4;
	if(tmp == dist5) return (EHandleNum)5;
  return (EHandleNum)0;
}



static void 
ShowHandles(EView *v_p, GraphicObj *p)
{
  EPyramidWCRec *l_p;

  l_p = (EPyramidWCRec *)p->spec.data;
  EGDrawHandle(v_p, &(l_p->point_1));
  EGDrawHandle(v_p, &(l_p->point_2));
  EGDrawHandle(v_p, &(l_p->point_3));
  EGDrawHandle(v_p, &(l_p->point_4));
  EGDrawHandle(v_p, &(l_p->point_5));
}
  


static void 
EraseHandles(EView *v_p, GraphicObj *p)
{
  EPyramidWCRec *l_p;

  l_p = (EPyramidWCRec *)p->spec.data;
  EGEraseHandle(v_p, &(l_p->point_1)); 
  EGEraseHandle(v_p, &(l_p->point_2));
  EGEraseHandle(v_p, &(l_p->point_3));
  EGEraseHandle(v_p, &(l_p->point_4));
  EGEraseHandle(v_p, &(l_p->point_5));
}



static void 
XYofHandle(GraphicObj *p, EHandleNum hn, WCRec *pos)
{
  EPyramidWCRec *l_p;

  l_p = (EPyramidWCRec *)p->spec.data;
  switch((int)hn) {
  case 1:
  default:
    pos->x = l_p->point_1.x;
    pos->y = l_p->point_1.y;
    pos->z = l_p->point_1.z;
    break;
  case 2:
    pos->x = l_p->point_2.x;
    pos->y = l_p->point_2.y;
    pos->z = l_p->point_2.z;
    break;
  case 3:
    pos->x = l_p->point_3.x;
    pos->y = l_p->point_3.y;
    pos->z = l_p->point_3.z;
    break;
  case 4:
    pos->x = l_p->point_4.x;
    pos->y = l_p->point_4.y;
    pos->z = l_p->point_4.z;
    break;  
  case 5:
    pos->x = l_p->point_5.x;
    pos->y = l_p->point_5.y;
    pos->z = l_p->point_5.z;
    break;  
  }
}



static void 
ChangeAttributes(GraphicObj *p)
{
  EPyramidWCRec *dp;
  unsigned long mask;

  dp = (EPyramidWCRec *)p->spec.data;

  mask = EASValsGetChangeMask();
  
  if ((mask & FILL_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.fill_style = EASValsGetFillStyle();
    dp->attributes.spec.have_any = YES;
  }
  if ((mask & COLOR_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.color = EASValsGetColor();
    dp->attributes.spec.have_any = YES;
  }
  if ((mask & EDGE_COLOR_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.edge_color = EASValsGetEdgeColor();
    dp->attributes.spec.have_any = YES;
  }
  if ((mask & EDGE_FLAG_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.edge_flag = EASValsGetEdgeFlag();
    dp->attributes.spec.have_any = YES;
  }
  if ((mask & SHRINK_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.shrink = EASValsGetShrink();
    dp->attributes.spec.have_any = YES;
  }
}



static void 
CopyAttributesToActiveSet(GraphicObj *p)
{
  EPyramidWCRec *dp;

  dp = (EPyramidWCRec *)p->spec.data;
  
  if (dp->attributes.spec.have_any) {
    EASValsSetFillStyle(dp->attributes.spec.fill_style);
    EASValsSetColor(dp->attributes.spec.color);
    EASValsSetEdgeColor(dp->attributes.spec.edge_color);
    EASValsSetEdgeFlag(dp->attributes.spec.edge_flag);
    EASValsSetShrink(dp->attributes.spec.shrink);
  }
}


static BOOLEAN
AttribValAsInAS(GraphicObj *p, unsigned long mask)  
{
  EPyramidWCRec *dp;

  dp = (EPyramidWCRec *)p->spec.data;
  
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
  if ((mask & SHRINK_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.shrink != EASValsGetShrink())
      return NO;

  return YES;
}

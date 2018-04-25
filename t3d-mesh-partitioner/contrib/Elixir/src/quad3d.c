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

/* This file holds the redefinition of graphic methods for the quad */
/* with data. */

#include "Econfig.h"
#include <stdlib.h>

#include "Eview.h"
#include "Egraphic.h"
#include "Egeomops.h"
#include "Ecolors.h"
#include "Eactset.h"

#include "Eline3d.h"
#define QUAD3D_PRIVATE_HEADER
#include "Equad3d.h"

/* ========================================================================= */
/* PRIVATE DATA */
/* ========================================================================= */

#define STRING_BUFF_SIZE 132

static char string_rep_buffer[STRING_BUFF_SIZE];

/* ========================================================================= */
/* PUBLIC */
/* ========================================================================= */

GraphicObj *
CreateQuad3D(WCRec *points)
{
  EQuad3DWCRec *thedp;
  GraphicObj *g_p;

  thedp = (EQuad3DWCRec *)make_node(sizeof(EQuad3DWCRec));
  g_p = EGCreateGraphics(&quad_3d_graphic_methods, (caddr_t)thedp,
			 sizeof(EQuad3DWCRec));
  ModifyGeometry(g_p, points);
  thedp->attributes.spec.fill_style = FILL_SOLID;
  thedp->attributes.spec.color      = 0x0;
  thedp->attributes.spec.edge_flag  = NO;
  thedp->attributes.spec.edge_color = 0x0;
  thedp->attributes.spec.shrink     = 1.0;
  thedp->attributes.spec.have_any   = YES;

  return g_p;
}



static BOOLEAN 
GetGeometry(GraphicObj *g_p, WCRec *points,
                              int *npoints)
{
  EQuad3DWCRec *thedp = (EQuad3DWCRec *)(g_p->spec.data);

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
  *npoints    = 4;
  return YES;
}


static void 
Draw(EView *v_p, GraphicObj *p)
{
  EQuad3DWCRec *thedp = (EQuad3DWCRec *)(p->spec.data);

  EVQuad3DPrimitive(v_p, p,
                    &thedp->point_1,
                    &thedp->point_2,
                    &thedp->point_3,
                    &thedp->point_4,
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
  return EG_QUAD3D;
}



static char *
AsString(GraphicObj *g_p)
{
  EQuad3DWCRec *l;

  l = (EQuad3DWCRec *)g_p->spec.data;
  sprintf(string_rep_buffer,
	  "QUAD [%f,%f,%f] [%f,%f,%f] \n"
    "     [%f,%f,%f] [%f,%f,%f]",
	  l->point_1.x,
          l->point_1.y,
          l->point_1.z,
	  l->point_2.x,
          l->point_2.y,
          l->point_2.z, 
	  l->point_3.x,
          l->point_3.y,
          l->point_3.z,
          l->point_4.x,
          l->point_4.y,
          l->point_4.z
          );
  
  return string_rep_buffer; /* Don't free this memory!!! */
}



static void 
RetrieveFrom(FILE *file_p, GraphicSpecObj *gsp)
{
  EQuad3DWCRec *p;
  int filled, edge_flag;
  char color[64], edge_color[64];
  EPixel pixel;
  BOOLEAN success;
  double x1, y1, z1, x2, y2, z2, x3,  y3, z3, x4, y4, z4;
  float shrink;
  
  if ((p = (EQuad3DWCRec *)make_node(sizeof(EQuad3DWCRec)))
      != NULL) {
    fscanf(file_p,
	   "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
	   &x1, &y1, &z1, &x2, &y2, &z2, &x3, &y3, &z3, &x4, &y4, &z4);
    fscanf(file_p,
	   "%d %d %f %d",
	   &(p->attributes.spec.have_any),
	   &filled, &shrink, &edge_flag);
    fscanf(file_p, "%s", edge_color);
    fscanf(file_p, "%s", color);
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
    p->attributes.spec.fill_style = filled;
    p->attributes.spec.shrink     = shrink;
    p->attributes.spec.edge_flag  = edge_flag;
    pixel = ColorGetPixelFromString(edge_color, &success);
    if (success)
      p->attributes.spec.edge_color = pixel;
    pixel = ColorGetPixelFromString(color, &success);
    if (success)
      p->attributes.spec.color = pixel;
  }
  gsp->data        = (caddr_t)p;
  gsp->data_length = sizeof(EQuad3DWCRec);
}
 


static void 
StoreOn(FILE *file_p, GraphicObj *g_p)
{
  EQuad3DWCRec *p;
  char *color, *edge_color;

  p = (EQuad3DWCRec *)g_p->spec.data;
  if (p->attributes.spec.have_any == YES) {
    color =
      ColorGetStringFromPixel(p->attributes.spec.color);
    if (color == NULL)
      color = "white";
    edge_color = ColorGetStringFromPixel(p->attributes.spec.edge_color);
    if (edge_color == NULL)
      edge_color = "white";
  } else {
    edge_color = color = "white";
  }
  fprintf(file_p,
	  "%f %f %f %f %f %f %f %f %f %f %f %f\n %d %d %f %d\n%s\n%s\n",
	  p->point_1.x,
          p->point_1.y,
          p->point_1.z,
	  p->point_2.x,
          p->point_2.y,
          p->point_2.z, 
	  p->point_3.x,
          p->point_3.y,
          p->point_3.z,
          p->point_4.x,
          p->point_4.y,
          p->point_4.z,
	  (p->attributes.spec.have_any),
	  p->attributes.spec.fill_style,
	  p->attributes.spec.shrink,
          p->attributes.spec.edge_flag,
          edge_color,
	  color);
}


                                /* <<<<<>>>>> */



static void 
BoundingBoxVC(EView *v_p, GraphicObj *g_p,
                      VCRec *ll_return, VCRec *ur_return)
{
  EQuad3DWCRec *p;
  FPNum tmp1, tmp2;
  VCRec p1, p2, p3, p4;
  
  p = (EQuad3DWCRec *)(g_p->spec.data);
  EVWCtoVC(v_p, &(p->point_1), &p1);
  EVWCtoVC(v_p, &(p->point_2), &p2);
  EVWCtoVC(v_p, &(p->point_3), &p3);
  EVWCtoVC(v_p, &(p->point_4), &p4);
  
#define THE_MIN(FOR, F, S) min(F.FOR, S.FOR)
#define THE_MAX(FOR, F, S) max(F.FOR, S.FOR)

  tmp1 = THE_MIN(u, p1, p2);
  tmp2 = THE_MIN(u, p3, p4);
  ll_return->u = min(tmp1, tmp2);
  tmp1 = THE_MIN(v, p1, p2);
  tmp2 = THE_MIN(v, p3, p4);
  ll_return->v = min(tmp1, tmp2);
  tmp1 = THE_MIN(n, p1, p2);
  tmp2 = THE_MIN(n, p3, p4);
  ll_return->n = min(tmp1, tmp2);

  tmp1 = THE_MAX(u, p1, p2);
  tmp2 = THE_MAX(u, p3, p4);
  ur_return->u = max(tmp1, tmp2);
  tmp1 = THE_MAX(v, p1, p2);
  tmp2 = THE_MAX(v, p3, p4);
  ur_return->v = max(tmp1, tmp2);
  tmp1 = THE_MAX(n, p1, p2);
  tmp2 = THE_MAX(n, p3, p4);
  ur_return->n = max(tmp1, tmp2);
}



static void 
BoundingBoxWC(GraphicObj *g_p,
                      WCRec *ll_return, WCRec *ur_return)
{
  EQuad3DWCRec *p = (EQuad3DWCRec *)(g_p->spec.data);
  FPNum tmp1, tmp2;
  
#define THE_MIN(FOR, F, S) min(F.FOR, S.FOR)
#define THE_MAX(FOR, F, S) max(F.FOR, S.FOR)

  tmp1 = THE_MIN(x, p->point_1, p->point_2);
  tmp2 = THE_MIN(x, p->point_3, p->point_4);
  ll_return->x = min(tmp1, tmp2);
  tmp1 = THE_MIN(y, p->point_1, p->point_2);
  tmp2 = THE_MIN(y, p->point_3, p->point_4);
  ll_return->y = min(tmp1, tmp2);
  tmp1 = THE_MIN(z, p->point_1, p->point_2);
  tmp2 = THE_MIN(z, p->point_3, p->point_4);
  ll_return->z = min(tmp1, tmp2);

  tmp1 = THE_MAX(x, p->point_1, p->point_2);
  tmp2 = THE_MAX(x, p->point_3, p->point_4);
  ur_return->x = max(tmp1, tmp2);
  tmp1 = THE_MAX(y, p->point_1, p->point_2);
  tmp2 = THE_MAX(y, p->point_3, p->point_4);
  ur_return->y = max(tmp1, tmp2);
  tmp1 = THE_MAX(z, p->point_1, p->point_2);
  tmp2 = THE_MAX(z, p->point_3, p->point_4);
  ur_return->z = max(tmp1, tmp2);
}




static void 
XorDraw(EView *v_p, GraphicObj *g_p)
{
  EQuad3DWCRec *thedp = (EQuad3DWCRec *)(g_p->spec.data);

  EVQuad3DPrimitive(v_p,g_p, 
                    &thedp->point_1,
                    &thedp->point_2,
                    &thedp->point_3,
                    &thedp->point_4,
                    thedp->attributes.spec.color,
                    FILL_HOLLOW, NO, 0, 1, DRAW_MODE_XORDRAW);
}

 


static GraphicObj *
ModifyGeometry(GraphicObj *g_p, WCRec *points)
{
  EQuad3DWCRec *thedp = (EQuad3DWCRec *)(g_p->spec.data);

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

  return g_p;
}



static void 
Hilite(EView *v_p, GraphicObj *g_p)
{
  EQuad3DWCRec *thedp = (EQuad3DWCRec *)(g_p->spec.data);

  EVQuad3DPrimitive(v_p, g_p, 
                    &thedp->point_1,
                    &thedp->point_2,
                    &thedp->point_3,
                    &thedp->point_4,
                    thedp->attributes.spec.color,
                    FILL_HOLLOW, NO, 0, 1, DRAW_MODE_HILITE);
}



static void 
Unhilite(EView *v_p, GraphicObj *g_p)
{
  EQuad3DWCRec *thedp = (EQuad3DWCRec *)(g_p->spec.data);

  EVQuad3DPrimitive(v_p, g_p, 
                    &thedp->point_1,
                    &thedp->point_2,
                    &thedp->point_3,
                    &thedp->point_4,
                    thedp->attributes.spec.color,
                    FILL_HOLLOW, NO, 0, 1, DRAW_MODE_UNHILITE);
}



static void 
Erase(EView *v_p, GraphicObj *g_p)
{
  EQuad3DWCRec *thedp = (EQuad3DWCRec *)(g_p->spec.data);

  EVQuad3DPrimitive(v_p, g_p, 
                    &thedp->point_1,
                    &thedp->point_2,
                    &thedp->point_3,
                    &thedp->point_4,
                    thedp->attributes.spec.color,
                    thedp->attributes.spec.fill_style, NO, 0,
                    thedp->attributes.spec.shrink,
                    DRAW_MODE_ERASE);
}



static GraphicObj *
Translate(GraphicObj *g_p, WCRec *by)
{
  EQuad3DWCRec *p;

  p = (EQuad3DWCRec *)g_p->spec.data;
  p->point_1.x += by->x, p->point_1.y += by->y, p->point_1.z += by->z;
  p->point_2.x += by->x, p->point_2.y += by->y, p->point_2.z += by->z;
  p->point_3.x += by->x, p->point_3.y += by->y, p->point_3.z += by->z;
  p->point_4.x += by->x, p->point_4.y += by->y, p->point_4.z += by->z;

  return g_p;
}



static GraphicObj *
Rotate(GraphicObj *g_p, WCRec *center, WCRec *axial)
{
  EQuad3DWCRec *p = (EQuad3DWCRec *)g_p->spec.data;
  WCRec x_c;

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
  return g_p;
}



static GraphicObj  *
MirrorPlane(GraphicObj *g_p,
                    WCRec *point_on_plane, WCRec *unit_plane_normal)
{
  EQuad3DWCRec *dp;

  dp = (EQuad3DWCRec *)g_p->spec.data;
  MirrorPointInPlane(point_on_plane, unit_plane_normal, &dp->point_1);
  MirrorPointInPlane(point_on_plane, unit_plane_normal, &dp->point_2);
  MirrorPointInPlane(point_on_plane, unit_plane_normal, &dp->point_3);
  MirrorPointInPlane(point_on_plane, unit_plane_normal, &dp->point_4);

  return g_p;
}



static GraphicObj  *
MirrorCenter(GraphicObj *g_p, WCRec *center)
{
  EQuad3DWCRec *dp;

  dp = (EQuad3DWCRec *)g_p->spec.data;
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
  return g_p;
}


 
static GraphicObj *
Scale(GraphicObj *g_p, WCRec *center, 
		   FPNum sx, FPNum sy, FPNum sz)
{
  EQuad3DWCRec *dp;

  dp = (EQuad3DWCRec *)g_p->spec.data;
  dp->point_1.x = center->x + (dp->point_1.x - center->x) * sx;
  dp->point_1.y = center->y + (dp->point_1.y - center->y) * sy;
  dp->point_1.z = center->z + (dp->point_1.z - center->z) * sz;
  dp->point_2.x = center->x + (dp->point_2.x - center->x) * sx;
  dp->point_2.y = center->y + (dp->point_2.y - center->y) * sy;
  dp->point_2.z = center->z + (dp->point_2.z - center->z) * sz;
  dp->point_3.x = center->x + (dp->point_3.x - center->x) * sx;
  dp->point_3.y = center->y + (dp->point_3.y - center->y) * sy;
  dp->point_3.z = center->z + (dp->point_3.z - center->z) * sz;
  dp->point_4.x = center->x + (dp->point_4.x - center->x) * sx;
  dp->point_4.y = center->y + (dp->point_4.y - center->y) * sy;
  dp->point_4.z = center->z + (dp->point_4.z - center->z) * sz;
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
      VCRec p1, p2, p3, p4;
      EQuad3DWCRec *p;
      
      p = (EQuad3DWCRec *)g_p->spec.data;
      EVWCtoVC(v_p, &(p->point_1), &p1);
      EVWCtoVC(v_p, &(p->point_2), &p2);
      EVWCtoVC(v_p, &(p->point_3), &p3);
      EVWCtoVC(v_p, &(p->point_4), &p4);
      return (BoxOverlapsTriangle(box,
                                  (VC2DRec *)&p1,
                                  (VC2DRec *)&p2,
                                  (VC2DRec *)&p3)
              ||
              BoxOverlapsTriangle(box,
                                  (VC2DRec *)&p1,
                                  (VC2DRec *)&p4,
                                  (VC2DRec *)&p3));
    }
    break;
  case INTERSECT:
  default:
    {
      VCRec p1, p2, p3, p4;
      EQuad3DWCRec *p;
      
      p = (EQuad3DWCRec *)g_p->spec.data;
      EVWCtoVC(v_p, &(p->point_1), &p1);
      EVWCtoVC(v_p, &(p->point_2), &p2);
      EVWCtoVC(v_p, &(p->point_3), &p3);
      EVWCtoVC(v_p, &(p->point_4), &p4);
      return (BoxIntersectsTriangle(box,
                                    (VC2DRec *)&p1,
                                    (VC2DRec *)&p2,
                                    (VC2DRec *)&p3)
              ||
              BoxIntersectsTriangle(box,
                                    (VC2DRec *)&p1,
                                    (VC2DRec *)&p4,
                                    (VC2DRec *)&p3));
    }
    break;
  }
}



static LIST 
KeyPoints(GraphicObj *p)
{
  LIST temp_list;
  KeyPointRec *key_p;
  EQuad3DWCRec *l_p;

  l_p       = (EQuad3DWCRec *)(p->spec.data);

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
  return temp_list;
}



static GraphicObj *
Reshape(GraphicObj *p, EHandleNum hn, WCRec *pos)
{
  EQuad3DWCRec *_p;

  _p = (EQuad3DWCRec *)(p->spec.data);
  
  if        (hn == 1) {
    _p->point_1.x = pos->x, _p->point_1.y = pos->y, _p->point_1.z = pos->z;
  } else if (hn == 2) {
    _p->point_2.x = pos->x, _p->point_2.y = pos->y, _p->point_2.z = pos->z;
  } else if (hn == 3) {
    _p->point_3.x = pos->x, _p->point_3.y = pos->y, _p->point_3.z = pos->z;
  } else if (hn == 4) {
    _p->point_4.x = pos->x, _p->point_4.y = pos->y, _p->point_4.z = pos->z;  
  }
  return p;
}



static EHandleNum 
ReshapeHandle(EView *v_p, GraphicObj *p, WCRec *nearp)
{
  EQuad3DWCRec *l_p;
  FPNum dist1, dist2, dist3, dist4, tmp1, tmp2;
  VCRec nvc, pvc;

  l_p       = (EQuad3DWCRec *)(p->spec.data);
  EVWCtoVC(v_p, nearp, &nvc);
  EVWCtoVC(v_p, &(l_p->point_1), &pvc);
  dist1 = DistOfPnts(pvc.u, pvc.v, nvc.u, nvc.v);
  EVWCtoVC(v_p, &(l_p->point_2), &pvc);
  dist2 = DistOfPnts(pvc.u, pvc.v, nvc.u, nvc.v);
  EVWCtoVC(v_p, &(l_p->point_3), &pvc);
  dist3 = DistOfPnts(pvc.u, pvc.v, nvc.u, nvc.v);
  EVWCtoVC(v_p, &(l_p->point_4), &pvc);
  dist4 = DistOfPnts(pvc.u, pvc.v, nvc.u, nvc.v);
  tmp1 = min(dist1, dist2);
  tmp2 = min(dist3, dist4);
  tmp1 = min(tmp1, tmp2);
  if (tmp1 == dist1) return (EHandleNum)1;
  if (tmp1 == dist2) return (EHandleNum)2;
  if (tmp1 == dist3) return (EHandleNum)3;
  if (tmp1 == dist4) return (EHandleNum)4;
  return (EHandleNum)1;
}



static void 
ShowHandles(EView *v_p, GraphicObj *p)
{
  EQuad3DWCRec *l_p;

  l_p = (EQuad3DWCRec *)p->spec.data;
  EGDrawHandle(v_p, &(l_p->point_1));
  EGDrawHandle(v_p, &(l_p->point_2));
  EGDrawHandle(v_p, &(l_p->point_3));
  EGDrawHandle(v_p, &(l_p->point_4));
}
  


static void 
EraseHandles(EView *v_p, GraphicObj *p)
{
  EQuad3DWCRec *l_p;

  l_p = (EQuad3DWCRec *)p->spec.data;
  EGEraseHandle(v_p, &(l_p->point_1)); 
  EGEraseHandle(v_p, &(l_p->point_2));
  EGEraseHandle(v_p, &(l_p->point_3));
  EGEraseHandle(v_p, &(l_p->point_4));
}



static void 
XYofHandle(GraphicObj *p, EHandleNum hn, WCRec *pos)
{
  EQuad3DWCRec *l_p;

  l_p = (EQuad3DWCRec *)p->spec.data;
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
  }
}



static void 
ChangeAttributes(GraphicObj *p)
{
  EQuad3DWCRec *dp;
  unsigned long mask;

  dp = (EQuad3DWCRec *)p->spec.data;

  mask = EASValsGetChangeMask();
  
  if ((mask & FILL_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.fill_style = EASValsGetFillStyle();
    dp->attributes.spec.have_any = YES;
  }
  if ((mask & COLOR_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.color = EASValsGetColor();
    dp->attributes.spec.have_any = YES;
  }
  if ((mask & EDGE_FLAG_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.edge_flag = EASValsGetEdgeFlag();
    dp->attributes.spec.have_any = YES;
  }
  if ((mask & EDGE_COLOR_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.edge_color = EASValsGetEdgeColor();
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
  EQuad3DWCRec *dp;
 
  dp = (EQuad3DWCRec *)p->spec.data;
  
  if (dp->attributes.spec.have_any) {
    EASValsSetFillStyle(dp->attributes.spec.fill_style);
    EASValsSetColor(dp->attributes.spec.color);
    EASValsSetShrink(dp->attributes.spec.shrink);
  }
}

static BOOLEAN
AttribValAsInAS(GraphicObj *p, unsigned long mask)  
{
  EQuad3DWCRec *dp;

  dp = (EQuad3DWCRec *)p->spec.data;
  
  if ((mask & FILL_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.fill_style != EASValsGetFillStyle())
      return NO;
  if ((mask & COLOR_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.color != EASValsGetColor())
      return NO;
  if ((mask & SHRINK_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.shrink != EASValsGetShrink())
      return NO;

  return YES;
}



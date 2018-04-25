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

/* This file holds the redefinition of graphic methods for the triangle */
/* with data. */

#include "Econfig.h"
#include <stdlib.h>

#include "Eview.h"
#include "Egraphic.h"
#include "Egeomops.h"
#include "Ecolors.h"
#include "Eactset.h"

#include "Eline3d.h"
#define TRIANGLE3D_PRIVATE_HEADER
#include "Etria3d.h"

/* ========================================================================= */
/* PRIVATE DATA */
/* ========================================================================= */

#define STRING_BUFF_SIZE 132

static char string_rep_buffer[STRING_BUFF_SIZE];

/* ========================================================================= */
/* PUBLIC */
/* ========================================================================= */

GraphicObj *
CreateTriangle3D(WCRec *points)
{
  ETriangle3DWCRec *thedp;
  GraphicObj *g_p;

  thedp = (ETriangle3DWCRec *)make_node(sizeof(ETriangle3DWCRec));
  g_p = EGCreateGraphics(&triangle_3d_graphic_methods, (caddr_t)thedp,
			 sizeof(ETriangle3DWCRec));
  ModifyGeometry(g_p, points);
  thedp->attributes.spec.fill_style = FILL_HOLLOW;
  thedp->attributes.spec.edge_flag  = NO;
  thedp->attributes.spec.edge_color = 0x0; 
  thedp->attributes.spec.color      = 0x0;
  thedp->attributes.spec.shrink     = 1.0;
  thedp->attributes.spec.have_any   = YES;
  return g_p;
}



static void 
Draw(EView *v_p, GraphicObj *p)
{
  ETriangle3DWCRec *thedp;

  thedp = (ETriangle3DWCRec *)(p->spec.data);

  EVTriangle3DPrimitive(v_p, p, 
			&thedp->point_1, &thedp->point_2, &thedp->point_3,
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
  return EG_TRIANGLE3D;
}



static char *
AsString(GraphicObj *g_p)
{
  ETriangle3DWCRec *l;

  l = (ETriangle3DWCRec *)g_p->spec.data;
  sprintf(string_rep_buffer,
	  "TRIANGLE [%f,%f,%f] [%f,%f,%f] [%f,%f,%f]",
	  l->point_1.x, l->point_1.y, l->point_1.z,
	  l->point_2.x, l->point_2.y, l->point_2.z, 
	  l->point_3.x, l->point_3.y, l->point_3.z);
  
  return string_rep_buffer; /* Don't free this memory!!! */
}



static void 
RetrieveFrom(FILE *file_p, GraphicSpecObj *gsp)
{
  ETriangle3DWCRec *p;
  int filled, edge_flag;
  char color[64];
  EPixel pixel;
  BOOLEAN success;
  double x1, y1, z1, x2, y2, z2, x3,  y3, z3;
  float shrink;
  
  if ((p = (ETriangle3DWCRec *)make_node(sizeof(ETriangle3DWCRec)))
      != NULL) {
    fscanf(file_p,
	   "%lf %lf %lf %lf %lf %lf %lf %lf %lf ",
	   &x1, &y1, &z1, &x2, &y2, &z2, &x3,  &y3, &z3);
    p->point_1.x = x1;
    p->point_1.y = y1;
    p->point_1.z = z1;
    p->point_2.x = x2;
    p->point_2.y = y2;
    p->point_2.z = z2;
    p->point_3.x = x3;
    p->point_3.y = y3;
    p->point_3.z = z3;
    fscanf(file_p, "%d %d %d %f",
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
  gsp->data_length = sizeof(ETriangle3DWCRec);
}
 


static void 
StoreOn(FILE *file_p, GraphicObj *g_p)
{
  ETriangle3DWCRec *p;
  char *color;
  char *edge_color;

  p = (ETriangle3DWCRec *)g_p->spec.data;
  if (p->attributes.spec.have_any == YES) {
    color = ColorGetStringFromPixel(p->attributes.spec.color);
    if (color == NULL)
      color = "white";
    edge_color = ColorGetStringFromPixel(p->attributes.spec.edge_color);
    if (edge_color == NULL)
      edge_color = "white";
  } else {
    color = "white";
    edge_color = "white";
  }
  fprintf(file_p, "%f %f %f %f %f %f %f %f %f\n",
	  p->point_1.x, p->point_1.y, p->point_1.z,
	  p->point_2.x, p->point_2.y, p->point_2.z, 
	  p->point_3.x, p->point_3.y, p->point_3.z);
  fprintf(file_p, "%d %d %d %f\n",
	  (p->attributes.spec.have_any),
	  p->attributes.spec.fill_style,
	  p->attributes.spec.edge_flag,
	  p->attributes.spec.shrink);
  fprintf(file_p, "%s\n%s\n", color, edge_color);
}




static void 
BoundingBoxVC(EView *v_p, GraphicObj *g_p,
			     VCRec *ll_return, VCRec *ur_return)
{
  ETriangle3DWCRec *p;
  FPNum tmp;
  VCRec p1, p2, p3;

  
  p = (ETriangle3DWCRec *)(g_p->spec.data);
  EVWCtoVC(v_p, &(p->point_1), &p1);
  EVWCtoVC(v_p, &(p->point_2), &p2);
  EVWCtoVC(v_p, &(p->point_3), &p3);
  
  tmp = min(p1.u, p2.u); ll_return->u = min(tmp, p3.u);
  
  tmp = min(p1.v, p2.v); ll_return->v = min(tmp, p3.v);
  
  tmp = min(p1.n, p2.n); ll_return->n = min(tmp, p3.n);
  
  tmp = max(p1.u, p2.u); ur_return->u = max(tmp, p3.u);
  
  tmp = max(p1.v, p2.v); ur_return->v = max(tmp, p3.v);
  
  tmp = max(p1.n, p2.n); ur_return->n = max(tmp, p3.n);
}



static void 
BoundingBoxWC(GraphicObj *g_p,
			     WCRec *ll_return, WCRec *ur_return)
{
  ETriangle3DWCRec *p;
  FPNum tmp;
  
  p = (ETriangle3DWCRec *)(g_p->spec.data);
  tmp          = min(p->point_1.x, p->point_2.x);
  ll_return->x = min(tmp, p->point_3.x);
  tmp          = min(p->point_1.y, p->point_2.y);
  ll_return->y = min(tmp, p->point_3.y);
  tmp          = min(p->point_1.z, p->point_2.z);
  ll_return->z = min(tmp, p->point_3.z);

  tmp          = max(p->point_1.x, p->point_2.x);
  ur_return->x = max(tmp, p->point_3.x);
  tmp          = max(p->point_1.y, p->point_2.y);
  ur_return->y = max(tmp, p->point_3.y);
  tmp          = max(p->point_1.z, p->point_2.z);
  ur_return->z = max(tmp, p->point_3.z);
}




static void 
XorDraw(EView *v_p, GraphicObj *g_p)
{
  ETriangle3DWCRec *thedp;

  thedp = (ETriangle3DWCRec *)(g_p->spec.data);

  EVTriangle3DPrimitive(v_p, g_p,
			&thedp->point_1, &thedp->point_2, &thedp->point_3,
			thedp->attributes.spec.color,
			thedp->attributes.spec.fill_style,
                        NO, 0,
			thedp->attributes.spec.shrink,
			DRAW_MODE_XORDRAW);
}

 


static GraphicObj *
ModifyGeometry(GraphicObj *g_p, WCRec *points)
{
  ETriangle3DWCRec *thedp;

  thedp = (ETriangle3DWCRec *)(g_p->spec.data);
  thedp->point_1.x = points[0].x;
  thedp->point_1.y = points[0].y;
  thedp->point_1.z = points[0].z;
  thedp->point_2.x = points[1].x;
  thedp->point_2.y = points[1].y;
  thedp->point_2.z = points[1].z;
  thedp->point_3.x = points[2].x;
  thedp->point_3.y = points[2].y;
  thedp->point_3.z = points[2].z;

  return g_p;
}



static BOOLEAN 
GetGeometry(GraphicObj *g_p, WCRec *points,
                              int *npoints)
{
  ETriangle3DWCRec *thedp;

  thedp = (ETriangle3DWCRec *)(g_p->spec.data);
  points[0].x = thedp->point_1.x;
  points[0].y = thedp->point_1.y;
  points[0].z = thedp->point_1.z;
  points[1].x = thedp->point_2.x;
  points[1].y = thedp->point_2.y;
  points[1].z = thedp->point_2.z;
  points[2].x = thedp->point_3.x;
  points[2].y = thedp->point_3.y;
  points[2].z = thedp->point_3.z;
  *npoints    = 3;
  
  return YES;
}



static void 
Hilite(EView *v_p, GraphicObj *g_p)
{
  ETriangle3DWCRec *thedp;
  
  thedp = (ETriangle3DWCRec *)(g_p->spec.data);

  EVTriangle3DPrimitive(v_p, g_p,
			&thedp->point_1, &thedp->point_2, &thedp->point_3,
			thedp->attributes.spec.color,
			FILL_HOLLOW, NO, 0,
			1.0,
			DRAW_MODE_HILITE);
}



static void 
Unhilite(EView *v_p, GraphicObj *g_p)
{
  ETriangle3DWCRec *thedp;
  
  thedp = (ETriangle3DWCRec *)(g_p->spec.data);

  EVTriangle3DPrimitive(v_p, g_p, 
			&thedp->point_1, &thedp->point_2, &thedp->point_3,
			thedp->attributes.spec.color,
			FILL_HOLLOW, NO, 0,
			1.0,
			DRAW_MODE_UNHILITE);
}



static void 
Erase(EView *v_p, GraphicObj *g_p)
{
  ETriangle3DWCRec *thedp;
  
  thedp = (ETriangle3DWCRec *)(g_p->spec.data);

  EVTriangle3DPrimitive(v_p, g_p,
			&thedp->point_1, &thedp->point_2, &thedp->point_3,
			thedp->attributes.spec.color,
			thedp->attributes.spec.fill_style,
                        NO, 0,
			thedp->attributes.spec.shrink,
			DRAW_MODE_ERASE);
}



static GraphicObj *
Translate(GraphicObj *g_p, WCRec *by)
{
  ETriangle3DWCRec *p;

  p = (ETriangle3DWCRec *)g_p->spec.data;
  p->point_1.x += by->x, p->point_1.y += by->y, p->point_1.z += by->z;
  p->point_2.x += by->x, p->point_2.y += by->y, p->point_2.z += by->z;
  p->point_3.x += by->x, p->point_3.y += by->y, p->point_3.z += by->z;

  return g_p;
}



static GraphicObj *
Rotate(GraphicObj *g_p, WCRec *center, WCRec *axial)
{
  ETriangle3DWCRec *p;
  WCRec x_c;

  p = (ETriangle3DWCRec *)g_p->spec.data;
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
  return g_p;
}



static GraphicObj  *
MirrorPlane(GraphicObj *g_p,
                      WCRec *point_on_plane, WCRec *unit_plane_normal)
{
  ETriangle3DWCRec *dp;

  dp = (ETriangle3DWCRec *)g_p->spec.data;
  MirrorPointInPlane(point_on_plane, unit_plane_normal, &dp->point_1);
  MirrorPointInPlane(point_on_plane, unit_plane_normal, &dp->point_2);
  MirrorPointInPlane(point_on_plane, unit_plane_normal, &dp->point_3);

  return g_p;
}



static GraphicObj  *
MirrorCenter(GraphicObj *g_p, WCRec *center)
{
  ETriangle3DWCRec *dp;

  dp = (ETriangle3DWCRec *)g_p->spec.data;
  dp->point_1.x = 2.*center->x - dp->point_1.x;
  dp->point_1.y = 2.*center->y - dp->point_1.y;
  dp->point_1.z = 2.*center->z - dp->point_1.z;
  dp->point_2.x = 2.*center->x - dp->point_2.x;
  dp->point_2.y = 2.*center->y - dp->point_2.y;
  dp->point_2.z = 2.*center->z - dp->point_2.z;
  dp->point_3.x = 2.*center->x - dp->point_3.x;
  dp->point_3.y = 2.*center->y - dp->point_3.y;
  dp->point_3.z = 2.*center->z - dp->point_3.z;
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
      VCRec p1, p2, p3;
      ETriangle3DWCRec *p;
      
      p = (ETriangle3DWCRec *)g_p->spec.data;
      EVWCtoVC(v_p, &(p->point_1), &p1);
      EVWCtoVC(v_p, &(p->point_2), &p2);
      EVWCtoVC(v_p, &(p->point_3), &p3);
      return BoxOverlapsTriangle(box,
				 (VC2DRec *)&p1,
				 (VC2DRec *)&p2,
				 (VC2DRec *)&p3);
    }
    break;
  case INTERSECT:
  default:
    {
      VCRec p1, p2, p3;
      ETriangle3DWCRec *p;
      
      p = (ETriangle3DWCRec *)g_p->spec.data;
      EVWCtoVC(v_p, &(p->point_1), &p1);
      EVWCtoVC(v_p, &(p->point_2), &p2);
      EVWCtoVC(v_p, &(p->point_3), &p3);
      return BoxIntersectsTriangle(box,
				   (VC2DRec *)&p1,
				   (VC2DRec *)&p2,
				   (VC2DRec *)&p3);
    }
    break;
  }
}



static LIST 
KeyPoints(GraphicObj *p)
{
  LIST temp_list;
  KeyPointRec *key_p;
  ETriangle3DWCRec *l_p;

  l_p       = (ETriangle3DWCRec *)(p->spec.data);

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
  return temp_list;
}



static GraphicObj *
Reshape(GraphicObj *p, EHandleNum hn, WCRec *pos)
{
  ETriangle3DWCRec *_p;

  _p = (ETriangle3DWCRec *)(p->spec.data);
  
  if        (hn == 1) {
    _p->point_1.x = pos->x, _p->point_1.y = pos->y, _p->point_1.z = pos->z;
  } else if (hn == 2) {
    _p->point_2.x = pos->x, _p->point_2.y = pos->y, _p->point_2.z = pos->z;
  } else if (hn == 3) {
    _p->point_3.x = pos->x, _p->point_3.y = pos->y, _p->point_3.z = pos->z;
  }
  return p;
}



static EHandleNum 
ReshapeHandle(EView *v_p, GraphicObj *p, WCRec *nearp)
{
  ETriangle3DWCRec *l_p;
  FPNum dist1, dist2, dist3;
  VCRec nvc, pvc;

  l_p       = (ETriangle3DWCRec *)(p->spec.data);
  EVWCtoVC(v_p, nearp, &nvc);
  EVWCtoVC(v_p, &(l_p->point_1), &pvc);
  dist1 = DistOfPnts(pvc.u, pvc.v, nvc.u, nvc.v);
  EVWCtoVC(v_p, &(l_p->point_2), &pvc);
  dist2 = DistOfPnts(pvc.u, pvc.v, nvc.u, nvc.v);
  EVWCtoVC(v_p, &(l_p->point_3), &pvc);
  dist3 = DistOfPnts(pvc.u, pvc.v, nvc.u, nvc.v);
  if (dist1 < dist2) {
    if (dist1 < dist3)
      return (EHandleNum)1;
    else
      return (EHandleNum)3;
  } else {
    if (dist2 < dist3)
      return (EHandleNum)2;
    else
      return (EHandleNum)3;
  }
}



static void 
ShowHandles(EView *v_p, GraphicObj *p)
{
  ETriangle3DWCRec *l_p;

  l_p = (ETriangle3DWCRec *)p->spec.data;
  EGDrawHandle(v_p, &(l_p->point_1));
  EGDrawHandle(v_p, &(l_p->point_2));
  EGDrawHandle(v_p, &(l_p->point_3));
}
  


static void 
EraseHandles(EView *v_p, GraphicObj *p)
{
  ETriangle3DWCRec *l_p;

  l_p = (ETriangle3DWCRec *)p->spec.data;
  EGEraseHandle(v_p, &(l_p->point_1)); 
  EGEraseHandle(v_p, &(l_p->point_2));
  EGEraseHandle(v_p, &(l_p->point_3));
}



static void 
XYofHandle(GraphicObj *p, EHandleNum hn, WCRec *pos)
{
  ETriangle3DWCRec *l_p;

  l_p = (ETriangle3DWCRec *)p->spec.data;
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
  }
}



static void 
ChangeAttributes(GraphicObj *p)
{
  ETriangle3DWCRec *dp;
  unsigned long mask;

  dp = (ETriangle3DWCRec *)p->spec.data;

  mask = EASValsGetChangeMask();
  
  if ((mask & FILL_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.fill_style = EASValsGetFillStyle();
    dp->attributes.spec.have_any = YES;
  }
  if ((mask & COLOR_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.color = EASValsGetColor();
    dp->attributes.spec.have_any = YES;
  }
  if ((mask & SHRINK_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.shrink = EASValsGetShrink();
    dp->attributes.spec.have_any = YES;
  }
  if ((mask & EDGE_COLOR_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.edge_color = EASValsGetEdgeColor();
    dp->attributes.spec.have_any = YES;
  }
  if ((mask & EDGE_FLAG_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.edge_flag  = EASValsGetEdgeFlag();
    dp->attributes.spec.have_any = YES;
  }
}



static void 
CopyAttributesToActiveSet(GraphicObj *p)
{
  ETriangle3DWCRec *dp;

  dp = (ETriangle3DWCRec *)p->spec.data;
  
  if (dp->attributes.spec.have_any) {
    EASValsSetFillStyle(dp->attributes.spec.fill_style);
    EASValsSetColor(dp->attributes.spec.color);
    EASValsSetShrink(dp->attributes.spec.shrink);
    EASValsSetEdgeColor(dp->attributes.spec.edge_color);
    EASValsSetEdgeFlag(dp->attributes.spec.edge_flag);
  }
}


static BOOLEAN
AttribValAsInAS(GraphicObj *p, unsigned long mask)  
{
  ETriangle3DWCRec *dp;

  dp = (ETriangle3DWCRec *)p->spec.data;
  
  if ((mask & FILL_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.fill_style != EASValsGetFillStyle())
      return NO;
  if ((mask & COLOR_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.color != EASValsGetColor())
      return NO;
  if ((mask & SHRINK_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.shrink != EASValsGetShrink())
      return NO;
  if ((mask & EDGE_COLOR_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.edge_color != EASValsGetEdgeColor())
      return NO;
  if ((mask & EDGE_FLAG_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.edge_flag != EASValsGetEdgeFlag())
      return NO;

  return YES;
}

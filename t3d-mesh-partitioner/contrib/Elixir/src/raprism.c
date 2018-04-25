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

#define RAPRISM_PRIVATE_HEADER
#include "Eraprism.h"

/* ========================================================================= */
/* PRIVATE DATA */
/* ========================================================================= */

#define STRING_BUFF_SIZE 256

static char string_rep_buffer[STRING_BUFF_SIZE];

#define ASSIGN_POINT_TO_POINT(DEST, SRC)                         \
  {                                                              \
     (DEST).x = (SRC).x; (DEST).y = (SRC).y; (DEST).z = (SRC).z; \
   }

static void
check_corners(GraphicObj *p);

static void
check_corners(GraphicObj *g_p)
{
  WCRec ll, ur;
  ERAPrismWCRec *thedp = (ERAPrismWCRec *)(g_p->spec.data);

  ll.x = min(thedp->lower_left_corner.x, thedp->upper_right_corner.x);
  ll.y = min(thedp->lower_left_corner.y, thedp->upper_right_corner.y);
  ll.z = min(thedp->lower_left_corner.z, thedp->upper_right_corner.z);
  ur.x = max(thedp->lower_left_corner.x, thedp->upper_right_corner.x);
  ur.y = max(thedp->lower_left_corner.y, thedp->upper_right_corner.y);
  ur.z = max(thedp->lower_left_corner.z, thedp->upper_right_corner.z);
  ASSIGN_POINT_TO_POINT(thedp->lower_left_corner, ll);
  ASSIGN_POINT_TO_POINT(thedp->upper_right_corner, ur);
}


/* ========================================================================= */
/* PUBLIC */
/* ========================================================================= */

GraphicObj *
CreateRAPrism(WCRec *points)   /* expects lower-left and upper-right corner */
{
  ERAPrismWCRec *thedp;
  GraphicObj *g_p;
 
  thedp = (ERAPrismWCRec *)make_node(sizeof(ERAPrismWCRec));
  g_p = EGCreateGraphics(&raprism_graphic_methods, (caddr_t)thedp,
			 sizeof(ERAPrismWCRec));
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
  ERAPrismWCRec *thedp;

  thedp = (ERAPrismWCRec *)(p->spec.data);

  EVRAPrismPrimitive(v_p, p,
                     &thedp->lower_left_corner, &thedp->upper_right_corner,
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
  return EG_RIGHT_ANGLE_PRISM;
}



static char *
AsString(GraphicObj *g_p)
{
  ERAPrismWCRec *l;

  l = (ERAPrismWCRec *)g_p->spec.data;
  sprintf(string_rep_buffer,
	  "RIGHT_ANGLE_PRISM [%f,%f,%f] [%f,%f,%f]",
	  l->lower_left_corner.x,
          l->lower_left_corner.y,
          l->lower_left_corner.z,
          l->upper_right_corner.x,
          l->upper_right_corner.y,
          l->upper_right_corner.z);
  
  return string_rep_buffer; /* Don't free this memory!!! */
}



static void 
RetrieveFrom(FILE *file_p, GraphicSpecObj *gsp)
{
  ERAPrismWCRec *p;
  int filled, edge_flag;
  char color[64];
  EPixel pixel;
  BOOLEAN success;
  double x1, y1, z1, x2, y2, z2;
  float shrink;
  
  if ((p = (ERAPrismWCRec *)make_node(sizeof(ERAPrismWCRec)))
      != NULL) {
    fscanf(file_p,
	   "%lf %lf %lf %lf %lf %lf",
	   &x1, &y1, &z1, &x2, &y2, &z2);
    p->lower_left_corner.x = x1;
    p->lower_left_corner.y = y1;
    p->lower_left_corner.z = z1;
    p->upper_right_corner.x = x2;
    p->upper_right_corner.y = y2;
    p->upper_right_corner.z = z2;
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
  gsp->data_length = sizeof(ERAPrismWCRec);
}
 


static void 
StoreOn(FILE *file_p, GraphicObj *g_p)
{
  ERAPrismWCRec *p;
  char *color, *edge_color;

  p = (ERAPrismWCRec *)g_p->spec.data;
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
	  "%f %f %f %f %f %f\n%d %d %d %f\n%s\n%s\n",
	  p->lower_left_corner.x,
          p->lower_left_corner.y,
          p->lower_left_corner.z,
          p->upper_right_corner.x,
          p->upper_right_corner.y,
          p->upper_right_corner.z,
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
  ERAPrismWCRec *p;
  WCRec wcp;
  VCRec pts[8];
  double a, b, c;
  int i;
  
  p = (ERAPrismWCRec *)(g_p->spec.data);
  a = p->upper_right_corner.x - p->lower_left_corner.x;
  b = p->upper_right_corner.y - p->lower_left_corner.y;
  c = p->upper_right_corner.z - p->lower_left_corner.z;
  EVWCtoVC(v_p, &(p->lower_left_corner), &pts[0]);
  EVWCtoVC(v_p, &(p->upper_right_corner), &pts[1]);
  ASSIGN_POINT_TO_POINT(wcp, p->lower_left_corner);
  wcp.y += b;
  EVWCtoVC(v_p, &wcp, &pts[2]);
  wcp.z += c;
  EVWCtoVC(v_p, &wcp, &pts[4]);
  wcp.y -= b;
  EVWCtoVC(v_p, &wcp, &pts[6]);
  ASSIGN_POINT_TO_POINT(wcp, p->upper_right_corner);
  wcp.y -= b;
  EVWCtoVC(v_p, &wcp, &pts[3]);
  wcp.z -= c;
  EVWCtoVC(v_p, &wcp, &pts[5]);
  wcp.y += b;
  EVWCtoVC(v_p, &wcp, &pts[7]);

  ll_return->u = pts[0].u;
  ll_return->v = pts[0].v;
  ll_return->n = pts[0].n;
  ur_return->u = pts[0].u;
  ur_return->v = pts[0].v;
  ur_return->n = pts[0].n;
  for (i = 1; i < 8; i++) {
    ll_return->u = min(ll_return->u, pts[i].u);
    ur_return->u = max(ur_return->u, pts[i].u);
    ll_return->v = min(ll_return->v, pts[i].v);
    ur_return->v = max(ur_return->v, pts[i].v);
    ll_return->n = min(ll_return->n, pts[i].n);
    ur_return->n = max(ur_return->n, pts[i].n);
  }
}



static void 
BoundingBoxWC(GraphicObj *g_p,
			     WCRec *ll_return, WCRec *ur_return)
{
  ERAPrismWCRec *p;
  
  p = (ERAPrismWCRec *)(g_p->spec.data);

  
  ll_return->x = p->lower_left_corner.x;
  ll_return->y = p->lower_left_corner.y;
  ll_return->z = p->lower_left_corner.z;

  ur_return->x = p->upper_right_corner.x;
  ur_return->y = p->upper_right_corner.y;
  ur_return->z = p->upper_right_corner.z;
}




static void 
XorDraw(EView *v_p, GraphicObj *g_p)
{
  ERAPrismWCRec *thedp;

  thedp = (ERAPrismWCRec *)(g_p->spec.data);

  EVRAPrismPrimitive(v_p, g_p,
                     &thedp->lower_left_corner, &thedp->upper_right_corner,
                     thedp->attributes.spec.color,
                     thedp->attributes.spec.fill_style,
                     NO, 0,
                     thedp->attributes.spec.shrink,
                     DRAW_MODE_XORDRAW);
}

 


static GraphicObj *
ModifyGeometry(GraphicObj *g_p, WCRec *points)
{
  ERAPrismWCRec *thedp;

  thedp = (ERAPrismWCRec *)(g_p->spec.data);
  
  thedp->lower_left_corner.x = points[0].x;
  thedp->lower_left_corner.y = points[0].y;
  thedp->lower_left_corner.z = points[0].z;
  thedp->upper_right_corner.x = points[1].x;
  thedp->upper_right_corner.y = points[1].y;
  thedp->upper_right_corner.z = points[1].z;

  check_corners(g_p);

  return g_p;
}



static BOOLEAN 
GetGeometry(GraphicObj *g_p, WCRec *points, int *npoints)
{
  ERAPrismWCRec *thedp;

  thedp = (ERAPrismWCRec *)(g_p->spec.data);
  points[0].x = thedp->lower_left_corner.x;
  points[0].y = thedp->lower_left_corner.y;
  points[0].z = thedp->lower_left_corner.z;
  points[1].x = thedp->upper_right_corner.x;
  points[1].y = thedp->upper_right_corner.y;
  points[1].z = thedp->upper_right_corner.z;
  *npoints    = 2;
  
  return YES;
}



static void 
Hilite(EView *v_p, GraphicObj *g_p)
{
  ERAPrismWCRec *thedp;
  
  thedp = (ERAPrismWCRec *)(g_p->spec.data);

  EVRAPrismPrimitive(v_p, g_p,
                     &thedp->lower_left_corner,
                     &thedp->upper_right_corner,
                     thedp->attributes.spec.color,
                     FILL_HOLLOW, NO, 0,
                     1.0,
                     DRAW_MODE_HILITE);
}



static void 
Unhilite(EView *v_p, GraphicObj *g_p)
{
  ERAPrismWCRec *thedp;
  
  thedp = (ERAPrismWCRec *)(g_p->spec.data);

  EVRAPrismPrimitive(v_p, g_p,
                     &thedp->lower_left_corner,
                     &thedp->upper_right_corner,
                     thedp->attributes.spec.color,
                     FILL_HOLLOW, NO, 0,
                     1.0,
                     DRAW_MODE_UNHILITE);
}



static void 
Erase(EView *v_p, GraphicObj *g_p)
{
  ERAPrismWCRec *thedp;
  
  thedp = (ERAPrismWCRec *)(g_p->spec.data);

  EVRAPrismPrimitive(v_p, g_p,
                     &thedp->lower_left_corner,
                     &thedp->upper_right_corner,
                     thedp->attributes.spec.color,
                     thedp->attributes.spec.fill_style,
                     NO, 0,
                     thedp->attributes.spec.shrink,
                     DRAW_MODE_ERASE);
}



static GraphicObj *
Translate(GraphicObj *g_p, WCRec *by)
{
  ERAPrismWCRec *p;

  p = (ERAPrismWCRec *)g_p->spec.data;
  p->lower_left_corner.x += by->x;
  p->lower_left_corner.y += by->y;
  p->lower_left_corner.z += by->z;
  p->upper_right_corner.x += by->x;
  p->upper_right_corner.y += by->y;
  p->upper_right_corner.z += by->z;

  return g_p;
}




static GraphicObj  *
MirrorPlane(GraphicObj *g_p, WCRec *point_on_plane,
                   WCRec *unit_plane_normal)
{
  ERAPrismWCRec *dp;

  dp = (ERAPrismWCRec *)g_p->spec.data;
  MirrorPointInPlane(point_on_plane, unit_plane_normal,
                     &dp->lower_left_corner);
  MirrorPointInPlane(point_on_plane, unit_plane_normal,
                     &dp->upper_right_corner);

  check_corners(g_p);

  return g_p;
}



static GraphicObj  *
MirrorCenter(GraphicObj *g_p, WCRec *center)
{
  ERAPrismWCRec *dp;

  dp = (ERAPrismWCRec *)g_p->spec.data;
  dp->lower_left_corner.x = 2.*center->x - dp->lower_left_corner.x;
  dp->lower_left_corner.y = 2.*center->y - dp->lower_left_corner.y;
  dp->lower_left_corner.z = 2.*center->z - dp->lower_left_corner.z;
  dp->upper_right_corner.x = 2.*center->x - dp->upper_right_corner.x;
  dp->upper_right_corner.y = 2.*center->y - dp->upper_right_corner.y;
  dp->upper_right_corner.z = 2.*center->z - dp->upper_right_corner.z;

  check_corners(g_p);
  
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
  WCRec wcp;
  VCRec pts[8];
  double a, b, c;

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
      ERAPrismWCRec *p;
      
      p = (ERAPrismWCRec *)g_p->spec.data;
      a = p->upper_right_corner.x - p->lower_left_corner.x;
      b = p->upper_right_corner.y - p->lower_left_corner.y;
      c = p->upper_right_corner.z - p->lower_left_corner.z;
      EVWCtoVC(v_p, &(p->lower_left_corner), &pts[0]);
      EVWCtoVC(v_p, &(p->upper_right_corner), &pts[1]);
      ASSIGN_POINT_TO_POINT(wcp, p->lower_left_corner);
      wcp.y += b;
      EVWCtoVC(v_p, &wcp, &pts[2]);
      wcp.z += c;
      EVWCtoVC(v_p, &wcp, &pts[4]);
      wcp.y -= b;
      EVWCtoVC(v_p, &wcp, &pts[6]);
      ASSIGN_POINT_TO_POINT(wcp, p->upper_right_corner);
      wcp.y -= b;
      EVWCtoVC(v_p, &wcp, &pts[3]);
      wcp.z -= c;
      EVWCtoVC(v_p, &wcp, &pts[5]);
      wcp.y += b;
      EVWCtoVC(v_p, &wcp, &pts[7]);
      if (BoxOverlapsTriangle(box,
                              (VC2DRec *)&pts[0],
                              (VC2DRec *)&pts[2],
                              (VC2DRec *)&pts[4]))
        return YES;
      if (BoxOverlapsTriangle(box,
                              (VC2DRec *)&pts[0],
                              (VC2DRec *)&pts[6],
                              (VC2DRec *)&pts[4]))
        return YES;
      if (BoxOverlapsTriangle(box,
                              (VC2DRec *)&pts[1],
                              (VC2DRec *)&pts[7],
                              (VC2DRec *)&pts[5]))
        return YES;        
      if (BoxOverlapsTriangle(box,
                              (VC2DRec *)&pts[1],
                              (VC2DRec *)&pts[3],
                              (VC2DRec *)&pts[5]))
        return YES;        
      if (BoxOverlapsTriangle(box,
                              (VC2DRec *)&pts[1],
                              (VC2DRec *)&pts[6],
                              (VC2DRec *)&pts[4]))
        return YES;        
      if (BoxOverlapsTriangle(box,
                              (VC2DRec *)&pts[1],
                              (VC2DRec *)&pts[3],
                              (VC2DRec *)&pts[6]))
        return YES;        
      if (BoxOverlapsTriangle(box,
                              (VC2DRec *)&pts[1],
                              (VC2DRec *)&pts[2],
                              (VC2DRec *)&pts[7]))
        return YES;        
      if (BoxOverlapsTriangle(box,
                              (VC2DRec *)&pts[1],
                              (VC2DRec *)&pts[2],
                              (VC2DRec *)&pts[4]))
        return YES;
      if (BoxOverlapsTriangle(box,
                              (VC2DRec *)&pts[0],
                              (VC2DRec *)&pts[6],
                              (VC2DRec *)&pts[3]))
        return YES;
      if (BoxOverlapsTriangle(box,
                              (VC2DRec *)&pts[0],
                              (VC2DRec *)&pts[5],
                              (VC2DRec *)&pts[3]))
        return YES; 

      return NO;
    }
    break;
  case INTERSECT:
  default:
    {
      ERAPrismWCRec *p;
      
      p = (ERAPrismWCRec *)g_p->spec.data;
      a = p->upper_right_corner.x - p->lower_left_corner.x;
      b = p->upper_right_corner.y - p->lower_left_corner.y;
      c = p->upper_right_corner.z - p->lower_left_corner.z;
      EVWCtoVC(v_p, &(p->lower_left_corner), &pts[0]);
      EVWCtoVC(v_p, &(p->upper_right_corner), &pts[1]);
      ASSIGN_POINT_TO_POINT(wcp, p->lower_left_corner);
      wcp.y += b;
      EVWCtoVC(v_p, &wcp, &pts[2]);
      wcp.z += c;
      EVWCtoVC(v_p, &wcp, &pts[4]);
      wcp.y -= b;
      EVWCtoVC(v_p, &wcp, &pts[6]);
      ASSIGN_POINT_TO_POINT(wcp, p->upper_right_corner);
      wcp.y -= b;
      EVWCtoVC(v_p, &wcp, &pts[3]);
      wcp.z -= c;
      EVWCtoVC(v_p, &wcp, &pts[5]);
      wcp.y += b;
      EVWCtoVC(v_p, &wcp, &pts[7]);
      if (BoxIntersectsLine(box,
                            (WC2DRec *)&pts[0],
                            (WC2DRec *)&pts[6]))
        return YES;
      if (BoxIntersectsLine(box,
                            (WC2DRec *)&pts[0],
                            (WC2DRec *)&pts[5]))
        return YES;
      if (BoxIntersectsLine(box,
                            (WC2DRec *)&pts[0],
                            (WC2DRec *)&pts[2]))
        return YES;
      if (BoxIntersectsLine(box,
                            (WC2DRec *)&pts[1],
                            (WC2DRec *)&pts[3]))
        return YES;
      if (BoxIntersectsLine(box,
                            (WC2DRec *)&pts[1],
                            (WC2DRec *)&pts[4]))
        return YES;
      if (BoxIntersectsLine(box,
                            (WC2DRec *)&pts[1],
                            (WC2DRec *)&pts[7]))
        return YES;
      if (BoxIntersectsLine(box,
                            (WC2DRec *)&pts[2],
                            (WC2DRec *)&pts[4]))
        return YES;
      if (BoxIntersectsLine(box,
                            (WC2DRec *)&pts[3],
                            (WC2DRec *)&pts[5]))
        return YES;
      if (BoxIntersectsLine(box,
                            (WC2DRec *)&pts[7],
                            (WC2DRec *)&pts[5]))
        return YES;
      if (BoxIntersectsLine(box,
                            (WC2DRec *)&pts[3],
                            (WC2DRec *)&pts[1]))
        return YES;
      if (BoxIntersectsLine(box,
                            (WC2DRec *)&pts[6],
                            (WC2DRec *)&pts[4]))
        return YES;
      if (BoxIntersectsLine(box,
                            (WC2DRec *)&pts[3],
                            (WC2DRec *)&pts[6]))
        return YES;
      return NO;
    }
    break;
  }
}



static LIST 
KeyPoints(GraphicObj *p)
{
  LIST temp_list;
  KeyPointRec *key_p;
  ERAPrismWCRec *l_p;
	double size_x, size_y, size_z;

  l_p       = (ERAPrismWCRec *)(p->spec.data);

	size_x = fabs(l_p->upper_right_corner.x - l_p->lower_left_corner.x);
	size_y = fabs(l_p->upper_right_corner.y - l_p->lower_left_corner.y);
	size_z = fabs(l_p->upper_right_corner.z - l_p->lower_left_corner.z);

  temp_list = make_list();
  key_p = (KeyPointRec*)add_to_tail(temp_list, make_node(sizeof(KeyPointRec)));
  key_p->coords.x = l_p->lower_left_corner.x;
  key_p->coords.y = l_p->lower_left_corner.y;
  key_p->coords.z = l_p->lower_left_corner.z;
  key_p->dist_from_hit = 0.0;
  key_p = (KeyPointRec*)add_to_tail(temp_list, make_node(sizeof(KeyPointRec)));
  key_p->coords.x = l_p->lower_left_corner.x + size_x;
  key_p->coords.y = l_p->lower_left_corner.y;
  key_p->coords.z = l_p->lower_left_corner.z;
  key_p->dist_from_hit = 0.0;
  key_p = (KeyPointRec*)add_to_tail(temp_list, make_node(sizeof(KeyPointRec)));
  key_p->coords.x = l_p->lower_left_corner.x;
  key_p->coords.y = l_p->lower_left_corner.y + size_y;
  key_p->coords.z = l_p->lower_left_corner.z;
  key_p->dist_from_hit = 0.0;
  key_p = (KeyPointRec*)add_to_tail(temp_list, make_node(sizeof(KeyPointRec)));
  key_p->coords.x = l_p->lower_left_corner.x + size_x;
  key_p->coords.y = l_p->lower_left_corner.y + size_y;
  key_p->coords.z = l_p->lower_left_corner.z;
  key_p->dist_from_hit = 0.0;
  key_p = (KeyPointRec*)add_to_tail(temp_list, make_node(sizeof(KeyPointRec)));
  key_p->coords.x = l_p->lower_left_corner.x;
  key_p->coords.y = l_p->lower_left_corner.y;
  key_p->coords.z = l_p->lower_left_corner.z + size_z;
  key_p->dist_from_hit = 0.0;
  key_p = (KeyPointRec*)add_to_tail(temp_list, make_node(sizeof(KeyPointRec)));
  key_p->coords.x = l_p->lower_left_corner.x + size_x;
  key_p->coords.y = l_p->lower_left_corner.y;
  key_p->coords.z = l_p->lower_left_corner.z + size_z;
  key_p->dist_from_hit = 0.0;
  key_p = (KeyPointRec*)add_to_tail(temp_list, make_node(sizeof(KeyPointRec)));
  key_p->coords.x = l_p->lower_left_corner.x;
  key_p->coords.y = l_p->lower_left_corner.y + size_y;
  key_p->coords.z = l_p->lower_left_corner.z + size_z;
  key_p->dist_from_hit = 0.0;
  key_p = (KeyPointRec*)add_to_tail(temp_list, make_node(sizeof(KeyPointRec)));
  key_p->coords.x = l_p->lower_left_corner.x + size_x;
  key_p->coords.y = l_p->lower_left_corner.y + size_y;
  key_p->coords.z = l_p->lower_left_corner.z + size_z;
  key_p->dist_from_hit = 0.0;

  return temp_list;
}



static GraphicObj *
Reshape(GraphicObj *p, EHandleNum hn, WCRec *pos)
{
  ERAPrismWCRec *_p;

  _p = (ERAPrismWCRec *)(p->spec.data);
  
  if        (hn == 1) {
    _p->lower_left_corner.x = pos->x;
    _p->lower_left_corner.y = pos->y;
    _p->lower_left_corner.z = pos->z;
  } else if (hn == 2) {
    _p->upper_right_corner.x = pos->x;
    _p->upper_right_corner.y = pos->y;
    _p->upper_right_corner.z = pos->z;
  }
  return p;
}



static EHandleNum 
ReshapeHandle(EView *v_p, GraphicObj *p, WCRec *nearp)
{
  ERAPrismWCRec *l_p;
  FPNum dist1, dist2;
  VCRec nvc, pvc;

  l_p       = (ERAPrismWCRec *)(p->spec.data);
  EVWCtoVC(v_p, nearp, &nvc);
  EVWCtoVC(v_p, &(l_p->lower_left_corner), &pvc);
  dist1 = DistOfPnts(pvc.u, pvc.v, nvc.u, nvc.v);
  EVWCtoVC(v_p, &(l_p->upper_right_corner), &pvc);
  dist2 = DistOfPnts(pvc.u, pvc.v, nvc.u, nvc.v);
  if (dist1 < dist2) {
    return (EHandleNum)1;
  } else {
    return (EHandleNum)2;
  }
}



static void 
ShowHandles(EView *v_p, GraphicObj *p)
{
  ERAPrismWCRec *l_p;

  l_p = (ERAPrismWCRec *)p->spec.data;
  EGDrawHandle(v_p, &(l_p->lower_left_corner));
  EGDrawHandle(v_p, &(l_p->upper_right_corner));
}
  


static void 
EraseHandles(EView *v_p, GraphicObj *p)
{
  ERAPrismWCRec *l_p;

  l_p = (ERAPrismWCRec *)p->spec.data;
  EGEraseHandle(v_p, &(l_p->lower_left_corner)); 
  EGEraseHandle(v_p, &(l_p->upper_right_corner));
}



static void 
XYofHandle(GraphicObj *p, EHandleNum hn, WCRec *pos)
{
  ERAPrismWCRec *l_p;

  l_p = (ERAPrismWCRec *)p->spec.data;
  switch((int)hn) {
  case 1:
  default:
    pos->x = l_p->lower_left_corner.x;
    pos->y = l_p->lower_left_corner.y;
    pos->z = l_p->lower_left_corner.z;
    break;
  case 2:
    pos->x = l_p->upper_right_corner.x;
    pos->y = l_p->upper_right_corner.y;
    pos->z = l_p->upper_right_corner.z;
    break;
  }
}



static void 
ChangeAttributes(GraphicObj *p)
{
  ERAPrismWCRec *dp;
  unsigned long mask;

  dp = (ERAPrismWCRec *)p->spec.data;

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
  ERAPrismWCRec *dp;

  dp = (ERAPrismWCRec *)p->spec.data;
  
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
  ERAPrismWCRec *dp;

  dp = (ERAPrismWCRec *)p->spec.data;
  
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



static GraphicObj *
Rotate(GraphicObj *g_p, WCRec *center, WCRec *axial)
{
  ERAPrismWCRec *p;
  WCRec x_c;

  p = (ERAPrismWCRec *)g_p->spec.data;
  x_c.x = p->lower_left_corner.x - center->x;
  x_c.y = p->lower_left_corner.y - center->y;
  x_c.z = p->lower_left_corner.z - center->z;
  RotVectAboutVect(axial, &x_c);
  p->lower_left_corner.x = center->x + x_c.x;
  p->lower_left_corner.y = center->y + x_c.y;
  p->lower_left_corner.z = center->z + x_c.z;
  
  x_c.x = p->upper_right_corner.x - center->x;
  x_c.y = p->upper_right_corner.y - center->y;
  x_c.z = p->upper_right_corner.z - center->z;
  RotVectAboutVect(axial, &x_c);
  p->upper_right_corner.x = center->x + x_c.x;
  p->upper_right_corner.y = center->y + x_c.y;
  p->upper_right_corner.z = center->z + x_c.z;
  
  check_corners(g_p);
  
  return g_p;
}


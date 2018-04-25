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

/* This file holds the redefinition of graphic methods for the straight */
/* line segment in 3D. */

#include "Econfig.h"
#include <stdlib.h>

#include "Eview.h"
#include "Egraphic.h"
#include "Egeomops.h"
#include "Ecolors.h"
#include "Eactset.h"

#define LINE3D_PRIVATE_HEADER
#include "Eline3d.h"


  
/* ========================================================================= */
/* PRIVATE DATA */
/* ========================================================================= */

#define STRING_BUFF_SIZE 132

static char string_rep_buffer[STRING_BUFF_SIZE];
 
/* ========================================================================= */
/* PUBLIC */
/* ========================================================================= */

GraphicObj *
CreateLine3D(WCRec points[2])
{
  ELine3DWCRec *r_p;
  GraphicObj *g_p;

  r_p = (ELine3DWCRec *)make_node(sizeof(ELine3DWCRec));
  g_p = EGCreateGraphics(&line3d_graphic_methods, (caddr_t)r_p,
			 sizeof(ELine3DWCRec));
  ModifyGeometry(g_p, points);
  r_p->attributes.spec.style = LineSolid;
  r_p->attributes.spec.width = 0;
  r_p->attributes.spec.color = 0x0;
  r_p->attributes.spec.shrink = 1.0;
  r_p->attributes.spec.have_any = NO;
  return g_p;
}

static void 
Draw(EView *v_p, GraphicObj *p)
{
  ELine3DWCRec *thedp;

  thedp = (ELine3DWCRec *)(p->spec.data);

  EVLine3DPrimitive(v_p, p, &thedp->start_point, &thedp->end_point, 
		    thedp->attributes.spec.style,
		    thedp->attributes.spec.color,
		    thedp->attributes.spec.width,
				thedp->attributes.spec.shrink,
		    DRAW_MODE_DRAW);
}



static void 
BoundingBoxVC(EView *v_p, GraphicObj *p, VCRec *ll, VCRec *ur)
{
  ELine3DWCRec *l_d_p;
  VCRec fpvc, lpvc;
  
  l_d_p = (ELine3DWCRec *)(p->spec.data);
  EVWCtoVC(v_p, &(l_d_p->start_point), &fpvc);
  EVWCtoVC(v_p, &(l_d_p->end_point), &lpvc);
  ll->u = min(fpvc.u, lpvc.u);
  ll->v = min(fpvc.v, lpvc.v);
  ll->n = min(fpvc.n, lpvc.n);
  ur->u = max(fpvc.u, lpvc.u);
  ur->v = max(fpvc.v, lpvc.v);
  ur->n = max(fpvc.n, lpvc.n);
}
  


static void 
BoundingBoxWC(GraphicObj *p, WCRec *ll, WCRec *ur)
{
  ELine3DWCRec *l_d_p;
  
  l_d_p = (ELine3DWCRec *)(p->spec.data);
  ll->x = min(l_d_p->start_point.x, l_d_p->end_point.x);
  ll->y = min(l_d_p->start_point.y, l_d_p->end_point.y);
  ll->z = min(l_d_p->start_point.z, l_d_p->end_point.z);
  ur->x = max(l_d_p->start_point.x, l_d_p->end_point.x); 
  ur->y = max(l_d_p->start_point.y, l_d_p->end_point.y);
  ur->z = max(l_d_p->start_point.z, l_d_p->end_point.z);
}



static void 
XorDraw(EView *v_p, GraphicObj *g_p)
{
  ELine3DWCRec *thedp;

  thedp = (ELine3DWCRec *)g_p->spec.data;
  
  EVLine3DPrimitive(v_p, g_p, &thedp->start_point, &thedp->end_point, 
		    thedp->attributes.spec.style,
		    thedp->attributes.spec.color,
		    thedp->attributes.spec.width,
		    thedp->attributes.spec.shrink,
		    DRAW_MODE_XORDRAW);
}

 


static GraphicObj *
ModifyGeometry(GraphicObj *g_p, WCRec *points)
{
  ELine3DWCRec *r_p;

  r_p = (ELine3DWCRec *)(g_p->spec.data);
  r_p->start_point.x = points[0].x;
  r_p->start_point.y = points[0].y;
  r_p->start_point.z = points[0].z;
  r_p->end_point.x   = points[1].x;
  r_p->end_point.y   = points[1].y;
  r_p->end_point.z   = points[1].z;

  return g_p;
}



static BOOLEAN 
GetGeometry(GraphicObj *g_p, WCRec *points,
                              int *npoints)
{
  ELine3DWCRec *r_p = (ELine3DWCRec *)(g_p->spec.data);

  points[0].x = r_p->start_point.x;
  points[0].y = r_p->start_point.y;
  points[0].z = r_p->start_point.z;
  points[1].x = r_p->end_point.x  ;
  points[1].y = r_p->end_point.y  ;
  points[1].z = r_p->end_point.z  ;
  *npoints    = 2;
  return YES;
}



static void 
Hilite(EView *v_p, GraphicObj *g_p)
{
  ELine3DWCRec *thedp;
  
  thedp = (ELine3DWCRec *)g_p->spec.data;
  
  EVLine3DPrimitive(v_p, g_p, &thedp->start_point, &thedp->end_point, 
		    thedp->attributes.spec.style,
		    thedp->attributes.spec.color,
		    thedp->attributes.spec.width,
				1.0,
		    DRAW_MODE_HILITE);
}



static void 
Unhilite(EView *v_p, GraphicObj *g_p)
{
  ELine3DWCRec *thedp;
  
  thedp = (ELine3DWCRec *)g_p->spec.data;
  
  EVLine3DPrimitive(v_p, g_p, &thedp->start_point, &thedp->end_point, 
		    thedp->attributes.spec.style,
		    thedp->attributes.spec.color,
		    thedp->attributes.spec.width,
				1.0,
		    DRAW_MODE_UNHILITE);
}



static void 
Erase(EView *v_p, GraphicObj *g_p)
{
  ELine3DWCRec *thedp;
  
  thedp = (ELine3DWCRec *)g_p->spec.data;
  
  EVLine3DPrimitive(v_p, g_p, &thedp->start_point, &thedp->end_point, 
		    thedp->attributes.spec.style,
		    thedp->attributes.spec.color,
		    thedp->attributes.spec.width,
				thedp->attributes.spec.shrink,
		    DRAW_MODE_ERASE);
}



static GraphicObj *
Translate(GraphicObj *g_p, WCRec *by)
{
  ELine3DWCRec *p;

  p = (ELine3DWCRec *)g_p->spec.data;
  p->start_point.x += by->x;
  p->start_point.y += by->y;
  p->start_point.z += by->z;
  p->end_point.x += by->x;
  p->end_point.y += by->y;
  p->end_point.z += by->z;
  return g_p;
}



static GraphicObj *
Rotate(GraphicObj *g_p, WCRec *center, WCRec *axial)
{
  ELine3DWCRec *c;
  WCRec x_c;

  c = (ELine3DWCRec *)g_p->spec.data;
  x_c.x = c->start_point.x - center->x;
  x_c.y = c->start_point.y - center->y;
  x_c.z = c->start_point.z - center->z;
  RotVectAboutVect(axial, &x_c);
  c->start_point.x = center->x + x_c.x;
  c->start_point.y = center->y + x_c.y;
  c->start_point.z = center->z + x_c.z;
  x_c.x = c->end_point.x - center->x;
  x_c.y = c->end_point.y - center->y;
  x_c.z = c->end_point.z - center->z;
  RotVectAboutVect(axial, &x_c);
  c->end_point.x = center->x + x_c.x;
  c->end_point.y = center->y + x_c.y;
  c->end_point.z = center->z + x_c.z;
  return g_p;
}



static GraphicObj  *
MirrorPlane(GraphicObj *g_p, WCRec *point_on_plane,
                  WCRec *unit_plane_normal)
{
  ELine3DWCRec *dp;

  dp = (ELine3DWCRec *)g_p->spec.data;

  MirrorPointInPlane(point_on_plane, unit_plane_normal, &dp->start_point);
  MirrorPointInPlane(point_on_plane, unit_plane_normal, &dp->end_point);
  
  return g_p;
}



static GraphicObj  *
MirrorCenter(GraphicObj *g_p, WCRec *center)
{
  ELine3DWCRec *dp;

  dp = (ELine3DWCRec *)g_p->spec.data;
  dp->start_point.x = 2.*center->x - dp->start_point.x;
  dp->start_point.y = 2.*center->y - dp->start_point.y;
  dp->start_point.z = 2.*center->z - dp->start_point.z;
  dp->end_point.x = 2.*center->x - dp->end_point.x;
  dp->end_point.y = 2.*center->y - dp->end_point.y;
  dp->end_point.z = 2.*center->z - dp->end_point.z;
  return g_p;
}



static GraphicObj *
Scale(GraphicObj *g_p,
		 WCRec *center, FPNum sx, FPNum sy, FPNum sz)
{
  return g_p;
}



static BOOLEAN 
HitByBox(EView *v_p, EBoxVC2DRec *box,
		       ESelectCriteria sel_crit, GraphicObj *p)
{
  VCRec ll, ur, fpvc, lpvc;
  WC2DRec p1, p2;
  ELine3DWCRec *l_d_p;

  switch(sel_crit) {
  case INSIDE:
    BoundingBoxVC(v_p, p, &ll, &ur);
    if (ll.u >= box->left   && ur.u <= box->right && 
	ll.v >= box->bottom && ur.v <= box->top)
      return YES;
    else
      return NO;
    break;
  case OVERLAP:
  case INTERSECT:
  default:
    l_d_p = (ELine3DWCRec *)p->spec.data;
    EVWCtoVC(v_p, &(l_d_p->start_point), &fpvc);
    EVWCtoVC(v_p, &(l_d_p->end_point), &lpvc);
    p1.x = fpvc.u; p1.y = fpvc.v; p2.x = lpvc.u; p2.y = lpvc.v;
    return BoxIntersectsLine(box, &p1, &p2);
    break;
  }
}



static EGraphicType 
Type(GraphicObj *g_p)
{
  return EG_LINE3D;
}



static char *
AsString(GraphicObj *g_p)
{
  ELine3DWCRec *l;

  l = (ELine3DWCRec *)g_p->spec.data;
  sprintf(string_rep_buffer, "LINE3D [%f,%f,%f] [%f,%f,%f]",
	  l->start_point.x, l->start_point.y, l->start_point.z,
	  l->end_point.x, l->end_point.y, l->end_point.z);
  
  return string_rep_buffer; /* Don't free this memory!!! */
}



static LIST 
KeyPoints(GraphicObj *p)
{
  LIST temp_list;
  KeyPointRec *key_p;
  ELine3DWCRec *l_p;
  WCRec cp, delta;
  int keypi, i;

  l_p       = (ELine3DWCRec *)(p->spec.data);
  temp_list = make_list();

  keypi = EASValsGetKeyPIntervals();

  cp.x = l_p->start_point.x;
  cp.y = l_p->start_point.y;
  cp.z = l_p->start_point.z;
  key_p = (KeyPointRec*)add_to_tail(temp_list, make_node(sizeof(KeyPointRec)));
  key_p->coords.x = cp.x; key_p->coords.y = cp.y; key_p->coords.z = cp.z;
  key_p->dist_from_hit = 0.0;
  delta.x = (l_p->end_point.x - l_p->start_point.x)/keypi;
  delta.y = (l_p->end_point.y - l_p->start_point.y)/keypi;
  delta.z = (l_p->end_point.z - l_p->start_point.z)/keypi;
  for (i = 1; i < keypi; i++) {
    cp.x += delta.x; cp.y += delta.y; cp.z += delta.z;
    key_p =
      (KeyPointRec*)add_to_tail(temp_list, make_node(sizeof(KeyPointRec)));
    key_p->coords.x = cp.x; key_p->coords.y = cp.y; key_p->coords.z = cp.z;
    key_p->dist_from_hit = 0.0;
  }
  key_p = (KeyPointRec*)add_to_tail(temp_list, make_node(sizeof(KeyPointRec)));
  key_p->coords.x      = l_p->end_point.x;
  key_p->coords.y      = l_p->end_point.y;
  key_p->coords.z      = l_p->end_point.z;
  key_p->dist_from_hit = 0.0;
  return temp_list;
}



static GraphicObj *
Reshape(GraphicObj *gp, EHandleNum hn, WCRec *pos)
{
  ELine3DWCRec *_p;
  
  _p = (ELine3DWCRec *)(gp->spec.data);
  
  if        (hn == 1) {
    _p->start_point.x = pos->x;
    _p->start_point.y = pos->y;
    _p->start_point.z = pos->z; 
  } else if (hn == 2) {
    _p->end_point.x = pos->x;
    _p->end_point.y = pos->y;
    _p->end_point.z = pos->z;
  }
  return gp;
}



static EHandleNum   
ReshapeHandle(EView *v_p, GraphicObj *p, WCRec *nearp)
{
  ELine3DWCRec *l_p;
  VCRec nvc, fvc, svc;

  l_p       = (ELine3DWCRec *)(p->spec.data);
  EVWCtoVC(v_p, nearp, &nvc);
  EVWCtoVC(v_p, &(l_p->start_point), &fvc);
  EVWCtoVC(v_p, &(l_p->end_point), &svc);
  /*    Line has two reshape handles: 1 and 2    */
  if (DistOfPnts(fvc.u, fvc.v, nvc.u, nvc.v) <=
      DistOfPnts(svc.u, svc.v, nvc.u, nvc.v)) {
    return (EHandleNum)1;
  } else {
    return (EHandleNum)2;
  }
}



static void 
ShowHandles(EView *v_p, GraphicObj *p)
{
  ELine3DWCRec *l_p;

  l_p = (ELine3DWCRec *)p->spec.data;
  EGDrawHandle(v_p, &(l_p->start_point));
  EGDrawHandle(v_p, &(l_p->end_point));
}
  


static void 
EraseHandles(EView *v_p, GraphicObj *p)
{
  ELine3DWCRec *l_p;

  l_p = (ELine3DWCRec *)p->spec.data;
  EGEraseHandle(v_p, &(l_p->start_point));
  EGEraseHandle(v_p, &(l_p->end_point));
}




static void 
XYofHandle(GraphicObj *p, EHandleNum hn, WCRec *pos)
{
  ELine3DWCRec *l_p;

  l_p = (ELine3DWCRec *)p->spec.data;
  switch((int)hn) {
  case 2:
    pos->x = l_p->end_point.x;
    pos->y = l_p->end_point.y;
    pos->z = l_p->end_point.z;
    break;
  case 1:
  default:
    pos->x = l_p->start_point.x;
    pos->y = l_p->start_point.y;
    pos->z = l_p->start_point.z;
    break;
  }
}



static void 
RetrieveFrom(FILE *file_p, GraphicSpecObj *gsp)
{
  ELine3DWCRec *p;
  int style, width;
  char color[64];
  EPixel pixel;
  BOOLEAN success;
  double sx, sy, sz, ex, ey, ez;
	float shrink;
  
  if ((p = (ELine3DWCRec *)make_node(sizeof(ELine3DWCRec))) != NULL) {
    fscanf(file_p,
	   "%lf %lf %lf %lf %lf %lf %d %d %d %s %f",
	   &sx, &sy, &sz, &ex, &ey, &ez,
	   &(p->attributes.spec.have_any),
	   &width, &style, color, &shrink);
    p->start_point.x = sx;
    p->start_point.y = sy;
    p->start_point.z = sz;
    p->end_point.x   = ex;
    p->end_point.y   = ey;
    p->end_point.z   = ez;
    p->attributes.spec.width = width;
    p->attributes.spec.style = style;
    p->attributes.spec.shrink = shrink;
    pixel = ColorGetPixelFromString(color, &success);
    if (success)
      p->attributes.spec.color = pixel;
  }
  gsp->data        = (caddr_t)p;
  gsp->data_length = sizeof(ELine3DWCRec);
}
 


static void 
StoreOn(FILE *file_p, GraphicObj *g_p)
{
  ELine3DWCRec *p;
  int style, width;
  char *color;
	float shrink;

  p = (ELine3DWCRec *)g_p->spec.data;
  if (p->attributes.spec.have_any == YES) {
    style = p->attributes.spec.style;
    width = p->attributes.spec.width;
    shrink = p->attributes.spec.shrink;
    color = ColorGetStringFromPixel(p->attributes.spec.color);
    if (color == NULL)
      color = "black";
  } else {
    style = LineSolid;
    width = 0;
		shrink = 1.0;
    color = "black";
  }
  fprintf(file_p,
	  "%f %f %f %f %f %f %d %d %d %s %f\n",
	  p->start_point.x, p->start_point.y, p->start_point.z, 
	  p->end_point.x, p->end_point.y, p->end_point.z,
	  p->attributes.spec.have_any,
	  width, style, color, shrink);
}



static void 
ChangeAttributes(GraphicObj *p)
{
  ELine3DWCRec *dp;
  unsigned long mask;

  mask = EASValsGetChangeMask();
  dp = (ELine3DWCRec *)p->spec.data;
  if ((mask & STYLE_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.style = EASValsGetLineStyle();
    dp->attributes.spec.have_any = YES;
  }
  if ((mask & WIDTH_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.width = EASValsGetLineWidth();
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
}
			       


static void 
CopyAttributesToActiveSet(GraphicObj *p)
{
  ELine3DWCRec *dp;

  dp = (ELine3DWCRec *)p->spec.data;

  if (dp->attributes.spec.have_any) {
    EASValsSetLineStyle(dp->attributes.spec.style);
    EASValsSetLineWidth(dp->attributes.spec.width);
    EASValsSetColor(dp->attributes.spec.color);
    EASValsSetShrink(dp->attributes.spec.shrink);
  }
}


static BOOLEAN
AttribValAsInAS(GraphicObj *p, unsigned long mask)  
{
  ELine3DWCRec *dp;

  dp = (ELine3DWCRec *)p->spec.data;
  
  if ((mask & STYLE_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.style != EASValsGetLineStyle())
      return NO;
  if ((mask & WIDTH_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.width != EASValsGetLineWidth())
      return NO;
  if ((mask & COLOR_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.color != EASValsGetColor())
      return NO;
  if ((mask & SHRINK_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.shrink != EASValsGetShrink())
      return NO;
  
  return YES;
}

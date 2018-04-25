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

#define CBEZC3D_PRIVATE_HEADER
#include "Ecbezc3d.h"


  
/* ========================================================================= */
/* PRIVATE  */
/* ========================================================================= */

#define STRING_BUFF_SIZE 132

static char string_rep_buffer[STRING_BUFF_SIZE];

/* ========================================================================= */
/* PUBLIC  */
/* ========================================================================= */

GraphicObj *
CreateCBezC3D(WCRec points[4])
{
  ECBezC3DWCRec *r_p;
  GraphicObj *g_p;
  int i;

  r_p = (ECBezC3DWCRec *)make_node(sizeof(ECBezC3DWCRec));
  g_p = EGCreateGraphics(&cbezc3d_graphic_methods, (caddr_t)r_p,
			 sizeof(ECBezC3DWCRec));
  for (i = 0; i < 4; i++) {
    r_p->points[i].x               = points[i].x; 
    r_p->points[i].y               = points[i].y;
    r_p->points[i].z               = points[i].z;
  }
  r_p->attributes.spec.tessel_intervals = 31;
  r_p->attributes.spec.show_polygon     = YES;
  r_p->attributes.spec.show_curve       = YES;
  r_p->attributes.spec.style            = LineSolid;
  r_p->attributes.spec.width            = 0;
  r_p->attributes.spec.color            = 0x0;
  r_p->attributes.spec.have_any         = NO;
  return g_p;
}



static void 
Draw(EView *v_p, GraphicObj *p)
{
  ECBezC3DWCRec *thedp;

  thedp = (ECBezC3DWCRec *)(p->spec.data);
  
  EVCBezC3DPrimitive(v_p, p, thedp->points,
		     thedp->attributes.spec.style,
		     thedp->attributes.spec.color,
		     thedp->attributes.spec.width,
		     thedp->attributes.spec.tessel_intervals,
		     thedp->attributes.spec.show_polygon,
		     DRAW_MODE_DRAW);
}



static void 
BoundingBoxVC(EView *v_p, GraphicObj *p, VCRec *ll, VCRec *ur)
{
  ECBezC3DWCRec *thedp;
  VCRec pvc;
  
  thedp = (ECBezC3DWCRec *)(p->spec.data);
  EVWCtoVC(v_p, &(thedp->points[0]), &pvc);
  ll->u = pvc.u;
  ll->v = pvc.v;
  ll->n = pvc.n;
  ur->u = pvc.u;
  ur->v = pvc.v;
  ur->n = pvc.n;
  EVWCtoVC(v_p, &(thedp->points[1]), &pvc);
  ll->u = min(pvc.u, ll->u);
  ll->v = min(pvc.v, ll->v);
  ll->n = min(pvc.n, ll->n);
  ur->u = max(pvc.u, ur->u);
  ur->v = max(pvc.v, ur->v);
  ur->n = max(pvc.n, ur->n);
  EVWCtoVC(v_p, &(thedp->points[2]), &pvc);
  ll->u = min(pvc.u, ll->u);
  ll->v = min(pvc.v, ll->v);
  ll->n = min(pvc.n, ll->n);
  ur->u = max(pvc.u, ur->u);
  ur->v = max(pvc.v, ur->v);
  ur->n = max(pvc.n, ur->n);
  EVWCtoVC(v_p, &(thedp->points[3]), &pvc);
  ll->u = min(pvc.u, ll->u);
  ll->v = min(pvc.v, ll->v);
  ll->n = min(pvc.n, ll->n);
  ur->u = max(pvc.u, ur->u);
  ur->v = max(pvc.v, ur->v);
  ur->n = max(pvc.n, ur->n);
}
  


static void 
BoundingBoxWC(GraphicObj *p, WCRec *ll, WCRec *ur)
{
  ECBezC3DWCRec *thedp;
  
  thedp = (ECBezC3DWCRec *)(p->spec.data);
  
  ll->x = min(thedp->points[0].x, thedp->points[1].x);
  ll->y = min(thedp->points[0].y, thedp->points[1].y);
  ll->z = min(thedp->points[0].z, thedp->points[1].z);
  ur->x = max(thedp->points[0].x, thedp->points[1].x); 
  ur->y = max(thedp->points[0].y, thedp->points[1].y);
  ur->z = max(thedp->points[0].z, thedp->points[1].z);

  ll->x = min(ll->x, thedp->points[2].x);
  ll->y = min(ll->y, thedp->points[2].y);
  ll->z = min(ll->z, thedp->points[2].z);
  ur->x = max(ur->x, thedp->points[2].x); 
  ur->y = max(ur->y, thedp->points[2].y);
  ur->z = max(ur->z, thedp->points[2].z);
  
  ll->x = min(ll->x, thedp->points[3].x);
  ll->y = min(ll->y, thedp->points[3].y);
  ll->z = min(ll->z, thedp->points[3].z);
  ur->x = max(ur->x, thedp->points[3].x); 
  ur->y = max(ur->y, thedp->points[3].y);
  ur->z = max(ur->z, thedp->points[3].z);
}



static void 
XorDraw(EView *v_p, GraphicObj *g_p)
{
  ECBezC3DWCRec *thedp;

  thedp = (ECBezC3DWCRec *)(g_p->spec.data);

  EVCBezC3DPrimitive(v_p, g_p, thedp->points,
		     thedp->attributes.spec.style,
		     thedp->attributes.spec.color,
		     thedp->attributes.spec.width,
		     thedp->attributes.spec.tessel_intervals,
		     thedp->attributes.spec.show_polygon,
		     DRAW_MODE_XORDRAW);
}

 


static GraphicObj *
ModifyGeometry(GraphicObj *g_p, WCRec points[4])
{
  ECBezC3DWCRec *r_p;
  int i;

  r_p = (ECBezC3DWCRec *)(g_p->spec.data);

  for (i = 0; i < 4; i++) {
    r_p->points[i].x               = points[i].x; 
    r_p->points[i].y               = points[i].y;
    r_p->points[i].z               = points[i].z;
  }
  return g_p;
}




static BOOLEAN
GetGeometry(GraphicObj *g_p, WCRec *points, int *npoints)
{
  ECBezC3DWCRec *r_p = (ECBezC3DWCRec *)(g_p->spec.data);
  int i;

  for (i = 0; i < 4; i++) {
    points[i].x = r_p->points[i].x; 
    points[i].y = r_p->points[i].y;
    points[i].z = r_p->points[i].z;
  }
  *npoints = 4;
  return YES;
}




static void 
Hilite(EView *v_p, GraphicObj *g_p)
{
  ECBezC3DWCRec *thedp;

  thedp = (ECBezC3DWCRec *)(g_p->spec.data);

  EVCBezC3DPrimitive(v_p,  g_p, thedp->points,
		     thedp->attributes.spec.style,
		     thedp->attributes.spec.color,
		     thedp->attributes.spec.width,
		     thedp->attributes.spec.tessel_intervals,
		     thedp->attributes.spec.show_polygon,
		     DRAW_MODE_HILITE);
}



static void 
Unhilite(EView *v_p, GraphicObj *g_p)
{
  ECBezC3DWCRec *thedp;

  thedp = (ECBezC3DWCRec *)(g_p->spec.data);

  EVCBezC3DPrimitive(v_p,  g_p, thedp->points,
		     thedp->attributes.spec.style,
		     thedp->attributes.spec.color,
		     thedp->attributes.spec.width,
		     thedp->attributes.spec.tessel_intervals,
		     thedp->attributes.spec.show_polygon,
		     DRAW_MODE_UNHILITE);
}



static void 
Erase(EView *v_p, GraphicObj *g_p)
{
  ECBezC3DWCRec *thedp;
  
  thedp = (ECBezC3DWCRec *)(g_p->spec.data);

  EVCBezC3DPrimitive(v_p,  g_p, thedp->points,
		     thedp->attributes.spec.style,
		     thedp->attributes.spec.color,
		     thedp->attributes.spec.width,
		     thedp->attributes.spec.tessel_intervals,
		     thedp->attributes.spec.show_polygon,
		     DRAW_MODE_ERASE); 
}



static GraphicObj *
Translate(GraphicObj *g_p, WCRec *by)
{
  ECBezC3DWCRec *p;
  int i;

  p = (ECBezC3DWCRec *)g_p->spec.data;
  for (i = 0; i < 4; i++) {
    p->points[i].x += by->x;
    p->points[i].y += by->y;
    p->points[i].z += by->z;
  }
  return g_p;
}



static GraphicObj *
Rotate(GraphicObj *g_p, WCRec *center, WCRec *axial)
{
  ECBezC3DWCRec *c;
  WCRec x_c;
  int i;

  c = (ECBezC3DWCRec *)g_p->spec.data;
  for (i = 0; i < 4; i++ ) {
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
MirrorPlane(GraphicObj *g_p,
                   WCRec *point_on_plane, WCRec *unit_plane_normal)
{
  ECBezC3DWCRec *dp;
  int j;

  dp = (ECBezC3DWCRec *)g_p->spec.data;
  for (j = 0; j < 4; j++) 
    MirrorPointInPlane(point_on_plane, unit_plane_normal, &dp->points[j]);
  
  return g_p;
}



static GraphicObj  *
MirrorCenter(GraphicObj *g_p, WCRec *center)
{
  ECBezC3DWCRec *dp;
  int j;

  dp = (ECBezC3DWCRec *)g_p->spec.data;
  for (j = 0; j < 4; j++) {
    dp->points[j].x = 2.*center->x - dp->points[j].x;
    dp->points[j].y = 2.*center->y - dp->points[j].y;
    dp->points[j].z = 2.*center->z - dp->points[j].z;
  }
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
  VCRec ll, ur, pvc;
  WC2DRec points[4];
  ECBezC3DWCRec *thedp;
  int i;

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
    BoundingBoxVC(v_p, p, &ll, &ur);
    if (ll.u >= box->left   && ur.u <= box->right && 
	ll.v >= box->bottom && ur.v <= box->top)
      return YES;
  case INTERSECT:
  default:
    thedp = (ECBezC3DWCRec *)p->spec.data;
    for (i = 0; i < 4; i++) {
      EVWCtoVC(v_p, &(thedp->points[i]), &pvc);
      points[i].x = pvc.u; points[i].y = pvc.v;
    }
    return BoxIntersectsCBezCurve(box, points);
    break;
  }
}



static EGraphicType 
Type(GraphicObj *g_p)
{
  return EG_CBEZCURVE3D;
}



static char *
AsString(GraphicObj *g_p)
{
  ECBezC3DWCRec *l;

  l = (ECBezC3DWCRec *)g_p->spec.data;
  sprintf(string_rep_buffer,
	  "CBEZCURVE3D [%f,%f,%f][%f,%f,%f][%f,%f,%f][%f,%f,%f]",
	  l->points[0].x, l->points[0].y, l->points[0].z,
	  l->points[1].x, l->points[1].y, l->points[1].z,
	  l->points[2].x, l->points[2].y, l->points[2].z,
	  l->points[3].x, l->points[3].y, l->points[3].z);
  
  return string_rep_buffer; /* Don't free this memory!!! */
}



static LIST 
KeyPoints(GraphicObj *p)
{
  LIST temp_list;
  KeyPointRec *key_p;
  ECBezC3DWCRec *l_p;

  l_p       = (ECBezC3DWCRec *)(p->spec.data);
  temp_list = make_list();

  key_p = (KeyPointRec*)add_to_tail(temp_list, make_node(sizeof(KeyPointRec)));
  key_p->coords.x = l_p->points[0].x;
  key_p->coords.y = l_p->points[0].y;
  key_p->coords.z = l_p->points[0].z;
  key_p->dist_from_hit = 0.0;
  key_p = (KeyPointRec*)add_to_tail(temp_list, make_node(sizeof(KeyPointRec)));
  key_p->coords.x = l_p->points[1].x;
  key_p->coords.y = l_p->points[1].y;
  key_p->coords.z = l_p->points[1].z;
  key_p->dist_from_hit = 0.0;
  key_p = (KeyPointRec*)add_to_tail(temp_list, make_node(sizeof(KeyPointRec)));
  key_p->coords.x = l_p->points[2].x;
  key_p->coords.y = l_p->points[2].y;
  key_p->coords.z = l_p->points[2].z;
  key_p->dist_from_hit = 0.0;
  key_p = (KeyPointRec*)add_to_tail(temp_list, make_node(sizeof(KeyPointRec)));
  key_p->coords.x = l_p->points[3].x;
  key_p->coords.y = l_p->points[3].y;
  key_p->coords.z = l_p->points[3].z;
  key_p->dist_from_hit = 0.0;
  return temp_list;
}



static GraphicObj *
Reshape(GraphicObj *gp, EHandleNum hn, WCRec *pos)
{
  ECBezC3DWCRec *_p;
  int i;
  
  _p = (ECBezC3DWCRec *)(gp->spec.data);

  i = hn - 1;
  if (hn < 1 || hn > 4) 
    i = 0;

  _p->points[i].x = pos->x;
  _p->points[i].y = pos->y;
  _p->points[i].z = pos->z;
  return gp;
}



static EHandleNum   
ReshapeHandle(EView *v_p, GraphicObj *p, WCRec *nearp)
{
  ECBezC3DWCRec *l_p;
  VCRec nvc, pvc[4];
  FPNum d1, d2, d3, d4;

  l_p       = (ECBezC3DWCRec *)(p->spec.data);
  EVWCtoVC(v_p, nearp, &nvc);
  EVWCtoVC(v_p, &(l_p->points[0]), &(pvc[0]));
  EVWCtoVC(v_p, &(l_p->points[1]), &(pvc[1]));
  EVWCtoVC(v_p, &(l_p->points[2]), &(pvc[2]));
  EVWCtoVC(v_p, &(l_p->points[3]), &(pvc[3]));

  d1 = DistOfPnts(pvc[0].u, pvc[0].v, nvc.u, nvc.v);
  d2 = DistOfPnts(pvc[1].u, pvc[1].v, nvc.u, nvc.v);
  d3 = DistOfPnts(pvc[2].u, pvc[2].v, nvc.u, nvc.v);
  d4 = DistOfPnts(pvc[3].u, pvc[3].v, nvc.u, nvc.v);
  if (d1 < d2) {
    if (d1 < d3) {
      if (d1 < d4) 
	return (EHandleNum)1;
      else
	return (EHandleNum)4;
    } else {
      if (d3 < d4)
	return (EHandleNum)3;
      else
	return (EHandleNum)4;
    }
  } else {
    if (d2 < d3)
      if (d2 < d4)
	return (EHandleNum)2;
      else
	return (EHandleNum)4;
    else
      if (d3 < d4)
	return (EHandleNum)3;
      else
	return (EHandleNum)4;
  }
}



static void 
ShowHandles(EView *v_p, GraphicObj *p)
{
  ECBezC3DWCRec *l_p;

  l_p = (ECBezC3DWCRec *)p->spec.data;
  EGDrawHandle(v_p, &(l_p->points[0]));
  EGDrawHandle(v_p, &(l_p->points[1]));
  EGDrawHandle(v_p, &(l_p->points[2]));
  EGDrawHandle(v_p, &(l_p->points[3]));
}
  


static void 
EraseHandles(EView *v_p, GraphicObj *p)
{
  ECBezC3DWCRec *l_p;

  l_p = (ECBezC3DWCRec *)p->spec.data;
  EGEraseHandle(v_p, &(l_p->points[0]));
  EGEraseHandle(v_p, &(l_p->points[1]));
  EGEraseHandle(v_p, &(l_p->points[2]));
  EGEraseHandle(v_p, &(l_p->points[3]));
}




static void 
XYofHandle(GraphicObj *p, EHandleNum hn, WCRec *pos)
{
  ECBezC3DWCRec *l_p;
  int i;

  l_p = (ECBezC3DWCRec *)p->spec.data;
  i = hn - 1;
  if (hn < 1 || hn > 4)
    i = 0;
  pos->x = l_p->points[i].x;
  pos->y = l_p->points[i].y;
  pos->z = l_p->points[i].z;
}



static void 
RetrieveFrom(FILE *file_p, GraphicSpecObj *gsp)
{
  ECBezC3DWCRec *p;
  int style, width, sp, sc, i, ti;
  char color[64];
  EPixel pixel;
  BOOLEAN success;
  double x[4], y[4], z[4];
  
  if ((p = (ECBezC3DWCRec *)make_node(sizeof(ECBezC3DWCRec))) != NULL) {
    fscanf(file_p,
	   "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
	   &x[0], &y[0], &z[0],
	   &x[1], &y[1], &z[1],
	   &x[2], &y[2], &z[2],
	   &x[3], &y[3], &z[3]);
    fscanf(file_p,
	   "%d %d %d %d %d %d %s",
	   &(p->attributes.spec.have_any),
	   &sp, &sc, &width, &style, &ti, color);
    for (i = 0; i < 4; i++) {
      p->points[i].x = x[i];
      p->points[i].y = y[i];
      p->points[i].z = z[i];
    }
    p->attributes.spec.show_polygon     = sp;
    p->attributes.spec.show_curve       = sc;
    p->attributes.spec.width            = width;
    p->attributes.spec.style            = style;
    p->attributes.spec.tessel_intervals = ti;
    pixel = ColorGetPixelFromString(color, &success);
    if (success)
      p->attributes.spec.color = pixel;
  }
  gsp->data        = (caddr_t)p;
  gsp->data_length = sizeof(ECBezC3DWCRec);
}
 


static void 
StoreOn(FILE *file_p, GraphicObj *g_p)
{
  ECBezC3DWCRec *p;
  int style, width, sp, sc, ti;
  char *color;

  p = (ECBezC3DWCRec *)g_p->spec.data;
  if (p->attributes.spec.have_any == YES) {
    style = p->attributes.spec.style;
    width = p->attributes.spec.width;
    sp    = p->attributes.spec.show_polygon;
    sc    = p->attributes.spec.show_curve;
    ti    = p->attributes.spec.tessel_intervals;
    color = ColorGetStringFromPixel(p->attributes.spec.color);
    if (color == NULL)
      color = "black";
  } else {
    style = LineSolid;
    sp    = YES;
    sc    = YES;
    ti    = 31;
    width = 0;
    color = "black";
  }
  fprintf(file_p,
	/*       0         1        2        3 */
	  "%f %f %f %f %f %f %f %f %f %f %f %f \n %d %d %d %d %d %d %s\n",
	  p->points[0].x, p->points[0].y, p->points[0].z,
	  p->points[1].x, p->points[1].y, p->points[1].z,
	  p->points[2].x, p->points[2].y, p->points[2].z,
	  p->points[3].x, p->points[3].y, p->points[3].z,
	  p->attributes.spec.have_any,
	  sp, sc, width, style, ti, color);
}



static void 
ChangeAttributes(GraphicObj *p)
{
  ECBezC3DWCRec *dp;
  unsigned long mask;

  mask = EASValsGetChangeMask();
  dp = (ECBezC3DWCRec *)p->spec.data;
  if ((mask & SHOW_POLY_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.show_polygon = EASValsGetShowPoly();
    dp->attributes.spec.have_any     = YES;
  }
  if ((mask & SHOW_ENTITY_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.show_curve   = EASValsGetShowEntity();
    dp->attributes.spec.have_any     = YES;
  }
  if ((mask & TESSEL_INTERVALS_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.tessel_intervals = EASValsGetTesselIntervals();
    dp->attributes.spec.have_any         = YES;
  }
  if ((mask & STYLE_MASK) || mask & (ALL_ATTRIB_MASK)) {
    dp->attributes.spec.style    = EASValsGetLineStyle();
    dp->attributes.spec.have_any = YES;
  }
  if ((mask & WIDTH_MASK) || mask & (ALL_ATTRIB_MASK)) {
    dp->attributes.spec.width    = EASValsGetLineWidth();
    dp->attributes.spec.have_any = YES;
  }
  if ((mask & COLOR_MASK) || mask & (ALL_ATTRIB_MASK)) {
    dp->attributes.spec.color    = EASValsGetColor();
    dp->attributes.spec.have_any = YES;
  }
}
			       


static void 
CopyAttributesToActiveSet(GraphicObj *p)
{
  ECBezC3DWCRec *dp;

  dp = (ECBezC3DWCRec *)p->spec.data;

  if (dp->attributes.spec.have_any) {
    EASValsSetLineStyle(dp->attributes.spec.style);
    EASValsSetLineWidth(dp->attributes.spec.width);
    EASValsSetColor(dp->attributes.spec.color);
    EASValsSetShowPoly(dp->attributes.spec.show_polygon);
    EASValsSetShowEntity(dp->attributes.spec.show_curve);
    EASValsSetTesselIntervals(dp->attributes.spec.tessel_intervals);
  }
}


static BOOLEAN
AttribValAsInAS(GraphicObj *p, unsigned long mask)  
{
  ECBezC3DWCRec *dp;

  dp = (ECBezC3DWCRec *)p->spec.data;

  if ((mask & SHOW_POLY_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.show_polygon != EASValsGetShowPoly())
      return NO;

  if ((mask & SHOW_ENTITY_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.show_curve   != EASValsGetShowEntity())
      return NO;

  if ((mask & TESSEL_INTERVALS_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.tessel_intervals != EASValsGetTesselIntervals())
      return NO;

  if ((mask & STYLE_MASK) || mask & (ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.style    != EASValsGetLineStyle())
      return NO;

  if ((mask & WIDTH_MASK) || mask & (ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.width    != EASValsGetLineWidth())
      return NO;

  if ((mask & COLOR_MASK) || mask & (ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.color    != EASValsGetColor())
      return NO;

  return YES;
}


#define ASSIGN_POINT_TO_POINT(DEST, SRC)                         \
  {                                                              \
     (DEST).x = (SRC).x; (DEST).y = (SRC).y; (DEST).z = (SRC).z; \
   }

GraphicObj *
LineToCBezC3D(GraphicObj *line)
{
  GraphicObj *g_p;
  WCRec lpts[2], pts[4], p;
  int nlpts;

  EGGetGraphicsGeometry(line, lpts, &nlpts);
  ASSIGN_POINT_TO_POINT(pts[0], lpts[0]);
  p.x = (2 * lpts[0].x + lpts[1].x) / 3; 
  p.y = (2 * lpts[0].y + lpts[1].y) / 3; 
  p.z = (2 * lpts[0].z + lpts[1].z) / 3;
  ASSIGN_POINT_TO_POINT(pts[1], p);
  p.x = (lpts[0].x + 2 * lpts[1].x) / 3; 
  p.y = (lpts[0].y + 2 * lpts[1].y) / 3; 
  p.z = (lpts[0].z + 2 * lpts[1].z) / 3;
  ASSIGN_POINT_TO_POINT(pts[2], p);
  ASSIGN_POINT_TO_POINT(pts[3], lpts[1]);

  g_p = CreateCBezC3D(pts);
  return g_p;
}

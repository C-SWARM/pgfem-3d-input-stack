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

#define BCBEZS3D_PRIVATE_HEADER
#include "Ebcbezs3d.h"


  
/* ========================================================================= */
/* PRIVATE  */
/* ========================================================================= */

#define STRING_BUFF_SIZE 256

static char string_rep_buffer[STRING_BUFF_SIZE];

/* ========================================================================= */
/* PUBLIC  */
/* ========================================================================= */

GraphicObj *
CreateBCBezS3D(WCRec *points)
{
  EBCBezS3DWCRec *thedp;
  GraphicObj *g_p;

  thedp = (EBCBezS3DWCRec *)make_node(sizeof(EBCBezS3DWCRec));
  g_p = EGCreateGraphics(&bcbezs3d_graphic_methods, (caddr_t)thedp,
			 sizeof(EBCBezS3DWCRec));
  ModifyGeometry(g_p, points);
  thedp->attributes.spec.tessel_intervals = 4;
  thedp->attributes.spec.show_polygon     = YES;
  thedp->attributes.spec.show_surface     = YES;
  thedp->attributes.spec.fill_style       = FILL_SOLID;
  thedp->attributes.spec.edge_flag        = NO;
  thedp->attributes.spec.edge_color       = 0x0;
  thedp->attributes.spec.color            = 0x0;
  thedp->attributes.spec.have_any         = NO;
  return g_p;
}

 

static void 
Draw(EView *v_p, GraphicObj *p)
{
  EBCBezS3DWCRec *thedp;

  thedp = (EBCBezS3DWCRec *)(p->spec.data);

  EVBCBezS3DPrimitive(v_p, p, thedp->points, thedp->attributes.spec.color,
		      thedp->attributes.spec.fill_style,
                      thedp->attributes.spec.edge_flag,
                      thedp->attributes.spec.edge_color,
		      thedp->attributes.spec.tessel_intervals,
		      thedp->attributes.spec.show_polygon,
		      NO,
		      DRAW_MODE_DRAW);
}



static void 
BoundingBoxVC(EView *v_p, GraphicObj *p, VCRec *ll, VCRec *ur)
{
  EBCBezS3DWCRec *thedp;
  VCRec vcpnts[4][4];
  int i, j;
  
  thedp = (EBCBezS3DWCRec *)(p->spec.data);

  for (i = 0; i < 4; i++)
    for (j = 0; j < 4; j++) 
      EVWCtoVC(v_p, &(thedp->points[i][j]), &(vcpnts[i][j]));

  ll->u = vcpnts[0][0].u;
  ll->v = vcpnts[0][0].v;
  ll->n = vcpnts[0][0].n;
  ur->u = vcpnts[0][0].u;
  ur->v = vcpnts[0][0].v;
  ur->n = vcpnts[0][0].n;
  for (i = 0; i < 4; i++)
    for (j = 0; j < 4; j++) {
      ll->u = min(vcpnts[i][j].u, ll->u);
      ll->v = min(vcpnts[i][j].v, ll->v);
      ll->n = min(vcpnts[i][j].n, ll->n);
      ur->u = max(vcpnts[i][j].u, ur->u);
      ur->v = max(vcpnts[i][j].v, ur->v);
      ur->n = max(vcpnts[i][j].n, ur->n);
    }
}
  


static void 
BoundingBoxWC(GraphicObj *p, WCRec *ll, WCRec *ur)
{
  EBCBezS3DWCRec *thedp;
  int i, j;
  
  thedp = (EBCBezS3DWCRec *)(p->spec.data);

  ll->x = thedp->points[0][0].x;
  ll->y = thedp->points[0][0].y;
  ll->z = thedp->points[0][0].z;
  ur->x = thedp->points[0][0].x;
  ur->y = thedp->points[0][0].y;
  ur->z = thedp->points[0][0].z;
  for (i = 0; i < 4; i++)
    for (j = 0; j < 4; j++) {
      ll->x = min(thedp->points[i][j].x, ll->x);
      ll->y = min(thedp->points[i][j].y, ll->y);
      ll->z = min(thedp->points[i][j].z, ll->z);
      ur->x = max(thedp->points[i][j].x, ur->x);
      ur->y = max(thedp->points[i][j].y, ur->y);
      ur->z = max(thedp->points[i][j].z, ur->z);
    }
}



static void 
XorDraw(EView *v_p, GraphicObj *g_p)
{
  EBCBezS3DWCRec *thedp;

  thedp = (EBCBezS3DWCRec *)(g_p->spec.data);
  EVBCBezS3DPrimitive(v_p,  g_p, thedp->points, thedp->attributes.spec.color,
		      FILL_HOLLOW, NO, 0,
		      thedp->attributes.spec.tessel_intervals,
		      thedp->attributes.spec.show_polygon,
		      NO,
		      DRAW_MODE_XORDRAW);
}

 


static GraphicObj *
ModifyGeometry(GraphicObj *g_p, WCRec *points)
{
  EBCBezS3DWCRec *thedp;
  int i, j;

  thedp = (EBCBezS3DWCRec *)(g_p->spec.data);

  for (i = 0; i < 4; i++)
    for (j = 0; j < 4; j++) {
      thedp->points[i][j].x = points[i*4+j].x; 
      thedp->points[i][j].y = points[i*4+j].y;
      thedp->points[i][j].z = points[i*4+j].z;
    }
  return g_p;
}


static BOOLEAN 
GetGeometry(GraphicObj *g_p, WCRec *points,
                              int *npoints)
{
  EBCBezS3DWCRec *thedp = (EBCBezS3DWCRec *)(g_p->spec.data);
  int i, j;

  for (i = 0; i < 4; i++)
    for (j = 0; j < 4; j++) {
      points[i*4+j].x = thedp->points[i][j].x; 
      points[i*4+j].y = thedp->points[i][j].y;
      points[i*4+j].z = thedp->points[i][j].z;
    }
  *npoints = 16;
  return YES;
}


static void 
Hilite(EView *v_p, GraphicObj *g_p)
{
  EBCBezS3DWCRec *thedp;

  thedp = (EBCBezS3DWCRec *)(g_p->spec.data);
  EVBCBezS3DPrimitive(v_p,  g_p, thedp->points, thedp->attributes.spec.color,
		      FILL_HOLLOW, NO, 0,
		      2,
		      thedp->attributes.spec.show_polygon,
		      NO,
		      DRAW_MODE_HILITE);
}



static void 
Unhilite(EView *v_p, GraphicObj *g_p)
{
  EBCBezS3DWCRec *thedp;

  thedp = (EBCBezS3DWCRec *)(g_p->spec.data);
  EVBCBezS3DPrimitive(v_p,  g_p, thedp->points, thedp->attributes.spec.color,
		      FILL_HOLLOW, NO, 0,
		      2,
		      thedp->attributes.spec.show_polygon,
		      NO,
		      DRAW_MODE_UNHILITE);
}



static void 
Erase(EView *v_p, GraphicObj *g_p)
{
  EBCBezS3DWCRec *thedp;
  
  thedp = (EBCBezS3DWCRec *)(g_p->spec.data);

  EVBCBezS3DPrimitive(v_p,  g_p, thedp->points, thedp->attributes.spec.color,
		      thedp->attributes.spec.fill_style, NO, 0,
		      thedp->attributes.spec.tessel_intervals,
		      thedp->attributes.spec.show_polygon,
		      NO,
		      DRAW_MODE_ERASE);  
}



static GraphicObj *
Translate(GraphicObj *g_p, WCRec *by)
{
  EBCBezS3DWCRec *p;
  int i, j;

  p = (EBCBezS3DWCRec *)g_p->spec.data;
  for (i = 0; i < 4; i++)
    for (j = 0; j < 4; j++) {
      p->points[i][j].x += by->x;
      p->points[i][j].y += by->y;
      p->points[i][j].z += by->z;
    }
  return g_p;
}



static GraphicObj *
Rotate(GraphicObj *g_p, WCRec *center, WCRec *axial)
{
  EBCBezS3DWCRec *c;
  WCRec x_c;
  int i, j;

  c = (EBCBezS3DWCRec *)g_p->spec.data;
  for (i = 0; i < 4; i++ ) 
    for (j = 0; j < 4; j++) {
      x_c.x = c->points[i][j].x - center->x;
      x_c.y = c->points[i][j].y - center->y;
      x_c.z = c->points[i][j].z - center->z;
      RotVectAboutVect(axial, &x_c);
      c->points[i][j].x = center->x + x_c.x;
      c->points[i][j].y = center->y + x_c.y;
      c->points[i][j].z = center->z + x_c.z;
    }
  return g_p;
}



static GraphicObj  *
MirrorPlane(GraphicObj *g_p,
                    WCRec *point_on_plane, WCRec *unit_plane_normal)
{
  EBCBezS3DWCRec *dp;
  int i, j;

  dp = (EBCBezS3DWCRec *)g_p->spec.data;
  for (i = 0; i < 4; i++)
    for (j = 0; j < 4; j++)
      MirrorPointInPlane(point_on_plane, unit_plane_normal, &dp->points[i][j]);
  
  return g_p;
}



static GraphicObj  *
MirrorCenter(GraphicObj *g_p, WCRec *center)
{
  EBCBezS3DWCRec *dp;
  int i, j;

  dp = (EBCBezS3DWCRec *)g_p->spec.data;
  for (i = 0; i < 4; i++)
    for (j = 0; j < 4; j++) {
      dp->points[i][j].x = 2.*center->x - dp->points[i][j].x;
      dp->points[i][j].y = 2.*center->y - dp->points[i][j].y;
      dp->points[i][j].z = 2.*center->z - dp->points[i][j].z;
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
  EBCBezS3DWCRec *thedp;
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
    thedp = (EBCBezS3DWCRec *)p->spec.data;
    for (i = 0; i < 4; i++) {
      EVWCtoVC(v_p, &(thedp->points[i][0]), &pvc);
      points[i].x = pvc.u; points[i].y = pvc.v;
    }
    if (BoxIntersectsCBezCurve(box, points))
      return YES;
    for (i = 0; i < 4; i++) {
      EVWCtoVC(v_p, &(thedp->points[i][3]), &pvc);
      points[i].x = pvc.u; points[i].y = pvc.v;
    }
    if (BoxIntersectsCBezCurve(box, points))
      return YES;
    for (i = 0; i < 4; i++) {
      EVWCtoVC(v_p, &(thedp->points[0][i]), &pvc);
      points[i].x = pvc.u; points[i].y = pvc.v;
    }
    if (BoxIntersectsCBezCurve(box, points))
      return YES;
    for (i = 0; i < 4; i++) {
      EVWCtoVC(v_p, &(thedp->points[3][i]), &pvc);
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
  return EG_BCBEZSURFACE3D;
}



static char *
AsString(GraphicObj *g_p)
{
  EBCBezS3DWCRec *l;

  l = (EBCBezS3DWCRec *)g_p->spec.data;
  sprintf(string_rep_buffer,
	  "BCBEZSURFACE3D [%f,%f,%f][%f,%f,%f][%f,%f,%f][%f,%f,%f]\n"
	  "               [%f,%f,%f][%f,%f,%f][%f,%f,%f][%f,%f,%f]\n"
	  "               [%f,%f,%f][%f,%f,%f][%f,%f,%f][%f,%f,%f]\n"
	  "               [%f,%f,%f][%f,%f,%f][%f,%f,%f][%f,%f,%f]",
	  l->points[0][0].x, l->points[0][0].y, l->points[0][0].z,
	  l->points[0][1].x, l->points[0][1].y, l->points[0][1].z,
	  l->points[0][2].x, l->points[0][2].y, l->points[0][2].z,
	  l->points[0][3].x, l->points[0][3].y, l->points[0][3].z,
	  l->points[1][0].x, l->points[1][0].y, l->points[1][0].z,
	  l->points[1][1].x, l->points[1][1].y, l->points[1][1].z,
	  l->points[1][2].x, l->points[1][2].y, l->points[1][2].z,
	  l->points[1][3].x, l->points[1][3].y, l->points[1][3].z,
	  l->points[2][0].x, l->points[2][0].y, l->points[2][0].z,
	  l->points[2][1].x, l->points[2][1].y, l->points[2][1].z,
	  l->points[2][2].x, l->points[2][2].y, l->points[2][2].z,
	  l->points[2][3].x, l->points[2][3].y, l->points[2][3].z,
	  l->points[3][0].x, l->points[3][0].y, l->points[3][0].z,
	  l->points[3][1].x, l->points[3][1].y, l->points[3][1].z,
	  l->points[3][2].x, l->points[3][2].y, l->points[3][2].z,
	  l->points[3][3].x, l->points[3][3].y, l->points[3][3].z); 
  
  return string_rep_buffer; /* Don't free this memory!!! */
}



static LIST 
KeyPoints(GraphicObj *p)
{
  LIST temp_list;
  KeyPointRec *key_p;
  EBCBezS3DWCRec *l_p;
  int i, j;

  l_p       = (EBCBezS3DWCRec *)(p->spec.data);
  temp_list = make_list();

  for (i = 0; i < 4; i++)
    for (j = 0; j < 4; j++) {
      key_p = (KeyPointRec*)add_to_tail(temp_list,
					make_node(sizeof(KeyPointRec)));
      key_p->coords.x = l_p->points[i][j].x;
      key_p->coords.y = l_p->points[i][j].y;
      key_p->coords.z = l_p->points[i][j].z;
      key_p->dist_from_hit = 0.0;
    }
  return temp_list;
}



static GraphicObj *
Reshape(GraphicObj *gp, EHandleNum hn, WCRec *pos)
{
  EBCBezS3DWCRec *_p;
  int i, j;
  
  _p = (EBCBezS3DWCRec *)(gp->spec.data);

  i = (hn-1)/4;
  j = (hn-1) - i * 4;
  if (hn < 1 || hn > 16) 
    i = 0, j = 0;

  _p->points[i][j].x = pos->x;
  _p->points[i][j].y = pos->y;
  _p->points[i][j].z = pos->z;
  return gp;
}



static EHandleNum   
ReshapeHandle(EView *v_p, GraphicObj *p, WCRec *nearp)
{
  EBCBezS3DWCRec *l_p;
  VCRec nvc, pvc;
  FPNum d, tmp;
  EHandleNum hn;
  int i, j;

  l_p       = (EBCBezS3DWCRec *)(p->spec.data);
  EVWCtoVC(v_p, nearp, &nvc);

  hn = 1;
  EVWCtoVC(v_p, &(l_p->points[0][0]), &pvc);
  d = DistOfPnts(pvc.u, pvc.v, nvc.u, nvc.v);
  for (i = 0; i < 4; i++)
    for (j = 0; j < 4; j++) {
      EVWCtoVC(v_p, &(l_p->points[i][j]), &pvc);
      if (d > (tmp = DistOfPnts(pvc.u, pvc.v, nvc.u, nvc.v)))
	hn = i * 4 + j + 1, d = tmp;
    }
  return hn;
}



static void 
ShowHandles(EView *v_p, GraphicObj *p)
{
  EBCBezS3DWCRec *l_p;
  int i, j;

  l_p = (EBCBezS3DWCRec *)p->spec.data;

  for (i = 0; i < 4; i++)
    for (j = 0; j < 4; j++) 
      EGDrawHandle(v_p, &(l_p->points[i][j]));
}
  


static void 
EraseHandles(EView *v_p, GraphicObj *p)
{
  EBCBezS3DWCRec *l_p;
  int i, j;

  l_p = (EBCBezS3DWCRec *)p->spec.data;

  for (i = 0; i < 4; i++)
    for (j = 0; j < 4; j++) 
      EGEraseHandle(v_p, &(l_p->points[i][j]));
}




static void 
XYofHandle(GraphicObj *p, EHandleNum hn, WCRec *pos)
{
  EBCBezS3DWCRec *l_p;
  int i, j;

  l_p = (EBCBezS3DWCRec *)p->spec.data;
  i = (hn-1)/4;
  j = (hn-1) - i * 4;
  if (hn < 1 || hn > 16)
    i = 0, j = 0;
  pos->x = l_p->points[i][j].x;
  pos->y = l_p->points[i][j].y;
  pos->z = l_p->points[i][j].z;
}



static void 
RetrieveFrom(FILE *file_p, GraphicSpecObj  *gsp)
{
  EBCBezS3DWCRec *p;
  int fill, eflag, sp, sc, ti;
  char color[64];
  EPixel pixel;
  BOOLEAN success;
  double x, y, z;
  
  if ((p = (EBCBezS3DWCRec *)make_node(sizeof(EBCBezS3DWCRec))) != NULL) {
    fscanf(file_p, "%lf %lf %lf ", &x, &y, &z);
    p->points[0][0].x = x;
    p->points[0][0].y = y;
    p->points[0][0].z = z;
    fscanf(file_p, "%lf %lf %lf ", &x, &y, &z);
    p->points[0][1].x = x;
    p->points[0][1].y = y;
    p->points[0][1].z = z;
    fscanf(file_p, "%lf %lf %lf ", &x, &y, &z);
    p->points[0][2].x = x;
    p->points[0][2].y = y;
    p->points[0][2].z = z;
    fscanf(file_p, "%lf %lf %lf ", &x, &y, &z);
    p->points[0][3].x = x;
    p->points[0][3].y = y;
    p->points[0][3].z = z;
    fscanf(file_p, "%lf %lf %lf ", &x, &y, &z);
    p->points[1][0].x = x;
    p->points[1][0].y = y;
    p->points[1][0].z = z;
    fscanf(file_p, "%lf %lf %lf ", &x, &y, &z);
    p->points[1][1].x = x;
    p->points[1][1].y = y;
    p->points[1][1].z = z;
    fscanf(file_p, "%lf %lf %lf ", &x, &y, &z);
    p->points[1][2].x = x;
    p->points[1][2].y = y;
    p->points[1][2].z = z;
    fscanf(file_p, "%lf %lf %lf ", &x, &y, &z);
    p->points[1][3].x = x;
    p->points[1][3].y = y;
    p->points[1][3].z = z;
    fscanf(file_p, "%lf %lf %lf ", &x, &y, &z);
    p->points[2][0].x = x;
    p->points[2][0].y = y;
    p->points[2][0].z = z;
    fscanf(file_p, "%lf %lf %lf ", &x, &y, &z);
    p->points[2][1].x = x;
    p->points[2][1].y = y;
    p->points[2][1].z = z;
    fscanf(file_p, "%lf %lf %lf ", &x, &y, &z);
    p->points[2][2].x = x;
    p->points[2][2].y = y;
    p->points[2][2].z = z;
    fscanf(file_p, "%lf %lf %lf ", &x, &y, &z);
    p->points[2][3].x = x;
    p->points[2][3].y = y;
    p->points[2][3].z = z;
    fscanf(file_p, "%lf %lf %lf ", &x, &y, &z);
    p->points[3][0].x = x;
    p->points[3][0].y = y;
    p->points[3][0].z = z;
    fscanf(file_p, "%lf %lf %lf ", &x, &y, &z);
    p->points[3][1].x = x;
    p->points[3][1].y = y;
    p->points[3][1].z = z;
    fscanf(file_p, "%lf %lf %lf ", &x, &y, &z);
    p->points[3][2].x = x;
    p->points[3][2].y = y;
    p->points[3][2].z = z;
    fscanf(file_p, "%lf %lf %lf ", &x, &y, &z);
    p->points[3][3].x = x;
    p->points[3][3].y = y;
    p->points[3][3].z = z;
    fscanf(file_p, "%d %d %d %d %d %d",
	   &(p->attributes.spec.have_any),
	   &sp, &sc, &fill, &eflag, &ti);
    p->attributes.spec.show_polygon     = sp;
    p->attributes.spec.show_surface     = sc;
    p->attributes.spec.fill_style       = fill;
    p->attributes.spec.edge_flag        = eflag;
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
  gsp->data_length = sizeof(EBCBezS3DWCRec);
}
 


static void 
StoreOn(FILE *file_p, GraphicObj *g_p)
{
  EBCBezS3DWCRec *p;
  int fill, sp, sc, ti, eflag;
  char *color, *edge_color;

  p = (EBCBezS3DWCRec *)g_p->spec.data;
  if (p->attributes.spec.have_any == YES) {
    fill  = p->attributes.spec.fill_style;
    eflag = p->attributes.spec.edge_flag;
    sp    = p->attributes.spec.show_polygon;
    sc    = p->attributes.spec.show_surface;
    ti    = p->attributes.spec.tessel_intervals;
    color = ColorGetStringFromPixel(p->attributes.spec.color);
    if (color == NULL)
      color = "black";
    edge_color = ColorGetStringFromPixel(p->attributes.spec.edge_color);
    if (edge_color == NULL)
      edge_color = "white";
  } else {
    fill  = FILL_HOLLOW;
    eflag = 0;
    sp    = YES;
    sc    = YES;
    ti    = 17;
    color = edge_color = "black";
  }
  fprintf(file_p,
	  " %f %f %f \n %f %f %f \n %f %f %f \n %f %f %f \n", 
	  p->points[0][0].x, p->points[0][0].y, p->points[0][0].z,
	  p->points[0][1].x, p->points[0][1].y, p->points[0][1].z,
	  p->points[0][2].x, p->points[0][2].y, p->points[0][2].z,
	  p->points[0][3].x, p->points[0][3].y, p->points[0][3].z);
  fprintf(file_p,
	  " %f %f %f \n %f %f %f \n %f %f %f \n %f %f %f \n", 
	  p->points[1][0].x, p->points[1][0].y, p->points[1][0].z,
	  p->points[1][1].x, p->points[1][1].y, p->points[1][1].z,
	  p->points[1][2].x, p->points[1][2].y, p->points[1][2].z,
	  p->points[1][3].x, p->points[1][3].y, p->points[1][3].z);
  fprintf(file_p,
	  " %f %f %f \n %f %f %f \n %f %f %f \n %f %f %f \n", 
	  p->points[2][0].x, p->points[2][0].y, p->points[2][0].z,
	  p->points[2][1].x, p->points[2][1].y, p->points[2][1].z,
	  p->points[2][2].x, p->points[2][2].y, p->points[2][2].z,
	  p->points[2][3].x, p->points[2][3].y, p->points[2][3].z);
  fprintf(file_p,
	  " %f %f %f \n %f %f %f \n %f %f %f \n %f %f %f \n", 
	  p->points[3][0].x, p->points[3][0].y, p->points[3][0].z,
	  p->points[3][1].x, p->points[3][1].y, p->points[3][1].z,
	  p->points[3][2].x, p->points[3][2].y, p->points[3][2].z,
	  p->points[3][3].x, p->points[3][3].y, p->points[3][3].z);
  fprintf(file_p, "%d %d %d %d %d %d\n%s\n%s\n",
	  p->attributes.spec.have_any,
	  sp, sc, fill, eflag, ti, color, edge_color);
}



static void 
ChangeAttributes(GraphicObj *p)
{
  EBCBezS3DWCRec *dp;
  unsigned long mask;

  mask = EASValsGetChangeMask();
  dp = (EBCBezS3DWCRec *)p->spec.data;
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
  EBCBezS3DWCRec *dp;

  dp = (EBCBezS3DWCRec *)p->spec.data;

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
  EBCBezS3DWCRec *dp;

  dp = (EBCBezS3DWCRec *)p->spec.data;

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

#define EPS 0.00001
#define POINTS_IDENTICAL(P1, P2)  \
     (   fabs((P1).x - (P2).x) < EPS  \
      && fabs((P1).y - (P2).y) < EPS  \
      && fabs((P1).z - (P2).z) < EPS)

/* Error status codes */
#define LOOP_NOT_CLOSED                -1

#define INVALID -1
#define NONE -1
#define START 0
#define END   3

static int status = 0; /* status of the operation */

typedef enum { UNDEF=0, AS_DEF=1, REV=-1 } curve_sense;

typedef struct curve_in_loop {
  int           id;
  curve_sense   sense;
}   curve_in_loop;


typedef struct loop_rec {
  int           how_many;
  curve_in_loop c[4];           /* curve numbers (or NONE) */
}   loop_rec;

static loop_rec loop;

static WCRec cp[4][4]; /* curve definition points */

static int
construct_loop(GraphicObj *c[], int how_many);
static void
bcbezs_points(WCRec points[4][4]);

GraphicObj *
CurvesToBCBezS3D(GraphicObj *c[], int how_many)
{
  GraphicObj *patch = NULL;
  int i, npts;
  WCRec points[4][4];

  /* the geometry */
  for (i = 0; i < how_many; i++)     
    if (!EGGetGraphicsGeometry(c[i], &cp[i][0], &npts))
      return NULL;
  
  if (!construct_loop(c, how_many)) {
    status = LOOP_NOT_CLOSED;
    return NULL;
  }
  
  /* compute points of the patch */
  bcbezs_points(points);

  patch = CreateBCBezS3D((WCRec *)points);
  return patch;
}

static BOOLEAN
common_vertex(int curve1, int curve2,
              WCRec *p, int *which1, int *which2);


static WCRec toreach, curp;

#define CID(CURVENUM) (loop.c[CURVENUM].id)
#define CS(CURVENUM) (loop.c[CURVENUM].sense)
#define CLOSED(aPoint) (POINTS_IDENTICAL(toreach, aPoint))
#define AT_END(CNum)   ((CS(CNum) == AS_DEF)? END: START)
#define AT_START(CNum) ((CS(CNum) == AS_DEF)? START: END)
#define POINT_ALONG_AT_END(CNum)   (cp[CID(CNum)][AT_END(CNum)])
#define POINT_ALONG_AT_START(CNum) (cp[CID(CNum)][AT_START(CNum)])

static int
construct_loop(GraphicObj *c[], int how_many)
{
  int i, w1, w2;
  WCRec p;

  for (i = 0; i < 4; i++) {
    loop.c[i].id = INVALID;
    loop.c[i].sense = UNDEF;
  }

  for (i = 1; i < how_many; i++) {
    if (common_vertex(0, i, &p, &w1, &w2)) {
      loop.c[0].id    = 0;
      loop.c[0].sense = (w1 == END? AS_DEF: REV);
      ASSIGN_POINT_TO_POINT(toreach, POINT_ALONG_AT_START(0));
      loop.c[1].id    = i;
      loop.c[1].sense = (w2 == START? AS_DEF: REV);
      ASSIGN_POINT_TO_POINT(curp, POINT_ALONG_AT_END(1));
      loop.how_many = 2;
      goto found1;
    }
  }
  status = LOOP_NOT_CLOSED;
  return 0;

 found1:
  if (CLOSED(curp) && how_many == 2) {
    return 1;
  } else if (!CLOSED(curp) && how_many >= 3) {
    for (i = 1; i < how_many; i++) {
      if (i != CID(1)) {
        if (common_vertex(CID(1), i, &p, &w1, &w2)) {
          loop.c[2].id    = i;
          loop.c[2].sense = (w2 == START? AS_DEF: REV);
          ASSIGN_POINT_TO_POINT(curp, POINT_ALONG_AT_END(2));
          loop.how_many = 3;
          goto found2;
        }
      }
    }
  }
  status = LOOP_NOT_CLOSED;
  return 0;
  
 found2:
  if (CLOSED(curp) && how_many == 3) {
    return 1;
  } else if (!CLOSED(curp) && how_many >= 4) {
    for (i = 1; i < how_many; i++) {
      if (i != CID(1) && i != CID(2)) {
        if (common_vertex(CID(2), i, &p, &w1, &w2)) {
          loop.c[3].id    = i;
          loop.c[3].sense = (w2 == START? AS_DEF: REV);
          ASSIGN_POINT_TO_POINT(curp, POINT_ALONG_AT_END(3));
          loop.how_many = 4;
          goto found3;
        }
      }
    }
  }
  status = LOOP_NOT_CLOSED;
  return 0;

 found3:
  return (CLOSED(curp));
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
bcbezs_points(WCRec points[4][4])
{
  int i0, i1, i2, i3, cn;
  WCRec by;
  
  switch (loop.how_many) {
  /* regular topology */
  case 4:
    cn = 0;
    if (loop.c[cn].sense == AS_DEF) { i0 = 0; i1 = 1; i2 = 2; i3 = 3; }
    else                            { i0 = 3; i1 = 2; i2 = 1; i3 = 0; }
    ASSIGN_POINT_TO_POINT(points[0][0], cp[loop.c[cn].id][i0]);
    ASSIGN_POINT_TO_POINT(points[1][0], cp[loop.c[cn].id][i1]);
    ASSIGN_POINT_TO_POINT(points[2][0], cp[loop.c[cn].id][i2]);
    ASSIGN_POINT_TO_POINT(points[3][0], cp[loop.c[cn].id][i3]);
    cn = 1;
    if (loop.c[cn].sense == AS_DEF) { i0 = 0; i1 = 1; i2 = 2; i3 = 3; }
    else                            { i0 = 3; i1 = 2; i2 = 1; i3 = 0; }
    ASSIGN_POINT_TO_POINT(points[3][0], cp[loop.c[cn].id][i0]);
    ASSIGN_POINT_TO_POINT(points[3][1], cp[loop.c[cn].id][i1]);
    ASSIGN_POINT_TO_POINT(points[3][2], cp[loop.c[cn].id][i2]);
    ASSIGN_POINT_TO_POINT(points[3][3], cp[loop.c[cn].id][i3]);
    cn = 2;
    if (loop.c[cn].sense == AS_DEF) { i0 = 0; i1 = 1; i2 = 2; i3 = 3; }
    else                            { i0 = 3; i1 = 2; i2 = 1; i3 = 0; }
    ASSIGN_POINT_TO_POINT(points[3][3], cp[loop.c[cn].id][i0]);
    ASSIGN_POINT_TO_POINT(points[2][3], cp[loop.c[cn].id][i1]);
    ASSIGN_POINT_TO_POINT(points[1][3], cp[loop.c[cn].id][i2]);
    ASSIGN_POINT_TO_POINT(points[0][3], cp[loop.c[cn].id][i3]);
    cn = 3;
    if (loop.c[cn].sense == AS_DEF) { i0 = 0; i1 = 1; i2 = 2; i3 = 3; }
    else                            { i0 = 3; i1 = 2; i2 = 1; i3 = 0; }
    ASSIGN_POINT_TO_POINT(points[0][3], cp[loop.c[cn].id][i0]);
    ASSIGN_POINT_TO_POINT(points[0][2], cp[loop.c[cn].id][i1]);
    ASSIGN_POINT_TO_POINT(points[0][1], cp[loop.c[cn].id][i2]);
    ASSIGN_POINT_TO_POINT(points[0][0], cp[loop.c[cn].id][i3]);

    SUBTR_POINTS(by, points[1][0], points[0][0]);
    ADD_UP_POINTS(points[1][1], points[0][1], by);
    SUBTR_POINTS(by, points[2][0], points[3][0]);
    ADD_UP_POINTS(points[2][1], points[3][1], by);
    SUBTR_POINTS(by, points[1][3], points[0][3]);
    ADD_UP_POINTS(points[1][2], points[0][2], by);
    SUBTR_POINTS(by, points[2][3], points[3][3]);
    ADD_UP_POINTS(points[2][2], points[3][2], by);
    break;
  /* one side collapsed */
  case 3:
    cn = 0;
    if (loop.c[cn].sense == AS_DEF) { i0 = 0; i1 = 1; i2 = 2; i3 = 3; }
    else                            { i0 = 3; i1 = 2; i2 = 1; i3 = 0; }
    ASSIGN_POINT_TO_POINT(points[0][0], cp[loop.c[cn].id][i0]);
    ASSIGN_POINT_TO_POINT(points[1][0], cp[loop.c[cn].id][i1]);
    ASSIGN_POINT_TO_POINT(points[2][0], cp[loop.c[cn].id][i2]);
    ASSIGN_POINT_TO_POINT(points[3][0], cp[loop.c[cn].id][i3]);
    cn = 1;
    if (loop.c[cn].sense == AS_DEF) { i0 = 0; i1 = 1; i2 = 2; i3 = 3; }
    else                            { i0 = 3; i1 = 2; i2 = 1; i3 = 0; }
    ASSIGN_POINT_TO_POINT(points[3][0], cp[loop.c[cn].id][i0]);
    ASSIGN_POINT_TO_POINT(points[3][1], cp[loop.c[cn].id][i1]);
    ASSIGN_POINT_TO_POINT(points[3][2], cp[loop.c[cn].id][i2]);
    ASSIGN_POINT_TO_POINT(points[3][3], cp[loop.c[cn].id][i3]);
    cn = 2;
    if (loop.c[cn].sense == AS_DEF) { i0 = 0; i1 = 1; i2 = 2; i3 = 3; }
    else                            { i0 = 3; i1 = 2; i2 = 1; i3 = 0; }
    ASSIGN_POINT_TO_POINT(points[3][3], cp[loop.c[cn].id][i0]);
    ASSIGN_POINT_TO_POINT(points[2][3], cp[loop.c[cn].id][i1]);
    ASSIGN_POINT_TO_POINT(points[1][3], cp[loop.c[cn].id][i2]);
    ASSIGN_POINT_TO_POINT(points[0][3], cp[loop.c[cn].id][i3]);

    cn = 3; /* degenerate curve */
    ASSIGN_POINT_TO_POINT(points[0][3], cp[loop.c[cn-1].id][i3]);
    ASSIGN_POINT_TO_POINT(points[0][2], cp[loop.c[cn-1].id][i3]);
    ASSIGN_POINT_TO_POINT(points[0][1], cp[loop.c[cn-1].id][i3]);
    ASSIGN_POINT_TO_POINT(points[0][0], cp[loop.c[cn-1].id][i3]);

    
    SUBTR_POINTS(by, points[1][0], points[0][0]);
    ADD_UP_POINTS(points[1][1], points[0][1], by);
    SUBTR_POINTS(by, points[2][0], points[3][0]);
    ADD_UP_POINTS(points[2][1], points[3][1], by);
    SUBTR_POINTS(by, points[1][3], points[0][3]);
    ADD_UP_POINTS(points[1][2], points[0][2], by);
    SUBTR_POINTS(by, points[2][3], points[3][3]);
    ADD_UP_POINTS(points[2][2], points[3][2], by);
    break;
  /* two sides collapsed into a point */
  case 2:
    cn = 0;
    if (loop.c[cn].sense == AS_DEF) { i0 = 0; i1 = 1; i2 = 2; i3 = 3; }
    else                            { i0 = 3; i1 = 2; i2 = 1; i3 = 0; }
    ASSIGN_POINT_TO_POINT(points[0][0], cp[loop.c[cn].id][i0]);
    ASSIGN_POINT_TO_POINT(points[1][0], cp[loop.c[cn].id][i1]);
    ASSIGN_POINT_TO_POINT(points[2][0], cp[loop.c[cn].id][i2]);
    ASSIGN_POINT_TO_POINT(points[3][0], cp[loop.c[cn].id][i3]);
    /* degenerate */
    ASSIGN_POINT_TO_POINT(points[3][0], cp[loop.c[0].id][i3]);
    ASSIGN_POINT_TO_POINT(points[3][1], cp[loop.c[0].id][i3]);
    ASSIGN_POINT_TO_POINT(points[3][2], cp[loop.c[0].id][i3]);
    ASSIGN_POINT_TO_POINT(points[3][3], cp[loop.c[0].id][i3]);
    cn = 1;
    if (loop.c[cn].sense == AS_DEF) { i0 = 0; i1 = 1; i2 = 2; i3 = 3; }
    else                            { i0 = 3; i1 = 2; i2 = 1; i3 = 0; }
    ASSIGN_POINT_TO_POINT(points[3][3], cp[loop.c[cn].id][i0]);
    ASSIGN_POINT_TO_POINT(points[2][3], cp[loop.c[cn].id][i1]);
    ASSIGN_POINT_TO_POINT(points[1][3], cp[loop.c[cn].id][i2]);
    ASSIGN_POINT_TO_POINT(points[0][3], cp[loop.c[cn].id][i3]);
    /* degenerate */
    ASSIGN_POINT_TO_POINT(points[0][3], cp[loop.c[1].id][i3]);
    ASSIGN_POINT_TO_POINT(points[0][2], cp[loop.c[1].id][i3]);
    ASSIGN_POINT_TO_POINT(points[0][1], cp[loop.c[1].id][i3]);
    ASSIGN_POINT_TO_POINT(points[0][0], cp[loop.c[1].id][i3]);
    
    SUBTR_POINTS(by, points[1][0], points[0][0]);
    ADD_UP_POINTS(points[1][1], points[0][1], by);
    SUBTR_POINTS(by, points[2][0], points[3][0]);
    ADD_UP_POINTS(points[2][1], points[3][1], by);
    SUBTR_POINTS(by, points[1][3], points[0][3]);
    ADD_UP_POINTS(points[1][2], points[0][2], by);
    SUBTR_POINTS(by, points[2][3], points[3][3]);
    ADD_UP_POINTS(points[2][2], points[3][2], by);
    break;
  }

}


GraphicObj *
ExtrudeCurveToBCBezS3D(GraphicObj *profile, GraphicObj *along)
{
  GraphicObj *patch = NULL;
  WCRec alongpts[4], profpts[4], by, points[4][4];
  int npts, i;
  
  if (!EGGetGraphicsGeometry(along, alongpts, &npts))
    return NULL;
  if (!EGGetGraphicsGeometry(profile, profpts, &npts))
    return NULL;

  /* adjust for not connected curves */
  by.x = profpts[0].x - alongpts[0].x;
  by.y = profpts[0].y - alongpts[0].y;
  by.z = profpts[0].z - alongpts[0].z;
  for (i = 0; i < 4; i++) {
    alongpts[i].x += by.x;
    alongpts[i].y += by.y;
    alongpts[i].z += by.z;
  }
  /* generate */
  for (i = 0; i < 4; i++) {
    ASSIGN_POINT_TO_POINT(points[i][1], profpts[i]);
  }
  by.x = alongpts[1].x - alongpts[0].x;
  by.y = alongpts[1].y - alongpts[0].y;
  by.z = alongpts[1].z - alongpts[0].z;
  for (i = 0; i < 4; i++) {
    ASSIGN_POINT_TO_POINT(points[i][1], profpts[i]);
    profpts[i].x += by.x;
    profpts[i].y += by.y;
    profpts[i].z += by.z;
  }
  by.x = alongpts[2].x - alongpts[1].x;
  by.y = alongpts[2].y - alongpts[1].y;
  by.z = alongpts[2].z - alongpts[1].z;
  for (i = 0; i < 4; i++) {
    ASSIGN_POINT_TO_POINT(points[i][2], profpts[i]);
    profpts[i].x += by.x;
    profpts[i].y += by.y;
    profpts[i].z += by.z;
  }
  for (i = 0; i < 4; i++) {
    ASSIGN_POINT_TO_POINT(points[i][3], profpts[i]);
  }
  patch = CreateBCBezS3D((WCRec *)points);
  return patch;
}



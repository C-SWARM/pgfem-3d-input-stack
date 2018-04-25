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

#define RBEZS3D_PRIVATE_HEADER
#include "Erbezs3d.h"



/* ========================================================================= */
/* PRIVATE  */
/* ========================================================================= */

#define STRING_BUFF_SIZE 4096

static char string_rep_buffer[STRING_BUFF_SIZE];

/* ========================================================================= */
/* PUBLIC  */
/* ========================================================================= */

GraphicObj *
CreateRBezS3D(int order_u, int order_v, WCRec *points, float *weights)
{
  ERBezS3DWCRec *thedp;
  GraphicObj *g_p;
  WCRec *p;
  float *w;
  int i;
  
  thedp = (ERBezS3DWCRec *)make_node(sizeof(ERBezS3DWCRec));
  g_p = EGCreateGraphics(&rbezs3d_graphic_methods, (caddr_t)thedp,
			 sizeof(ERBezS3DWCRec));
  if((thedp -> points = p
      = (WCRec *)calloc(order_u * order_v, sizeof(WCRec))) == NULL){
    fprintf(stderr, "Memory allocation for RBezS3D failed");
    exit(0);
  }
  if((thedp -> weights = w
      = (float *)calloc(order_u * order_v, sizeof(float))) == NULL){
    fprintf(stderr, "Memory allocation for RBezS3D failed");
    exit(0);
  }
  
  thedp->order_u = order_u;
  thedp->order_v = order_v;
  for(i = order_u * order_v; i > 0; i--, p++, points++){
    p->x = points->x;
    p->y = points->y;
    p->z = points->z;
    *w++ = *weights++;
  }
  thedp->attributes.spec.tessel_u         = 16;
  thedp->attributes.spec.tessel_v         = 16;
  thedp->attributes.spec.show_polygon     = YES;
  thedp->attributes.spec.show_surface     = YES;
  thedp->attributes.spec.style            = LineSolid;
  thedp->attributes.spec.width            = 0;
  thedp->attributes.spec.fill_style       = FILL_HOLLOW;
  thedp->attributes.spec.edge_flag        = NO;
  thedp->attributes.spec.edge_color       = 0x0;
  thedp->attributes.spec.color            = 0x0;
  thedp->attributes.spec.have_any         = NO;
  return g_p;
}



static void 
Draw(EView *v_p, GraphicObj *p)
{
  ERBezS3DWCRec *thedp;
  
  thedp = (ERBezS3DWCRec *)(p->spec.data);
  
  EVRBezS3DPrimitive(v_p, p, thedp -> order_u, thedp -> order_v,
                     thedp->points, thedp -> weights,
                     thedp->attributes.spec.style,
                     thedp->attributes.spec.color,
                     thedp->attributes.spec.width,
                     thedp->attributes.spec.fill_style,
                     thedp->attributes.spec.edge_flag,
                     thedp->attributes.spec.edge_color,
                     thedp->attributes.spec.tessel_u,
                     thedp->attributes.spec.tessel_v,
                     thedp->attributes.spec.show_polygon,
                     NO,
                     DRAW_MODE_DRAW);
}



static void 
BoundingBoxVC(EView *v_p, GraphicObj *p, VCRec *ll, VCRec *ur)
{
  ERBezS3DWCRec *thedp;
  VCRec pvc;
  int i, order_u, order_v;
  
  thedp = (ERBezS3DWCRec *)(p->spec.data);
  order_u = thedp -> order_u;
  order_v = thedp -> order_v;
  
  EVWCtoVC(v_p, &(thedp->points[0]), &pvc);
  ll->u = pvc.u;
  ll->v = pvc.v;
  ll->n = pvc.n;
  ur->u = pvc.u;
  ur->v = pvc.v;
  ur->n = pvc.n;
  for(i = 1; i < order_u * order_v; i++){
    EVWCtoVC(v_p, &(thedp->points[i]), &pvc);
    ll->u = min(pvc.u, ll->u);
    ll->v = min(pvc.v, ll->v);
    ll->n = min(pvc.n, ll->n);
    ur->u = max(pvc.u, ur->u);
    ur->v = max(pvc.v, ur->v);
    ur->n = max(pvc.n, ur->n);
  }
}



static void 
BoundingBoxWC(GraphicObj *p, WCRec *ll, WCRec *ur)
{
  ERBezS3DWCRec *thedp;
  int i, order_u, order_v;
  
  thedp = (ERBezS3DWCRec *)(p->spec.data);
  order_u = thedp -> order_u;
  order_v = thedp -> order_v;
  
  ll->x = thedp->points[0].x;
  ll->y = thedp->points[0].y;
  ll->z = thedp->points[0].z;
  ur->x = thedp->points[0].x;
  ur->y = thedp->points[0].y;
  ur->z = thedp->points[0].z;
  for(i = 1; i < order_u * order_v; i++){
    ll->x = min(ll->x, thedp->points[i].x);
    ll->y = min(ll->y, thedp->points[i].y);
    ll->z = min(ll->z, thedp->points[i].z);
    ur->x = max(ur->x, thedp->points[i].x); 
    ur->y = max(ur->y, thedp->points[i].y);
    ur->z = max(ur->z, thedp->points[i].z);
  }
}



static void 
XorDraw(EView *v_p, GraphicObj *g_p)
{
  ERBezS3DWCRec *thedp;
  
  thedp = (ERBezS3DWCRec *)(g_p->spec.data);
  EVRBezS3DPrimitive(v_p,  g_p, thedp -> order_u, thedp -> order_v,
                     thedp->points, thedp -> weights, 
                     thedp->attributes.spec.style,
                     thedp->attributes.spec.color,
                     thedp->attributes.spec.width,
                     FILL_HOLLOW, NO, 0,
                     thedp->attributes.spec.tessel_u,
                     thedp->attributes.spec.tessel_v,
                     thedp->attributes.spec.show_polygon,
                     NO,
                     DRAW_MODE_XORDRAW);
}




static GraphicObj *
ModifyGeometry(GraphicObj *g_p, WCRec *points)
{
  ERBezS3DWCRec *thedp;
  WCRec *p;
  int i, order_u, order_v;
  
  thedp = (ERBezS3DWCRec *)(g_p->spec.data);
  order_u = thedp -> order_u;
  order_v = thedp -> order_v;
  p = thedp -> points;
  
  for (i = 0; i < order_u * order_v; i++, p++, points++){
    p->x = points->x;
    p->y = points->y;
    p->z = points->z;
  }
  return g_p;
}


static BOOLEAN 
GetGeometry(GraphicObj *g_p, WCRec *points, int *npoints)
     /* array points must be large enough */
{
  ERBezS3DWCRec *thedp = (ERBezS3DWCRec *)(g_p->spec.data);
  WCRec *p;
  int i, order_u, order_v;
  
  order_u = thedp -> order_u;
  order_v = thedp -> order_v;
  p = thedp -> points;
  
  for (i = 0; i < order_u * order_v; i++, p++, points++){
    points->x = p->x;
    points->y = p->y;
    points->z = p->z;
  }
  *npoints = order_u * order_v;
  return YES;
}



static GraphicObj *
ModifyWeights(GraphicObj *g_p, FPNum *weights)
{
  ERBezS3DWCRec *r_p;
  float *w;
  int i, order_u, order_v;
  
  r_p = (ERBezS3DWCRec *)(g_p->spec.data);
  order_u = r_p->order_u;
  order_v = r_p->order_v;
  w = r_p -> weights;
  
  for (i = 0; i < order_u * order_v; i++)
    *w++ = *weights++;
  return g_p;
}




static BOOLEAN
GetWeights(GraphicObj *g_p, FPNum *weights, int *nweights)
     /* array weights must be large enough */
{
  ERBezS3DWCRec *r_p = (ERBezS3DWCRec *)(g_p->spec.data);
  float *w;
  int i, order_u, order_v;
  
  order_u = r_p->order_u;
  order_v = r_p->order_v;
  w = r_p -> weights;
  
  for (i = 0; i < order_u * order_v; i++)
    *weights++ = *w++;
  *nweights = order_u * order_v;
  return YES;
}



static void 
Hilite(EView *v_p, GraphicObj *g_p)
{
  ERBezS3DWCRec *thedp;
  
  thedp = (ERBezS3DWCRec *)(g_p->spec.data);
  EVRBezS3DPrimitive(v_p,  g_p, thedp -> order_u, thedp -> order_v,
                     thedp->points, thedp -> weights,
                     thedp->attributes.spec.style,
                     thedp->attributes.spec.color,
                     thedp->attributes.spec.width,
                     FILL_HOLLOW, NO, 0,
                     thedp -> order_u, thedp -> order_v,
                     thedp->attributes.spec.show_polygon,
                     NO,
                     DRAW_MODE_HILITE);
}



static void 
Unhilite(EView *v_p, GraphicObj *g_p)
{
  ERBezS3DWCRec *thedp;
  
  thedp = (ERBezS3DWCRec *)(g_p->spec.data);
  EVRBezS3DPrimitive(v_p,  g_p, thedp -> order_u, thedp -> order_v,
                     thedp->points, thedp -> weights, 
                     thedp->attributes.spec.style,
                     thedp->attributes.spec.color,
                     thedp->attributes.spec.width,
                     FILL_HOLLOW, NO, 0,
                     thedp -> order_u, thedp -> order_v,
                     thedp->attributes.spec.show_polygon,
                     NO,
                     DRAW_MODE_UNHILITE);
}



static void 
Erase(EView *v_p, GraphicObj *g_p)
{
  ERBezS3DWCRec *thedp;
  
  thedp = (ERBezS3DWCRec *)(g_p->spec.data);
  
  EVRBezS3DPrimitive(v_p,  g_p, thedp -> order_u, thedp -> order_v,
                     thedp->points, thedp -> weights, 
                     thedp->attributes.spec.style,
                     thedp->attributes.spec.color,
                     thedp->attributes.spec.width,
                     thedp->attributes.spec.fill_style, NO, 0,
                     thedp->attributes.spec.tessel_u,
                     thedp->attributes.spec.tessel_v,
                     thedp->attributes.spec.show_polygon,
                     NO,
                     DRAW_MODE_ERASE);  
}



static GraphicObj *
Translate(GraphicObj *g_p, WCRec *by)
{
  ERBezS3DWCRec *thedp;
  WCRec *p;
  int i, order_u, order_v;
  
  thedp = (ERBezS3DWCRec *)g_p->spec.data;
  order_u = thedp -> order_u;
  order_v = thedp -> order_v;
  p = thedp -> points;
  
  for (i = 0; i < order_u * order_v; i++, p++){
    p->x += by->x;
    p->y += by->y;
    p->z += by->z;
  }
  return g_p;
}



static GraphicObj *
Rotate(GraphicObj *g_p, WCRec *center, WCRec *axial)
{
  ERBezS3DWCRec *c;
  WCRec x_c, *p;
  int i, order_u, order_v;
  
  c = (ERBezS3DWCRec *)g_p->spec.data;
  order_u = c -> order_u;
  order_v = c -> order_v;
  p = c -> points;
  
  for (i = 0; i < order_u * order_v; i++, p++) {
    x_c.x = p->x - center->x;
    x_c.y = p->y - center->y;
    x_c.z = p->z - center->z;
    RotVectAboutVect(axial, &x_c);
    p->x = center->x + x_c.x;
    p->y = center->y + x_c.y;
    p->z = center->z + x_c.z;
  }
  return g_p;
}



static GraphicObj  *
MirrorPlane(GraphicObj *g_p,
            WCRec *point_on_plane, WCRec *unit_plane_normal)
{
  ERBezS3DWCRec *dp;
  WCRec *p;
  int i, order_u, order_v;
  
  dp = (ERBezS3DWCRec *)g_p->spec.data;
  order_u = dp -> order_u;
  order_v = dp -> order_v;
  p = dp -> points;
  
  for (i = 0; i < order_u * order_v; i++, p++)
    MirrorPointInPlane(point_on_plane, unit_plane_normal, p);
  
  return g_p;
}



static GraphicObj  *
MirrorCenter(GraphicObj *g_p, WCRec *center)
{
  ERBezS3DWCRec *dp;
  WCRec *p;
  int i, order_u, order_v;
  
  dp = (ERBezS3DWCRec *)g_p->spec.data;
  order_u = dp -> order_u;
  order_v = dp -> order_v;
  p = dp -> points;
  
  for (i = 0; i < order_u * order_v; i++, p++){
    p->x = 2.*center->x - p->x;
    p->y = 2.*center->y - p->y;
    p->z = 2.*center->z - p->z;
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
         ESelectCriteria sel_crit, GraphicObj *g_p)
{
  VCRec ll, ur, pvc;
  WCRec *p;
  WC2DRec *points;
  float *weights, *w;
  ERBezS3DWCRec *thedp;
  int i, order_u, order_v;
  
  switch(sel_crit) {
  case INSIDE: /* only if the whole bounding box is inside */
    BoundingBoxVC(v_p, g_p, &ll, &ur);
    if (ll.u >= box->left   && ur.u <= box->right && 
	ll.v >= box->bottom && ur.v <= box->top)
      return YES;
    else
      return NO;
    break;
  case OVERLAP:
  case INTERSECT: /* hits the boundary curves */
  default:
    thedp = (ERBezS3DWCRec *)g_p->spec.data;
    order_u = thedp -> order_u;
    order_v = thedp -> order_v;
    p = thedp -> points;
    w = thedp -> weights;
    if((points = (WC2DRec *)calloc(max(order_u, order_v), sizeof(WC2DRec)))
       == NULL){
      fprintf(stderr, "Memory allocation in HitByBox failed");
      return(NO);
    }
    if((weights = (float *)calloc(max(order_u, order_v), sizeof(float)))
       == NULL){
      fprintf(stderr, "Memory allocation in HitByBox failed");
      free(points);
      return(NO);
    }
    for (i = 0; i < order_u; i++) {   /* curve 1 */
      EVWCtoVC(v_p, &p[i], &pvc);
      points[i].x = pvc.u; points[i].y = pvc.v;
      weights[i] = w[i];
    }
    if (BoxIntersectsRBezCurve(box, order_u, points, weights)){
      free(points);
      free(weights);
      return YES;
    }
    for (i = 0; i < order_v; i++) {      /* curve 2 */
      EVWCtoVC(v_p, &p[i * order_u], &pvc);
      points[i].x = pvc.u; points[i].y = pvc.v;
      weights[i] = w[i * order_u];
    }
    if (BoxIntersectsRBezCurve(box, order_v, points, weights)){
      free(points);
      free(weights);
      return YES;
    }
    for (i = 0; i < order_u; i++) {      /* curve 3 */
      EVWCtoVC(v_p, &p[i + (order_v - 1) * order_u], &pvc);
      points[i].x = pvc.u; points[i].y = pvc.v;
      weights[i] = w[i + (order_v - 1) * order_u];
    }
    if (BoxIntersectsRBezCurve(box, order_u, points, weights)){
      free(points);
      free(weights);
      return YES;
    }
    for (i = 0; i < order_v; i++) {     /* curve 4 */
      EVWCtoVC(v_p, &p[(i + 1) * order_u - 1], &pvc);
      points[i].x = pvc.u; points[i].y = pvc.v;
      weights[i] = w[(i + 1) * order_u - 1];
    }
    if (BoxIntersectsRBezCurve(box, order_v, points, weights)){
      free(points);
      free(weights);
      return YES;
    }
    free(points);
    free(weights);
    return NO;
    break;
  }
}



static EGraphicType 
Type(GraphicObj *g_p)
{
  return EG_RBEZSURFACE3D;
}



static char *
AsString(GraphicObj *g_p)
{
  ERBezS3DWCRec *l;
  WCRec *p;
  float *w;
  int i, order_u, order_v;
  char buff[128];
  
  l = (ERBezS3DWCRec *)g_p->spec.data;
  order_u = l->order_u;
  order_v = l->order_v;
  p = l->points;
  w = l->weights;
  sprintf(string_rep_buffer,
	  "RBEZSURFACE3D %d %d \n", order_u, order_v);
  for(i = 0; i < order_u * order_v; i++, p++, w++){
    sprintf(buff, "              [%f,%f,%f|%f] \n", p->x, p->y, p->z, *w);
    strcat(string_rep_buffer, buff);
  }
  return string_rep_buffer; /* Don't free this memory!!! */
}




static LIST 
KeyPoints(GraphicObj *g_p)
{
  LIST temp_list;
  KeyPointRec *key_p;
  ERBezS3DWCRec *l_p;
  WCRec *p;
  int i, order_u, order_v;
  
  l_p       = (ERBezS3DWCRec *)(g_p->spec.data);
  order_u = l_p->order_u;
  order_v = l_p->order_v;
  p = l_p->points;
  temp_list = make_list();
  
  for (i = 0; i < order_u * order_v; i++, p++){
    key_p = (KeyPointRec*)add_to_tail(temp_list,
                                      make_node(sizeof(KeyPointRec)));
    key_p->coords.x = p->x;
    key_p->coords.y = p->y;
    key_p->coords.z = p->z;
    key_p->dist_from_hit = 0.0;
  }
  return temp_list;
}



static GraphicObj *
Reshape(GraphicObj *gp, EHandleNum hn, WCRec *pos)
{
  ERBezS3DWCRec *_p;
  int i, order_u, order_v;
  
  _p = (ERBezS3DWCRec *)(gp->spec.data);
  order_u = _p->order_u;
  order_v = _p->order_v;
  
  i = hn - 1;
  if (hn < 1 || hn > order_u * order_v) 
    i = 0;
  
  _p->points[i].x = pos->x;
  _p->points[i].y = pos->y;
  _p->points[i].z = pos->z;
  return gp;
}



static EHandleNum   
ReshapeHandle(EView *v_p, GraphicObj *gp, WCRec *nearp)
{
  ERBezS3DWCRec *l_p;
  VCRec nvc, pvc;
  WCRec *p;
  EHandleNum hn;
  double dist, d;
  int i, order_u, order_v;
  
  l_p       = (ERBezS3DWCRec *)(gp->spec.data);
  order_u = l_p->order_u;
  order_v = l_p->order_v;
  p = l_p->points;
  
  EVWCtoVC(v_p, nearp, &nvc);
  EVWCtoVC(v_p, p, &pvc);
  dist = DistOfPnts(pvc.u, pvc.v, nvc.u, nvc.v);
  hn = 0;
  for(i = 1, p++; i < order_u * order_v; i++, p++){
    EVWCtoVC(v_p, p, &pvc);
    d = DistOfPnts(pvc.u, pvc.v, nvc.u, nvc.v);
    if(d < dist){
      dist = d;
      hn = i;
    }
  }
  return((EHandleNum)(hn + 1));
}



static void 
ShowHandles(EView *v_p, GraphicObj *gp)
{
  ERBezS3DWCRec *l_p;
  WCRec *p;
  int i, order_u, order_v;
  
  l_p = (ERBezS3DWCRec *)gp->spec.data;
  order_u = l_p->order_u;
  order_v = l_p->order_v;
  p = l_p->points;
  
  for (i = 0; i < order_u * order_v; i++, p++)
    EGDrawHandle(v_p, p);
}



static void 
EraseHandles(EView *v_p, GraphicObj *gp)
{
  ERBezS3DWCRec *l_p;
  WCRec *p;
  int i, order_u, order_v;
  
  l_p = (ERBezS3DWCRec *)gp->spec.data;
  order_u = l_p->order_u;
  order_v = l_p->order_v;
  p = l_p->points;
  
  for (i = 0; i < order_u * order_v; i++, p++)
    EGEraseHandle(v_p, p);
}




static void 
XYofHandle(GraphicObj *p, EHandleNum hn, WCRec *pos)
{
  ERBezS3DWCRec *l_p;
  int i, order_u, order_v;
  
  l_p = (ERBezS3DWCRec *)p->spec.data;
  order_u = l_p->order_u;
  order_v = l_p->order_v;
  
  i = hn - 1;
  if (hn < 1 || hn > order_u * order_v)
    i = 0;
  pos->x = l_p->points[i].x;
  pos->y = l_p->points[i].y;
  pos->z = l_p->points[i].z;
}



static void 
RetrieveFrom(FILE *file_p, GraphicSpecObj  *gsp)
{
  ERBezS3DWCRec *p;
  int fill, eflag, sp, sc, tu, tv, order_u, order_v, i;
  char color[64];
  EPixel pixel;
  BOOLEAN success;
  double x, y, z, weight;
  
  if ((p = (ERBezS3DWCRec *)make_node(sizeof(ERBezS3DWCRec))) != NULL) {
    fscanf(file_p, "%d %d", &order_u, &order_v);
    p -> order_u = order_u;
    p -> order_v = order_v;
    if((p -> points = (WCRec *)calloc(order_u * order_v, sizeof(WCRec)))
       == NULL){
      fprintf(stderr, "Memory allocation for RBezS3D in RetrieveFrom failed");
      exit(0);
    }
    if((p -> weights = (float *)calloc(order_u * order_v, sizeof(float)))
       == NULL){
      fprintf(stderr, "Memory allocation for RBezS3D in RetrieveFrom failed");
      exit(0);
    }
    for (i = 0; i < order_u * order_v; i++) {
      fscanf(file_p, "%lf %lf %lf %lf", &x, &y, &z, &weight);
      p->points[i].x = x;
      p->points[i].y = y;
      p->points[i].z = z;
      p->weights[i] = weight;
    }
    fscanf(file_p, "%d %d %d %d %d %d %d",
	   &(p->attributes.spec.have_any),
	   &sp, &sc, &fill, &eflag, &tu, &tv);
    p->attributes.spec.show_polygon     = sp;
    p->attributes.spec.show_surface     = sc;
    p->attributes.spec.fill_style       = fill;
    p->attributes.spec.edge_flag        = eflag;
    p->attributes.spec.tessel_u         = tu;
    p->attributes.spec.tessel_v         = tv;
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
  gsp->data_length = sizeof(ERBezS3DWCRec);
}



static void 
StoreOn(FILE *file_p, GraphicObj *g_p)
{
  ERBezS3DWCRec *p;
  int fill, sp, sc, tu, tv, eflag, order_u, order_v, i;
  char *color, *edge_color;
  
  p = (ERBezS3DWCRec *)g_p->spec.data;
  order_u = p->order_u;
  order_v = p->order_v;
  if (p->attributes.spec.have_any == YES) {
    fill  = p->attributes.spec.fill_style;
    eflag = p->attributes.spec.edge_flag;
    sp    = p->attributes.spec.show_polygon;
    sc    = p->attributes.spec.show_surface;
    tu    = p->attributes.spec.tessel_u;
    tv    = p->attributes.spec.tessel_v;
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
    tu    = 16;
    tv    = 16;
    color = edge_color = "black";
  }
  fprintf(file_p, "%d %d \n", order_u, order_v);
  for(i = 0; i < order_u * order_v; i++)
    fprintf(file_p, "%f %f %f %f \n",
            p->points[i].x, p->points[i].y, p->points[i].z, p->weights[i]);
  fprintf(file_p, "%d %d %d %d %d %d %d\n%s\n%s\n",
	  p->attributes.spec.have_any,
	  sp, sc, fill, eflag, tu, tv, color, edge_color);
}



static void 
ChangeAttributes(GraphicObj *p)
{
  ERBezS3DWCRec *dp;
  unsigned long mask;
  
  mask = EASValsGetChangeMask();
  dp = (ERBezS3DWCRec *)p->spec.data;
  if ((mask & SHOW_POLY_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.show_polygon = EASValsGetShowPoly();
    dp->attributes.spec.have_any     = YES;
  }
  if ((mask & SHOW_ENTITY_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.show_surface   = EASValsGetShowEntity();
    dp->attributes.spec.have_any     = YES;
  }
  if ((mask & TESSEL_U_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.tessel_u = EASValsGetUTesselIntervals();
    dp->attributes.spec.have_any         = YES;
  }
  if ((mask & TESSEL_V_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.tessel_v = EASValsGetVTesselIntervals();
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
  ERBezS3DWCRec *dp;
  
  dp = (ERBezS3DWCRec *)p->spec.data;
  
  if (dp->attributes.spec.have_any) {
    EASValsSetFillStyle(dp->attributes.spec.fill_style);
    EASValsSetColor(dp->attributes.spec.color);
    EASValsSetEdgeColor(dp->attributes.spec.edge_color);
    EASValsSetEdgeFlag(dp->attributes.spec.edge_flag);
    EASValsSetShowPoly(dp->attributes.spec.show_polygon);
    EASValsSetShowEntity(dp->attributes.spec.show_surface);
    EASValsSetUTesselIntervals(dp->attributes.spec.tessel_u);
    EASValsSetVTesselIntervals(dp->attributes.spec.tessel_v);
  }
}

static BOOLEAN
AttribValAsInAS(GraphicObj *p, unsigned long mask)  
{
  ERBezS3DWCRec *dp;
  
  dp = (ERBezS3DWCRec *)p->spec.data;
  
  if ((mask & SHOW_POLY_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.show_polygon != EASValsGetShowPoly())
      return NO;
  if ((mask & SHOW_ENTITY_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.show_surface != EASValsGetShowEntity())
      return NO;
  
  if ((mask & TESSEL_U_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.tessel_u != EASValsGetUTesselIntervals())
      return NO;
  
  if ((mask & TESSEL_V_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.tessel_v != EASValsGetVTesselIntervals())
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




static GraphicObj *
DeepCopy(GraphicObj *p)
{
  GraphicObj *gp;
  ERBezS3DWCRec *thedp;
  caddr_t new_data_p;
  WCRec *points_p, *points_gp;
  float *weights_p, *weights_gp;
  int i, order_u, order_v;
  
  /*	gp = EGDefaultCopyGraphics(p); */
  if ((new_data_p =
       (caddr_t)copy_node(p->spec.data, p->spec.data_length)) == NULL) {
    fprintf(stderr, "Failed copy data\n");
    exit(0);
  }
  gp = EGCreateGraphics(p->methods, new_data_p, p->spec.data_length);
  
  thedp = (ERBezS3DWCRec *)(gp->spec.data);
  order_u = thedp->order_u;
  order_v = thedp->order_v;
  
  if((thedp -> points = (WCRec *)calloc(order_u * order_v, sizeof(WCRec)))
     == NULL){
    fprintf(stderr, "Memory allocation for RBezS3D in DeepCopy failed");
    exit(0);
  }
  if((thedp -> weights = (float *)calloc(order_u * order_v, sizeof(float)))
     == NULL){
    fprintf(stderr, "Memory allocation for RBezS3D in DeepCopy failed");
    exit(0);
  }
  
  points_p = ((ERBezS3DWCRec *)(p->spec.data)) -> points;
  weights_p = ((ERBezS3DWCRec *)(p->spec.data)) -> weights;
  points_gp = thedp -> points;
  weights_gp = thedp -> weights;
  
  for(i = 0; i < order_u * order_v; i++){
    points_gp[i].x = points_p[i].x;
    points_gp[i].y = points_p[i].y;
    points_gp[i].z = points_p[i].z;
    weights_gp[i] = weights_p[i];
  }
  return(gp);
}



static void
DeepDestroy(GraphicObj *p)
{
  ERBezS3DWCRec *thedp;
  thedp = (ERBezS3DWCRec *)(p->spec.data);
  
  free(thedp->points);
  free(thedp->weights);
  
  /* 	EGDefaultDestroyGraphics(p); */
  free_node(thedp);                        /* free specialized data */
  free_node(p);                            /* free top struct */
}


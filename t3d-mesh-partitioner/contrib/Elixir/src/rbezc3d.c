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

#define RBEZC3D_PRIVATE_HEADER
#include "Erbezc3d.h"



/* ========================================================================= */
/* PRIVATE  */
/* ========================================================================= */

#define STRING_BUFF_SIZE 2048

static char string_rep_buffer[STRING_BUFF_SIZE];

#define ADVANCED_BOX

/* ========================================================================= */
/* PUBLIC  */
/* ========================================================================= */

GraphicObj *
CreateRBezC3D(int order, WCRec *points, float *weights)
{
  ERBezC3DWCRec *r_p;
  GraphicObj *g_p;
  int i;
  
  r_p = (ERBezC3DWCRec *)make_node(sizeof(ERBezC3DWCRec));
  g_p = EGCreateGraphics(&rbezc3d_graphic_methods, (caddr_t)r_p,
			 sizeof(ERBezC3DWCRec));
  if((r_p -> points = (WCRec *)calloc(order, sizeof(WCRec))) == NULL){
    fprintf(stderr, "Memory allocation for RBezC3D failed");
    exit(0);
  }
  if((r_p -> weights = (float *)calloc(order, sizeof(float))) == NULL){
    fprintf(stderr, "Memory allocation for RBezC3D failed");
    exit(0);
  }
  
  r_p -> order = order;
  for (i = 0; i < order; i++, points++) {
    r_p->points[i].x               = points -> x; 
    r_p->points[i].y               = points -> y;
    r_p->points[i].z               = points -> z;
    r_p->weights[i]                = *weights++;
  }
  r_p->attributes.spec.tessel_intervals = 32;
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
  ERBezC3DWCRec *thedp;
  
  thedp = (ERBezC3DWCRec *)(p->spec.data);
  
  EVRBezC3DPrimitive(v_p, p, thedp->order, thedp->points, thedp->weights,
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
  ERBezC3DWCRec *thedp;
  VCRec pvc;
  int order, i;
#ifdef ADVANCED_BOX
	WCRec pnt;
	double *bt = NULL, weight, tmp;
	int j;
#endif  
  
  thedp = (ERBezC3DWCRec *)(p->spec.data);
  order = thedp->order;
  
  EVWCtoVC(v_p, &(thedp->points[0]), &pvc);
  ll->u = pvc.u;
  ll->v = pvc.v;
  ll->n = pvc.n;
  ur->u = pvc.u;
  ur->v = pvc.v;
  ur->n = pvc.n;

#ifdef ADVANCED_BOX
	if(order > 2 && thedp->attributes.spec.show_polygon == NO){
		if((bt = (double *)calloc(order, sizeof(double))) != NULL){

			Bernstein(order, 0.5, bt);
			weight = 0.0;
			pnt.x = pnt.y = pnt.z = 0.0;
			for(i = 0; i < order; i++){
				weight += (tmp = thedp->weights[i] * bt[i]);
				pnt.x += thedp->points[i].x * tmp;
				pnt.y += thedp->points[i].y * tmp;
				pnt.z += thedp->points[i].z * tmp;
			}
			pnt.x /= weight;
			pnt.y /= weight;
			pnt.z /= weight;

			EVWCtoVC(v_p, &pnt, &pvc);
			ll->u = min(pvc.u, ll->u);
			ll->v = min(pvc.v, ll->v);
			ll->n = min(pvc.n, ll->n);
			ur->u = max(pvc.u, ur->u);
			ur->v = max(pvc.v, ur->v);
			ur->n = max(pvc.n, ur->n);

			while(--order > 1){
				Bernstein(order, 0.5, bt);

				weight = 0.0;
				pnt.x = pnt.y = pnt.z = 0.0;
				for(i = 0; i < order; i++){
					weight += (tmp = thedp->weights[i] * bt[i]);
					pnt.x += thedp->points[i].x * tmp;
					pnt.y += thedp->points[i].y * tmp;
					pnt.z += thedp->points[i].z * tmp;
				}
				pnt.x /= weight;
				pnt.y /= weight;
				pnt.z /= weight;

				EVWCtoVC(v_p, &pnt, &pvc);
				ll->u = min(pvc.u, ll->u);
				ll->v = min(pvc.v, ll->v);
				ll->n = min(pvc.n, ll->n);
				ur->u = max(pvc.u, ur->u);
				ur->v = max(pvc.v, ur->v);
				ur->n = max(pvc.n, ur->n);

				weight = 0.0;
				pnt.x = pnt.y = pnt.z = 0.0;
				for(j = 0; j < order; j++){
					i = thedp->order - j - 1;
					weight += (tmp = thedp->weights[i] * bt[j]);
					pnt.x += thedp->points[i].x * tmp;
					pnt.y += thedp->points[i].y * tmp;
					pnt.z += thedp->points[i].z * tmp;
				}
				pnt.x /= weight;
				pnt.y /= weight;
				pnt.z /= weight;

				EVWCtoVC(v_p, &pnt, &pvc);
				ll->u = min(pvc.u, ll->u);
				ll->v = min(pvc.v, ll->v);
				ll->n = min(pvc.n, ll->n);
				ur->u = max(pvc.u, ur->u);
				ur->v = max(pvc.v, ur->v);
				ur->n = max(pvc.n, ur->n);

				order--;
			}
			order = thedp->order;

			EVWCtoVC(v_p, &(thedp->points[order - 1]), &pvc);
			ll->u = min(pvc.u, ll->u);
			ll->v = min(pvc.v, ll->v);
			ll->n = min(pvc.n, ll->n);
			ur->u = max(pvc.u, ur->u);
			ur->v = max(pvc.v, ur->v);
			ur->n = max(pvc.n, ur->n);

			free(bt);
			return;
		}
	}
#endif

  for(i = 1; i < order; i++){
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
  ERBezC3DWCRec *thedp;
  int order, i;
#ifdef ADVANCED_BOX
	WCRec pnt;
	double *bt = NULL, weight, tmp;
	int j;
#endif  

  thedp = (ERBezC3DWCRec *)(p->spec.data);
  order = thedp->order;
  
  ll->x = thedp->points[0].x;
  ll->y = thedp->points[0].y;
  ll->z = thedp->points[0].z;
  ur->x = thedp->points[0].x;
  ur->y = thedp->points[0].y;
  ur->z = thedp->points[0].z;

#ifdef ADVANCED_BOX
	if(order > 2 && thedp->attributes.spec.show_polygon == NO){
		if((bt = (double *)calloc(order, sizeof(double))) != NULL){

			Bernstein(order, 0.5, bt);
			weight = 0.0;
			pnt.x = pnt.y = pnt.z = 0.0;
			for(i = 0; i < order; i++){
				weight += (tmp = thedp->weights[i] * bt[i]);
				pnt.x += thedp->points[i].x * tmp;
				pnt.y += thedp->points[i].y * tmp;
				pnt.z += thedp->points[i].z * tmp;
			}
			pnt.x /= weight;
			pnt.y /= weight;
			pnt.z /= weight;

			ll->x = min(ll->x, pnt.x);
			ll->y = min(ll->y, pnt.y);
			ll->z = min(ll->z, pnt.z);
			ur->x = max(ur->x, pnt.x); 
			ur->y = max(ur->y, pnt.y);
			ur->z = max(ur->z, pnt.z);

			while(--order > 1){
				Bernstein(order, 0.5, bt);

				weight = 0.0;
				pnt.x = pnt.y = pnt.z = 0.0;
				for(i = 0; i < order; i++){
					weight += (tmp = thedp->weights[i] * bt[i]);
					pnt.x += thedp->points[i].x * tmp;
					pnt.y += thedp->points[i].y * tmp;
					pnt.z += thedp->points[i].z * tmp;
				}
				pnt.x /= weight;
				pnt.y /= weight;
				pnt.z /= weight;

				ll->x = min(ll->x, pnt.x);
				ll->y = min(ll->y, pnt.y);
				ll->z = min(ll->z, pnt.z);
				ur->x = max(ur->x, pnt.x); 
				ur->y = max(ur->y, pnt.y);
				ur->z = max(ur->z, pnt.z);

				weight = 0.0;
				pnt.x = pnt.y = pnt.z = 0.0;
				for(j = 0; j < order; j++){
					i = thedp->order - j;
					weight += (tmp = thedp->weights[i] * bt[i]);
					pnt.x += thedp->points[i].x * tmp;
					pnt.y += thedp->points[i].y * tmp;
					pnt.z += thedp->points[i].z * tmp;
				}
				pnt.x /= weight;
				pnt.y /= weight;
				pnt.z /= weight;

				ll->x = min(ll->x, pnt.x);
				ll->y = min(ll->y, pnt.y);
				ll->z = min(ll->z, pnt.z);
				ur->x = max(ur->x, pnt.x); 
				ur->y = max(ur->y, pnt.y);
				ur->z = max(ur->z, pnt.z);

				order--;
			}
			order = thedp->order;

			ll->x = min(ll->x, thedp->points[order].x);
			ll->y = min(ll->y, thedp->points[order].y);
			ll->z = min(ll->z, thedp->points[order].z);
			ur->x = max(ur->x, thedp->points[order].x); 
			ur->y = max(ur->y, thedp->points[order].y);
			ur->z = max(ur->z, thedp->points[order].z);

			free(bt);
			return;
		}
	}
#endif

  for(i = 1; i < order; i++){
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
  ERBezC3DWCRec *thedp;
  
  thedp = (ERBezC3DWCRec *)(g_p->spec.data);
  
  EVRBezC3DPrimitive(v_p, g_p, thedp->order, thedp->points, thedp->weights,
		     thedp->attributes.spec.style,
		     thedp->attributes.spec.color,
		     thedp->attributes.spec.width,
		     thedp->attributes.spec.tessel_intervals,
		     thedp->attributes.spec.show_polygon,
		     DRAW_MODE_XORDRAW);
}




static GraphicObj *
ModifyGeometry(GraphicObj *g_p, WCRec *points)
{
  ERBezC3DWCRec *r_p;
  int i, order;
  
  r_p = (ERBezC3DWCRec *)(g_p->spec.data);
  order = r_p->order;
  
  for (i = 0; i < order; i++, points++) {
    r_p->points[i].x               = points -> x; 
    r_p->points[i].y               = points -> y;
    r_p->points[i].z               = points -> z;
  }
  return g_p;
}




static BOOLEAN
GetGeometry(GraphicObj *g_p, WCRec *points, int *npoints)     /* array points must be large enough */
{
  ERBezC3DWCRec *r_p = (ERBezC3DWCRec *)(g_p->spec.data);
  int i, order = r_p->order;
  
  for (i = 0; i < order; i++, points++) {
    points -> x = r_p->points[i].x; 
    points -> y = r_p->points[i].y;
    points -> z = r_p->points[i].z;
  }
  *npoints = order;
  return YES;
}




static GraphicObj *
ModifyWeights(GraphicObj *g_p, FPNum *weights)
{
  ERBezC3DWCRec *r_p;
  int i, order;
  
  r_p = (ERBezC3DWCRec *)(g_p->spec.data);
  order = r_p->order;
  
  for (i = 0; i < order; i++)
    r_p->weights[i] = *weights++;
  return g_p;
}




static BOOLEAN
GetWeights(GraphicObj *g_p, FPNum *weights, int *nweights)   /* array weights must be large enough */
{
  ERBezC3DWCRec *r_p = (ERBezC3DWCRec *)(g_p->spec.data);
  int i, order = r_p->order;
  
  for (i = 0; i < order; i++)
    *weights++ = r_p->weights[i];
  *nweights = order;
  return YES;
}


static void 
Hilite(EView *v_p, GraphicObj *g_p)
{
  ERBezC3DWCRec *thedp;
  
  thedp = (ERBezC3DWCRec *)(g_p->spec.data);
  
  EVRBezC3DPrimitive(v_p, g_p, thedp->order, thedp->points, thedp->weights,
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
  ERBezC3DWCRec *thedp;
  
  thedp = (ERBezC3DWCRec *)(g_p->spec.data);
  
  EVRBezC3DPrimitive(v_p, g_p, thedp->order, thedp->points, thedp->weights,
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
  ERBezC3DWCRec *thedp;
  
  thedp = (ERBezC3DWCRec *)(g_p->spec.data);
  
  EVRBezC3DPrimitive(v_p, g_p, thedp->order, thedp->points, thedp->weights,
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
  ERBezC3DWCRec *p;
  int i, order;
  
  p = (ERBezC3DWCRec *)g_p->spec.data;
  order = p->order;
  for (i = 0; i < order; i++) {
    p->points[i].x += by->x;
    p->points[i].y += by->y;
    p->points[i].z += by->z;
  }
  return g_p;
}



static GraphicObj *
Rotate(GraphicObj *g_p, WCRec *center, WCRec *axial)
{
  ERBezC3DWCRec *c;
  WCRec x_c;
  int i, order;
  
  c = (ERBezC3DWCRec *)g_p->spec.data;
  order = c->order;
  for (i = 0; i < order; i++ ) {
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
  ERBezC3DWCRec *dp;
  int j, order;
  
  dp = (ERBezC3DWCRec *)g_p->spec.data;
  order = dp -> order;
  for (j = 0; j < order; j++) 
    MirrorPointInPlane(point_on_plane, unit_plane_normal, &dp->points[j]);
  
  return g_p;
}



static GraphicObj  *
MirrorCenter(GraphicObj *g_p, WCRec *center)
{
  ERBezC3DWCRec *dp;
  int j, order;
  
  dp = (ERBezC3DWCRec *)g_p->spec.data;
  order = dp -> order;
  for (j = 0; j < order; j++) {
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
  WC2DRec *points;
  ERBezC3DWCRec *thedp;
  int i, order;
  BOOLEAN hit;
  
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
    thedp = (ERBezC3DWCRec *)p->spec.data;
    order = thedp->order;
    if((points = (WC2DRec *)calloc(order, sizeof(WC2DRec))) == NULL){
      fprintf(stderr, "Memory allocation in HitByBox failed");
      return(NO);
    }
    for (i = 0; i < order; i++) {
      EVWCtoVC(v_p, &(thedp->points[i]), &pvc);
      points[i].x = pvc.u; points[i].y = pvc.v;
    }
    hit = BoxIntersectsRBezCurve(box, order, points, thedp->weights);
    free(points);
    return(hit);
    break;
  }
}



static EGraphicType 
Type(GraphicObj *g_p)
{
  return EG_RBEZCURVE3D;
}



static char *
AsString(GraphicObj *g_p)
{
  ERBezC3DWCRec *l;
  int i, order;
  char buff[128];
  
  l = (ERBezC3DWCRec *)g_p->spec.data;
  order = l->order;
  sprintf(string_rep_buffer,
	  "RBEZCURVE3D %d \n", order);
  for(i = 0; i < order; i++){
    sprintf(buff, "            [%f,%f,%f|%f] \n",
            l->points[i].x, l->points[i].y, l->points[i].z, l->weights[i]);
    strcat(string_rep_buffer, buff);
  }
  return string_rep_buffer; /* Don't free this memory!!! */
}



static LIST 
KeyPoints(GraphicObj *p)
{
  LIST temp_list;
  KeyPointRec *key_p;
  ERBezC3DWCRec *l_p;
  int order, i;
  
  l_p       = (ERBezC3DWCRec *)(p->spec.data);
  order = l_p -> order;
  temp_list = make_list();
  
  for(i = 0; i < order; i++){
    key_p = (KeyPointRec*)add_to_tail(temp_list, make_node(sizeof(KeyPointRec)));
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
  ERBezC3DWCRec *_p;
  int i, order;
  
  _p = (ERBezC3DWCRec *)(gp->spec.data);
  order = _p->order;
  
  i = hn - 1;
  if (hn < 1 || hn > order) 
    i = 0;
  
  _p->points[i].x = pos->x;
  _p->points[i].y = pos->y;
  _p->points[i].z = pos->z;
  return gp;
}



static EHandleNum   
ReshapeHandle(EView *v_p, GraphicObj *p, WCRec *nearp)
{
  ERBezC3DWCRec *l_p;
  VCRec nvc, pvc;
  FPNum d, dist;
  int i, order, hn;
  
  l_p       = (ERBezC3DWCRec *)(p->spec.data);
  order = l_p -> order;
  
  EVWCtoVC(v_p, nearp, &nvc);
  EVWCtoVC(v_p, &(l_p->points[0]), &pvc);
  dist = DistOfPnts(pvc.u, pvc.v, nvc.u, nvc.v);
  hn = 0;
  for(i = 1; i < order; i++){
    EVWCtoVC(v_p, &(l_p->points[i]), &pvc);
    d = DistOfPnts(pvc.u, pvc.v, nvc.u, nvc.v);
    if(d < dist){
      dist = d;
      hn = i;
    }
  }
  return((EHandleNum)(hn + 1));
}



static void 
ShowHandles(EView *v_p, GraphicObj *p)
{
  ERBezC3DWCRec *l_p;
  int i, order;
  
  l_p = (ERBezC3DWCRec *)p->spec.data;
  order = l_p -> order;
  for(i = 0; i < order; i++)
    EGDrawHandle(v_p, &(l_p->points[i]));
}



static void 
EraseHandles(EView *v_p, GraphicObj *p)
{
  ERBezC3DWCRec *l_p;
  int i, order;
  
  l_p = (ERBezC3DWCRec *)p->spec.data;
  order = l_p -> order;
  for(i = 0; i < order; i++)
    EGEraseHandle(v_p, &(l_p->points[i]));
}




static void 
XYofHandle(GraphicObj *p, EHandleNum hn, WCRec *pos)
{
  ERBezC3DWCRec *l_p;
  int i, order;
  
  l_p = (ERBezC3DWCRec *)p->spec.data;
  order = l_p -> order;
  i = hn - 1;
  if (hn < 1 || hn > order)
    i = 0;
  pos->x = l_p->points[i].x;
  pos->y = l_p->points[i].y;
  pos->z = l_p->points[i].z;
}



static void 
RetrieveFrom(FILE *file_p, GraphicSpecObj *gsp)
{
  ERBezC3DWCRec *p;
  int style, width, sp, sc, i, ti, order;
  char color[64];
  EPixel pixel;
  BOOLEAN success;
  double x, y, z, weight;
  
  if ((p = (ERBezC3DWCRec *)make_node(sizeof(ERBezC3DWCRec))) != NULL) {
    fscanf(file_p, "%d", &order);
    p -> order = order;
    if((p -> points = (WCRec *)calloc(order, sizeof(WCRec))) == NULL){
      fprintf(stderr, "Memory allocation for RBezC3D in RetrieveFrom failed");
      exit(0);
    }
    if((p -> weights = (float *)calloc(order, sizeof(float))) == NULL){
      fprintf(stderr, "Memory allocation for RBezC3D in RetrieveFrom failed");
      exit(0);
    }
    for (i = 0; i < order; i++) {
      fscanf(file_p, "%lf %lf %lf %lf", &x, &y, &z, &weight);
      p->points[i].x = x;
      p->points[i].y = y;
      p->points[i].z = z;
      p->weights[i] = weight;
    }
    fscanf(file_p,
	   "%d %d %d %d %d %d %s",
	   &(p->attributes.spec.have_any),
	   &sp, &sc, &width, &style, &ti, color);
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
  gsp->data_length = sizeof(ERBezC3DWCRec);
}



static void 
StoreOn(FILE *file_p, GraphicObj *g_p)
{
  ERBezC3DWCRec *p;
  int style, width, sp, sc, ti, i, order;
  char *color;
  
  p = (ERBezC3DWCRec *)g_p->spec.data;
  order = p->order;
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
  fprintf(file_p, "%d \n", order);
  for(i = 0; i < order; i++)
    fprintf(file_p, "%f %f %f %f \n",
            p->points[i].x, p->points[i].y, p->points[i].z, p->weights[i]);
  fprintf(file_p, "%d %d %d %d %d %d %s\n",	  p->attributes.spec.have_any,
	  sp, sc, width, style, ti, color);
}



static void 
ChangeAttributes(GraphicObj *p)
{
  ERBezC3DWCRec *dp;
  unsigned long mask;
  
  mask = EASValsGetChangeMask();
  dp = (ERBezC3DWCRec *)p->spec.data;
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
  ERBezC3DWCRec *dp;
  
  dp = (ERBezC3DWCRec *)p->spec.data;
  
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
  ERBezC3DWCRec *dp;
  
  dp = (ERBezC3DWCRec *)p->spec.data;
  
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


static GraphicObj *
DeepCopy(GraphicObj *p)
{
  GraphicObj *gp;
  ERBezC3DWCRec *thedp;
  caddr_t new_data_p;
  WCRec *points_p, *points_gp;
  float *weights_p, *weights_gp;
  int i, order;
  
  /*	gp = EGDefaultCopyGraphics(p); */
  if ((new_data_p =
       (caddr_t)copy_node(p->spec.data, p->spec.data_length)) == NULL) {
    fprintf(stderr, "Failed copy data\n");
    exit(0);
  }
  gp = EGCreateGraphics(p->methods, new_data_p, p->spec.data_length);
  
  thedp = (ERBezC3DWCRec *)(gp->spec.data);
  order = thedp->order;
  
  if((thedp -> points = (WCRec *)calloc(order, sizeof(WCRec))) == NULL){
    fprintf(stderr, "Memory allocation for RBezC3D in DeepCopy failed");
    exit(0);
  }
  if((thedp -> weights = (float *)calloc(order, sizeof(float))) == NULL){
    fprintf(stderr, "Memory allocation for RBezC3D in DeepCopy failed");
    exit(0);
  }
  
  points_p = ((ERBezC3DWCRec *)(p->spec.data)) -> points;
  weights_p = ((ERBezC3DWCRec *)(p->spec.data)) -> weights;
  points_gp = thedp -> points;
  weights_gp = thedp -> weights;
  
  for(i = 0; i < order; i++){
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
  ERBezC3DWCRec *thedp;
  thedp = (ERBezC3DWCRec *)(p->spec.data);
  
  free(thedp->points);
  free(thedp->weights);
  
  /* 	EGDefaultDestroyGraphics(p); */
  free_node(thedp);                        /* free specialized data */
  free_node(p);                            /* free top struct */
}



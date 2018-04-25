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

#define HEXAH_PRIVATE_HEADER
#include "Ehexah.h"

/* ========================================================================= */
/* PRIVATE DATA */
/* ========================================================================= */

#define STRING_BUFF_SIZE 512

static char string_rep_buffer[STRING_BUFF_SIZE];

/* ========================================================================= */
/* PUBLIC */
/* ========================================================================= */

GraphicObj *
CreateHexahedron(WCRec *points)
{
  EHexahedronWCRec *thedp;
  GraphicObj *g_p;
 
  thedp = (EHexahedronWCRec *)make_node(sizeof(EHexahedronWCRec));
  g_p = EGCreateGraphics(&hexah_graphic_methods, (caddr_t)thedp,
			 sizeof(EHexahedronWCRec));
  ModifyGeometry(g_p, points);
  thedp->attributes.spec.fill_style = FILL_SOLID;
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
  EHexahedronWCRec *thedp;

  thedp = (EHexahedronWCRec *)(p->spec.data);

  EVHexahPrimitive(v_p, p,
                   thedp->points,
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
  return EG_HEXAHEDRON;
}



static char *
AsString(GraphicObj *g_p)
{
  EHexahedronWCRec *l;

  l = (EHexahedronWCRec *)g_p->spec.data;
  sprintf(string_rep_buffer,
	  "HEXA [%f,%f,%f] [%f,%f,%f] \n"
    "     [%f,%f,%f] [%f,%f,%f] \n"
    "     [%f,%f,%f] [%f,%f,%f] \n"
    "     [%f,%f,%f] [%f,%f,%f]",
	  l->points[0].x, l->points[0].y, l->points[0].z, 
          l->points[1].x, l->points[1].y, l->points[1].z, 
          l->points[2].x, l->points[2].y, l->points[2].z, 
          l->points[3].x, l->points[3].y, l->points[3].z, 
          l->points[4].x, l->points[4].y, l->points[4].z, 
          l->points[5].x, l->points[5].y, l->points[5].z, 
          l->points[6].x, l->points[6].y, l->points[6].z, 
          l->points[7].x, l->points[7].y, l->points[7].z);
  
  return string_rep_buffer; /* Don't free this memory!!! */
}



static void 
RetrieveFrom(FILE *file_p, GraphicSpecObj *gsp)
{
  EHexahedronWCRec *p;
  int filled, edge_flag;
  char color[64];
  EPixel pixel;
  BOOLEAN success;
  double x1, y1, z1, x2, y2, z2, x3,  y3, z3, x4,  y4, z4;
  double x5, y5, z5, x6, y6, z6, x7,  y7, z7, x8,  y8, z8;
  float shrink;
  
  if ((p = (EHexahedronWCRec *)make_node(sizeof(EHexahedronWCRec)))
      != NULL) {
    fscanf(file_p,
	   "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf \n"
	   "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
	   &x1, &y1, &z1, &x2, &y2, &z2, &x3, &y3, &z3, &x4, &y4, &z4,
           &x5, &y5, &z5, &x6, &y6, &z6, &x7, &y7, &z7, &x8, &y8, &z8);
    p->points[0].x = x1;
    p->points[0].y = y1;
    p->points[0].z = z1;
    p->points[1].x = x2;
    p->points[1].y = y2;
    p->points[1].z = z2;
    p->points[2].x = x3;
    p->points[2].y = y3;
    p->points[2].z = z3;
    p->points[3].x = x4;
    p->points[3].y = y4;
    p->points[3].z = z4;
    p->points[4].x = x5;
    p->points[4].y = y5;
    p->points[4].z = z5;
    p->points[5].x = x6;
    p->points[5].y = y6;
    p->points[5].z = z6;
    p->points[6].x = x7;
    p->points[6].y = y7;
    p->points[6].z = z7;
    p->points[7].x = x8;
    p->points[7].y = y8;
    p->points[7].z = z8;
    
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
  gsp->data_length = sizeof(EHexahedronWCRec);
}
 


static void 
StoreOn(FILE *file_p, GraphicObj *g_p)
{
  EHexahedronWCRec *thedp = (EHexahedronWCRec *)g_p->spec.data;
  char *color, *edge_color;

  if (thedp->attributes.spec.have_any == YES) {
    color = ColorGetStringFromPixel(thedp->attributes.spec.color);
    if (color == NULL)
      color = "white";
    edge_color = ColorGetStringFromPixel(thedp->attributes.spec.edge_color);
    if (edge_color == NULL)
      edge_color = "white";
  } else {
    edge_color = color = "white";
  }
  fprintf(file_p,
	  "%f %f %f %f %f %f %f %f %f %f %f %f\n"
          "%f %f %f %f %f %f %f %f %f %f %f %f\n%d %d %d %f\n%s\n%s\n",
	  thedp->points[0].x, thedp->points[0].y, thedp->points[0].z, 
          thedp->points[1].x, thedp->points[1].y, thedp->points[1].z, 
          thedp->points[2].x, thedp->points[2].y, thedp->points[2].z, 
          thedp->points[3].x, thedp->points[3].y, thedp->points[3].z, 
          thedp->points[4].x, thedp->points[4].y, thedp->points[4].z, 
          thedp->points[5].x, thedp->points[5].y, thedp->points[5].z, 
          thedp->points[6].x, thedp->points[6].y, thedp->points[6].z, 
          thedp->points[7].x, thedp->points[7].y, thedp->points[7].z,
	  (thedp->attributes.spec.have_any),
	  thedp->attributes.spec.fill_style,
	  thedp->attributes.spec.edge_flag,
	  thedp->attributes.spec.shrink,
	  color, edge_color);
}




static void 
BoundingBoxVC(EView *v_p, GraphicObj *g_p,
              VCRec *ll_return, VCRec *ur_return)
{
  EHexahedronWCRec *thedp = (EHexahedronWCRec *)(g_p->spec.data);
  VCRec vcp[8];
  int i;
  
  for (i = 0; i < 8; i++)
    EVWCtoVC(v_p, &(thedp->points[i]), &vcp[i]);

  ur_return->u = ll_return->u = vcp[0].u;
  ur_return->v = ll_return->v = vcp[0].v;
  ur_return->n = ll_return->n = vcp[0].n;

  for (i = 1; i < 8; i++) {
    ll_return->u = min(ll_return->u, vcp[i].u);
    ll_return->v = min(ll_return->v, vcp[i].v);
    ll_return->n = min(ll_return->n, vcp[i].n);
    ur_return->u = max(ur_return->u, vcp[i].u);
    ur_return->v = max(ur_return->v, vcp[i].v);
    ur_return->n = max(ur_return->n, vcp[i].n);
  }
}



static void 
BoundingBoxWC(GraphicObj *g_p, WCRec *ll_return, WCRec *ur_return)
{
  EHexahedronWCRec *thedp = (EHexahedronWCRec *)(g_p->spec.data);
  int i;
  
  ur_return->x = ll_return->x = thedp->points[0].x;
  ur_return->y = ll_return->y = thedp->points[0].y;
  ur_return->z = ll_return->z = thedp->points[0].z;

  for (i = 1; i < 8; i++) {
    ll_return->x = min(ll_return->x, thedp->points[i].x);
    ll_return->y = min(ll_return->y, thedp->points[i].y);
    ll_return->z = min(ll_return->z, thedp->points[i].z);
    ur_return->x = max(ur_return->x, thedp->points[i].x);
    ur_return->y = max(ur_return->y, thedp->points[i].y);
    ur_return->z = max(ur_return->z, thedp->points[i].z);
  }
}




static void 
XorDraw(EView *v_p, GraphicObj *g_p)
{
  EHexahedronWCRec *thedp = (EHexahedronWCRec *)(g_p->spec.data);

  EVHexahPrimitive(v_p, g_p,
                   thedp->points,
                   thedp->attributes.spec.color,
                   thedp->attributes.spec.fill_style,
                   NO, 0,
                   thedp->attributes.spec.shrink,
                   DRAW_MODE_XORDRAW);
}

 


static GraphicObj *
ModifyGeometry(GraphicObj *g_p, WCRec *points)
{
  EHexahedronWCRec *thedp = (EHexahedronWCRec *)(g_p->spec.data);
  int i;

  for (i = 0; i < 8; i++) {
    thedp->points[i].x = points[i].x;
    thedp->points[i].y = points[i].y;
    thedp->points[i].z = points[i].z;
  }

  return g_p;
}



static BOOLEAN 
GetGeometry(GraphicObj *g_p, WCRec *points, int *npoints)
{
  EHexahedronWCRec *thedp = (EHexahedronWCRec *)(g_p->spec.data);
  int i;

  for (i = 0; i < 8; i++) {
    points[i].x = thedp->points[i].x;
    points[i].y = thedp->points[i].y;
    points[i].z = thedp->points[i].z;
  }
  *npoints    = 8;
  
  return YES;
}



static void 
Hilite(EView *v_p, GraphicObj *g_p)
{
  EHexahedronWCRec *thedp = (EHexahedronWCRec *)(g_p->spec.data);
  
  EVHexahPrimitive(v_p, g_p,
                   thedp->points,
                   thedp->attributes.spec.color,
                   FILL_HOLLOW, NO, 0,
                   1.0,
                   DRAW_MODE_HILITE);
}



static void 
Unhilite(EView *v_p, GraphicObj *g_p)
{
  EHexahedronWCRec *thedp = (EHexahedronWCRec *)(g_p->spec.data);

  EVHexahPrimitive(v_p, g_p,
                   thedp->points,
                   thedp->attributes.spec.color,
                   FILL_HOLLOW, NO, 0,
                   1.0,
                   DRAW_MODE_UNHILITE);
}



static void 
Erase(EView *v_p, GraphicObj *g_p)
{
  EHexahedronWCRec *thedp  = (EHexahedronWCRec *)(g_p->spec.data);

  EVHexahPrimitive(v_p, g_p,
                   thedp->points,
                   thedp->attributes.spec.color,
                   thedp->attributes.spec.fill_style,
                   NO, 0,
                   thedp->attributes.spec.shrink,
                   DRAW_MODE_ERASE);
}



static GraphicObj *
Translate(GraphicObj *g_p, WCRec *by)
{
  EHexahedronWCRec *thedp = (EHexahedronWCRec *)g_p->spec.data;
  int i;

  for (i = 0; i < 8; i++) {
    thedp->points[i].x += by->x;
    thedp->points[i].y += by->y;
    thedp->points[i].z += by->z;
  }

  return g_p;
}



static GraphicObj *
Rotate(GraphicObj *g_p, WCRec *center, WCRec *axial)
{
  EHexahedronWCRec *thedp = (EHexahedronWCRec *)g_p->spec.data;
  WCRec x_c;
  int i;

  for (i = 0; i < 8; i++) {
    x_c.x = thedp->points[i].x - center->x;
    x_c.y = thedp->points[i].y - center->y;
    x_c.z = thedp->points[i].z - center->z;
    RotVectAboutVect(axial, &x_c);
    thedp->points[i].x = center->x + x_c.x;
    thedp->points[i].y = center->y + x_c.y;
    thedp->points[i].z = center->z + x_c.z;
  }

  return g_p;
}



static GraphicObj  *
MirrorPlane(GraphicObj *g_p, WCRec *point_on_plane,
            WCRec *unit_plane_normal)
{
  EHexahedronWCRec *thedp = (EHexahedronWCRec *)g_p->spec.data;
  int i;

  for (i = 0; i < 8; i++) {
    MirrorPointInPlane(point_on_plane, unit_plane_normal, &thedp->points[i]);
  }

  return g_p;
}



static GraphicObj  *
MirrorCenter(GraphicObj *g_p, WCRec *center)
{
  EHexahedronWCRec *thedp = (EHexahedronWCRec *)g_p->spec.data;
  int i;

  for (i = 0; i < 8; i++) {
    thedp->points[i].x = 2.*center->x - thedp->points[i].x;
    thedp->points[i].y = 2.*center->y - thedp->points[i].y;
    thedp->points[i].z = 2.*center->z - thedp->points[i].z;
  }

  return g_p;
}


 
static GraphicObj *
Scale(GraphicObj *g_p, WCRec *center, 
		   FPNum sx, FPNum sy, FPNum sz)
{
  EHexahedronWCRec *thedp = (EHexahedronWCRec *)g_p->spec.data;
  int i;

  for (i = 0; i < 8; i++) {
    thedp->points[i].x = center->x + (thedp->points[i].x - center->x) * sx;
    thedp->points[i].y = center->y + (thedp->points[i].y - center->y) * sy;
    thedp->points[i].z = center->z + (thedp->points[i].z - center->z) * sz;
  }
  return g_p;
}



static BOOLEAN 
HitByBox(EView *v_p, EBoxVC2DRec *box,
			   ESelectCriteria sel_crit, GraphicObj *g_p)
{
  VCRec ll, ur;
  EHexahedronWCRec *thedp = (EHexahedronWCRec *)g_p->spec.data;
  int i;
  VCRec vcp[8];
  WC2DRec wcp[2];
  
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
      for (i = 0; i < 8; i++)
        EVWCtoVC(v_p, &(thedp->points[i]), &vcp[i]);

      if (!BoxOverlapsTriangle(box,
                               (VC2DRec *)&vcp[0],
                               (VC2DRec *)&vcp[1],
                               (VC2DRec *)&vcp[5]))
        if (!BoxOverlapsTriangle(box,
                                 (VC2DRec *)&vcp[0],
                                 (VC2DRec *)&vcp[5],
                                 (VC2DRec *)&vcp[4]))
          if (!BoxOverlapsTriangle(box,
                                   (VC2DRec *)&vcp[1],
                                   (VC2DRec *)&vcp[2],
                                   (VC2DRec *)&vcp[6]))
            if (!BoxOverlapsTriangle(box,
                                     (VC2DRec *)&vcp[1],
                                     (VC2DRec *)&vcp[5],
                                     (VC2DRec *)&vcp[6]))
              if (!BoxOverlapsTriangle(box,
                                       (VC2DRec *)&vcp[2],
                                       (VC2DRec *)&vcp[3],
                                       (VC2DRec *)&vcp[7]))
                if (!BoxOverlapsTriangle(box,
                                         (VC2DRec *)&vcp[2],
                                         (VC2DRec *)&vcp[6],
                                         (VC2DRec *)&vcp[7]))
                  if (!BoxOverlapsTriangle(box,
                                           (VC2DRec *)&vcp[3],
                                           (VC2DRec *)&vcp[0],
                                           (VC2DRec *)&vcp[4]))
                    if (!BoxOverlapsTriangle(box,
                                             (VC2DRec *)&vcp[3],
                                             (VC2DRec *)&vcp[7],
                                             (VC2DRec *)&vcp[4]))
                      if (!BoxOverlapsTriangle(box,
                                               (VC2DRec *)&vcp[4],
                                               (VC2DRec *)&vcp[5],
                                               (VC2DRec *)&vcp[6]))
                        if (!BoxOverlapsTriangle(box,
                                                 (VC2DRec *)&vcp[4],
                                                 (VC2DRec *)&vcp[7],
                                                 (VC2DRec *)&vcp[6]))
                          if (!BoxOverlapsTriangle(box,
                                                   (VC2DRec *)&vcp[0],
                                                   (VC2DRec *)&vcp[1],
                                                   (VC2DRec *)&vcp[2]))
                            if (!BoxOverlapsTriangle(box,
                                                     (VC2DRec *)&vcp[0],
                                                     (VC2DRec *)&vcp[3],
                                                     (VC2DRec *)&vcp[2]))
                              return NO;
      return YES;
        
    }
    break;
  case INTERSECT:
  default:
    {
      for (i = 0; i < 8; i++)
        EVWCtoVC(v_p, &(thedp->points[i]), &vcp[i]);

#define ASS(wcp, vcp) {wcp.x = vcp.u; wcp.y = vcp.v;}

      ASS(wcp[0], vcp[0]); ASS(wcp[1], vcp[1]);
      if (!BoxIntersectsLine(box, &wcp[0], &wcp[1])) {
        ASS(wcp[0], vcp[2]);
        if (!BoxIntersectsLine(box, &wcp[0], &wcp[1])) {
          ASS(wcp[1], vcp[3]);
          if (!BoxIntersectsLine(box, &wcp[0], &wcp[1])) {
            ASS(wcp[0], vcp[0]);
            if (!BoxIntersectsLine(box, &wcp[0], &wcp[1])) {
              ASS(wcp[1], vcp[4]);
              if (!BoxIntersectsLine(box, &wcp[0], &wcp[1])) {
                 ASS(wcp[0], vcp[5]);
                 if (!BoxIntersectsLine(box, &wcp[0], &wcp[1])) {
                   ASS(wcp[1], vcp[6]);
                   if (!BoxIntersectsLine(box, &wcp[0], &wcp[1])) {
                     ASS(wcp[0], vcp[7]);
                     if (!BoxIntersectsLine(box, &wcp[0], &wcp[1])) {
                       ASS(wcp[1], vcp[4]);
                       if (!BoxIntersectsLine(box, &wcp[0], &wcp[1])) {
                         return NO;
                       }
                     }
                   }
                 }
               }
            }
          }
        }
      }
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
  EHexahedronWCRec *thedp = (EHexahedronWCRec *)(p->spec.data);
  int i;
  
  temp_list = make_list();

  for (i=0; i < 8; i++) {
    key_p =
      (KeyPointRec*)add_to_tail(temp_list, make_node(sizeof(KeyPointRec)));
    key_p->coords.x = thedp->points[i].x;
    key_p->coords.y = thedp->points[i].y;
    key_p->coords.z = thedp->points[i].z;
    key_p->dist_from_hit = 0.0;
  }

  return temp_list;
}



static GraphicObj *
Reshape(GraphicObj *p, EHandleNum hn, WCRec *pos)
{
  EHexahedronWCRec *thedp;

  thedp = (EHexahedronWCRec *)(p->spec.data);

  if (hn < 1 || hn > 8) return p;

  thedp->points[hn-1].x = pos->x;
  thedp->points[hn-1].y = pos->y;
  thedp->points[hn-1].z = pos->z;

  return p;
}



static EHandleNum 
ReshapeHandle(EView *v_p, GraphicObj *p, WCRec *nearp)
{
  EHexahedronWCRec *thedp = (EHexahedronWCRec *)(p->spec.data);
  FPNum tmp, dist;
  VCRec nvc, vcp[8];
  int i, at;

  for (i = 0; i < 8; i++)
    EVWCtoVC(v_p, &(thedp->points[i]), &vcp[i]);

  EVWCtoVC(v_p, nearp, &nvc);
  
  dist = DistOfPnts(vcp[0].u, vcp[0].v, nvc.u, nvc.v);
  at   = 1;

  for (i = 1; i < 8; i++) {
    tmp = DistOfPnts(vcp[i].u, vcp[i].v, nvc.u, nvc.v);
    if (tmp < dist) {
      at   = i+1;
      dist = tmp;
    }
  }
  return (EHandleNum) at;
}



static void 
ShowHandles(EView *v_p, GraphicObj *p)
{
  EHexahedronWCRec *thedp = (EHexahedronWCRec *)p->spec.data;
  int i;

  for (i = 0; i < 8; i++) {
    EGDrawHandle(v_p, &(thedp->points[i]));
  }
}
  


static void 
EraseHandles(EView *v_p, GraphicObj *p)
{
  EHexahedronWCRec *thedp = (EHexahedronWCRec *)p->spec.data;
  int i;

  for (i = 0; i < 8; i++) {
    EGEraseHandle(v_p, &(thedp->points[i]));
  }
}



static void 
XYofHandle(GraphicObj *p, EHandleNum hn, WCRec *pos)
{
  EHexahedronWCRec *thedp = (EHexahedronWCRec *)p->spec.data;

  if (hn > 0 && hn <= 8) {
    pos->x = thedp->points[hn-1].x;
    pos->y = thedp->points[hn-1].y;
    pos->z = thedp->points[hn-1].z;
  } else {
    pos->x = 0;
    pos->y = 0;
    pos->z = 0;
  }
}



static void 
ChangeAttributes(GraphicObj *p)
{
  EHexahedronWCRec *dp;
  unsigned long mask;

  dp = (EHexahedronWCRec *)p->spec.data;

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
  EHexahedronWCRec *dp;

  dp = (EHexahedronWCRec *)p->spec.data;
  
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
  EHexahedronWCRec *dp;

  dp = (EHexahedronWCRec *)p->spec.data;
  
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

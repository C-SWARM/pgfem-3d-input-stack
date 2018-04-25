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

/* This file holds the redefinition of graphic methods for the vector */
/* with data. */

#include "Econfig.h"
#include <stdlib.h>

#include "Eview.h"
#include "Egraphic.h"
#include "Egeomops.h"
#include "Ecolors.h"
#include "Eactset.h"

#define VECTOR3D_PRIVATE_HEADER
#include "Evector3d.h"

/* ========================================================================= */
/* PRIVATE DATA */
/* ========================================================================= */

#define STRING_BUFF_SIZE 132

static char string_rep_buffer[STRING_BUFF_SIZE];

/* ========================================================================= */
/* PUBLIC */
/* ========================================================================= */

GraphicObj *
CreateVector3D(WCRec *points)
{
  EVector3DWCRec *m_p;
  GraphicObj *g_p;

  m_p = (EVector3DWCRec *)make_node(sizeof(EVector3DWCRec));
  g_p = EGCreateGraphics(&vector3d_graphic_methods, (caddr_t)m_p,
			 sizeof(EVector3DWCRec));
  ModifyGeometry(g_p, points);
  m_p->attributes.spec.have_any = YES;
	m_p->attributes.spec.scale    = 1.0;
  m_p->attributes.spec.rate     = 0.15;
  m_p->attributes.spec.type     = ARROW_VECMARKER;
	m_p->attributes.spec.use_fringe_table = NO;
	m_p->attributes.spec.shift_flag = NO;
  return g_p;
}



static void 
Draw(EView *v_p, GraphicObj *p)
{
  EVector3DWCRec *thedp;

  thedp = (EVector3DWCRec *)(p->spec.data);

  EVVector3DPrimitive(v_p, p, 
			&thedp->origin, &thedp->components,
			thedp->attributes.spec.type,
			thedp->attributes.spec.scale,
			thedp->attributes.spec.rate,
			thedp->attributes.spec.color,
			thedp->attributes.spec.use_fringe_table,
		  thedp->attributes.spec.shift_flag,									
			DRAW_MODE_DRAW);
}



static EGraphicType 
Type(GraphicObj *g_p)
{
  return EG_VECTOR3D;
}



static char *
AsString(GraphicObj *g_p)
{
  EVector3DWCRec *l;

  l = (EVector3DWCRec *)g_p->spec.data;
  sprintf(string_rep_buffer,
	  "VECTOR origin [%f,%f,%f] \n"
    "       components [%f,%f,%f] \n"
    "       type %d, scale %f, rate %f",
	  l->origin.x, l->origin.y, l->origin.z,
	  l->components.x, l->components.y, l->components.z,
    l->attributes.spec.type, l->attributes.spec.scale, l->attributes.spec.rate);
  
  return string_rep_buffer; /* Don't free this memory!!! */
}



static void 
RetrieveFrom(FILE *file_p, GraphicSpecObj *gsp)
{
  EVector3DWCRec *p;
  double scale, rate;
	BOOLEAN use_ftable, shift_flag;
  int type;
  char color[64];
  EPixel pixel;
  BOOLEAN success;
  double x, y, z, dx, dy, dz;
  
  if ((p = (EVector3DWCRec *)make_node(sizeof(EVector3DWCRec))) != NULL) {
    fscanf(file_p,
	   "%lf %lf %lf %lf %lf %lf %d %d %lf %lf %d %d %s",
	   &x, &y, &z, &dx, &dy, &dz,
	   &(p->attributes.spec.have_any),
	   &type, &scale, &rate, &use_ftable, &shift_flag, color);
    p->origin.x = x;     p->origin.y = y;    p->origin.z = z;
    p->components.x = dx;     p->components.y = dy;    p->components.z = dz;
    p->attributes.spec.type = (EVecMarkerType)type;
    p->attributes.spec.scale = (FPNum)scale;
		p->attributes.spec.rate = (FPNum)rate;
	  p->attributes.spec.use_fringe_table = (BOOLEAN)use_ftable;	
	  p->attributes.spec.shift_flag = (BOOLEAN)shift_flag;	
    pixel = ColorGetPixelFromString(color, &success);
    if (success)
      p->attributes.spec.color = pixel;
  }
  gsp->data        = (caddr_t)p;
  gsp->data_length = sizeof(EVector3DWCRec);
}



static void    
StoreOn(FILE *file_p, GraphicObj *g_p)
{
  EVector3DWCRec *p;
  char *color;
  
  p = (EVector3DWCRec *)g_p->spec.data;
  if (p->attributes.spec.have_any == YES) {
    color = ColorGetStringFromPixel(p->attributes.spec.color);
    if (color == NULL)
      color = "black";
  } else 
    color = "black";
  fprintf(file_p,
	  "%f %f %f %f %f %f %d %d %f %f %d %d %s\n",
	  p->origin.x, p->origin.y, p->origin.z, 
	  p->components.x, p->components.y, p->components.z, 
	  p->attributes.spec.have_any,
	  p->attributes.spec.type,
	  p->attributes.spec.scale,
	  p->attributes.spec.rate,
		p->attributes.spec.use_fringe_table,
		p->attributes.spec.shift_flag,
	  color);
}



static void 
BoundingBoxVC(EView *v_p, GraphicObj *p,
			   VCRec *ll_return, VCRec *ur_return)
{
  EVector3DWCRec *thedp;
  VCRec vcp_o, vcp_t;
	WCRec tail;
	FPNum scale;

  thedp = (EVector3DWCRec *)(p->spec.data);
	scale = thedp->attributes.spec.scale;
	if(thedp->attributes.spec.shift_flag == YES)scale = -scale;
	tail.x = thedp->origin.x - thedp->components.x * scale;
	tail.y = thedp->origin.y - thedp->components.y * scale;
	tail.z = thedp->origin.z - thedp->components.z * scale;
  EVWCtoVC(v_p, &(thedp->origin), &vcp_o);
  EVWCtoVC(v_p, &tail, &vcp_t);
  ll_return->u = min(vcp_o.u, vcp_t.u);
  ll_return->v = min(vcp_o.v, vcp_t.v);
  ll_return->n = min(vcp_o.n, vcp_t.n);
  ur_return->u = max(vcp_o.u, vcp_t.u);
  ur_return->v = max(vcp_o.v, vcp_t.v);
  ur_return->n = max(vcp_o.n, vcp_t.n);
}



static void 
BoundingBoxWC(GraphicObj *p, WCRec *ll_return, WCRec *ur_return)
{
  EVector3DWCRec *thedp;
	FPNum comp, scale;

  thedp = (EVector3DWCRec *)(p->spec.data);
	scale = thedp->attributes.spec.scale;
	if(thedp->attributes.spec.shift_flag == YES)scale = -scale;
	if((comp = thedp->components.x * scale) > 0){
		ll_return->x = thedp->origin.x - comp;
		ur_return->x = thedp->origin.x;
	}
	else{
		ll_return->x = thedp->origin.x;
		ur_return->x = thedp->origin.x - comp;
	}
	if((comp = thedp->components.y * scale) > 0){
		ll_return->y = thedp->origin.y - comp;
		ur_return->y = thedp->origin.y;
	}
	else{
		ll_return->y = thedp->origin.y;
		ur_return->y = thedp->origin.y - comp;
	}
	if((comp = thedp->components.z * scale) > 0){
		ll_return->z = thedp->origin.z - comp;
		ur_return->z = thedp->origin.z;
	}
	else{
		ll_return->z = thedp->origin.z;
		ur_return->z = thedp->origin.z - comp;
	}
}




static void 
XorDraw(EView *v_p, GraphicObj *g_p)
{
  EVector3DWCRec *thedp;

  thedp = (EVector3DWCRec *)(g_p->spec.data);
  EVVector3DPrimitive(v_p, g_p, &(thedp->origin), &(thedp->components),
			thedp->attributes.spec.type,
			thedp->attributes.spec.scale,
			thedp->attributes.spec.rate,
			thedp->attributes.spec.color,
			thedp->attributes.spec.use_fringe_table,
		  thedp->attributes.spec.shift_flag,									
      DRAW_MODE_XORDRAW);
}




static GraphicObj *
ModifyGeometry(GraphicObj *g_p, WCRec *p)
{
  EVector3DWCRec *m_p;

  m_p = (EVector3DWCRec *)(g_p->spec.data);
  m_p->origin.x = p[0].x;
  m_p->origin.y = p[0].y;
  m_p->origin.z = p[0].z;
  m_p->components.x = p[1].x;
  m_p->components.y = p[1].y;
  m_p->components.z = p[1].z;
  return g_p;
}



static BOOLEAN 
GetGeometry(GraphicObj *g_p, WCRec *points,
                              int *npoints)
{
  EVector3DWCRec *m_p = (EVector3DWCRec *)(g_p->spec.data);

  points[0].x = m_p->origin.x;
  points[0].y = m_p->origin.y;
  points[0].z = m_p->origin.z;
  points[1].x = m_p->components.x;
  points[1].y = m_p->components.y;
  points[1].z = m_p->components.z;
  *npoints    = 2;
  return YES;
}


static void 
Hilite(EView *v_p, GraphicObj *g_p)
{
  EVector3DWCRec *thedp;

  thedp = (EVector3DWCRec *)(g_p->spec.data);
  EVVector3DPrimitive(v_p, g_p, &(thedp->origin), &(thedp->components),
			thedp->attributes.spec.type,
			thedp->attributes.spec.scale,
			thedp->attributes.spec.rate,
			thedp->attributes.spec.color,
			thedp->attributes.spec.use_fringe_table,
		  thedp->attributes.spec.shift_flag,									
      DRAW_MODE_HILITE);
}



static void 
Unhilite(EView *v_p, GraphicObj *g_p)
{
  EVector3DWCRec *thedp;

  thedp = (EVector3DWCRec *)(g_p->spec.data);
  EVVector3DPrimitive(v_p, g_p, &(thedp->origin), &(thedp->components),
			thedp->attributes.spec.type,
			thedp->attributes.spec.scale,
			thedp->attributes.spec.rate,
			thedp->attributes.spec.color,
			thedp->attributes.spec.use_fringe_table,
		  thedp->attributes.spec.shift_flag,									
      DRAW_MODE_UNHILITE);
}



static void 
Erase(EView *v_p, GraphicObj *g_p)
{
  EVector3DWCRec *thedp;

  thedp = (EVector3DWCRec *)(g_p->spec.data);
  EVVector3DPrimitive(v_p, g_p, &(thedp->origin), &(thedp->components),
			thedp->attributes.spec.type,
			thedp->attributes.spec.scale,
			thedp->attributes.spec.rate,
			thedp->attributes.spec.color,
			thedp->attributes.spec.use_fringe_table,
		  thedp->attributes.spec.shift_flag,									
      DRAW_MODE_ERASE);
}



static GraphicObj *
Translate(GraphicObj *g_p, WCRec *by)
{
  EVector3DWCRec *c;

  c = (EVector3DWCRec *)g_p->spec.data;
  c->origin.x += by->x, c->origin.y += by->y, c->origin.z += by->z;
  return g_p;
}



static GraphicObj *
Rotate(GraphicObj *g_p, WCRec *center, WCRec *axial)
{
  WCRec xo_c, xt_c;
  EVector3DWCRec *c;

  c = (EVector3DWCRec *)g_p->spec.data;
  xt_c.x = (xo_c.x = c->origin.x - center->x) - c->components.x;
  xt_c.y = (xo_c.y = c->origin.y - center->y) - c->components.y;
  xt_c.z = (xo_c.z = c->origin.z - center->z) - c->components.z;
  RotVectAboutVect(axial, &xo_c);
  RotVectAboutVect(axial, &xt_c);
  c->origin.x = center->x + xo_c.x;
  c->origin.y = center->y + xo_c.y;
  c->origin.z = center->z + xo_c.z;
	c->components.x = xo_c.x - xt_c.x;
	c->components.y = xo_c.y - xt_c.y;
	c->components.z = xo_c.z - xt_c.z;
  return g_p;
}



static GraphicObj  *
MirrorPlane(GraphicObj *g_p,
                    WCRec *point_on_plane, WCRec *unit_plane_normal)
{
  WCRec x_c;
  EVector3DWCRec *dp;

  dp = (EVector3DWCRec *)g_p->spec.data;
	x_c.x = dp->origin.x - dp->components.x ;
	x_c.y = dp->origin.y - dp->components.y ;
	x_c.z = dp->origin.z - dp->components.z ;
  MirrorPointInPlane(point_on_plane, unit_plane_normal, &dp->origin);
	MirrorPointInPlane(point_on_plane, unit_plane_normal, &x_c);
	dp -> components.x = dp->origin.x - x_c.x;
	dp -> components.y = dp->origin.y - x_c.y;
	dp -> components.z = dp->origin.z - x_c.z;
  return g_p;
}



static GraphicObj  *
MirrorCenter(GraphicObj *g_p, WCRec *center)
{
  EVector3DWCRec *dp;

  dp = (EVector3DWCRec *)g_p->spec.data;
  dp->origin.x = 2.*center->x - dp->origin.x;
  dp->origin.y = 2.*center->y - dp->origin.y;
  dp->origin.z = 2.*center->z - dp->origin.z;
	dp->components.x = -dp->components.x;
	dp->components.y = -dp->components.y;
	dp->components.z = -dp->components.z;
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
			 ESelectCriteria sel_crit, GraphicObj *p)
{
  EVector3DWCRec *c;
  VCRec vcp_o, vcp_t;
  VCRec ll, ur;
	WCRec tail;
  WC2DRec p1, p2;
	FPNum scale;
  
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
		c = (EVector3DWCRec *)p->spec.data;
		scale = c->attributes.spec.scale;
		if(c->attributes.spec.shift_flag == YES)scale = -scale;
		tail.x = c->origin.x - c->components.x * scale;
		tail.y = c->origin.y - c->components.y * scale;
		tail.z = c->origin.z - c->components.z * scale;
		EVWCtoVC(v_p, &(c->origin), &vcp_o);
		EVWCtoVC(v_p, &tail, &vcp_t);
		p1.x = vcp_o.u; p1.y = vcp_o.v;
		p2.x = vcp_t.u; p2.y = vcp_t.v;
    return BoxIntersectsLine(box, &p1, &p2);
    break;
  }
}




static LIST 
KeyPoints(GraphicObj *p)
{
  LIST temp_list;
  KeyPointRec *key_p;
  EVector3DWCRec *m_p;

  m_p       = (EVector3DWCRec *)(p->spec.data);
  temp_list = make_list();
  key_p = (KeyPointRec*)add_to_tail(temp_list, make_node(sizeof(KeyPointRec)));
  key_p->coords.x = m_p->origin.x;
  key_p->coords.y = m_p->origin.y;
  key_p->coords.z = m_p->origin.z;
  key_p->dist_from_hit = 0.0;

  return temp_list;
}



static void 
ChangeAttributes(GraphicObj *p)
{
  EVector3DWCRec *dp;
  unsigned long mask;

  mask = EASValsGetChangeMask();
  dp = (EVector3DWCRec *)p->spec.data;

  if ((mask & VEC_TYPE_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.type = EASValsGetVecMType();
    dp->attributes.spec.have_any = YES;
  }
  if ((mask & VEC_SCALE_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.scale = EASValsGetVecScale();
    dp->attributes.spec.have_any = YES;
  }
  if ((mask & VEC_RATE_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.rate = EASValsGetVecRate();
    dp->attributes.spec.have_any = YES;
  }
  if ((mask & VEC_SHIFT_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.shift_flag = EASValsGetVecShiftFlag();
    dp->attributes.spec.have_any = YES;
  }
  if ((mask & COLOR_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.color = EASValsGetColor();
    dp->attributes.spec.have_any = YES;
  }
}




static void 
CopyAttributesToActiveSet(GraphicObj *p)
{
  EVector3DWCRec *dp;

  dp = (EVector3DWCRec *)p->spec.data;
  if (dp->attributes.spec.have_any) {
    EASValsSetVecMType(dp->attributes.spec.type);
    EASValsSetVecScale(dp->attributes.spec.scale);
    EASValsSetVecRate(dp->attributes.spec.rate);
    EASValsSetVecShiftFlag(dp->attributes.spec.shift_flag);
    EASValsSetColor(dp->attributes.spec.color);
  }
}



static BOOLEAN
AttribValAsInAS(GraphicObj *p, unsigned long mask)  
{
  EVector3DWCRec *dp;

  dp = (EVector3DWCRec *)p->spec.data;
  
  if ((mask & VEC_TYPE_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.type != EASValsGetVecMType())
      return NO;
  
  if ((mask & VEC_SCALE_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.scale != EASValsGetVecScale())
      return NO;
  
  if ((mask & VEC_RATE_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.rate != EASValsGetVecRate())
      return NO;
  
  if ((mask & VEC_SHIFT_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.shift_flag != EASValsGetVecShiftFlag())
      return NO;
  
  if ((mask & COLOR_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.color != EASValsGetColor())
      return NO;

  return YES;
}

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




/* This file holds the redefinition of graphic methods for the vecmarker. */

#include "Econfig.h"
#include <stdlib.h>
#include <math.h>

#include "Eview.h"
#include "Egraphic.h"
#include "Egeomops.h"
#include "Ecolors.h"
#include "Eactset.h"

#define VECMARKER3D_PRIVATE_HEADER
#include "Evecmark3d.h"

  
/* ========================================================================= */
/* PRIVATE DATA */
/* ========================================================================= */

#define STRING_BUFF_SIZE 132

static char string_rep_buffer[STRING_BUFF_SIZE];

/* ========================================================================= */
/* PUBLIC METHODS */
/* ========================================================================= */

GraphicObj *
CreateVecMarker3D(WCRec *p)
{
  EVecMarker3DWCRec *m_p;
  GraphicObj *g_p;

  m_p = (EVecMarker3DWCRec *)make_node(sizeof(EVecMarker3DWCRec));
  g_p = EGCreateGraphics(&vecmarker3d_graphic_methods, (caddr_t)m_p,
			 sizeof(EVecMarker3DWCRec));
  ModifyGeometry(g_p, p);
  m_p->attributes.spec.have_any = YES;
  m_p->attributes.spec.size     = 40;
	m_p->attributes.spec.shift    = 0;
	m_p->attributes.spec.rate     = 0.3;
  m_p->attributes.spec.type     = ARROW_VECMARKER;
  return g_p;
}



static GraphicObj *
ModifyGeometry(GraphicObj *g_p, WCRec *p)
{
  EVecMarker3DWCRec *m_p;
	double size;

  m_p = (EVecMarker3DWCRec *)(g_p->spec.data);
  m_p->origin.x = p[0].x;
  m_p->origin.y = p[0].y;
  m_p->origin.z = p[0].z;

	size = sqrt(p[1].x * p[1].x + p[1].y * p[1].y + p[1].z * p[1].z);
	if(size == 0.0){
		p[1].x = 1.0;
		p[1].y = 2.0;
		p[1].z = 3.0;
		size = sqrt(14.0);
	}

  m_p->components.x = p[1].x / size;
  m_p->components.y = p[1].y / size;
  m_p->components.z = p[1].z / size;
  return g_p;
}




static BOOLEAN 
GetGeometry(GraphicObj *g_p, WCRec *points,
                              int *npoints)
{
  EVecMarker3DWCRec *m_p = (EVecMarker3DWCRec *)(g_p->spec.data);

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
Draw(EView *v_p, GraphicObj *g_p)
{
  EVecMarker3DWCRec *thedp;
  
  thedp = (EVecMarker3DWCRec *)(g_p->spec.data);
  EVVecMarker3DPrimitive(v_p, g_p, &(thedp->origin), &(thedp->components),
		      thedp->attributes.spec.size,
		      thedp->attributes.spec.shift,
		      thedp->attributes.spec.rate,
		      thedp->attributes.spec.type,
		      thedp->attributes.spec.color,
		      DRAW_MODE_DRAW);
}



static void 
BoundingBoxVC(EView *v_p, GraphicObj *p,
			   VCRec *ll_return, VCRec *ur_return)
{
  EVecMarker3DWCRec *thedp;
  VCRec vcp;

  thedp = (EVecMarker3DWCRec *)(p->spec.data);
  EVWCtoVC(v_p, &(thedp->origin), &vcp);
  ll_return->u = vcp.u;
  ll_return->v = vcp.v;
  ll_return->n = vcp.n;
  ur_return->u = vcp.u;
  ur_return->v = vcp.v;
  ur_return->n = vcp.n;
}



static void 
BoundingBoxWC(GraphicObj *p, WCRec *ll_return, WCRec *ur_return)
{
  EVecMarker3DWCRec *thedp;

  thedp = (EVecMarker3DWCRec *)(p->spec.data);
  ll_return->x = thedp->origin.x;
  ll_return->y = thedp->origin.y;
  ll_return->z = thedp->origin.z;
  ur_return->x = thedp->origin.x;
  ur_return->y = thedp->origin.y;
  ur_return->z = thedp->origin.z;
}



static void 
XorDraw(EView *v_p, GraphicObj *g_p)
{
  EVecMarker3DWCRec *thedp;

  thedp = (EVecMarker3DWCRec *)(g_p->spec.data);
  EVVecMarker3DPrimitive(v_p, g_p, &(thedp->origin), &(thedp->components),
		      thedp->attributes.spec.size,
		      thedp->attributes.spec.shift,
		      thedp->attributes.spec.rate,
		      thedp->attributes.spec.type,
		      thedp->attributes.spec.color,
		      DRAW_MODE_XORDRAW);
}



static void 
Hilite(EView *v_p, GraphicObj *g_p)
{
  EVecMarker3DWCRec *thedp;

  thedp = (EVecMarker3DWCRec *)(g_p->spec.data);
  EVVecMarker3DPrimitive(v_p, g_p, &(thedp->origin), &(thedp->components),
		      thedp->attributes.spec.size,
		      thedp->attributes.spec.shift,
		      thedp->attributes.spec.rate,
		      thedp->attributes.spec.type,
		      thedp->attributes.spec.color,
		      DRAW_MODE_HILITE);
}



static void 
Unhilite(EView *v_p, GraphicObj *g_p)
{
  EVecMarker3DWCRec *thedp;

  thedp = (EVecMarker3DWCRec *)(g_p->spec.data);
  EVVecMarker3DPrimitive(v_p, g_p, &(thedp->origin), &(thedp->components),
		      thedp->attributes.spec.size,
		      thedp->attributes.spec.shift,
		      thedp->attributes.spec.rate,
		      thedp->attributes.spec.type,
		      thedp->attributes.spec.color,
		      DRAW_MODE_UNHILITE);
}



static void 
Erase(EView *v_p, GraphicObj *g_p)
{
  EVecMarker3DWCRec *thedp;

  thedp = (EVecMarker3DWCRec *)(g_p->spec.data);
  EVVecMarker3DPrimitive(v_p, g_p, &(thedp->origin), &(thedp->components),
		      thedp->attributes.spec.size,
		      thedp->attributes.spec.shift,
		      thedp->attributes.spec.rate,
		      thedp->attributes.spec.type,
		      thedp->attributes.spec.color,
		      DRAW_MODE_ERASE);
}



static GraphicObj *
Translate(GraphicObj *g_p, WCRec *by)
{
  EVecMarker3DWCRec *c;

  c = (EVecMarker3DWCRec *)g_p->spec.data;
  c->origin.x += by->x, c->origin.y += by->y, c->origin.z += by->z;
  return g_p;
}



static GraphicObj *
Rotate(GraphicObj *g_p, WCRec *center, WCRec *axial)
{
  WCRec xo_c, xt_c;
  EVecMarker3DWCRec *c;

  c = (EVecMarker3DWCRec *)g_p->spec.data;
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
  EVecMarker3DWCRec *dp;

  dp = (EVecMarker3DWCRec *)g_p->spec.data;
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
  EVecMarker3DWCRec *dp;

  dp = (EVecMarker3DWCRec *)g_p->spec.data;
  dp->origin.x = 2.*center->x - dp->origin.x;
  dp->origin.y = 2.*center->y - dp->origin.y;
  dp->origin.z = 2.*center->z - dp->origin.z;
	dp->components.x = -dp->components.x;
	dp->components.y = -dp->components.y;
	dp->components.z = -dp->components.z;
  return g_p;
}



static GraphicObj *
Scale(GraphicObj *g_p, WCRec *origin,
			  FPNum sx, FPNum sy, FPNum sz) 
{
  return g_p;
}



static BOOLEAN 
HitByBox(EView *v_p, EBoxVC2DRec *box,
			 ESelectCriteria sel_crit, GraphicObj *p)
{
  EVecMarker3DWCRec *c;
  VCRec vcp;
  
  c = (EVecMarker3DWCRec *)p->spec.data;

  EVWCtoVC(v_p, &(c->origin), &vcp);
  if (vcp.u >= box->left   && vcp.u <= box->right &&
      vcp.v >= box->bottom && vcp.v <= box->top)
    return YES;
  else
    return NO;
}



static EGraphicType 
Type(GraphicObj *g_p)
{
  return EG_VECMARKER3D;
}



static char *
AsString(GraphicObj *g_p)
{
  EVecMarker3DWCRec *c;

  c = (EVecMarker3DWCRec *)g_p->spec.data;
  sprintf(string_rep_buffer,
	  "VECMARKER3D origin [%f,%f,%f] \n"
    "            components [%f,%f,%f] \n"
    "            type %d, size %d, shift %d, rate %f",
	  c->origin.x, c->origin.y, c->origin.z,
	  c->components.x, c->components.y, c->components.z,
	  c->attributes.spec.type,
	  c->attributes.spec.size,
	  c->attributes.spec.shift,
		c->attributes.spec.rate);
    
  return string_rep_buffer; /* Don't free this memory!!! */
}



static LIST 
KeyPoints(GraphicObj *p)
{
  LIST temp_list;
  KeyPointRec *key_p;
  EVecMarker3DWCRec *m_p;

  m_p       = (EVecMarker3DWCRec *)(p->spec.data);
  temp_list = make_list();
  key_p = (KeyPointRec*)add_to_tail(temp_list, make_node(sizeof(KeyPointRec)));
  key_p->coords.x = m_p->origin.x;
  key_p->coords.y = m_p->origin.y;
  key_p->coords.z = m_p->origin.z;
  key_p->dist_from_hit = 0.0;

  return temp_list;
}



static void 
RetrieveFrom(FILE *file_p, GraphicSpecObj *gsp)
{
  EVecMarker3DWCRec *p;
  int size;
	int shift;
  int type;
	double rate;
  char color[64];
  EPixel pixel;
  BOOLEAN success;
  double x, y, z, dx, dy, dz;
  
  if ((p = (EVecMarker3DWCRec *)make_node(sizeof(EVecMarker3DWCRec))) != NULL) {
    fscanf(file_p,
	   "%lf %lf %lf %lf %lf %lf %d %d %d %d %lf %s",
	   &x, &y, &z, &dx, &dy, &dz,
	   &(p->attributes.spec.have_any),
	   &type, &size, &shift, &rate, color);
    p->origin.x = x;     p->origin.y = y;    p->origin.z = z;
    p->components.x = dx;     p->components.y = dy;    p->components.z = dz;
    p->attributes.spec.type = (EVecMarkerType)type;
    p->attributes.spec.size = (Dimension)size;
    p->attributes.spec.shift = (Dimension)shift;
		p->attributes.spec.rate = (FPNum)rate;
    pixel = ColorGetPixelFromString(color, &success);
    if (success)
      p->attributes.spec.color = pixel;
  }
  gsp->data        = (caddr_t)p;
  gsp->data_length = sizeof(EVecMarker3DWCRec);
}



static void    
StoreOn(FILE *file_p, GraphicObj *g_p)
{
  EVecMarker3DWCRec *p;
  char *color;
  
  p = (EVecMarker3DWCRec *)g_p->spec.data;
  if (p->attributes.spec.have_any == YES) {
    color = ColorGetStringFromPixel(p->attributes.spec.color);
    if (color == NULL)
      color = "black";
  } else 
    color = "black";
  fprintf(file_p,
	  "%f %f %f %f %f %f %d %d %d %d %f %s\n",
	  p->origin.x, p->origin.y, p->origin.z, 
	  p->components.x, p->components.y, p->components.z, 
	  p->attributes.spec.have_any,
	  p->attributes.spec.type,
	  p->attributes.spec.size,
	  p->attributes.spec.shift,
		p->attributes.spec.rate,
	  color);
}





static void 
ChangeAttributes(GraphicObj *p)
{
  EVecMarker3DWCRec *dp;
  unsigned long mask;

  mask = EASValsGetChangeMask();
  dp = (EVecMarker3DWCRec *)p->spec.data;

  if ((mask & VEC_TYPE_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.type = EASValsGetVecMType();
    dp->attributes.spec.have_any = YES;
  }
  if ((mask & MSIZE_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.size = EASValsGetMSize();
    dp->attributes.spec.have_any = YES;
  }
  if ((mask & VEC_RATE_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.rate = EASValsGetVecRate();
    dp->attributes.spec.have_any = YES;
  }
  if ((mask & VEC_SHIFT_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.shift = EASValsGetVecShift();
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
  EVecMarker3DWCRec *dp;

  dp = (EVecMarker3DWCRec *)p->spec.data;
  if (dp->attributes.spec.have_any) {
    EASValsSetVecMType(dp->attributes.spec.type);
    EASValsSetMSize(dp->attributes.spec.size);
    EASValsSetVecShift(dp->attributes.spec.shift);
    EASValsSetVecRate(dp->attributes.spec.rate);
    EASValsSetColor(dp->attributes.spec.color);
  }
}

static BOOLEAN
AttribValAsInAS(GraphicObj *p, unsigned long mask)  
{
  EVecMarker3DWCRec *dp;

  dp = (EVecMarker3DWCRec *)p->spec.data;
  
  if ((mask & VEC_TYPE_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.type != EASValsGetVecMType())
      return NO;
  
  if ((mask & MSIZE_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.size != EASValsGetMSize())
      return NO;
  
  if ((mask & VEC_SHIFT_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.shift != EASValsGetVecShift())
      return NO;
  
  if ((mask & VEC_RATE_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.rate != EASValsGetVecRate())
      return NO;
  
  if ((mask & COLOR_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.color != EASValsGetColor())
      return NO;

  return YES;
}

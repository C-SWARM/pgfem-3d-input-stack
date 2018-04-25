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

/* This file holds the redefinition of graphic methods for the marker. */

#include "Econfig.h"
#include <stdlib.h>
#include <math.h>

#include "Eview.h"
#include "Egraphic.h"

#define MARKER3D_PRIVATE_HEADER
#include "Emark3d.h"
#include "Egeomops.h"
#include "Ecolors.h"
#include "Eactset.h"


  
/* ========================================================================= */
/* PRIVATE DATA */
/* ========================================================================= */

#define STRING_BUFF_SIZE 132

static char string_rep_buffer[STRING_BUFF_SIZE];

/* ========================================================================= */
/* PUBLIC METHODS */
/* ========================================================================= */

GraphicObj *
CreateMarker3D(WCRec *p)
{
  EMarker3DWCRec *m_p;
  GraphicObj *g_p;

  m_p = (EMarker3DWCRec *)make_node(sizeof(EMarker3DWCRec));
  g_p = EGCreateGraphics(&marker3d_graphic_methods, (caddr_t)m_p,
			 sizeof(EMarker3DWCRec));
  ModifyGeometry(g_p, p);
  m_p->attributes.spec.have_any = YES;
  m_p->attributes.spec.size     = 8;
  m_p->attributes.spec.type     = CIRCLE_MARKER;
  return g_p;
}



static GraphicObj *
ModifyGeometry(GraphicObj *g_p, WCRec *p)
{
  EMarker3DWCRec *m_p;

  m_p = (EMarker3DWCRec *)(g_p->spec.data);
  m_p->center.x = p->x;
  m_p->center.y = p->y;
  m_p->center.z = p->z;
  return g_p;
}




static BOOLEAN 
GetGeometry(GraphicObj *g_p, WCRec *points,
                              int *npoints)
{
  EMarker3DWCRec *m_p = (EMarker3DWCRec *)(g_p->spec.data);

  points[0].x = m_p->center.x;
  points[0].y = m_p->center.y;
  points[0].z = m_p->center.z;
  *npoints    = 1;
  return YES;
}


static void 
Draw(EView *v_p, GraphicObj *g_p)
{
  EMarker3DWCRec *thedp;
  
  thedp = (EMarker3DWCRec *)(g_p->spec.data);
  EVMarker3DPrimitive(v_p, g_p, &(thedp->center),
		      thedp->attributes.spec.size,
		      thedp->attributes.spec.type,
		      thedp->attributes.spec.color,
		      DRAW_MODE_DRAW);
}



static void 
BoundingBoxVC(EView *v_p, GraphicObj *p,
			   VCRec *ll_return, VCRec *ur_return)
{
  EMarker3DWCRec *thedp;
  VCRec vcp;

  thedp = (EMarker3DWCRec *)(p->spec.data);
  EVWCtoVC(v_p, &(thedp->center), &vcp);
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
  EMarker3DWCRec *thedp;

  thedp = (EMarker3DWCRec *)(p->spec.data);
  ll_return->x = thedp->center.x;
  ll_return->y = thedp->center.y;
  ll_return->z = thedp->center.z;
  ur_return->x = thedp->center.x;
  ur_return->y = thedp->center.y;
  ur_return->z = thedp->center.z;
}



static void 
XorDraw(EView *v_p, GraphicObj *g_p)
{
  EMarker3DWCRec *thedp;

  thedp = (EMarker3DWCRec *)(g_p->spec.data);
  EVMarker3DPrimitive(v_p, g_p, &(thedp->center),
		      thedp->attributes.spec.size,
		      thedp->attributes.spec.type,
		      thedp->attributes.spec.color,
		      DRAW_MODE_XORDRAW);
}



static void 
Hilite(EView *v_p, GraphicObj *g_p)
{
  EMarker3DWCRec *thedp;

  thedp = (EMarker3DWCRec *)(g_p->spec.data);
  EVMarker3DPrimitive(v_p, g_p, &(thedp->center),
		      thedp->attributes.spec.size,
		      thedp->attributes.spec.type,
		      thedp->attributes.spec.color,
		      DRAW_MODE_HILITE);
}



static void 
Unhilite(EView *v_p, GraphicObj *g_p)
{
  EMarker3DWCRec *thedp;

  thedp = (EMarker3DWCRec *)(g_p->spec.data);
  EVMarker3DPrimitive(v_p, g_p, &(thedp->center),
		      thedp->attributes.spec.size,
		      thedp->attributes.spec.type,
		      thedp->attributes.spec.color,
		      DRAW_MODE_UNHILITE);
}



static void 
Erase(EView *v_p, GraphicObj *g_p)
{
  EMarker3DWCRec *thedp;

  thedp = (EMarker3DWCRec *)(g_p->spec.data);
  EVMarker3DPrimitive(v_p, g_p, &(thedp->center),
		      thedp->attributes.spec.size,
		      thedp->attributes.spec.type,
		      thedp->attributes.spec.color,
		      DRAW_MODE_ERASE);
}



static GraphicObj *
Translate(GraphicObj *g_p, WCRec *by)
{
  EMarker3DWCRec *c;

  c = (EMarker3DWCRec *)g_p->spec.data;
  c->center.x += by->x, c->center.y += by->y, c->center.z += by->z;
  return g_p;
}



static GraphicObj *
Rotate(GraphicObj *g_p, WCRec *center, WCRec *axial)
{
  WCRec x_c;
  EMarker3DWCRec *c;

  c = (EMarker3DWCRec *)g_p->spec.data;
  x_c.x = c->center.x - center->x;
  x_c.y = c->center.y - center->y;
  x_c.z = c->center.z - center->z;
  RotVectAboutVect(axial, &x_c);
  c->center.x = center->x + x_c.x;
  c->center.y = center->y + x_c.y;
  c->center.z = center->z + x_c.z;
  
  return g_p;
}



static GraphicObj  *
MirrorPlane(GraphicObj *g_p,
                    WCRec *point_on_plane, WCRec *unit_plane_normal)
{
  EMarker3DWCRec *dp;

  dp = (EMarker3DWCRec *)g_p->spec.data;
  MirrorPointInPlane(point_on_plane, unit_plane_normal, &dp->center);
  
  return g_p;
}



static GraphicObj  *
MirrorCenter(GraphicObj *g_p, WCRec *center)
{
  EMarker3DWCRec *dp;

  dp = (EMarker3DWCRec *)g_p->spec.data;
  dp->center.x = 2.*center->x - dp->center.x;
  dp->center.y = 2.*center->y - dp->center.y;
  dp->center.z = 2.*center->z - dp->center.z;
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
  EMarker3DWCRec *c;
  VCRec vcp;
  
  c = (EMarker3DWCRec *)p->spec.data;

  EVWCtoVC(v_p, &(c->center), &vcp);
  if (vcp.u >= box->left   && vcp.u <= box->right &&
      vcp.v >= box->bottom && vcp.v <= box->top)
    return YES;
  else
    return NO;
}



static EGraphicType 
Type(GraphicObj *g_p)
{
  return EG_MARKER3D;
}



static char *
AsString(GraphicObj *g_p)
{
  EMarker3DWCRec *c;

  c = (EMarker3DWCRec *)g_p->spec.data;
  sprintf(string_rep_buffer,
	  "MARKER3D center [%f,%f,%f] \n"
    "         size %d type %d",
	  c->center.x, c->center.y, c->center.z,
	  c->attributes.spec.size,
	  c->attributes.spec.type);
    
  return string_rep_buffer; /* Don't free this memory!!! */
}



static LIST 
KeyPoints(GraphicObj *p)
{
  LIST temp_list;
  KeyPointRec *key_p;
  EMarker3DWCRec *m_p;

  m_p       = (EMarker3DWCRec *)(p->spec.data);
  temp_list = make_list();
  key_p = (KeyPointRec*)add_to_tail(temp_list, make_node(sizeof(KeyPointRec)));
  key_p->coords.x = m_p->center.x;
  key_p->coords.y = m_p->center.y;
  key_p->coords.z = m_p->center.z;
  key_p->dist_from_hit = 0.0;

  return temp_list;
}



static void 
RetrieveFrom(FILE *file_p, GraphicSpecObj *gsp)
{
  EMarker3DWCRec *p;
  int size;
  int type;
  char color[64];
  EPixel pixel;
  BOOLEAN success;
  double x, y, z;
  
  if ((p = (EMarker3DWCRec *)make_node(sizeof(EMarker3DWCRec))) != NULL) {
    fscanf(file_p,
	   "%lf %lf %lf %d %d %d %s",
	   &x, &y, &z,
	   &(p->attributes.spec.have_any),
	   &type, &size, color);
    p->center.x = x;     p->center.y = y;    p->center.z = z;
    p->attributes.spec.type = (EMarkerType)type;
    p->attributes.spec.size = (Dimension)size;
    pixel = ColorGetPixelFromString(color, &success);
    if (success)
      p->attributes.spec.color = pixel;
  }
  gsp->data        = (caddr_t)p;
  gsp->data_length = sizeof(EMarker3DWCRec);
}



static void    
StoreOn(FILE *file_p, GraphicObj *g_p)
{
  EMarker3DWCRec *p;
  char *color;
  
  p = (EMarker3DWCRec *)g_p->spec.data;
  if (p->attributes.spec.have_any == YES) {
    color = ColorGetStringFromPixel(p->attributes.spec.color);
    if (color == NULL)
      color = "black";
  } else 
    color = "black";
  fprintf(file_p,
	  "%f %f %f %d %d %d %s\n",
	  p->center.x, p->center.y, p->center.z, 
	  p->attributes.spec.have_any,
	  p->attributes.spec.type,
	  p->attributes.spec.size,
	  color);
}





static void 
ChangeAttributes(GraphicObj *p)
{
  EMarker3DWCRec *dp;
  unsigned long mask;

  mask = EASValsGetChangeMask();
  dp = (EMarker3DWCRec *)p->spec.data;

  if ((mask & MTYPE_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.type = EASValsGetMType();
    dp->attributes.spec.have_any = YES;
  }
  if ((mask & MSIZE_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.size = EASValsGetMSize();
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
  EMarker3DWCRec *dp;

  dp = (EMarker3DWCRec *)p->spec.data;
  if (dp->attributes.spec.have_any) {
    EASValsSetMType(dp->attributes.spec.type);
    EASValsSetMSize(dp->attributes.spec.size);
    EASValsSetColor(dp->attributes.spec.color);
  }
}

static BOOLEAN
AttribValAsInAS(GraphicObj *p, unsigned long mask)  
{
  EMarker3DWCRec *dp;

  dp = (EMarker3DWCRec *)p->spec.data;
  
  if ((mask & MTYPE_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.type != EASValsGetMType())
      return NO;
  
  if ((mask & MSIZE_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.size != EASValsGetMSize())
      return NO;
  
  if ((mask & COLOR_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.color != EASValsGetColor())
      return NO;

  return YES;
}


static GraphicObj *
Reshape(GraphicObj *p, EHandleNum hn, WCRec *pos)
{
  EMarker3DWCRec *m_p;
  
  m_p       = (EMarker3DWCRec *)(p->spec.data);
  m_p -> center.x = pos -> x;
  m_p -> center.y = pos -> y;
  m_p -> center.z = pos -> z;
  return p;
}



static void 
XYofHandle(GraphicObj *p, EHandleNum hn, WCRec *pos)
{
  EMarker3DWCRec *m_p = (EMarker3DWCRec *)p->spec.data;

  pos->x = m_p->center.x;
  pos->y = m_p->center.y;
  pos->z = m_p->center.z;
}


static EHandleNum   
ReshapeHandle(EView *v_p, GraphicObj *p, WCRec *nearp)
{
  return (EHandleNum)1;
}

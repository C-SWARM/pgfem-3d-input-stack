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

#define MARKERWD3D_PRIVATE_HEADER
#include "Emarkwd3d.h"
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
CreateMarkerWD3D(WCRec *p, double value)
{
  EMarkerWD3DWCRec *m_p;
  GraphicObj *g_p;
  static BOOLEAN first_time = YES;

  if (first_time) {
    EMaskArray ma = {0, 0};
    EGMethodsRec mt =     {
      Type,
      AsString,
      StoreOn,
      RetrieveFrom,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      Draw,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      ModifyAssocData,
      GetAssocData,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
    };
    
    EGSetOverrideEntry(ma, (&mt), type_func, Type);
    EGSetOverrideEntry(ma, (&mt), string_rep_func, AsString);
    EGSetOverrideEntry(ma, (&mt), store_on_func, StoreOn);
    EGSetOverrideEntry(ma, (&mt), retrieve_from_func, RetrieveFrom);
    EGSetOverrideEntry(ma, (&mt), draw_func, Draw);
    EGSetOverrideEntry(ma, (&mt), modify_assoc_data_func, ModifyAssocData);
    EGSetOverrideEntry(ma, (&mt), get_assoc_data_func, GetAssocData);

    /* set up the methods  */
    g_p = CreateMarker3D(p); /* create a temp. obj. to inherit from */
    /* method table to use */
    marker_wd_3d_graphic_methods = EGCopyMethodTable(g_p, ma, &mt);
    EGDeepDestroyGraphics(g_p); /* destroy the temporary object */
    first_time = NO;
  }

	m_p = (EMarkerWD3DWCRec *)make_node(sizeof(EMarkerWD3DWCRec));
	g_p = EGCreateGraphics(marker_wd_3d_graphic_methods, (caddr_t)m_p,
												 sizeof(EMarkerWD3DWCRec));
  EGInvokeMethod(g_p, modify_geom_func, (g_p, p));
	m_p->attributes.spec.have_any = YES;
	m_p->attributes.spec.size     = 8;
	m_p->attributes.spec.type     = CIRCLE_MARKER;
		
	m_p->attributes.spec.val      = value;
	return g_p;
}



static void 
Draw(EView *v_p, GraphicObj *g_p)
{
  EMarkerWD3DWCRec *thedp;
  EFringeTable ft;
  int range;
  FPNum value;

  thedp = (EMarkerWD3DWCRec *)(g_p->spec.data);
  value = thedp->attributes.spec.val;
  ft = EVGetAssocFringeTable(v_p);
  range = ColorFringeValueToRange(ft, value);

  EVMarker3DPrimitive(v_p, g_p, &(thedp->center),
		      thedp->attributes.spec.size,
		      thedp->attributes.spec.type,
		      ColorFringeRangeToColor(range),
		      DRAW_MODE_DRAW);
}



static EGraphicType 
Type(GraphicObj *g_p)
{
  return EG_MARKERWD3D;
}



static char *
AsString(GraphicObj *g_p)
{
  EMarkerWD3DWCRec *c;

  c = (EMarkerWD3DWCRec *)g_p->spec.data;
  sprintf(string_rep_buffer,
	  "MARKERWD3D center [%f,%f,%f] \n"
    "           size %d type %d val %f",
	  c->center.x, c->center.y, c->center.z,
	  c->attributes.spec.size,
	  c->attributes.spec.type,
    c->attributes.spec.val);
    
  return string_rep_buffer; /* Don't free this memory!!! */
}


static void 
RetrieveFrom(FILE *file_p, GraphicSpecObj *gsp)
{
  EMarkerWD3DWCRec *p;
  int size;
  int type;
  double x, y, z, val;
  
  if ((p = (EMarkerWD3DWCRec *)make_node(sizeof(EMarkerWD3DWCRec))) != NULL) {
    fscanf(file_p,
	   "%lf %lf %lf %d %d %d %lf",
	   &x, &y, &z,
	   &(p->attributes.spec.have_any),
	   &type, &size, &val);
    p->center.x = x;     p->center.y = y;    p->center.z = z;
    p->attributes.spec.type = (EMarkerType)type;
    p->attributes.spec.size = (Dimension)size;
    p->attributes.spec.val = val;
  }
  gsp->data        = (caddr_t)p;
  gsp->data_length = sizeof(EMarkerWD3DWCRec);
}



static void    
StoreOn(FILE *file_p, GraphicObj *g_p)
{
  EMarkerWD3DWCRec *p;
  char *color;
  
  p = (EMarkerWD3DWCRec *)g_p->spec.data;
  if (p->attributes.spec.have_any == YES) {
    color = ColorGetStringFromPixel(p->attributes.spec.color);
    if (color == NULL)
      color = "black";
  } else 
    color = "black";
  fprintf(file_p,
	  "%f %f %f %d %d %d %f\n",
	  p->center.x, p->center.y, p->center.z, 
	  p->attributes.spec.have_any,
	  p->attributes.spec.type,
	  p->attributes.spec.size,
	  p->attributes.spec.val);
}



static GraphicObj *
ModifyAssocData(GraphicObj *g_p, FPNum *val)
{
  EMarkerWD3DWCRec *p = (EMarkerWD3DWCRec *)g_p->spec.data;
  p->attributes.spec.val  = *val;

  return g_p;
}



static BOOLEAN
GetAssocData(GraphicObj *g_p, FPNum *val, int *nvals)
{
  EMarkerWD3DWCRec *p = (EMarkerWD3DWCRec *)g_p->spec.data;
  *val = p->attributes.spec.val;
	*nvals = 1;
  return YES; /* the values are valid */
}


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

/* This file holds the redefinition of graphic methods for the quad */
/* with data. */

#include "Econfig.h"
#include <stdlib.h>

#include "Eview.h"
#include "Egraphic.h"
#include "Egeomops.h"
#include "Ecolors.h"
#include "Eactset.h"

#define QUADWD3D_PRIVATE_HEADER
#include "Equadwd3d.h"

/* ========================================================================= */
/* PRIVATE DATA */
/* ========================================================================= */

#define STRING_BUFF_SIZE 256

static char string_rep_buffer[STRING_BUFF_SIZE];

/* ========================================================================= */
/* PUBLIC */
/* ========================================================================= */

GraphicObj *
CreateQuadWD3D(WCRec *points, double v1, double v2, double v3, double v4)
{
  EQuadWD3DWCRec *thedp;
  GraphicObj *g_p;
  static BOOLEAN first_time = YES;

  if (first_time) {
    EMaskArray ma = {0, 0};
    EGMethodsRec mt = {
      Type,
      AsString,
      StoreOn,
      RetrieveFrom,
      NULL, /* inherits */
      NULL, /* inherits */
      NULL, /* inherits */
      NULL, /* inherits */
      NULL, /* inherits */
      NULL, /* inherits */
      NULL, /* inherits */
      Draw,
      NULL, /* inherits */
      NULL, /* inherits */
      NULL, /* inherits */
      NULL, /* inherits */
      NULL, /* inherits */
      NULL, /* inherits */
      NULL, /* inherits */
      NULL, /* inherits */
      NULL, /* inherits */
      NULL, /* inherits */
      NULL, /* inherits */
      ModifyAssocData,
      GetAssocData,
      NULL, /* inherits */
      NULL, /* inherits */
      NULL, /* inherits */
      NULL, /* inherits */
      NULL, /* inherits */
      NULL, /* inherits */
      NULL, /* inherits */
      NULL, /* inherits */
      NULL, /* inherits */
      NULL  /* inherits */
    };

    EGSetOverrideEntry(ma, (&mt), type_func, Type);
    EGSetOverrideEntry(ma, (&mt), string_rep_func, AsString);
    EGSetOverrideEntry(ma, (&mt), store_on_func, StoreOn);
    EGSetOverrideEntry(ma, (&mt), retrieve_from_func, RetrieveFrom);
    EGSetOverrideEntry(ma, (&mt), draw_func, Draw);
    EGSetOverrideEntry(ma, (&mt), modify_assoc_data_func, ModifyAssocData);
    EGSetOverrideEntry(ma, (&mt), get_assoc_data_func, GetAssocData);
    
    /* set up the methods  */
    g_p = CreateQuad3D(points); /* create a temporary object to inherit from */
    /* method table to use */
    quad_wd_3d_graphic_methods = EGCopyMethodTable(g_p, ma, &mt);
    EGDeepDestroyGraphics(g_p); /* destroy the temporary object */
    first_time = NO;
  }
    
  thedp = (EQuadWD3DWCRec *)make_node(sizeof(EQuadWD3DWCRec));
  g_p = EGCreateGraphics(quad_wd_3d_graphic_methods, (caddr_t)thedp,
			 sizeof(EQuadWD3DWCRec));
  EGInvokeMethod(g_p, modify_geom_func, (g_p, points));
  thedp->quad3d.attributes.spec.fill_style = FILL_SOLID;
  thedp->quad3d.attributes.spec.color      = 0x0;
  thedp->quad3d.attributes.spec.edge_flag  = NO;
  thedp->quad3d.attributes.spec.edge_color = 0x0;
  thedp->quad3d.attributes.spec.shrink     = 1.0;
  thedp->quad3d.attributes.spec.have_any   = YES;
  thedp->assoc_data.first_val  = v1;
  thedp->assoc_data.second_val = v2;
  thedp->assoc_data.third_val  = v3;
  thedp->assoc_data.fourth_val = v4;
  return g_p;
}




static void 
Draw(EView *v_p, GraphicObj *p)
{
  EQuadWD3DWCRec *thedp = (EQuadWD3DWCRec *)(p->spec.data);

  EVQuadWD3DPrimitive(v_p, p,
                      &thedp->quad3d.point_1,
                      &thedp->quad3d.point_2,
                      &thedp->quad3d.point_3,
                      &thedp->quad3d.point_4,
                      thedp->assoc_data.first_val,
                      thedp->assoc_data.second_val,
                      thedp->assoc_data.third_val,
                      thedp->assoc_data.fourth_val,
                      thedp->quad3d.attributes.spec.color,
                      thedp->quad3d.attributes.spec.fill_style,
                      thedp->quad3d.attributes.spec.edge_flag,
                      thedp->quad3d.attributes.spec.edge_color,
                      thedp->quad3d.attributes.spec.shrink,
                      DRAW_MODE_DRAW);
}



static EGraphicType 
Type(GraphicObj *g_p)
{
  return EG_QUADWD3D;
}



static char *
AsString(GraphicObj *g_p)
{
  EQuadWD3DWCRec *l;

  l = (EQuadWD3DWCRec *)g_p->spec.data;
  sprintf(string_rep_buffer,
	  "QUAD WITH DATA [%f,%f,%f] [%f,%f,%f] \n"
    "               [%f,%f,%f] [%f,%f,%f] \n"
    "               [%f,%f,%f,%f]",
	  l->quad3d.point_1.x,
          l->quad3d.point_1.y,
          l->quad3d.point_1.z,
	  l->quad3d.point_2.x,
          l->quad3d.point_2.y,
          l->quad3d.point_2.z, 
	  l->quad3d.point_3.x,
          l->quad3d.point_3.y,
          l->quad3d.point_3.z,
          l->quad3d.point_4.x,
          l->quad3d.point_4.y,
          l->quad3d.point_4.z,
          l->assoc_data.first_val,
          l->assoc_data.second_val,
          l->assoc_data.third_val,
          l->assoc_data.fourth_val
          );
  
  return string_rep_buffer; /* Don't free this memory!!! */
}



static void 
RetrieveFrom(FILE *file_p, GraphicSpecObj *gsp)
{
  EQuadWD3DWCRec *p;
  int filled, edge_flag;
  char color[64], edge_color[64];
  EPixel pixel;
  BOOLEAN success;
  double x1, y1, z1, x2, y2, z2, x3,  y3, z3, x4, y4, z4, v1, v2, v3, v4;
  float shrink;
  
  if ((p = (EQuadWD3DWCRec *)make_node(sizeof(EQuadWD3DWCRec)))
      != NULL) {
    fscanf(file_p,
	   "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
	   &x1, &y1, &z1, &x2, &y2, &z2, &x3, &y3, &z3, &x4, &y4, &z4,
           &v1, &v2, &v3, &v4);
    fscanf(file_p,
	   "%d %d %f %d",
	   &(p->quad3d.attributes.spec.have_any),
	   &filled, &shrink, &edge_flag);
    fscanf(file_p, "%s", edge_color);
    fscanf(file_p, "%s", color);
    p->quad3d.point_1.x = x1;
    p->quad3d.point_1.y = y1;
    p->quad3d.point_1.z = z1;
    p->quad3d.point_2.x = x2;
    p->quad3d.point_2.y = y2;
    p->quad3d.point_2.z = z2;
    p->quad3d.point_3.x = x3;
    p->quad3d.point_3.y = y3;
    p->quad3d.point_3.z = z3;
    p->quad3d.point_4.x = x4;
    p->quad3d.point_4.y = y4;
    p->quad3d.point_4.z = z4;
    p->quad3d.attributes.spec.fill_style = filled;
    p->quad3d.attributes.spec.shrink     = shrink;
    p->quad3d.attributes.spec.edge_flag  = edge_flag;
    pixel = ColorGetPixelFromString(edge_color, &success);
    if (success)
      p->quad3d.attributes.spec.edge_color = pixel;
    pixel = ColorGetPixelFromString(color, &success);
    if (success)
      p->quad3d.attributes.spec.color = pixel;
    p->assoc_data.first_val  = v1;
    p->assoc_data.second_val = v2;
    p->assoc_data.third_val  = v3;
    p->assoc_data.fourth_val = v4;
  }
  gsp->data        = (caddr_t)p;
  gsp->data_length = sizeof(EQuadWD3DWCRec);
}
 


static void 
StoreOn(FILE *file_p, GraphicObj *g_p)
{
  EQuadWD3DWCRec *p;
  char *color, *edge_color;

  p = (EQuadWD3DWCRec *)g_p->spec.data;
  if (p->quad3d.attributes.spec.have_any == YES) {
    color =
      ColorGetStringFromPixel(p->quad3d.attributes.spec.color);
    if (color == NULL)
      color = "white";
    edge_color = ColorGetStringFromPixel(p->quad3d.attributes.spec.edge_color);
    if (edge_color == NULL)
      edge_color = "white";
  } else {
    edge_color = color = "white";
  }
  fprintf(file_p,
	  "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n"
          "%d %d %f %d\n%s\n%s\n",
	  p->quad3d.point_1.x,
          p->quad3d.point_1.y,
          p->quad3d.point_1.z,
	  p->quad3d.point_2.x,
          p->quad3d.point_2.y,
          p->quad3d.point_2.z, 
	  p->quad3d.point_3.x,
          p->quad3d.point_3.y,
          p->quad3d.point_3.z,
          p->quad3d.point_4.x,
          p->quad3d.point_4.y,
          p->quad3d.point_4.z,
          p->assoc_data.first_val,
          p->assoc_data.second_val,
          p->assoc_data.third_val,
          p->assoc_data.fourth_val,
	  (p->quad3d.attributes.spec.have_any),
	  p->quad3d.attributes.spec.fill_style,
	  p->quad3d.attributes.spec.shrink,
          p->quad3d.attributes.spec.edge_flag,
          edge_color,
	  color);
}



static GraphicObj *
ModifyAssocData(GraphicObj *g_p, FPNum *vals)
{
  EQuadWD3DWCRec *p = (EQuadWD3DWCRec *)g_p->spec.data;

  p->assoc_data.first_val  = vals[0];
  p->assoc_data.second_val = vals[1];
  p->assoc_data.third_val  = vals[2];
  p->assoc_data.fourth_val = vals[3];
  return g_p;
}



static BOOLEAN
GetAssocData(GraphicObj *g_p, FPNum *vals, int *nvals)
{
  EQuadWD3DWCRec *p = (EQuadWD3DWCRec *)g_p->spec.data;

  vals[0] = p->assoc_data.first_val;
  vals[1] = p->assoc_data.second_val;
  vals[2] = p->assoc_data.third_val;
  vals[3] = p->assoc_data.fourth_val;
  *nvals  = 4;

  return YES; /* the values are valid */
}


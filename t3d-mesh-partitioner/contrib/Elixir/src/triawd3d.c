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

/* This file holds the redefinition of graphic methods for the triangle */
/* with data. */

#include "Econfig.h"
#include <stdlib.h>

#include "Eview.h"
#include "Egraphic.h"
#include "Egeomops.h"
#include "Ecolors.h"
#include "Eactset.h"

#define TRIANGLEWD3D_PRIVATE_HEADER
#include "Etriawd3d.h"

/* ========================================================================= */
/* PRIVATE DATA */
/* ========================================================================= */

#define STRING_BUFF_SIZE 132

static char string_rep_buffer[STRING_BUFF_SIZE];

/* ========================================================================= */
/* PUBLIC */
/* ========================================================================= */

GraphicObj *
CreateTriangleWD3D(WCRec *points, double v1, double v2, double v3)
{
  ETriangleWD3DWCRec *thedp;
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
    g_p = CreateTriangle3D(points); /* create a temp. obj. to inherit from */
    /* method table to use */
    triangle_wd_3d_graphic_methods = EGCopyMethodTable(g_p, ma, &mt);
    EGDeepDestroyGraphics(g_p); /* destroy the temporary object */
    first_time = NO;
  }
    
  thedp = (ETriangleWD3DWCRec *)make_node(sizeof(ETriangleWD3DWCRec));
  g_p = EGCreateGraphics(triangle_wd_3d_graphic_methods, (caddr_t)thedp,
			 sizeof(ETriangleWD3DWCRec));
  EGInvokeMethod(g_p, modify_geom_func, (g_p, points));
  thedp->triangle3d.attributes.spec.fill_style = FILL_SOLID;
  thedp->triangle3d.attributes.spec.color      = 0x0;
  thedp->triangle3d.attributes.spec.edge_flag  = NO;
  thedp->triangle3d.attributes.spec.edge_color = 0x0;
  thedp->triangle3d.attributes.spec.shrink     = 1.0;
  thedp->triangle3d.attributes.spec.have_any   = YES;
  thedp->assoc_data.first_val  = v1;
  thedp->assoc_data.second_val = v2;
  thedp->assoc_data.third_val  = v3;
  return g_p;
}



static void 
Draw(EView *v_p, GraphicObj *p)
{
  ETriangleWD3DWCRec *thedp;

  thedp = (ETriangleWD3DWCRec *)(p->spec.data);

  EVTriangleWD3DPrimitive(v_p, p,
                          &thedp->triangle3d.point_1,
                          &thedp->triangle3d.point_2,
                          &thedp->triangle3d.point_3,
                          thedp->assoc_data.first_val,
                          thedp->assoc_data.second_val,
                          thedp->assoc_data.third_val,
                          thedp->triangle3d.attributes.spec.color,
                          thedp->triangle3d.attributes.spec.fill_style,
                          thedp->triangle3d.attributes.spec.edge_flag,
                          thedp->triangle3d.attributes.spec.edge_color,
                          thedp->triangle3d.attributes.spec.shrink,
                          DRAW_MODE_DRAW);
}



static EGraphicType 
Type(GraphicObj *g_p)
{
  return EG_TRIANGLEWD3D;
}



static char *
AsString(GraphicObj *g_p)
{
  ETriangleWD3DWCRec *l;

  l = (ETriangleWD3DWCRec *)g_p->spec.data;
  sprintf(string_rep_buffer,
	  "TRIANGLE WITH DATA [%f,%f,%f] [%f,%f,%f] [%f,%f,%f]\n"
    "                   [%f,%f,%f]",
	  l->triangle3d.point_1.x,
          l->triangle3d.point_1.y,
          l->triangle3d.point_1.z,
	  l->triangle3d.point_2.x,
          l->triangle3d.point_2.y,
          l->triangle3d.point_2.z, 
	  l->triangle3d.point_3.x,
          l->triangle3d.point_3.y,
          l->triangle3d.point_3.z,
          l->assoc_data.first_val,
          l->assoc_data.second_val,
          l->assoc_data.third_val
          );
  
  return string_rep_buffer; /* Don't free this memory!!! */
}



static void 
RetrieveFrom(FILE *file_p, GraphicSpecObj *gsp)
{
  ETriangleWD3DWCRec *p;
  int filled, edge_flag;
  char color[64];
  EPixel pixel;
  BOOLEAN success;
  double x1, y1, z1, x2, y2, z2, x3,  y3, z3, v1, v2, v3;
  float shrink;
  
  if ((p = (ETriangleWD3DWCRec *)make_node(sizeof(ETriangleWD3DWCRec)))
      != NULL) {
    fscanf(file_p,
	   "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
	   &x1, &y1, &z1, &x2, &y2, &z2, &x3,  &y3, &z3, &v1, &v2, &v3);
    p->triangle3d.point_1.x = x1;
    p->triangle3d.point_1.y = y1;
    p->triangle3d.point_1.z = z1;
    p->triangle3d.point_2.x = x2;
    p->triangle3d.point_2.y = y2;
    p->triangle3d.point_2.z = z2;
    p->triangle3d.point_3.x = x3;
    p->triangle3d.point_3.y = y3;
    p->triangle3d.point_3.z = z3;
    p->assoc_data.first_val  = v1;
    p->assoc_data.second_val = v2;
    p->assoc_data.third_val  = v3;
    fscanf(file_p,
	   "%d %d %d %f",
	   &(p->triangle3d.attributes.spec.have_any),
	   &filled, &edge_flag, &shrink);
    p->triangle3d.attributes.spec.fill_style = filled;
    p->triangle3d.attributes.spec.edge_flag  = edge_flag;
    p->triangle3d.attributes.spec.shrink     = shrink;
    fscanf(file_p, "%s", color);
    pixel = ColorGetPixelFromString(color, &success);
    if (success)
      p->triangle3d.attributes.spec.color = pixel;
    fscanf(file_p, "%s", color);
    pixel = ColorGetPixelFromString(color, &success);
    if (success)
      p->triangle3d.attributes.spec.edge_color = pixel;
    pixel = ColorGetPixelFromString(color, &success);
  }
  gsp->data        = (caddr_t)p;
  gsp->data_length = sizeof(ETriangleWD3DWCRec);
}
 


static void 
StoreOn(FILE *file_p, GraphicObj *g_p)
{
  ETriangleWD3DWCRec *p;
  char *color, *edge_color;

  p = (ETriangleWD3DWCRec *)g_p->spec.data;
  if (p->triangle3d.attributes.spec.have_any == YES) {
    color =
      ColorGetStringFromPixel(p->triangle3d.attributes.spec.color);
    if (color == NULL)
      color = "white";
    edge_color =
      ColorGetStringFromPixel(p->triangle3d.attributes.spec.edge_color);
    if (edge_color == NULL)
      edge_color = "white";
  } else {
    edge_color = color = "white";
  }
  fprintf(file_p,
	  "%f %f %f %f %f %f %f %f %f %f %f %f\n %d %d %d %f\n%s\n%s\n",
	  p->triangle3d.point_1.x,
          p->triangle3d.point_1.y,
          p->triangle3d.point_1.z,
	  p->triangle3d.point_2.x,
          p->triangle3d.point_2.y,
          p->triangle3d.point_2.z, 
	  p->triangle3d.point_3.x,
          p->triangle3d.point_3.y,
          p->triangle3d.point_3.z,
          p->assoc_data.first_val,
          p->assoc_data.second_val,
          p->assoc_data.third_val,
	  (p->triangle3d.attributes.spec.have_any),
	  p->triangle3d.attributes.spec.fill_style,
	  p->triangle3d.attributes.spec.edge_flag,
	  p->triangle3d.attributes.spec.shrink,
	  color, edge_color);
}


static GraphicObj *
ModifyAssocData(GraphicObj *g_p, FPNum *vals)
{
  ETriangleWD3DWCRec *p = (ETriangleWD3DWCRec *)g_p->spec.data;

  p->assoc_data.first_val  = vals[0];
  p->assoc_data.second_val = vals[1];
  p->assoc_data.third_val  = vals[2];
  return g_p;
}



static BOOLEAN
GetAssocData(GraphicObj *g_p, FPNum *vals, int *nvals)
{
  ETriangleWD3DWCRec *p = (ETriangleWD3DWCRec *)g_p->spec.data;

  vals[0] = p->assoc_data.first_val;
  vals[1] = p->assoc_data.second_val;
  vals[2] = p->assoc_data.third_val;
  *nvals  = 3;

  return YES; /* the values are valid */
}


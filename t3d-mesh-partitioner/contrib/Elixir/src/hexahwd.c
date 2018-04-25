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

#define HEXAHWD_PRIVATE_HEADER
#include "Ehexahwd.h"

/* ========================================================================= */
/* PRIVATE DATA */
/* ========================================================================= */

#define STRING_BUFF_SIZE 1024

static char string_rep_buffer[STRING_BUFF_SIZE];

/* ========================================================================= */
/* PUBLIC */
/* ========================================================================= */

GraphicObj *
CreateHexahedronWD(WCRec *points, double *vals)
{
  EHexahedronWDWCRec *thedp;
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
    g_p = CreateHexahedron(points); /* create a temp. obj. to inherit from */
    /* method table to use */
    hexah_wd_3d_graphic_methods = EGCopyMethodTable(g_p, ma, &mt);
    EGDeepDestroyGraphics(g_p); /* destroy the temporary object */
    first_time = NO;
  }
    
  thedp = (EHexahedronWDWCRec *)make_node(sizeof(EHexahedronWDWCRec));
  g_p = EGCreateGraphics(hexah_wd_3d_graphic_methods, (caddr_t)thedp,
			 sizeof(EHexahedronWDWCRec));
  EGInvokeMethod(g_p, modify_geom_func, (g_p, points));
  thedp->hexah.attributes.spec.fill_style = FILL_SOLID;
  thedp->hexah.attributes.spec.color      = 0x0;
  thedp->hexah.attributes.spec.edge_flag  = NO;
  thedp->hexah.attributes.spec.edge_color = 0x0;
  thedp->hexah.attributes.spec.shrink     = 1.0;
  thedp->hexah.attributes.spec.have_any   = YES;
  {
    int i;
    for (i=0;i<8;i++)
      thedp->assoc_data.vals[i]  = vals[i];
  }
  return g_p;
}



static void 
Draw(EView *v_p, GraphicObj *p)
{
  EHexahedronWDWCRec *thedp;

  thedp = (EHexahedronWDWCRec *)(p->spec.data);

  EVHexahWDPrimitive(v_p, p,
                     thedp->hexah.points,
                     thedp->assoc_data.vals,
                     thedp->hexah.attributes.spec.color,
                     thedp->hexah.attributes.spec.fill_style,
                     thedp->hexah.attributes.spec.edge_flag,
                     thedp->hexah.attributes.spec.edge_color,
                     thedp->hexah.attributes.spec.shrink,
                     DRAW_MODE_DRAW);
}



static EGraphicType 
Type(GraphicObj *g_p)
{
  return EG_HEXAHEDRONWD;
}



static char *
AsString(GraphicObj *g_p)
{
  EHexahedronWDWCRec *p;

  p = (EHexahedronWDWCRec *)g_p->spec.data;
  sprintf(string_rep_buffer,
	  "HEX WITH DATA [%f,%f,%f] [%f,%f,%f] \n"
    "              [%f,%f,%f] [%f,%f,%f] \n"
    "              [%f,%f,%f] [%f,%f,%f] \n"
    "              [%f,%f,%f] [%f,%f,%f] \n"
    "              [%f,%f,%f,%f,%f,%f,%f,%f]",
          p->hexah.points[0].x, p->hexah.points[0].y, p->hexah.points[0].z, 
          p->hexah.points[1].x, p->hexah.points[1].y, p->hexah.points[1].z, 
          p->hexah.points[2].x, p->hexah.points[2].y, p->hexah.points[2].z, 
          p->hexah.points[3].x, p->hexah.points[3].y, p->hexah.points[3].z, 
          p->hexah.points[4].x, p->hexah.points[4].y, p->hexah.points[4].z, 
          p->hexah.points[5].x, p->hexah.points[5].y, p->hexah.points[5].z, 
          p->hexah.points[6].x, p->hexah.points[6].y, p->hexah.points[6].z, 
          p->hexah.points[7].x, p->hexah.points[7].y, p->hexah.points[7].z,
          p->assoc_data.vals[0],
          p->assoc_data.vals[1],
          p->assoc_data.vals[2],
          p->assoc_data.vals[3],
          p->assoc_data.vals[4],
          p->assoc_data.vals[5],
          p->assoc_data.vals[6],
          p->assoc_data.vals[7]);
  
  return string_rep_buffer; /* Don't free this memory!!! */
}



static void 
RetrieveFrom(FILE *file_p, GraphicSpecObj *gsp)
{
  EHexahedronWDWCRec *p;
  int filled, edge_flag;
  char color[64];
  EPixel pixel;
  BOOLEAN success;
  double x1, y1, z1, x2, y2, z2, x3,  y3, z3, x4,  y4, z4;
  double x5, y5, z5, x6, y6, z6, x7,  y7, z7, x8,  y8, z8;
  double v1, v2, v3, v4, v5, v6, v7, v8;
  float shrink;
  
  if ((p = (EHexahedronWDWCRec *)make_node(sizeof(EHexahedronWDWCRec)))
      != NULL) {
    fscanf(file_p,
           "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n"
	   "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n"
           "%lf %lf %lf %lf %lf %lf %lf %lf",
           &x1, &y1, &z1, &x2, &y2, &z2, &x3, &y3, &z3, &x4, &y4, &z4,
           &x5, &y5, &z5, &x6, &y6, &z6, &x7, &y7, &z7, &x8, &y8, &z8,
           &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8);
    p->hexah.points[0].x = x1;
    p->hexah.points[0].y = y1;
    p->hexah.points[0].z = z1;
    p->hexah.points[1].x = x2;
    p->hexah.points[1].y = y2;
    p->hexah.points[1].z = z2;
    p->hexah.points[2].x = x3;
    p->hexah.points[2].y = y3;
    p->hexah.points[2].z = z3;
    p->hexah.points[3].x = x4;
    p->hexah.points[3].y = y4;
    p->hexah.points[3].z = z4;
    p->hexah.points[4].x = x5;
    p->hexah.points[4].y = y5;
    p->hexah.points[4].z = z5;
    p->hexah.points[5].x = x6;
    p->hexah.points[5].y = y6;
    p->hexah.points[5].z = z6;
    p->hexah.points[6].x = x7;
    p->hexah.points[6].y = y7;
    p->hexah.points[6].z = z7;
    p->hexah.points[7].x = x8;
    p->hexah.points[7].y = y8;
    p->hexah.points[7].z = z8;

    p->assoc_data.vals[0]  = v1;
    p->assoc_data.vals[1]  = v2;
    p->assoc_data.vals[2]  = v3;
    p->assoc_data.vals[3]  = v4;
    p->assoc_data.vals[4]  = v5;
    p->assoc_data.vals[5]  = v6;
    p->assoc_data.vals[6]  = v7;
    p->assoc_data.vals[7]  = v8;
    
    fscanf(file_p,
	   "%d %d %d %f",
	   &(p->hexah.attributes.spec.have_any),
	   &filled, &edge_flag, &shrink);
    p->hexah.attributes.spec.fill_style = filled;
    p->hexah.attributes.spec.edge_flag  = edge_flag;
    p->hexah.attributes.spec.shrink     = shrink;
    fscanf(file_p, "%s", color);
    pixel = ColorGetPixelFromString(color, &success);
    if (success)
      p->hexah.attributes.spec.color = pixel;
    fscanf(file_p, "%s", color);
    pixel = ColorGetPixelFromString(color, &success);
    if (success)
      p->hexah.attributes.spec.edge_color = pixel;
  }
  gsp->data        = (caddr_t)p;
  gsp->data_length = sizeof(EHexahedronWDWCRec);
}
 


static void 
StoreOn(FILE *file_p, GraphicObj *g_p)
{
  EHexahedronWDWCRec *p;
  char *color, *edge_color;

  p = (EHexahedronWDWCRec *)g_p->spec.data;
  if (p->hexah.attributes.spec.have_any == YES) {
    color = ColorGetStringFromPixel(p->hexah.attributes.spec.color);
    if (color == NULL)
      color = "white";
    edge_color = ColorGetStringFromPixel(p->hexah.attributes.spec.edge_color);
    if (edge_color == NULL)
      edge_color = "white";
  } else {
    edge_color = color = "white";
  }
  fprintf(file_p,
	  "%f %f %f %f %f %f %f %f %f %f %f %f\n"
          "%f %f %f %f %f %f %f %f %f %f %f %f\n"
          "%f %f %f %f %f %f %f %f\n"
          "%d %d %d %f\n"
          "%s\n"
          "%s\n",
          p->hexah.points[0].x, p->hexah.points[0].y, p->hexah.points[0].z, 
          p->hexah.points[1].x, p->hexah.points[1].y, p->hexah.points[1].z, 
          p->hexah.points[2].x, p->hexah.points[2].y, p->hexah.points[2].z, 
          p->hexah.points[3].x, p->hexah.points[3].y, p->hexah.points[3].z, 
          p->hexah.points[4].x, p->hexah.points[4].y, p->hexah.points[4].z, 
          p->hexah.points[5].x, p->hexah.points[5].y, p->hexah.points[5].z, 
          p->hexah.points[6].x, p->hexah.points[6].y, p->hexah.points[6].z, 
          p->hexah.points[7].x, p->hexah.points[7].y, p->hexah.points[7].z,
          p->assoc_data.vals[0],
          p->assoc_data.vals[1],
          p->assoc_data.vals[2],
          p->assoc_data.vals[3],
          p->assoc_data.vals[4],
          p->assoc_data.vals[5],
          p->assoc_data.vals[6],
          p->assoc_data.vals[7],
	  (p->hexah.attributes.spec.have_any),
	  p->hexah.attributes.spec.fill_style,
	  p->hexah.attributes.spec.edge_flag,
	  p->hexah.attributes.spec.shrink,
	  color, edge_color);
}


static GraphicObj *
ModifyAssocData(GraphicObj *g_p, FPNum *vals)
{
  EHexahedronWDWCRec *p = (EHexahedronWDWCRec *)g_p->spec.data;
  int i;

  for (i=0; i < 8; i++)
    p->assoc_data.vals[i]  = vals[i];

  return g_p;
}



static BOOLEAN
GetAssocData(GraphicObj *g_p, FPNum *vals, int *nvals)
{
  EHexahedronWDWCRec *p = (EHexahedronWDWCRec *)g_p->spec.data;
  int i;

  for (i=0; i < 8; i++)
    vals[i] = p->assoc_data.vals[i];
  *nvals  = 8;

  return YES; /* the values are valid */
}


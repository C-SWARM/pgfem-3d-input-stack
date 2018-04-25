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

#define TETRAWD_PRIVATE_HEADER
#include "Etetrawd.h"

/* ========================================================================= */
/* PRIVATE DATA */
/* ========================================================================= */

#define STRING_BUFF_SIZE 1024

static char string_rep_buffer[STRING_BUFF_SIZE];

/* ========================================================================= */
/* PUBLIC */
/* ========================================================================= */

GraphicObj *
CreateTetraWD(WCRec *points, double *vals)
{
  ETetrahedronWDWCRec *thedp;
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
    g_p = CreateTetra(points); /* create a temp. obj. to inherit from */
    /* method table to use */
    tetra_wd_3d_graphic_methods = EGCopyMethodTable(g_p, ma, &mt);
    EGDeepDestroyGraphics(g_p); /* destroy the temporary object */
    first_time = NO;
  }
    
  thedp = (ETetrahedronWDWCRec *)make_node(sizeof(ETetrahedronWDWCRec));
  g_p = EGCreateGraphics(tetra_wd_3d_graphic_methods, (caddr_t)thedp,
			 sizeof(ETetrahedronWDWCRec));
  EGInvokeMethod(g_p, modify_geom_func, (g_p, points));
  thedp->tetra.attributes.spec.fill_style = FILL_SOLID;
  thedp->tetra.attributes.spec.color      = 0x0;
  thedp->tetra.attributes.spec.edge_flag  = NO;
  thedp->tetra.attributes.spec.edge_color = 0x0;
  thedp->tetra.attributes.spec.shrink     = 1.0;
  thedp->tetra.attributes.spec.have_any   = YES;
  {
    int i;
    for (i=0;i<4;i++)
      thedp->assoc_data.vals[i]  = vals[i];
  }
  return g_p;
}



static void 
Draw(EView *v_p, GraphicObj *p)
{
  ETetrahedronWDWCRec *thedp;

  thedp = (ETetrahedronWDWCRec *)(p->spec.data);

  EVTetraWDPrimitive(v_p, p,
                     &thedp->tetra.point_1, &thedp->tetra.point_2,
                     &thedp->tetra.point_3, &thedp->tetra.point_4,
                     thedp->assoc_data.vals,
                     thedp->tetra.attributes.spec.color,
                     thedp->tetra.attributes.spec.fill_style,
                     thedp->tetra.attributes.spec.edge_flag,
                     thedp->tetra.attributes.spec.edge_color,
                     thedp->tetra.attributes.spec.shrink,
                     DRAW_MODE_DRAW);
}



static EGraphicType 
Type(GraphicObj *g_p)
{
  return EG_TETRAHEDRONWD;
}



static char *
AsString(GraphicObj *g_p)
{
  ETetrahedronWDWCRec *p;

  p = (ETetrahedronWDWCRec *)g_p->spec.data;
  sprintf(string_rep_buffer,
	  "TETRA WITH DATA [%f,%f,%f] [%f,%f,%f] \n"
    "                [%f,%f,%f] [%f,%f,%f] \n"
    "                [%f,%f,%f,%f]",
          p->tetra.point_1.x, p->tetra.point_1.y, p->tetra.point_1.z, 
          p->tetra.point_2.x, p->tetra.point_2.y, p->tetra.point_2.z, 
          p->tetra.point_3.x, p->tetra.point_3.y, p->tetra.point_3.z, 
          p->tetra.point_4.x, p->tetra.point_4.y, p->tetra.point_4.z, 
          p->assoc_data.vals[0],
          p->assoc_data.vals[1],
          p->assoc_data.vals[2],
          p->assoc_data.vals[3]);
 
  return string_rep_buffer; /* Don't free this memory!!! */
}



static void 
RetrieveFrom(FILE *file_p, GraphicSpecObj *gsp)
{
  ETetrahedronWDWCRec *p;
  int filled, edge_flag;
  char color[64];
  EPixel pixel;
  BOOLEAN success;
  double x1, y1, z1, x2, y2, z2, x3,  y3, z3, x4,  y4, z4;
  double v1, v2, v3, v4;
  float shrink;
  
  if ((p = (ETetrahedronWDWCRec *)make_node(sizeof(ETetrahedronWDWCRec)))
      != NULL) {
    fscanf(file_p,
           "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n"
           "%lf %lf %lf %lf ",
           &x1, &y1, &z1, &x2, &y2, &z2, &x3, &y3, &z3, &x4, &y4, &z4,
           &v1, &v2, &v3, &v4);
    p->tetra.point_1.x = x1;
    p->tetra.point_1.y = y1;
    p->tetra.point_1.z = z1;
    p->tetra.point_2.x = x2;
    p->tetra.point_2.y = y2;
    p->tetra.point_2.z = z2;
    p->tetra.point_3.x = x3;
    p->tetra.point_3.y = y3;
    p->tetra.point_3.z = z3;
    p->tetra.point_4.x = x4;
    p->tetra.point_4.y = y4;
    p->tetra.point_4.z = z4;

    p->assoc_data.vals[0]  = v1;
    p->assoc_data.vals[1]  = v2;
    p->assoc_data.vals[2]  = v3;
    p->assoc_data.vals[3]  = v4;
    
    fscanf(file_p,
	   "%d %d %d %f",
	   &(p->tetra.attributes.spec.have_any),
	   &filled, &edge_flag, &shrink);
    p->tetra.attributes.spec.fill_style = filled;
    p->tetra.attributes.spec.edge_flag  = edge_flag;
    p->tetra.attributes.spec.shrink     = shrink;
    fscanf(file_p, "%s", color);
    pixel = ColorGetPixelFromString(color, &success);
    if (success)
      p->tetra.attributes.spec.color = pixel;
    fscanf(file_p, "%s", color);
    pixel = ColorGetPixelFromString(color, &success);
    if (success)
      p->tetra.attributes.spec.edge_color = pixel;
    pixel = ColorGetPixelFromString(color, &success);
  }
  gsp->data        = (caddr_t)p;
  gsp->data_length = sizeof(ETetrahedronWDWCRec);
}
 


static void 
StoreOn(FILE *file_p, GraphicObj *g_p)
{
  ETetrahedronWDWCRec *p;
  char *color, *edge_color;

  p = (ETetrahedronWDWCRec *)g_p->spec.data;
  if (p->tetra.attributes.spec.have_any == YES) {
    color = ColorGetStringFromPixel(p->tetra.attributes.spec.color);
    if (color == NULL)
      color = "white";
    edge_color = ColorGetStringFromPixel(p->tetra.attributes.spec.edge_color);
    if (edge_color == NULL)
      edge_color = "white";
  } else {
    edge_color = color = "white";
  }
  fprintf(file_p,
	  "%f %f %f %f %f %f %f %f %f %f %f %f\n"
          "%f %f %f %f\n"
          "%d %d %d %f\n"
          "%s\n"
          "%s\n",
          p->tetra.point_1.x, p->tetra.point_1.y, p->tetra.point_1.z, 
          p->tetra.point_2.x, p->tetra.point_2.y, p->tetra.point_2.z, 
          p->tetra.point_3.x, p->tetra.point_3.y, p->tetra.point_3.z, 
          p->tetra.point_4.x, p->tetra.point_4.y, p->tetra.point_4.z, 
          p->assoc_data.vals[0],
          p->assoc_data.vals[1],
          p->assoc_data.vals[2],
          p->assoc_data.vals[3],
	  (p->tetra.attributes.spec.have_any),
	  p->tetra.attributes.spec.fill_style,
	  p->tetra.attributes.spec.edge_flag,
	  p->tetra.attributes.spec.shrink,
	  color, edge_color);
}


static GraphicObj *
ModifyAssocData(GraphicObj *g_p, FPNum *vals)
{
  ETetrahedronWDWCRec *p = (ETetrahedronWDWCRec *)g_p->spec.data;
  int i;

  for (i=0; i < 4; i++)
    p->assoc_data.vals[i]  = vals[i];

  return g_p;
}



static BOOLEAN
GetAssocData(GraphicObj *g_p, FPNum *vals, int *nvals)
{
  ETetrahedronWDWCRec *p = (ETetrahedronWDWCRec *)g_p->spec.data;
  int i;

  for (i=0; i < 4; i++)
    vals[i] = p->assoc_data.vals[i];
  *nvals  = 4;

  return YES; /* the values are valid */
}


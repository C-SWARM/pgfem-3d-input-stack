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

#include "Esimple.h"          


  /* this could be finite elements */
typedef struct element_t {
  int dummy_element_data;
  GraphicObj *el_graphic_representation;
}  element_t;

#define NI 30 /* number of intervals in the square plotted */

#define NUMEL NI*NI /* number of elements in the mesh */

static element_t elements[NUMEL]; /* the quad elements of the mesh */

static double xmin = -1, xmax = 1, ymin = -1, ymax = 1; /* region on which */
                                                        /* the quad mesh is */
                                                        /* constructed */

static double fvalmax, fvalmin; /* funky extremal values */

static double t = 1; /* this is the parameter used by funky */

static double
funky(double x, double y);

static void
funky_extrema(double *fvalmin, double *fvalmax,
              double (*func)(double x, double y),
              double xmin, double xmax,
              double ymin, double ymax,
              double deltax, double deltay);

static int
generate_quad_mesh(void);

int 
main(int argc, char **argv)
{
  EFringeTable ft; /* this is the color table for filling the mesh */

  ESIBuildInterface(ESI_GRAPHIC_EDITOR_MASK, argc, argv); /* create GUI */

  ft = ColorCreateFringeTable(); /* set up the color table */
  EMSetAssocFringeTable(ESIModel(), ft);
  t = 1; /* first the full range */
  funky_extrema(&fvalmin, &fvalmax, funky,
               xmin, xmax, ymin, ymax, (xmax-xmin)/10, (ymax-ymin)/10);
  ColorSetupFringeTableByMinMax(ft, fvalmin, fvalmax);
  t = 0.1; /* now let some margin for recomputation */

  generate_quad_mesh(); /* create quad mesh on which the function */
                        /* will be displayed */

  ESIPopupAndRun(); /* Now let us see what we can do with it. */
  return 1;
}

static Widget toggle_z, toggle_render_mode, recompute, toggle_shrink;

static void 
ToggleZ(Widget w, XtPointer text_ptr, XtPointer call_data);

static BOOLEAN z_is_on = NO;

static void
modify_z_on(BOOLEAN z_is_on);

 /* callback to toggle display of the Z coordinate on the mesh */
static void
ToggleZ(Widget w, XtPointer text_ptr, XtPointer call_data)
{
  if (z_is_on) z_is_on = NO;
  else         z_is_on = YES;
  modify_z_on(z_is_on);
  redraw_view_CB(ESITopLevelWidget(), NULL, NULL);
}



static BOOLEAN HS_is_on = NO;

static void 
ToggleRenderMode(Widget w, XtPointer text_ptr, XtPointer call_data);

 /* callback to toggle rendering mode: ON means HIDDEN SURFACE (FILL), */
 /*                                    OFF means normal mode */
static void
ToggleRenderMode(Widget w, XtPointer text_ptr, XtPointer call_data)
{
  if (!HS_is_on) {
    HS_is_on = YES;
    ESIHandleCmd("RENDER FILL"); /* pass in a command, */
                                             /* just as it could be typed */
                                             /* at the command window */
  } else {
    HS_is_on = NO;
    ESIHandleCmd("RENDER NORMAL");
  }
}

static int
Redisplay(NODE v_p, NODE ptr); /* the message sent to all dependent views */


static double shrink = 1.0;

static void
change_shrink(double shrink);

static void 
ToggleShrink(Widget w, XtPointer text_ptr, XtPointer call_data);

 /* callback to toggle shrinked display of the quads */
static void
ToggleShrink(Widget w, XtPointer text_ptr, XtPointer call_data)
{
  if (shrink == 1)     shrink = 0.9;
  else                 shrink = 1.0;
  change_shrink(shrink);
  EMDispatchToDependentViews(ESIModel(), Redisplay, NULL);
}


static int
Redisplay(NODE ptr, NODE v_p)
{
  EVForceRedisplay((EView *)v_p);
  return 1;
}

static void 
Recompute(Widget w, XtPointer text_ptr, XtPointer call_data);

 /* callback to toggle rendering mode: ON means HIDDEN SURFACE (FILL), */
 /*                                    OFF means normal mode */
static void
Recompute(Widget w, XtPointer text_ptr, XtPointer call_data)
{
  static did_it = 0;

  if (did_it++ == 3) {
    did_it = 0;
    ERptStatusMessage(1, 1, "Can allow only 3 recomputations. \n"
                      "Restarting at the same value.");
    t = 0.1;
  }
  t = t * exp(0.01*t+1);
  modify_z_on(z_is_on); /* display the new function value over the region */
  EMDispatchToDependentViews(ESIModel(), Redisplay, NULL);
}


  /* This is called from SimpleInterface to allow for */
  /* customization.  Here we add a toggle button. */
void
ESICustomize(Widget parent_pane)
{
  Arg args[3];
  int argn;
  
  argn = 0;                     /* set args */
  toggle_z = ESIAddButton("toggle_z", /* this is the widget name */
                          "DISPLAY Z COORDINATE",
                          toggleWidgetClass, /* widget class */
                          parent_pane, /* parent widget */
                          args, argn, /* args to set the widget appearance */
                                      /* and behaviour */
                          ToggleZ, NULL /* callback: gets invoked when toggle*/
                                        /* button pressed */
                          );

  argn = 0;                     /* set args */
  toggle_render_mode =
    ESIAddButton("toggle_render_mode",
                 "RENDER IN HIDDEN SURFACE MODE",
                 toggleWidgetClass, parent_pane,
                 args, argn, ToggleRenderMode, NULL);

  argn = 0;                     /* set args */
  toggle_shrink =
    ESIAddButton("toggle_shrink", "DISPLAY SHRINKED",
                 toggleWidgetClass, parent_pane,
                 args, argn, ToggleShrink, NULL);

  argn = 0;                     /* set args */
  recompute =
    ESIAddButton("recompute", "RECOMPUTE", commandWidgetClass, parent_pane,
                 args, argn, Recompute, NULL);
}


#include "Etypes.h"
#include "Efonts.h"
#include "Ecolors.h"
#include "Eview.h"
#include "Eactset.h"
#include "Equadwd3d.h"


static void
change_shrink(double shrink)
{
  int el;

  EASValsSetChangeMask(SHRINK_MASK);
  EASValsSetShrink(shrink);

  for (el = 0; el < NUMEL; el++) {
    EGWithMaskChangeAttributes(SHRINK_MASK,
                               elements[el].el_graphic_representation);
  }
}

static void
modify_z_on(BOOLEAN z_is_on)
{
  int el;
  WCRec pnts[4];
  FPNum vals[4];
  int npnts;
  
  for (el = 0; el < NUMEL; el++) {
    if (!EGGetGraphicsGeometry(elements[el].el_graphic_representation,
                               pnts, &npnts))
      exit(0); /* error -- nothing fancy aborting */
    if (!EGGetAssocData(elements[el].el_graphic_representation, vals, &npnts))
      exit(0); /* error -- nothing fancy aborting */
    if (z_is_on) {
      vals[0] = pnts[0].z = funky(pnts[0].x, pnts[0].y);
      vals[1] = pnts[1].z = funky(pnts[1].x, pnts[1].y);
      vals[2] = pnts[2].z = funky(pnts[2].x, pnts[2].y);
      vals[3] = pnts[3].z = funky(pnts[3].x, pnts[3].y);
    } else {
      pnts[0].z = pnts[1].z = pnts[2].z = pnts[3].z = 0;
      vals[0] = funky(pnts[0].x, pnts[0].y);
      vals[1] = funky(pnts[1].x, pnts[1].y);
      vals[2] = funky(pnts[2].x, pnts[2].y);
      vals[3] = funky(pnts[3].x, pnts[3].y);
    }
    EGModifyGraphicsGeometry(elements[el].el_graphic_representation, pnts);
    EGModifyAssocData(elements[el].el_graphic_representation, vals);
  }
}


  /* generate the quad mesh, i.e. its graphic representation */
static int
generate_quad_mesh(void)
{
  double v1, v2, v3, v4;
  double deltax = (xmax - xmin)/NI, deltay = (ymax - ymin)/NI;
  double x, y;
  WCRec pnts[4];
  GraphicObj *g;
  int nq;

  nq = 0; /* the first quad element */
  y = ymin;
 continue_1:
  x = xmin;
 continue_10:
  pnts[0].x = x;
  pnts[0].y = y;
  pnts[0].z = pnts[1].z = pnts[2].z = pnts[3].z = 0;
  v1 = funky(pnts[0].x, pnts[0].y); /* value at the 1st corner */
  pnts[1].x = x + deltax;     
  pnts[1].y = y;
  v2 = funky(pnts[1].x, pnts[1].y); /* value at the 2nd corner */
  pnts[2].x = x + deltax;
  pnts[2].y = y + deltay;
  v3 = funky(pnts[2].x, pnts[2].y); /* value at the 3rd corner */
  pnts[3].x = x;
  pnts[3].y = y + deltay;
  v4 = funky(pnts[3].x, pnts[3].y); /* value at the 4th corner */
  
  EMAddGraphicsToModel(ESIModel(), /* add to a model */
                       (g = CreateQuadWD3D(pnts, v1, v2, v3, v4))); 
  EGChangeAttributes(g); /* change its graphic attributes to */
  /* attach the object so that we can print the associated data */

 /* save its graphic representation with the element */
  elements[nq++].el_graphic_representation = g;
  
  x += deltax;
  if (xmax - x > deltax/2)
    goto continue_10;
  y += deltay;
  if (ymax - y > deltay/2) 
    goto continue_1;
  
  return 1;
}



  /* this is the function we plot on the quad mesh */
static double
funky(double x, double y)
{
  return t * x * y * cos(6.0*x*x + 2.0*y*y);
}


  /* to compute the lowest and highest value so that the color */
  /* table can be adjusted accordingly*/
static void
funky_extrema(double *fvalmin, double *fvalmax,
             double (*funky)(double x, double y),
             double xmin, double xmax,
             double ymin, double ymax,
             double deltax, double deltay)
{
  double x, y, f;

  *fvalmax = -FLT_MAX;
  *fvalmin = FLT_MAX;
  
  for (x = xmin; x <= 1.001*xmax; x += deltax) {
    for (y = ymin; y <= 1.001*ymax; y += deltay) {
      f = (*funky)(x, y);
      *fvalmax = max(*fvalmax, f);
      *fvalmin = min(*fvalmin, f);
    }
  }
}


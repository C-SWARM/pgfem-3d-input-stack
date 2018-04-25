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

/* This file holds methods on a graphic. A graphic is any geometric          */
/* entity with a graphic representation (line, rectangle, circle,            */
/* arc, etc.) and any composite object consisting of graphic objects.        */

/* The methods on a graphic are invoked through their generic invocations    */
/* defined in this file, but these generic methods do in most cases just     */
/* the following: they invoke specific method accessed through the ``class'' */
/* descriptor of the individual graphic. The generic methods can be in       */
/* a sense considered ``virtual'' in C++ parlance.                           */

#include "Econfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Eview.h"
#include "Egraphic.h"
#include "Ecolors.h"
#include "Efonts.h"
#include "Eactset.h"
#include "Eutils.h"

/* Include headers of specializations of graphic objects in ELIXIR */
/* to be able to register their methods */
#include "Eanntxt3d.h"
#include "Ebcbezs3d.h"
#include "Ebcbhex.h"
#include "Ecbezc3d.h"
#include "Ectlblock.h"
#include "Eggroup.h"
#include "Ehexah.h"
#include "Ehexahwd.h"
#include "Eline3d.h"
#include "Emark3d.h"
#include "Emarkwd3d.h"
#include "Epyramid.h"
#include "Equad3d.h"
#include "Equadwd3d.h"
#include "Eraprism.h"
#include "Etetra.h"
#include "Etetrawd.h"
#include "Etria3d.h"
#include "Etriawd3d.h"
#include "Evecmark3d.h"
#include "Evector3d.h"
#include "Erbezc3d.h"
#include "Erbezs3d.h"


/* ========================================================================= */
/* PRIVATE */
/* ========================================================================= */

void EGDefaultDestroyGraphics(GraphicObj *g_p);
GraphicObj *EGDefaultCopyGraphics(GraphicObj *g_p);

static EGMethodsRec *registered_methods[EG_MAX_METHOD_TABLE] = {
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL
    };

static int handle_size_2 = 6;

static char *any_graphics_string_rep = "ANY GRAPHIC";

static int elixir_file_version = 0;

void
EGSetFileVersion(int version)
{
  elixir_file_version = version;
}

/* ========================================================================= */
/* PUBLIC */
/* ========================================================================= */

#define DUMMY_NO     56     /* must increase if necessary!!! */


void 
EGInitGraphics(void)
{
  GraphicObj *g_p;
  WCRec dummy_points[DUMMY_NO];
  float dummy_weights[4] = {1.0, 1.0, 1.0, 1.0};
  FPNum vals[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  int i;
	static int inited = NO;

  if (inited) return;

	for(i = 0; i < DUMMY_NO; i++)dummy_points[i].x = dummy_points[i].y = dummy_points[i].z = (double)i;

  EASValsMakeActiveSet();
  
  g_p = CreateCtlBlock(ACTIVE_SET_CTLBLOCK);
  EGRegisterMethods(g_p);
  EGDeepDestroyGraphics(g_p);
  
  g_p = CreateGgroup(NULL);
  EGRegisterMethods(g_p);
  EGDeepDestroyGraphics(g_p);

  g_p = CreateMarker3D(dummy_points);
  EGRegisterMethods(g_p);
  EGDeepDestroyGraphics(g_p);

  g_p = CreateLine3D(dummy_points);
  EGRegisterMethods(g_p);
  EGDeepDestroyGraphics(g_p);

  g_p = CreateAnnText3D(dummy_points, " ");
  EGRegisterMethods(g_p);
  EGDeepDestroyGraphics(g_p);

  g_p = CreateTriangle3D(dummy_points);
  EGRegisterMethods(g_p);
  EGDeepDestroyGraphics(g_p);

  g_p = CreateTriangleWD3D(dummy_points, 0, 0, 0);
  EGRegisterMethods(g_p);
  EGDeepDestroyGraphics(g_p);

  g_p = CreateQuad3D(dummy_points);
  EGRegisterMethods(g_p);
  EGDeepDestroyGraphics(g_p);

  g_p = CreateQuadWD3D(dummy_points, 0, 0, 0, 0);
  EGRegisterMethods(g_p);
  EGDeepDestroyGraphics(g_p);

  g_p = CreateBCBezS3D(dummy_points);
  EGRegisterMethods(g_p);
  EGDeepDestroyGraphics(g_p);
  
  g_p = CreateCBezC3D(dummy_points);
  EGRegisterMethods(g_p);
  EGDeepDestroyGraphics(g_p);
  
  g_p = CreateTetra(dummy_points);
  EGRegisterMethods(g_p);
  EGDeepDestroyGraphics(g_p);
  
  g_p = CreateBCBHex(dummy_points);
  EGRegisterMethods(g_p);
  EGDeepDestroyGraphics(g_p);
  
  g_p = CreateRAPrism(dummy_points);
  EGRegisterMethods(g_p);
  EGDeepDestroyGraphics(g_p);
  
  g_p = CreatePyramid(dummy_points);
  EGRegisterMethods(g_p);
  EGDeepDestroyGraphics(g_p);

  g_p = CreateHexahedron(dummy_points);
  EGRegisterMethods(g_p);
  EGDeepDestroyGraphics(g_p);

  g_p = CreateVecMarker3D(dummy_points);
  EGRegisterMethods(g_p);
  EGDeepDestroyGraphics(g_p);

  g_p = CreateVector3D(dummy_points);
  EGRegisterMethods(g_p);
  EGDeepDestroyGraphics(g_p);

  g_p = CreateRBezC3D(2, dummy_points, dummy_weights);
  EGRegisterMethods(g_p);
  EGDeepDestroyGraphics(g_p);

  g_p = CreateRBezS3D(2, 2, dummy_points, dummy_weights);
  EGRegisterMethods(g_p);
  EGDeepDestroyGraphics(g_p);

  g_p = CreateHexahedronWD(dummy_points, vals);
  EGRegisterMethods(g_p);
  EGDeepDestroyGraphics(g_p);

  g_p = CreateTetraWD(dummy_points, vals);
  EGRegisterMethods(g_p);
  EGDeepDestroyGraphics(g_p);

  g_p = CreateMarkerWD3D(dummy_points, vals[0]);
  EGRegisterMethods(g_p);
  EGDeepDestroyGraphics(g_p);

  inited = YES;
}

BOOLEAN 
EGRegisterMethods(GraphicObj *g_p)
{
  if (g_p == NULL) return NO;
  
  registered_methods[EGGraphicType(g_p)] = g_p->methods;
  
  return YES;
}


EGraphicType 
EGGraphicType(GraphicObj *g_p) 
{
  if (EGMethodIsDefined(g_p, type_func))
    return EGInvokeMethod(g_p, type_func, (g_p));
  else
    return EG_UNKNOWN_TYPE;
}


void 
EGGraphicStoreOn(FILE *fp, GraphicObj *g_p)
{
  if (EGMethodIsDefined(g_p, store_on_func)) {
    fprintf(fp,
	    "%lu %lu %d %d %d %d\n",
	    g_p->time_id,
	    g_p->serial_id,
	    (int)EGGraphicType(g_p),
            (int)g_p->layer,
            (int)g_p->invisible_when_clipped,
            (int)g_p->not_clippable);
    EGInvokeMethod(g_p, store_on_func, (fp, g_p));
  }
}



GraphicObj *
EGGraphicRetrieveFrom(FILE *fp)
{
  GraphicObj *g_p = NULL;
  EGraphicId          t_id, s_id;
  EGraphicType        type;
  int                 layer, invisible_when_clipped, not_clippable;

  if (elixir_file_version > 0) {
    if(fscanf(fp, "%lu %lu %d %d %d %d", &t_id, &s_id,
              (int *)&type,
              &layer,
              &invisible_when_clipped,
              &not_clippable) != 6) {
      if (!feof(fp))
        fprintf(stderr, "Error while reading in GGraphicRetrieveFrom\n");
      return NULL; /* Error or end-of-file */
    }
  } else {
    if (fscanf(fp, "%lu %lu %d %d", &t_id, &s_id,
               (int *)&type,
               &layer) != 4) {
      if (!feof(fp))
        fprintf(stderr, "Error while reading in GGraphicRetrieveFrom\n");
      return NULL; /* Error or end-of-file */
    }
    invisible_when_clipped = NO;
    not_clippable          = NO;
  }
  
  if (type >= EG_MAX_METHOD_TABLE)
    return NULL; /* Invalid type */

  if (registered_methods[type] == NULL)
    return NULL; /* Cannot handle: no method to read the data */
  
  if ((g_p = (GraphicObj *)make_node(sizeof(GraphicObj))) == NULL) {
    EUFailedMakeNode("EGGraphicRetrieveFrom");
  } else {
    g_p->time_id   = t_id;
    g_p->serial_id = s_id;
    g_p->methods   = registered_methods[type];
    if (EGMethodIsDefined(g_p, retrieve_from_func))
      EGInvokeMethod(g_p, retrieve_from_func, (fp, &(g_p->spec)));
    g_p->marked                 = NO;
    g_p->selected               = NO;
    g_p->pickable_mask          = (unsigned long)0;
    g_p->layer                  = layer;
    g_p->invisible_when_clipped = invisible_when_clipped;
    g_p->not_clippable          = not_clippable;
    g_p->app_obj                = NULL;
  }
  return g_p;
}

void 
EGAttachObject(GraphicObj *g_p, EObjectP app_obj)
{
  g_p->app_obj     = app_obj;
}



EObjectP
EGGetAppObj(GraphicObj *g_p)
{
  return g_p->app_obj;
}


#define CONDITIONALLY_SET_ENTRY(mask, mtp, e, arg_mtp)  \
   if (mask & e##_mask1) mtp->e = arg_mtp->e

EGMethodsRec *
EGCopyMethodTable(GraphicObj *g_p,
                  EMaskArray mask_array, EGMethodsRec *at)
{
  EGMethodsRec *newm;
  
  /* Copy the old method table */
  newm = (EGMethodsRec *)copy_node(g_p->methods, sizeof(EGMethodsRec));

  /* Now override specified methods */ 
  if (mask_array[1] & partition_1_mask) {
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, type_func, at);
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, string_rep_func, at);
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, store_on_func, at);
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, retrieve_from_func, at);
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, copy_func, at);
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, deep_copy_func, at);
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, destroy_func, at);
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, deep_destroy_func, at);
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, interpolate_func, at);
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, bbox_func_VC, at);
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, bbox_func_WC, at);
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, draw_func, at);
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, xordraw_func, at);
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, hilite_func, at);
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, unhilite_func, at);
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, erase_func, at);
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, hit_func, at);
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, key_point_list_func, at);
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, reshape_handle_func, at);
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, xy_of_handle_func, at);
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, reshape_func, at);
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, modify_geom_func, at);
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, get_geom_func, at);
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, modify_assoc_data_func, at);
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, get_assoc_data_func, at);
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, translate_func, at);
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, rotate_func, at);
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, mirror_plane_func, at);
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, mirror_center_func, at);
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, scale_func, at);
  }
  if (mask_array[1] & partition_2_mask) {
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, show_handles_func, at);
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, erase_handles_func, at);
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, change_attrib_func, at);
    CONDITIONALLY_SET_ENTRY(mask_array[0], newm, attrib_val_as_in_AS_func, at);
  } 

  return (newm);
}


EGMethodsRec *
EGOverrideMethodTable(GraphicObj *g_p, EGMethodsRec *new_mt)
{
  EGMethodsRec *old_mt;

  old_mt = g_p->methods;
  g_p->methods = new_mt;

  return (old_mt);
}
  

void 
EGDrawGraphics(EView *v_p, GraphicObj *g_p)
{
  if (EGMethodIsDefined(g_p, draw_func))
    EGInvokeMethod(g_p, draw_func, (v_p, g_p));
}

void 
EGXORDrawGraphics(EView *v_p, GraphicObj *g_p)
{
  if (EGMethodIsDefined(g_p, xordraw_func))
    EGInvokeMethod(g_p, xordraw_func, (v_p, g_p));
}

void 
EGEraseGraphics(EView *v_p, GraphicObj *g_p)
{
  if (EGMethodIsDefined(g_p, erase_func))
    EGInvokeMethod(g_p, erase_func, (v_p, g_p));
}

void 
EGHiliteGraphics(EView *v_p, GraphicObj *g_p)
{
  if (EGMethodIsDefined(g_p, hilite_func))
    EGInvokeMethod(g_p, hilite_func, (v_p, g_p));
}


void 
EGUnhiliteGraphics(EView *v_p, GraphicObj *g_p)
{
  if (EGMethodIsDefined(g_p, unhilite_func))
    EGInvokeMethod(g_p, unhilite_func, (v_p, g_p));
}

void 
EGGraphicsBoundingBoxVC(EView *v_p, GraphicObj *g_p, VCRec *ll, VCRec *ur)
{
  if (EGMethodIsDefined(g_p, bbox_func_VC))
    EGInvokeMethod(g_p, bbox_func_VC, (v_p, g_p, ll, ur));
  else
    ll->u = ll->v = ll->n = ur->u = ur->v = ur->n = 0.0;
}

void 
EGGraphicsBoundingBoxWC(GraphicObj *g_p, WCRec *ll, WCRec *ur)
{
  if (EGMethodIsDefined(g_p, bbox_func_WC))
    EGInvokeMethod(g_p, bbox_func_WC, (g_p, ll, ur));
  else
    ll->x = ll->y = ll->z = ur->x = ur->y = ur->z = 0.0;
}

BOOLEAN 
EGGraphicHitByBox(EView *v_p, EBoxVC2DRec *box,
			 ESelectCriteria crit, GraphicObj *g_p)
{
  if (EGMethodIsDefined(g_p, hit_func))
    return EGInvokeMethod(g_p, hit_func, (v_p, box, crit, g_p));
  else
    return NO;
}

LIST 
EGGraphicKeyPointList(GraphicObj *g_p)
{
  if (EGMethodIsDefined(g_p, key_point_list_func))
    return EGInvokeMethod(g_p, key_point_list_func, (g_p));
  else
    return NULL;
}


GraphicObj *
EGCreateGraphics(EGMethodsRec *methods_p,
			     caddr_t data_p, int data_length)
{
  GraphicObj *g_p;
  static unsigned long g_id = 0;

  if ((g_p = (GraphicObj *)make_node(sizeof(GraphicObj))) == NULL) {
    EUFailedMakeNode("GCreateGraphics");
  }
  /* Id */
  g_p->serial_id = g_id++;
  g_p->time_id   = time(NULL);
  
  /* Methods */
  if ((g_p->methods = methods_p) == NULL) {
    fprintf(stderr, "Null methods-pointer in GCreateGraphics\n");
    exit(ESISetErrorExitCode(ELIXIR_ERROR_EXIT));
  }

  /* Marked or not? */
  g_p->marked = NO;

  /* Selected or not? */
  g_p->selected = NO;

  /* Pickable mask */
  g_p->pickable_mask = (unsigned long)0;

  /* Layer */
  g_p->layer = EASValsGetLayer();

  /* Invisible when clipped (touchy clipping)? */
  g_p->invisible_when_clipped = NO;

  /* Clippable? */
  g_p->not_clippable          = NO;

  /* Structural data */
  if ((g_p->spec.data = (caddr_t)data_p) == NULL) {
    fprintf(stderr, "Null data-pointer in GCreateGraphics\n");
    exit(ESISetErrorExitCode(ELIXIR_ERROR_EXIT));
  }
  g_p->spec.data_length = data_length;
  g_p->app_obj          = NULL;

  return g_p;
}


void 
EGSetSelected(GraphicObj *g_p, BOOLEAN selected)
{
  g_p->selected = selected;
}

BOOLEAN 
EGIsSelected(GraphicObj *g_p)
{
  return g_p->selected;
}

void 
EGSetMarked(GraphicObj *g_p, BOOLEAN marked)
{
  g_p->marked = marked;
}

BOOLEAN 
EGIsMarked(GraphicObj *g_p)
{
  return g_p->marked;
}

void 
EGSetLayer(GraphicObj *g_p, int layer)
{
  g_p->layer = layer;
}

int 
EGGetLayer(GraphicObj *g_p)
{
  return g_p->layer;
}


void 
EGDefaultDestroyGraphics(GraphicObj *g_p)
{
  free_node(g_p->spec.data); /* Default action: free specialized data */
  free_node(g_p);                            /* free top struct */
}


void 
EGDestroyGraphics(GraphicObj *g_p)
{
  if (EGMethodIsDefined(g_p, destroy_func))
    EGInvokeMethod(g_p, destroy_func, (g_p));
  else
    EGDefaultDestroyGraphics(g_p);
}


void 
EGDeepDestroyGraphics(GraphicObj *g_p)
{
  if (EGMethodIsDefined(g_p, deep_destroy_func))
    EGInvokeMethod(g_p, deep_destroy_func, (g_p));
  else 
    EGDefaultDestroyGraphics(g_p);
}


GraphicObj *
EGDefaultCopyGraphics(GraphicObj *g_p)
{
  caddr_t new_data_p;
  
  if ((new_data_p =
       (caddr_t)copy_node(g_p->spec.data, g_p->spec.data_length)) == NULL) {
    fprintf(stderr, "Failed copy data\n");
    exit(ESISetErrorExitCode(ELIXIR_ERROR_EXIT));
  }
  return EGCreateGraphics(g_p->methods, new_data_p, g_p->spec.data_length);
}

GraphicObj *
EGCopyGraphics(GraphicObj *g_p)
{
	GraphicObj *new_g_p = NULL;

  if (EGMethodIsDefined(g_p, copy_func))
    new_g_p = EGInvokeMethod(g_p, copy_func, (g_p));
  else
    new_g_p = EGDefaultCopyGraphics(g_p);

	if(EASValsGetPreserveLayerFlag() == YES)new_g_p->layer = g_p->layer;
	return(new_g_p);
}

GraphicObj *
EGDeepCopyGraphics(GraphicObj *g_p)
{
	GraphicObj *new_g_p = NULL;

  if (EGMethodIsDefined(g_p, deep_copy_func))
    new_g_p = EGInvokeMethod(g_p, deep_copy_func, (g_p));
  else
    new_g_p = EGDefaultCopyGraphics(g_p);

	if(EASValsGetPreserveLayerFlag() == YES)new_g_p->layer = g_p->layer;
	return(new_g_p);
}


EGraphicId 
EGGraphicSerialID(GraphicObj *g_p)
{
  return g_p->serial_id;
}


EGraphicId 
EGGraphicTimeID(GraphicObj *g_p)
{
  return g_p->time_id;
}


GraphicObj *
EGModifyGraphicsGeometry(GraphicObj *g_p, WCRec *points)
{
  if (EGMethodIsDefined(g_p, modify_geom_func)) 
    EGInvokeMethod(g_p, modify_geom_func, (g_p, points));
  return g_p;
}



BOOLEAN
EGGetGraphicsGeometry(GraphicObj *g_p, WCRec *points, int *npoints)
{
  if (EGMethodIsDefined(g_p, get_geom_func)) 
    return EGInvokeMethod(g_p, get_geom_func, (g_p, points, npoints));
  else
    return NO;
}


GraphicObj *
EGModifyGraphicsByHandle(GraphicObj *g_p, EHandleNum hn, WCRec *new_p)
{
  if (EGMethodIsDefined(g_p, reshape_func)) 
    EGInvokeMethod(g_p, reshape_func, (g_p, hn, new_p));
  return g_p;
}


EHandleNum   
EGReshapeHandle(EView *v_p, GraphicObj *g_p, WCRec *near_point)
{
  if (EGMethodIsDefined(g_p, reshape_handle_func)) 
    return (EHandleNum)EGInvokeMethod(g_p, reshape_handle_func,
                                      (v_p, g_p, near_point));
  else
    return (EHandleNum)(0);
}

void 
EGDrawHandle(EView *v_p, WCRec *h_pos)
{
  int atDCx, atDCy;
  Widget w;

  EVWCtoDC(v_p, h_pos, &atDCx, &atDCy);

  w = v_p->view_widget;
  XDrawLine(XtDisplay(w), v_p->draw_into, EVDefaultCopyGC(v_p),
	    atDCx, atDCy-handle_size_2, atDCx+handle_size_2, atDCy);
  XDrawLine(XtDisplay(w), v_p->draw_into, EVDefaultCopyGC(v_p),
	    atDCx+handle_size_2, atDCy, atDCx, atDCy+handle_size_2);
  XDrawLine(XtDisplay(w), v_p->draw_into, EVDefaultCopyGC(v_p),
	    atDCx, atDCy+handle_size_2, atDCx-handle_size_2, atDCy);
  XDrawLine(XtDisplay(w), v_p->draw_into, EVDefaultCopyGC(v_p),
	    atDCx-handle_size_2, atDCy, atDCx, atDCy-handle_size_2);
}

void 
EGEraseHandle(EView *v_p, WCRec *h_pos)
{
  int atDCx, atDCy;
  Widget w;


  EVWCtoDC(v_p, h_pos, &atDCx, &atDCy);

  w = v_p->view_widget; 
  XDrawLine(XtDisplay(w), v_p->draw_into, EVDefaultEraseGC(v_p),
	    atDCx, atDCy-handle_size_2, atDCx+handle_size_2, atDCy);
  XDrawLine(XtDisplay(w), v_p->draw_into, EVDefaultEraseGC(v_p),
	    atDCx+handle_size_2, atDCy, atDCx, atDCy+handle_size_2);
  XDrawLine(XtDisplay(w), v_p->draw_into, EVDefaultEraseGC(v_p),
	    atDCx, atDCy+handle_size_2, atDCx-handle_size_2, atDCy);
  XDrawLine(XtDisplay(w), v_p->draw_into, EVDefaultEraseGC(v_p),
	    atDCx-handle_size_2, atDCy, atDCx, atDCy-handle_size_2);
}


void 
EGGraphicShowHandles(EView *v_p, GraphicObj *g_p)
{
  if (EGMethodIsDefined(g_p, show_handles_func))
    EGInvokeMethod(g_p, show_handles_func, (v_p, g_p));
}


void 
EGGraphicEraseHandles(EView *v_p, GraphicObj *g_p)
{
  if (EGMethodIsDefined(g_p, erase_handles_func))
    EGInvokeMethod(g_p, erase_handles_func, (v_p, g_p));
}


void 
EGXYofGraphicsHandle(GraphicObj *g_p, EHandleNum hn, WCRec *h_pos)
{
  if (EGMethodIsDefined(g_p, xy_of_handle_func))
    EGInvokeMethod(g_p, xy_of_handle_func, (g_p, hn, h_pos));
  else
    h_pos->x = h_pos->y = h_pos->z = 0.0;
}


char *EGStringGraphicRepresentation(GraphicObj *g_p)
{
  if (EGMethodIsDefined(g_p, string_rep_func))
    return EGInvokeMethod(g_p, string_rep_func, (g_p));
  else
    return any_graphics_string_rep;
}


GraphicObj *
EGTranslateGraphic(GraphicObj *g_p, WCRec *by)
{
  if (EGMethodIsDefined(g_p, translate_func))
    EGInvokeMethod(g_p, translate_func, (g_p, by));
  return g_p;
}


GraphicObj *
EGScaleGraphic(GraphicObj *g_p, WCRec *center, 
        FPNum sx, FPNum sy, FPNum sz)
{
  if (EGMethodIsDefined(g_p, scale_func))
    EGInvokeMethod(g_p, scale_func, (g_p, center, sx, sy, sz));
  return g_p;
}


GraphicObj *
EGRotateGraphic(GraphicObj *g_p, WCRec *center, WCRec *axial)
{
  if (EGMethodIsDefined(g_p, rotate_func))
    EGInvokeMethod(g_p, rotate_func, (g_p, center, axial));
  return g_p;
}

GraphicObj *
EGMirrorPlaneGraphic(GraphicObj *g_p,
				 WCRec *point_on_plane, WCRec *plane_normal)
{
  if (EGMethodIsDefined(g_p, mirror_plane_func))
    EGInvokeMethod(g_p, mirror_plane_func,
                   (g_p, point_on_plane, plane_normal));
  return g_p;
}


GraphicObj *
EGMirrorCenterGraphic(GraphicObj *g_p, WCRec *center)
{
  if (EGMethodIsDefined(g_p, mirror_center_func))
    EGInvokeMethod(g_p, mirror_center_func, (g_p, center));
  return g_p;
}


int 
EGModifyCurveDrawGC(Display *dsp, GC agc,
		  int style, int width, EPixel color,
		  int cap, int join)
{
  XSetLineAttributes(dsp, agc, (unsigned)width, style, cap, join);
  XSetForeground(dsp, agc, color);
  return 1;
}


void         
EGChangeAttributes(GraphicObj *g_p)
{
  unsigned long mask;

  mask = EASValsGetChangeMask();
  if ((mask & LAYER_MASK) || (mask & ALL_ATTRIB_MASK))
    EGSetLayer(g_p, EASValsGetLayer());
  if ((mask & NOT_CLIPPABLE_MASK) || (mask & ALL_ATTRIB_MASK))
    EGSetNotClippable(g_p, EASValsGetNotClippable());
  if ((mask & INVISIBLE_WHEN_CLIPPED_MASK) || (mask & ALL_ATTRIB_MASK))
    EGSetInvisibleWhenClipped(g_p, EASValsGetInvisibleWhenClipped());
  
  if (EGMethodIsDefined(g_p, change_attrib_func))
    EGInvokeMethod(g_p, change_attrib_func, (g_p));
}

void 
EGWithMaskChangeAttributes(unsigned long mask, GraphicObj *g_p)
{
  unsigned long old_mask;

  if ((mask & LAYER_MASK) || (mask & ALL_ATTRIB_MASK))
    EGSetLayer(g_p, EASValsGetLayer());
  if ((mask & NOT_CLIPPABLE_MASK) || (mask & ALL_ATTRIB_MASK))
    EGSetNotClippable(g_p, EASValsGetNotClippable());
  if ((mask & INVISIBLE_WHEN_CLIPPED_MASK) || (mask & ALL_ATTRIB_MASK))
    EGSetInvisibleWhenClipped(g_p, EASValsGetInvisibleWhenClipped());
  
  old_mask = EASValsGetChangeMask();
  EASValsSetChangeMask(mask);
  if (EGMethodIsDefined(g_p, change_attrib_func))
    EGInvokeMethod(g_p, change_attrib_func, (g_p));
  EASValsSetChangeMask(old_mask);
}

void 
EGCopyAttributesToActiveSet(GraphicObj *g_p)
{
  if (EGMethodIsDefined(g_p, copy_attrib_func))
    EGInvokeMethod(g_p, copy_attrib_func, (g_p));
}

void          
EGSetPickableMask(GraphicObj *g_p, unsigned long mask)
{
  g_p->pickable_mask = mask;
}

unsigned long 
EGGetPickableMask(GraphicObj *g_p)
{
  return g_p->pickable_mask;
}

BOOLEAN      
EGAttribValAsInAS(GraphicObj *g_p, unsigned long mask)
{
  if (EGMethodIsDefined(g_p, attrib_val_as_in_AS_func))
    return EGInvokeMethod(g_p, attrib_val_as_in_AS_func, (g_p, mask));
  else
    return NO;
}
 



GraphicObj *
EGModifyAssocData(GraphicObj *g_p, FPNum *vals)
{
  if (EGMethodIsDefined(g_p, modify_assoc_data_func)) 
    EGInvokeMethod(g_p, modify_assoc_data_func, (g_p, vals));
  return g_p;
}



BOOLEAN
EGGetAssocData(GraphicObj *g_p, FPNum *points, int *npoints)
{
  if (EGMethodIsDefined(g_p, get_assoc_data_func)) 
    return EGInvokeMethod(g_p, get_assoc_data_func, (g_p, points, npoints));
  else
    return NO;
}



void 
EGSetNotClippable(GraphicObj *g_p, BOOLEAN flag)
{
  g_p->not_clippable = flag;
}


BOOLEAN 
EGIsClippable(GraphicObj *g_p)
{
  return (!g_p->not_clippable);
}


void 
EGSetInvisibleWhenClipped(GraphicObj *g_p, BOOLEAN flag)
{
  g_p->invisible_when_clipped = flag;
}


BOOLEAN 
EGInvisibleWhenClipped(GraphicObj *g_p)
{
  return g_p->invisible_when_clipped;
}

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
#include "Eactset.h"
#include "Ecolors.h"
#include "Efonts.h"


/* ========================================================================= */
/* PRIVATE */
/* ========================================================================= */

static GraphicObj *as = NULL;

static EActiveSetRec fallback_active_set = {
  0x0,
  0,
  0x0,
  LineSolid,
  0,
  (Font)0,
  FILL_SOLID,
  NULL,
  0,
  CIRCLE_MARKER,
  2,
  1.0,
  PI/6,
  YES,
  YES,
  31,
  OVERLAP,
  NO,
  0,
  NO,
  NO,
  ARROW_VECMARKER,
  1.0,
  0.15,
  1,
  1,
  1,
  16,
  16,
	NO,
	NO
};

static EActiveSetRec *active_set = &fallback_active_set;


/* ========================================================================= */
/* PUBLIC */
/* ========================================================================= */


void
EASValsMakeActiveSet(void)
{
  ECtlBlockRec *cb;
  
  if (as == NULL) {
    as = CreateCtlBlock(ACTIVE_SET_CTLBLOCK);
    cb = (ECtlBlockRec *)as->spec.data;
    active_set = &cb->block_data.active_set;
  }
  active_set->change_mask                 =
    fallback_active_set.change_mask;
  active_set->layer                       =
    fallback_active_set.layer;
  active_set->curve_color                 =
    fallback_active_set.curve_color;
  active_set->curve_style                 =
    fallback_active_set.curve_style;
  active_set->curve_width                 =
    fallback_active_set.curve_width;
  active_set->font_id                     =
    fallback_active_set.font_id;
  active_set->fill_style                  =
    fallback_active_set.fill_style;
  active_set->text                        =
    fallback_active_set.text;
  active_set->marker_size                 =
    fallback_active_set.marker_size;
  active_set->marker_type                 =
    fallback_active_set.marker_type;
  active_set->key_point_intervals         =
    fallback_active_set.key_point_intervals;
  active_set->shrink                      =
    fallback_active_set.shrink;
  active_set->rotation_angle              =
    fallback_active_set.rotation_angle;
  active_set->show_polygon                =
    fallback_active_set.show_polygon;
  active_set->show_entity                 =
    fallback_active_set.show_entity;
  active_set->tessel_intervals            =
    fallback_active_set.tessel_intervals;
  active_set->select_criteria             =
    fallback_active_set.select_criteria;
  active_set->edge_flag                   =
    fallback_active_set.edge_flag;
  active_set->edge_color                  =
    fallback_active_set.edge_color;
  active_set->not_clippable_flag          =
    fallback_active_set.not_clippable_flag;
  active_set->invisible_when_clipped_flag =
    fallback_active_set.invisible_when_clipped_flag;
  active_set->vecmarker_type              =
    fallback_active_set.vecmarker_type;
  active_set->vector_scale                =
    fallback_active_set.vector_scale;
  active_set->vector_rate                 =
    fallback_active_set.vector_rate;
  active_set->scale_x                     =
    fallback_active_set.scale_x;
  active_set->scale_y                     =
    fallback_active_set.scale_y;
  active_set->scale_z                     =
    fallback_active_set.scale_z;
  active_set->tessel_u                   =
    fallback_active_set.tessel_u;
  active_set->tessel_v                   =
    fallback_active_set.tessel_v;
  active_set->vec_shift_flag              =
    fallback_active_set.vec_shift_flag;
	active_set->preserve_layer_flag         =
		fallback_active_set.preserve_layer_flag;
}


void
EASValsReplaceActiveSet(GraphicObj *new_as)
{
  ECtlBlockRec *cb;
  
  if (as != NULL) {
    EGDeepDestroyGraphics(as);
  }
  as = new_as;
  cb = (ECtlBlockRec *)as->spec.data;
  active_set = &cb->block_data.active_set;
}


GraphicObj *
EASValsGetActiveSet(void)
{
  if (as == NULL)
    EASValsMakeActiveSet();
  return as;
}


void 
EASValsSetLineWidth(unsigned width)
{
  active_set->curve_width = width;
}



unsigned 
EASValsGetLineWidth()
{
  return active_set->curve_width;
}



void 
EASValsSetLineStyle(int style)
{
  active_set->curve_style = style;
}



int 
EASValsGetLineStyle()
{
  return active_set->curve_style;
}



void 
EASValsSetMType(EMarkerType type)
{
  active_set->marker_type = type;
}



EMarkerType 
EASValsGetMType()
{
  return active_set->marker_type;
}


void 
EASValsSetMSize(unsigned size)
{
  active_set->marker_size = size;
}



unsigned 
EASValsGetMSize(void)
{
  return active_set->marker_size;
}



void 
EASValsSetColor(EPixel color)
{
  active_set->curve_color = color;
}



EPixel 
EASValsGetColor()
{
  return active_set->curve_color;
}



void 
EASValsSetEdgeColor(EPixel color)
{
  active_set->edge_color = color;
}



EPixel 
EASValsGetEdgeColor()
{
  return active_set->edge_color;
}



void 
EASValsSetFontId(Font font_id)
{
  active_set->font_id = font_id;
}



Font 
EASValsGetFontId()
{
  if (active_set->font_id == (Font)0)
    active_set->font_id = FontDefaultFont();
  return active_set->font_id;
}



void 
EASValsSetText(const char *text)
{
  active_set->text = strdup((char *)text);
}



char *
EASValsGetText(void)
{
  return active_set->text;
}



void 
EASValsSetKeyPIntervals(unsigned keypi)
{
  active_set->key_point_intervals = max(keypi, 1);
}



unsigned 
EASValsGetKeyPIntervals(void)
{
  return active_set->key_point_intervals;
}



void 
EASValsSetFillStyle(int fill)
{
  active_set->fill_style = fill;
}



int 
EASValsGetFillStyle(void)
{
  return active_set->fill_style;
}



void 
EASValsSetChangeMask(unsigned long mask)
{
  active_set->change_mask = mask;
}




unsigned long 
EASValsGetChangeMask(void)
{
  return active_set->change_mask;
}



void 
EASValsSetShrink(double shrink)
{
  active_set->shrink = shrink;
}



float 
EASValsGetShrink(void)
{
  return active_set->shrink;
}



void 
EASValsSetRotationAngle(double angle)
{
  active_set->rotation_angle = angle;
}




float 
EASValsGetRotationAngle(void)
{
  return active_set->rotation_angle;
}



void          
EASValsSetShowPoly(BOOLEAN show)
{
  active_set->show_polygon = show;
}



BOOLEAN       
EASValsGetShowPoly(void)
{
  return active_set->show_polygon;
}



void          
EASValsSetShowEntity(BOOLEAN show)
{
  active_set->show_entity = show;
}



BOOLEAN       
EASValsGetShowEntity(void)
{
  return active_set->show_entity;
}



void 
EASValsSetTesselIntervals(int i)
{
  active_set->tessel_intervals = max(i, 1);
}



int 
EASValsGetTesselIntervals(void)
{
  return active_set->tessel_intervals;
}



void 
EASValsSetSelectCriterion(ESelectCriteria sc)
{
  active_set->select_criteria = sc;
}



ESelectCriteria 
EASValsGetSelectCriterion(void)
{
  return active_set->select_criteria;
}



void 
EASValsSetLayer(int layer)
{
  active_set->layer = layer;
}



int 
EASValsGetLayer()
{
  return active_set->layer;
}




void 
EASValsSetEdgeFlag(BOOLEAN edge_flag)
{
  active_set->edge_flag = edge_flag;
}



BOOLEAN 
EASValsGetEdgeFlag()
{
  return active_set->edge_flag;
}




void 
EASValsSetNotClippable(BOOLEAN not_clippable_flag)
{
  active_set->not_clippable_flag = not_clippable_flag;
}



BOOLEAN 
EASValsGetNotClippable()
{
  return active_set->not_clippable_flag;
}



void 
EASValsSetInvisibleWhenClipped(BOOLEAN invisible_when_clipped_flag)
{
  active_set->invisible_when_clipped_flag = invisible_when_clipped_flag;
}



BOOLEAN 
EASValsGetInvisibleWhenClipped(void)
{
  return active_set->invisible_when_clipped_flag;
}



void 
EASValsSetVecMType(EVecMarkerType type)
{
  active_set->vecmarker_type = type;
}



EVecMarkerType 
EASValsGetVecMType()
{
  return active_set->vecmarker_type;
}



void
EASValsSetVecScale(FPNum scale)
{
	active_set->vector_scale = scale;
}


FPNum
EASValsGetVecScale(void)
{
	return active_set->vector_scale;
}


void
EASValsSetVecRate(FPNum rate)
{
	active_set->vector_rate = rate;
}


FPNum
EASValsGetVecRate(void)
{
	return active_set->vector_rate;
}



void
EASValsSetScaleX(FPNum scale)
{
  active_set->scale_x = scale;
}


FPNum
EASValsGetScaleX(void)
{
  return active_set->scale_x;
}


void
EASValsSetScaleY(FPNum scale)
{
  active_set->scale_y = scale;
}


FPNum
EASValsGetScaleY(void)
{
  return active_set->scale_y;
}


void
EASValsSetScaleZ(FPNum scale)
{
  active_set->scale_z = scale;
}


FPNum
EASValsGetScaleZ(void)
{
  return active_set->scale_z;
}



void
EASValsSetIsotropicScale(FPNum scale)
{
  active_set->scale_x = active_set->scale_y = active_set->scale_z = scale;
}


FPNum
EASValsGetIsotropicScale(void)
{
  return (active_set->scale_x + active_set->scale_y + active_set->scale_z) / 3;
}


void 
EASValsSetUTesselIntervals(int i)
{
  active_set->tessel_u = max(i, 1);
}



int 
EASValsGetUTesselIntervals(void)
{
  return active_set->tessel_u;
}



void 
EASValsSetVTesselIntervals(int i)
{
  active_set->tessel_v = max(i, 1);
}



int 
EASValsGetVTesselIntervals(void)
{
  return active_set->tessel_v;
}



void 
EASValsSetVecShiftFlag(BOOLEAN shift_flag)
{
  active_set->vec_shift_flag = shift_flag;
}



BOOLEAN 
EASValsGetVecShiftFlag()
{
  return active_set->vec_shift_flag;
}



void
EASValsSetPreserveLayerFlag(BOOLEAN preserve_layer_flag)
{
	active_set->preserve_layer_flag = preserve_layer_flag;
}


BOOLEAN
EASValsGetPreserveLayerFlag(void)
{
	return active_set->preserve_layer_flag;
}



void 
EASValsSetVecShift(int shift)
{
  active_set->vec_shift = shift;
}



int
EASValsGetVecShift(void)
{
  return active_set->vec_shift;
}




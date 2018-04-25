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


#ifndef EACTSET_H
#define EACTSET_H

#ifdef __cplusplus
extern "C" {
#endif

#include <X11/X.h>
#include "Etypes.h"
#include "Egraphic.h"


/* ========================================================================= */
/* PRIVATE */
/* ========================================================================= */

/* Data structures; never to be accessed from application programs */

typedef struct EActiveSetRec {
  unsigned long     change_mask;
  int               layer;
  EPixel            curve_color;
  int               curve_style;
  unsigned          curve_width;
  Font              font_id;
  int               fill_style;
  char             *text;
  Dimension         marker_size;
  EMarkerType       marker_type;
  unsigned          key_point_intervals;
  float             shrink;
  float             rotation_angle;
  BOOLEAN           show_polygon;
  BOOLEAN           show_entity;
  int               tessel_intervals;
  ESelectCriteria   select_criteria;
  BOOLEAN           edge_flag;
  EPixel            edge_color;
  BOOLEAN           not_clippable_flag;
  BOOLEAN           invisible_when_clipped_flag;
  EVecMarkerType    vecmarker_type;
  FPNum             vector_scale;
  FPNum             vector_rate;
  FPNum             scale_x;
  FPNum             scale_y;
  FPNum             scale_z;
  int               tessel_u;
  int               tessel_v;
	BOOLEAN           vec_shift_flag;
	BOOLEAN           preserve_layer_flag;
	int               vec_shift;
}                                            EActiveSetRec;

#define CTLBLOCK_TYPEDEFS
#include "Ectlblock.h"


/* ========================================================================= */
/* PUBLIC DEFINITIONS */
/* ========================================================================= */

#define ALL_ATTRIB_MASK                       (1L<<0)
#define STYLE_MASK                            (1L<<1)
#define LAYER_MASK                            (1L<<2)
#define WIDTH_MASK                            (1L<<3)
#define COLOR_MASK                            (1L<<4)
#define FONT_MASK                             (1L<<5)
#define MTYPE_MASK                            (1L<<6)
#define MSIZE_MASK                            (1L<<7)
#define FILL_MASK                             (1L<<8)
#define SHOW_POLY_MASK                        (1L<<9)
#define SHOW_ENTITY_MASK                      (1L<<10)
#define TESSEL_INTERVALS_MASK                 (1L<<11)
#define TEXT_STRING_MASK                      (1L<<12)
#define SHRINK_MASK                           (1L<<13)
#define EDGE_COLOR_MASK                       (1L<<14)
#define EDGE_FLAG_MASK                        (1L<<15)
#define NOT_CLIPPABLE_MASK                    (1L<<16)
#define INVISIBLE_WHEN_CLIPPED_MASK           (1L<<17)
#define VEC_TYPE_MASK                         (1L<<18)
#define VEC_SCALE_MASK                        (1L<<19)
#define VEC_RATE_MASK                         (1L<<20)
#define TESSEL_U_MASK                         (1L<<21)
#define TESSEL_V_MASK                         (1L<<22)
#define VEC_SHIFT_MASK                        (1L<<23)
#define PRESERVE_LAYER                        (1L<<24)

/* obsolete masks */

#define VECTOR_TYPE_MASK                      (1L<<18)
#define VECMTYPE_MASK                         (1L<<18)
#define VECTOR_SCALE_MASK                     (1L<<19)
#define VECMSCALE_MASK                        (1L<<19)
#define VECTOR_RATE_MASK                      (1L<<20)
#define VECMRATE_MASK                         (1L<<20)
#define VEC_SHIFT_FLAG_MASK                   (1L<<23)



#define SOLID_STYLE   LineSolid
#define DASHED_STYLE  LineOnOffDash

#define FILL_HOLLOW -1
#define FILL_SOLID  FillSolid

/* ========================================================================= */
/* PUBLIC FUNCTIONS */
/* ========================================================================= */

void
EASValsMakeActiveSet(void);
void
EASValsReplaceActiveSet(GraphicObj *new_as);
GraphicObj *
EASValsGetActiveSet(void);

void             EASValsSetLineWidth(unsigned width);
unsigned         EASValsGetLineWidth(void);
void             EASValsSetLineStyle(int style);
int              EASValsGetLineStyle(void);
void             EASValsSetColor(EPixel color);
EPixel           EASValsGetColor(void);
void             EASValsSetMType(EMarkerType type);
EMarkerType      EASValsGetMType(void);
void             EASValsSetMSize(unsigned size);
unsigned         EASValsGetMSize(void);
void             EASValsSetFontId(Font font_id);
Font             EASValsGetFontId(void);
void             EASValsSetText(const char *text);
char            *EASValsGetText(void);
void             EASValsSetKeyPIntervals(unsigned keypi);
unsigned         EASValsGetKeyPIntervals(void);
void             EASValsSetFillStyle(int fill);
int              EASValsGetFillStyle(void);
void             EASValsSetChangeMask(unsigned long mask);
unsigned long    EASValsGetChangeMask(void);
void             EASValsSetShrink(double shrink);
float            EASValsGetShrink(void);
void             EASValsSetRotationAngle(double angle);
float            EASValsGetRotationAngle(void);
void             EASValsSetShowPoly(BOOLEAN show);
BOOLEAN          EASValsGetShowPoly(void);
void             EASValsSetShowEntity(BOOLEAN show);
BOOLEAN          EASValsGetShowEntity(void);
void             EASValsSetTesselIntervals(int i);
int              EASValsGetTesselIntervals(void);
void             EASValsSetSelectCriterion(ESelectCriteria sc);
ESelectCriteria  EASValsGetSelectCriterion(void);
void             EASValsSetLayer(int layer);
int              EASValsGetLayer(void);
void             EASValsSetEdgeColor(EPixel color);
EPixel           EASValsGetEdgeColor(void);
void             EASValsSetEdgeFlag(BOOLEAN edge_flag);
BOOLEAN          EASValsGetEdgeFlag(void);
void             EASValsSetNotClippable(BOOLEAN not_clippable_flag);
BOOLEAN          EASValsGetNotClippable(void);
void             EASValsSetVecMType(EVecMarkerType type);
EVecMarkerType   EASValsGetVecMType(void);
void             EASValsSetVecScale(FPNum scale);
FPNum            EASValsGetVecScale(void);
void             EASValsSetVecRate(FPNum rate);
FPNum            EASValsGetVecRate(void);
void             EASValsSetScaleX(FPNum scale);
FPNum            EASValsGetScaleX(void);
void             EASValsSetScaleY(FPNum scale);
FPNum            EASValsGetScaleY(void);
void             EASValsSetScaleZ(FPNum scale);
FPNum            EASValsGetScaleZ(void);
void             EASValsSetIsotropicScale(FPNum scale);
FPNum            EASValsGetIsotropicScale(void);
void             EASValsSetInvisibleWhenClipped(BOOLEAN
                                                invisible_when_clipped_flag);
BOOLEAN          EASValsGetInvisibleWhenClipped(void);
void             EASValsSetUTesselIntervals(int tessel);
int              EASValsGetUTesselIntervals(void);
void             EASValsSetVTesselIntervals(int tessel);
int              EASValsGetVTesselIntervals(void);
void             EASValsSetVecShiftFlag(BOOLEAN shift_flag);
BOOLEAN          EASValsGetVecShiftFlag(void);
void             EASValsSetPreserveLayerFlag(BOOLEAN preserve_layer_flag);
BOOLEAN          EASValsGetPreserveLayerFlag(void);
void             EASValsSetVecShift(int shift);
int              EASValsGetVecShift(void);



#ifdef __cplusplus
}
#endif

#endif
